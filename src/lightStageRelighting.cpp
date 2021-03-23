/*
 *     Image-Based Relighting Framework
 *
 *     Author:  Antoine TOISOUL LE CANN
 *
 *     Copyright © 2016 Antoine TOISOUL LE CANN, Imperial College London
 *              All rights reserved
 *
 *
 * Image-Based Relighting Framework is free software: you can redistribute it and/or modify
 *
 * it under the terms of the GNU Lesser General Public License as published by
 *
 * the Free Software Foundation, either version 3 of the License, or
 *
 * (at your option) any later version.
 *
 * Image-Based Relighting Framework is distributed in the hope that it will be useful,
 *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 *
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * \file lightStageRelighting.cpp
 * \brief Class to compute the light stage relighting using Paul Debevec's light stage data (www.pauldebevec.com).
 * \author Antoine Toisoul Le Cann
 * \date August, 7th, 2014
 *
 * Class that inherites of the Relighting class. Computes the light stage relighting of an object.
 */

#include "lightStageRelighting.h"

using namespace std;
using namespace cv;

/**
 * Constructor of the LightStageRelighting class.
 * @brief LightStageRelighting
 */
LightStageRelighting::LightStageRelighting(): Relighting(), m_voronoi(new Voronoi())
{

}

/**
  * Destructor of the LightStageRelighting class.
  */
LightStageRelighting::~LightStageRelighting()
{
    delete m_voronoi;
}

/**
 * Computes the relighting of the object using the light stage method.
 * @brief relighting
 */
void LightStageRelighting::relighting()
{
    //Prints in the progress window what relighting is happening
    this->updateProgressWindow(QString("Relighting the " + m_object + " in " +m_environmentMapName), 0);

    /*---Loads the EM---*/
    this->loadEnvironmentMap();
    m_voronoi->setEnvironmentMapSize(m_environmentMapWidth, m_environmentMapHeight);

    /*---Loads the reflectance field ---*/
    //Load images in CV_8UC3
    loadReflectanceField();
    this->updateProgressWindow(QString("Images loaded"), 25);

    //The files provided have a gamma correction of GAMMA
    //It is removed for the calculations
    this->removeGammaReflectanceField(GAMMA);
    this->updateProgressWindow(QString("Gamma correction removed"), 50);

    /*---Read the light directions ---*/
    std::vector<std::vector<float> > lightDirectionsCartesian;
    std::vector<Point2i> lightDirectionsLatLongMap;
    readFile(this->getFolderPath() + "/light_directions.txt", lightDirectionsCartesian);

    //The object is taken as a reference
    //The directions given in the text file are from the light stage towards the object
    //The directions used are from the object to the light stage
    for(unsigned int n = 0 ; n< lightDirectionsCartesian.size() ; n++)
    {
        lightDirectionsCartesian[n][0] *= -1;
        lightDirectionsCartesian[n][1] *= -1;
        lightDirectionsCartesian[n][2] *= -1;
    }

    //Loop to generate several results (rotate the environment map depending on the offset)
    int progressBarValue = 50;

    float offset = 0.0;
    for(unsigned int l = 0 ; l<m_numberOfOffsets ; l++)
    {

        offset = (float) 2.0*l*M_PI/m_numberOfOffsets;

        //Convert the light direction from the cartesian coordinate system to the spherical coordinate system
        //Output : points point2f that correspond to a pixel in the environment map
        cartesianToLatLongVector2i(lightDirectionsCartesian, lightDirectionsLatLongMap,m_environmentMapWidth, m_environmentMapHeight);

        //Voronoi tesselation using the light directions and the environment map
        //The object basis calculates the voronoi tesselation when the light directions are added as point light sources
        m_voronoi->clearVoronoi();
        m_voronoi->setVoronoi(lightDirectionsLatLongMap);

        //Many images are saved here to understand each step of the relighting
        //Save the voronoi diagrams to files
        this->saveLightStageDirection();
        this->saveLightStageIntensities();
        this->saveVoronoiTesselation(l);

        //Compute the weight of each voronoi cell (sum of the intensities taking into account the solid angle)
        m_voronoi->computeVoronoiIntensity(m_environmentMap);

        //Compute the weight of each voronoi cell independently for each RGB channel (average of the color of the cell taking into account the solid angle)
        if(m_lightType.toStdString() == "Gaussian")
        {
            m_voronoi->computeVoronoiWeightsGaussian(m_environmentMap, offset);
        }
        else if(m_lightType.toStdString() == "Point")
        {
            m_voronoi->computeVoronoiWeightsRGB(m_environmentMap, offset);
        }

        //Normalise the weights for display purposes
        m_weightsRGB = m_voronoi->getRGBWeights();
        normalizeWeightsRGB(m_weightsRGB);

        //Save the weights diagram
        this->saveVoronoiWeights(l);

        progressBarValue += 25/m_numberOfOffsets;
        this->updateProgressWindow(QString("Weights computed"), progressBarValue);

        //Compute the result of the linear combination
        //Change the background, hange the exposure and apply gamma
        this->computeFinalRelighting();
        this->rayTraceBackground(offset);
        this->changeExposure(EXPOSURE);
        this->gammaCorrection(GAMMA);

        //Save the final result
        ostringstream osstream;
        osstream << this->getFolderPath() << "/Results/light_stage/" << m_object.toStdString() << "_" << m_lightType.toStdString() << "_" << m_environmentMapName.toStdString() << "_offset" << l << ".jpg";
        this->saveResult(SAVE_8BITS, osstream.str());

        emit updateImage(QString(osstream.str().c_str()));
        osstream.str("");

        progressBarValue += 25/m_numberOfOffsets;
        this->updateProgressWindow(QString("Result " + QString::number(l) + " generated"), progressBarValue);
    }

    this->updateProgressWindow(QString("Done"), 100);
}

/**
 * @brief loadReflectanceField
 */
bool LightStageRelighting::loadReflectanceField()
{
    //Only allocate memory if it has not been already allocated
    if(m_reflectanceField == NULL)
        m_reflectanceField = new Mat[m_numberOfLightingConditions];

    string file;
    string extension;

    if(m_object == "Plant")
    {
        file = string("light_stage/plant_left_");
        extension = string(".png");
    }
    else if(m_object == "Helmet")
    {
        file = string("light_stage/helmet_side_left_");
        extension = string(".png");
    }
    else
    {
        cerr << "Object does not exist : " << m_object.toStdString() << endl;
        return EXIT_FAILURE;
    }

    ostringstream osstream;

    for(unsigned int i = 0 ; i<m_numberOfLightingConditions ; i++)
    {

       osstream << this->getFolderPath();

       if(i<10)
       {
          osstream << "/images/" << file << "000" << i << extension;
       }
       else if(i<100)
       {
          osstream << "/images/" << file << "00" << i << extension;
       }
       else
       {
          osstream << "/images/" << file << "0" << i << extension;
       }

       m_reflectanceField[i] = imread(osstream.str(), CV_LOAD_IMAGE_COLOR);

       if(!m_reflectanceField[i].data)
       {
          cerr << "Couldn't open the file : " << osstream.str() << endl;
          return EXIT_FAILURE;
       }


       m_reflectanceField[i].convertTo(m_reflectanceField[i], CV_32FC3, 1.0/255.0);
       osstream.str("");
    }

    //Load the mask
    osstream << this->getFolderPath() << "/images/light_stage/" << m_object.toStdString() << "_mask.png";

    m_objectMask = imread(osstream.str(), CV_LOAD_IMAGE_COLOR);

    if(!m_objectMask.data)
    {
        cerr << "Couldn't open the file : " << osstream.str() << endl;
        return EXIT_FAILURE;
    }

    m_objectMask.convertTo(m_objectMask, CV_32FC3, 1.0/255.0);

    return EXIT_SUCCESS;
}


/**
 * Function that sets all the parameters for the relighting.
 * @brief setRelighting
 * @param INPUT : environmentMap Name of the environment map.
 * @param INPUT : lightType Type of light sources used.
 * @param INPUT : numberOfLightingConditions Number of lighting conditions.
 * @param INPUT : numberOfOffsets number of offsets (rotations of the environment map) under which the object will be relit.
 * @param INPUT : exposure exposure of the final result.
 */
void LightStageRelighting::setRelighting(QString &object, QString &environmentMap, QString &lightType, unsigned int numberOfLightingConditions,
                              unsigned int numberOfOffsets)
{
    m_voronoi->clearVoronoi();
    this->setObject(object);
    this->setEnvironmentMap(environmentMap);
    this->setLightType(lightType);
    this->setNumberOfOffsets(numberOfOffsets);
    this->setNumberOfLightingConditions(numberOfLightingConditions);

}

/**
 * Restart the relighting by reinitialising all the variables.
 * @brief clearRelighting
 */
void LightStageRelighting::clearRelighting()
{
    m_voronoi->clearVoronoi();
    m_object = QString("");
    m_environmentMapName = QString("");
    m_lightType = QString("");
    m_numberOfOffsets = 0;
    m_numberOfLightingConditions = 1;

    //Environment Map parameters
    m_environmentMapWidth = 1024;
    m_environmentMapHeight = 512;
    m_numberOfComponents = 3;
}

/**
 * Saves the environment map with the voronoi tesselation displayed.
 * @brief saveVoronoiTesselation
 */
void LightStageRelighting::saveVoronoiTesselation(int offset)
{
    //Load the environment map to display the voronoi tesselation
    Mat environmentMap = loadPFM(this->getFolderPath() + "/environment_maps/" + m_environmentMapName.toStdString() + ".pfm"); //Load the environment map to display the voronoi tesselation

    if(!environmentMap.data)
    {
        cerr << "Could not load : " << this->getFolderPath() + "/environment_maps/" + m_environmentMapName.toStdString() + ".pfm" << endl;
    }

    Mat EMGamma, environmentMapRotated;

    //Apply gamma correction
    gammaCorrectionImage(environmentMap, EMGamma, GAMMA);

    //Rotate and paint
    rotateLatLongMap(EMGamma, offset, environmentMapRotated);
    m_voronoi->paintVoronoi(environmentMapRotated);

    //Convert to 8 bits and save it
    environmentMapRotated.convertTo(environmentMapRotated, CV_8UC3, 255.0);

    ostringstream osstream;
    osstream << this->getFolderPath() << "/Results/light_stage/voronoi_diagram_" << m_lightType.toStdString() << "_" << m_environmentMapName.toStdString() << "_offset" << offset << ".jpg";

    imwrite(osstream.str(),environmentMapRotated);
}

/**
 * Saves the Voronoi diagram with each cell painted as its corresponding RGB weight.
 * @brief saveVoronoiWeights
 * @param offset
 */
void LightStageRelighting::saveVoronoiWeights(int offset)
{
    //Load the environment map to display the voronoi tesselation
    Mat environmentMap = loadPFM(this->getFolderPath() + "/environment_maps/" + m_environmentMapName.toStdString() + ".pfm");

    if(!environmentMap.data)
    {
        cerr << "Could not load : " << this->getFolderPath() + "/environment_maps/" + m_environmentMapName.toStdString() + ".pfm" << endl;
    }

    Mat EMWeights;

    gammaCorrectionImage(environmentMap, EMWeights, GAMMA);

    m_voronoi->paintVoronoiCells(EMWeights);

    //Convert to 8 bits and save it
    EMWeights.convertTo(EMWeights, CV_8UC3);

    ostringstream osstream;
    osstream << this->getFolderPath() << "/Results/light_stage/voronoi_diagram_avg_" << m_lightType.toStdString() << "_" << m_environmentMapName.toStdString() << "_offset" << offset << ".jpg";
    imwrite(osstream.str(), EMWeights);
}

/**
 * Saves the environment map with the light stage directions mapped on it.
 * @brief saveLightStageDirection
 */
void LightStageRelighting::saveLightStageDirection()
{
    Mat environmentMap = loadPFM(this->getFolderPath() + "/environment_maps/" + m_environmentMapName.toStdString() + ".pfm"); //Load the environment map to display the voronoi tesselation

    if(!environmentMap.data)
    {
        cerr << "Could not load : " << this->getFolderPath() + "/environment_maps/" + m_environmentMapName.toStdString() + ".pfm" << endl;
    }

    Mat EMLSDirections;

    gammaCorrectionImage(environmentMap, EMLSDirections, GAMMA);

    //Convert to 8 bits and save it
    EMLSDirections.convertTo(EMLSDirections, CV_8UC3, 255.0);

    m_voronoi->paintPointLights(EMLSDirections);
    imwrite(this->getFolderPath() + "/Results/light_stage/LSDirections.jpg", EMLSDirections);
}

/**
 * Saves the intensities of the light stage light sources.
 * @brief saveLightStageIntensities
 */
void LightStageRelighting::saveLightStageIntensities()
{
    Mat environmentMap = loadPFM(this->getFolderPath() + "/environment_maps/" + m_environmentMapName.toStdString() + ".pfm"); //Load the environment map to display the voronoi tesselation

    if(!environmentMap.data)
    {
        cerr << "Could not load : " << this->getFolderPath() + "/environment_maps/" + m_environmentMapName.toStdString() + ".pfm" << endl;
    }

    Mat environmentMapLSIntensities;

    gammaCorrectionImage(environmentMap, environmentMapLSIntensities,GAMMA);

    //Convert to 8 bits and save it
    environmentMapLSIntensities.convertTo(environmentMapLSIntensities, CV_8UC3, 255.0);

    m_voronoi->paintLightStageIntensities(environmentMapLSIntensities);
    imwrite(this->getFolderPath() + "/Results/light_stage/lightStageIntensities.png",environmentMapLSIntensities);
}

/**
 * Virtual method to update the progress window.
 * @brief updateProgressWindow
 * @param updateText
 * @param progressBarValue
 */
void LightStageRelighting::updateProgressWindow(QString updateText, int progressBarValue)
{
    emit statusUpdate(updateText);
    emit updateProgressBar(progressBarValue);
    qApp->processEvents(); //Refresh the main window to process the signals
}
