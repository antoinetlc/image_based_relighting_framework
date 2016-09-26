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
 * \file freeformlightstage.cpp
 * \brief Class to compute the free form light relighting.
 * \author Antoine Toisoul Le Cann
 * \date August, 7th, 2014
 *
 * Class that inherites of the Relighting class. Performs the Free form relighting of an object.
 */

#include "freeformlightstage.h"

using namespace std;
using namespace cv;

/**
 * Constructor of the FreeFormLightStage class.
 * @brief FreeFormLightStage
 */
FreeFormLightStage::FreeFormLightStage(): Relighting(), m_voronoi(new Voronoi()), m_exposure(0.0)
{

}

/**
  *Destructor of the FreeFormLightStage. Release memory.
  */
FreeFormLightStage::~FreeFormLightStage()
{
    delete m_voronoi;
}

/**
 * Computes the relighting of the object using the free form light stage method.
 * @brief relighting
 */
void FreeFormLightStage::relighting()
{
    m_object = QString("Egg");
    this->updateProgressWindow(QString("Relighting the " + m_object + " in " +m_environmentMapName), 0);

    /*---Loads the EM---*/
    this->loadEnvironmentMap();
    m_voronoi->setEnvironmentMapSize(m_environmentMapWidth, m_environmentMapHeight);

    //Load the reflectance field and remove dark room
    this->loadReflectanceField();
    this->removeDarkRoom();

    this->updateProgressWindow(QString("Images loaded"), 25);

    //Mouse parameters used to identified the lights manually
    MouseParameters mouseParameters;
    mouseParameters.voronoi = m_voronoi;
    mouseParameters.windowName = "Lighting condition";
    mouseParameters.latLongWidth = m_environmentMapWidth;
    mouseParameters.latLongHeight = m_environmentMapHeight;
    mouseParameters.isPressed = false;
    mouseParameters.numberOfLightSourcesAdded = 0;

    //Manual identification of light sources
    if(m_identificationMethod == "Manual")
    {
        this->identifyLightsUser(mouseParameters);
        if(m_saveVoronoi)
        {
           m_voronoi->saveVoronoi();
        }
    }
    else if(m_identificationMethod == "Load")
    {
        m_voronoi->loadVoronoi();
    }

    this->updateProgressWindow(QString("Voronoi diagram generated"), 50);


    //Offsets
    float offset = 0.0;
    int progressBarValue = 50;
    for(unsigned int l = 0 ; l<m_numberOfOffsets ; l++)
    {
        offset = (float) 2.0*l*M_PI/m_numberOfOffsets;

        //Save the Voronoi diagram
        this->saveVoronoiTesselation(l);

        //Compute the weights of the voronoi diagram
        //The integration can be modulated by a gaussian
        if(m_lightType.toStdString() == "Gaussian")
        {
            float* varianceX = new float[m_numberOfLightingConditions];
            float* varianceY = new float[m_numberOfLightingConditions];

            for(unsigned int m = 0 ; m<m_numberOfLightingConditions ; m++)
            {
                varianceX[m] = 300.0;
                varianceY[m] = 300.0;
            }
            m_voronoi->clearWeights(); //Reinitialise the weights
            m_voronoi->computeVoronoiWeightsGaussianOR(m_environmentMap, offset, varianceX, varianceY);
            m_weightsRGB = m_voronoi->getRGBWeights();

            delete[] varianceX;
            delete[] varianceY;

        }
        else if(m_lightType.toStdString() == "Point")
        {
           m_voronoi->clearWeights(); //Reinitialise the weights
           m_voronoi->computeVoronoiWeightsOR(m_environmentMap, offset);
           m_weightsRGB = m_voronoi->getRGBWeights();
        }


        progressBarValue += 25/(m_numberOfOffsets);
        this->updateProgressWindow(QString("Weights computed"), progressBarValue);

        //Normalize the weights
        normalizeWeightsRGB(m_weightsRGB);

        //Calculate the result, change the background, the exposure and apply the gamma
        this->computeFinalRelighting();
        this->rayTraceBackground(offset+M_PI);//Offset by Pi. Reason not found yet
        this->changeExposure(m_exposure);
        this->gammaCorrection(2.2);

        //Saves the weights
        this->saveVoronoiWeights(l);

        //imshow takes into account the format of the matrix and will display CV_32FC3 matrices (with values between 0 and 1)
        //imwrite will save black images if the values of the pixels are between 0 and 1
        //All the values are scaled between 0 and 255 before raytracing the background
        ostringstream osstream;
        osstream << this->getFolderPath() << "/Results/free_form/" << m_object.toStdString() << "_" << m_lightType.toStdString() << "_" << m_environmentMapName.toStdString() << "_offset" << l << ".jpg";
        this->saveResult(SAVE_8BITS, osstream.str());

        emit updateImage(QString(osstream.str().c_str()));
        osstream.str("");

        progressBarValue += 25/(m_numberOfOffsets);
        this->updateProgressWindow(QString("Result " + QString::number(l) + " generated"), progressBarValue);
    }

    this->updateProgressWindow(QString("Done"), 100);
}

/**
  * Virtual pure function.
  * Loads the reflectance field of the object.
  * @brief loadReflectanceField
  * @return
  */
bool FreeFormLightStage::loadReflectanceField()
{
    //Only allocate memory if it has not been already allocated
    if(m_reflectanceField == NULL)
        m_reflectanceField = new Mat[m_numberOfLightingConditions];

    string file("free_form/EggFF_");
    string extension(".png");

    ostringstream osstream;

    //Load the images
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

       m_reflectanceField[i] = imread(osstream.str(), CV_LOAD_IMAGE_ANYDEPTH | CV_LOAD_IMAGE_COLOR);

       if(!m_reflectanceField[i].data)
       {
          cerr << "Couldn't open the file : " << osstream.str() << endl;
          return EXIT_FAILURE;
       }

       m_reflectanceField[i].convertTo(m_reflectanceField[i], CV_32FC3, 1.0/255.0);

       osstream.str("");
    }

    //Load the mask
    //Only one object for Free form acquisition
    osstream << this->getFolderPath() << "/images/free_form/EggFF_mask.png";

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
 * Removes the dark room measurement from the reflectance field.
 * @brief removeDarkRoom
 */
void FreeFormLightStage::removeDarkRoom()
{
    //Load the dark room picture
    Mat darkRoom = imread(this->getFolderPath() + "/images/free_form/darkRoom.png", CV_LOAD_IMAGE_COLOR);

    if(!darkRoom.data)
    {
        cerr << "Could not load the file : " << this->getFolderPath() << "/images/free_form/darkRoom.png" << endl;
    }

    darkRoom.convertTo(darkRoom, CV_32FC3);
    darkRoom /= 255.0;

    darkRoom *= pow(2.0,-2);

    for(unsigned int i = 0 ; i<m_numberOfLightingConditions ; i++)
    {
        m_reflectanceField[i]-= darkRoom;
        m_reflectanceField[i] = max(m_reflectanceField[i], 0.0); // clamp
    }
}

/**
 * Method to manually select the incoming light directions in the environment map of the office.
 * @brief identifyLightsUser
 * @param INPUT : mouseParameters parameters that are sent to the callback function to manually select light sources.
 */
void FreeFormLightStage::identifyLightsUser(MouseParameters& mouseParameters)
{
    Mat lightingCondition;
    ostringstream osstream;
    int cellNumber = 0;
    std::vector<std::vector<int> > cellNumberPerPicture;
    int numberOfLightSourcesAdded = 0;

    //Display each image of the room and ask the user to draw the lights
    //Save the light position in the LightingBasis
    for(unsigned int i = 0 ; i<m_numberOfLightingConditions ; i++)
    {
        osstream << this->getFolderPath();

        if(i<10)
        {
            osstream << "/lighting_conditions/free_form/condition_00" << i << ".ppm";
        }
        else if(i<100)
        {
            osstream << "/lighting_conditions/free_form/condition_0" << i << ".ppm";
        }
        else // i is greater than 100
        {
            osstream << "/lighting_conditions/free_form/condition_" << i << ".ppm";
        }

        lightingCondition = imread(osstream.str(), CV_LOAD_IMAGE_COLOR);

        if(!lightingCondition.data)
            cout << "Could not load " << osstream.str() << endl;

        //Enable user interaction with the mouse
        mouseParameters.environmentMap = osstream.str();
        imshow(mouseParameters.windowName, lightingCondition);
        setMouseCallback(mouseParameters.windowName, callBackMouseEvents, &mouseParameters);
        waitKey(0);

        //Fill the vector the contains the voronoi cell numbers for each picture
        std::vector<int> cellsForImagei;
        numberOfLightSourcesAdded = mouseParameters.numberOfLightSourcesAdded;

        for(int j = 0 ; j<numberOfLightSourcesAdded ; j++)
        {
            cellsForImagei.push_back(cellNumber);
            cellNumber++;
        }

        cellNumberPerPicture.push_back(cellsForImagei);

        //Initialise parameters for next pictures
        osstream.str("");
        mouseParameters.numberOfLightSourcesAdded = 0;
    }

    //Add the vector that makes the correspondence between cellNumbers and pictures to the basis
    m_voronoi->setCellNumberPerPicture(cellNumberPerPicture);

    //Remove the mouse callback
    setMouseCallback(mouseParameters.windowName, NULL, NULL);
    destroyWindow(mouseParameters.windowName);
}

/**
 * Saves the environment map with the voronoi tesselation displayed.
 * @brief saveVoronoiTesselation
 */
void FreeFormLightStage::saveVoronoiTesselation(int offset)
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
    osstream << this->getFolderPath() << "/Results/free_form/voronoi_diagram_" << m_lightType.toStdString() << "_" << m_environmentMapName.toStdString() << "_offset" << offset << ".jpg";

    imwrite(osstream.str(),environmentMapRotated);
}

/**
 * Saves the Voronoi diagram with each cell painted as its corresponding RGB weight.
 * @brief saveVoronoiWeights
 * @param offset
 */
void FreeFormLightStage::saveVoronoiWeights(int offset)
{
    //Load the environment map to display the voronoi tesselation
    Mat environmentMap = loadPFM(this->getFolderPath() + "/environment_maps/" + m_environmentMapName.toStdString() + ".pfm");

    if(!environmentMap.data)
    {
        cerr << "Could not load : " << this->getFolderPath() + "/environment_maps/" + m_environmentMapName.toStdString() + ".pfm" << endl;
    }

    Mat EMWeights;
    gammaCorrectionImage(environmentMap, EMWeights, GAMMA);

    m_voronoi->paintVoronoiCellsOR(EMWeights);

    //Convert to 8 bits and save it
    EMWeights.convertTo(EMWeights, CV_8UC3);

    ostringstream osstream;
    osstream << this->getFolderPath() << "/Results/free_form/voronoi_diagram_avg_" << m_lightType.toStdString() << "_" << m_environmentMapName.toStdString() << "_offset" << offset << ".jpg";
    imwrite(osstream.str(), EMWeights);
}

/**
 * Setter to change the exposure of the final result.
 * @brief setExposure
 * @param INPUT : exposure double that increases the final exposure by exposure f-stops.
 */
void FreeFormLightStage::setExposure(double exposure)
{
    m_exposure = exposure;
}

/**
 * Setter that sets the method to select light sources.
 * @brief setIdentificationMethod
 * @param INPUT : identificationMethod method to select the light sources.
 */
void FreeFormLightStage::setIdentificationMethod(QString& identificationMethod)
{
    m_identificationMethod = identificationMethod;
}

/**
 * Setter to change the value of the boolean that saves of the Voronoi diagram to a file.
 * @brief setSaveVoronoiDiagram
 * @param INPUT : save boolean to save the diagram.
 */
void FreeFormLightStage::setSaveVoronoiDiagram(bool save)
{
    m_saveVoronoi = save;
}

/**
 * Function that sets all the parameters for the relighting.
 * @brief setRelighting
 * @param INPUT : environmentMap Name of the environment map.
 * @param INPUT : lightType Type of light sources used.
 * @param INPUT : numberOfLightingConditions Number of lighting conditions.
 * @param INPUT : numberOfOffsets number of offsets (rotations of the environment map) under which the object will be relit.
 * @param INPUT : exposure exposure of the final result.
 * @param INPUT : identificationMethod Method for the selection of light sources. It can have two values Manual or Load.
 * @param INPUT : save boolean to know if the voronoi diagram has to be saved or not (manual selection only)
 */
void FreeFormLightStage::setRelighting(QString &environmentMap, QString &lightType, unsigned int numberOfLightingConditions,
                   unsigned int numberOfOffsets, double exposure, QString identificationMethod, bool save)
{
    m_voronoi->clearVoronoi();
    this->setEnvironmentMap(environmentMap);
    this->setLightType(lightType);
    this->setNumberOfOffsets(numberOfOffsets);
    this->setNumberOfLightingConditions(numberOfLightingConditions);
    this->setExposure(exposure);
    this->setIdentificationMethod(identificationMethod);
    this->setSaveVoronoiDiagram(save);
}

/**
 * Restart the relighting by reinitialising all the variables.
 * @brief clearRelighting
 */
void FreeFormLightStage::clearRelighting()
{
    m_voronoi->clearVoronoi();
    m_object = QString("");
    m_environmentMapName = QString("");
    m_lightType = QString("");
    m_numberOfOffsets = 0;
    m_numberOfLightingConditions = 1;
    m_exposure = 0.0;

    //Environment Map parameters. by default 1024x512
    m_environmentMapWidth = 1024;
    m_environmentMapHeight = 512;
    m_numberOfComponents = 3;
}

/**
 * Virtual method to update the progress window.
 * @brief updateProgressWindow
 * @param updateText
 * @param progressBarValue
 */
void FreeFormLightStage::updateProgressWindow(QString updateText, int progressBarValue)
{
    emit statusUpdate(updateText);
    emit updateProgressBar(progressBarValue);
    qApp->processEvents(); //Refresh the main window to process the signals
}
