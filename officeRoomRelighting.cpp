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
 * \file officeroomrelighting.h
 * \brief Class to compute the office room relighting.
 * \author Antoine Toisoul Le Cann
 * \date August, 7th, 2014
 *
 * Class that inherites of the Relighting class. Computes the office room relighting of an object.
 */

#include "officeRoomRelighting.h"

using namespace std;
using namespace cv;

/**
 * Constructor of the OfficeRoomRelighting class.
 * @brief LightStageRelighting
 */
OfficeRoomRelighting::OfficeRoomRelighting(): Relighting(), m_voronoi(new Voronoi()), m_roomType(string()), m_indirectLightPicture(4),
    m_identificationMethod(QString("Median Energy")), m_optimisationMethod(QString("Disabled")), m_numberOfSamplesInverseCDF(0), m_exposure(0)
{

}

/**
  * Destructor of the OfficeRoomRelighting class.
  */
OfficeRoomRelighting::~OfficeRoomRelighting()
{
    delete m_voronoi;
}

/**
 * Computes the relighting of the object using the office room method.
 * @brief relighting
 */
void OfficeRoomRelighting::relighting()
{
    //Sets the room and mask types
    this->setMaskAndRoomTypes();

    /*---Loads the EM---*/
    this->updateProgressWindow(QString("Relighting the " + m_object + " in " + m_environmentMapName), 0);
    this->loadEnvironmentMap();
    m_voronoi->setEnvironmentMapSize(m_environmentMapWidth, m_environmentMapHeight);

    //Remove indirect light and overlaps between the lights
    if(m_computeBasisMasks)
    {
        if(m_roomType == "bedroom"  || m_roomType == "bedroom45")
        {
            prepareBasis_bedroom();
        }
        else
        {
            prepareBasis_office();
        }

        prepareMasks();
    }

    //Load the reflectance field
    this->loadReflectanceField();

    if(m_roomType == "bedroom" || m_roomType == "bedroom45")
    {
        this->prepareReflectanceField_bedroom();
    }
    else
    {
        this->prepareReflectanceField_office();
    }

    this->updateProgressWindow(QString("Images loaded"), 25);

    //Mouse parameters used to identified the lights manually
    MouseParameters mouseParameters;
    mouseParameters.voronoi = m_voronoi;
    mouseParameters.windowName = "Lighting condition";
    mouseParameters.latLongWidth = m_environmentMapWidth;
    mouseParameters.latLongHeight = m_environmentMapHeight;
    mouseParameters.isPressed = false;
    mouseParameters.numberOfLightSourcesAdded = 0;

    if(m_identificationMethod == "Manual") //Lights are identified manually
    {
        this->identifyLightsUser(mouseParameters);      
        this->updateProgressWindow(QString("Voronoi diagram generated"), 50);
    }
    else if(m_identificationMethod == "Inverse CDF")//Lights are identified automatically
    {
        this->identifyLightsAutomatically();
        this->updateProgressWindow(QString("Voronoi diagram generated"), 50);
    }
    else if(m_identificationMethod == "Median Energy")//Lights are identified automatically
    {
        this->identifyMedianEnergy();
        this->updateProgressWindow(QString("Voronoi diagram generated"), 50);
    }

    //Prepare for optimisation
    //It is a convex optimisation. Any initialisation point should converge to the same minimum
    double* startingPointArray = new double[m_numberOfLightingConditions];
    for(unsigned int n = 0 ; n<m_numberOfLightingConditions ; n++)
    {
        startingPointArray[n] = 1.0;
    }

    //Offsets
    int progressBarValue = 50;
    float offset = 0.0;
    for(unsigned int l = 0 ; l<m_numberOfOffsets ; l++)
    {
        offset = (float) 2.0*l*M_PI/m_numberOfOffsets;

        //Compute the weights of the voronoi diagram
        if(m_lightType.toStdString() == "Gaussian")
        {
            m_voronoi->clearWeights(); //Reinitialise the weights
            //m_voronoi->computeVoronoiWeightsGaussianOR(environmentMapHDR, offset);
            m_weightsRGB = m_voronoi->getRGBWeights();
        }
        else if(m_lightType.toStdString() == "Point")
        {
            if(m_identificationMethod == "Masks")//If the masks are used, the voronoi diagram is not needed
            {
                m_weightsRGB = this->computeWeightsMasks(m_environmentMap, offset);
            }
            else
            {
                m_voronoi->clearWeights(); //Reinitialise the weights
                m_voronoi->computeVoronoiWeightsOR(m_environmentMap, offset);
                m_weightsRGB = m_voronoi->getRGBWeights();
            }
        }

        progressBarValue += 25/(m_numberOfOffsets);
        this->updateProgressWindow(QString("Weights computed"), progressBarValue);

        //Optimisation process
        if(m_optimisationMethod == "Original Space")
        {
            this->updateProgressWindow(QString("Starting optimisation in original space"), progressBarValue);

            bool weightsExist = this->weightsTableOptimisation(l); //l = offset number

            if(!weightsExist)
            {
                Optimisation optimisation(m_environmentMapName.toStdString(), m_environmentMapWidth, m_environmentMapHeight, m_numberOfComponents,
                                     m_numberOfLightingConditions, m_indirectLightPicture, offset, m_roomType, m_masksType.toStdString(),m_weightsRGB);
                optimisation.environmentMapOptimisation(startingPointArray);
                m_weightsRGB = optimisation.getRGBWeights();
            }

            this->updateProgressWindow(QString("Optimisation done"), progressBarValue);
        }
        else if(m_optimisationMethod == "PCA Space")
        {
            this->updateProgressWindow(QString("Starting optimisation in PCA space"), progressBarValue);

            Optimisation optimisation(m_environmentMapName.toStdString(), m_environmentMapWidth, m_environmentMapHeight, m_numberOfComponents,
                                      m_numberOfLightingConditions, m_indirectLightPicture, offset, m_roomType, m_masksType.toStdString(), m_weightsRGB);
            optimisation.environmentMapPCAOptimisation(startingPointArray);
            m_weightsRGB = optimisation.getRGBWeights();

            this->updateProgressWindow(QString("Optimisation done"), progressBarValue);
        }

        //Normalize the weights
        normalizeWeightsRGB(m_weightsRGB);

        if(m_identificationMethod == "Manual")
        {
           this->saveVoronoiWeights(l);
        }

        //Calculate the result
        this->computeFinalRelighting();
        this->changeExposure(m_exposure);
        this->rayTraceBackground(offset+M_PI, true, 2.2); //Apply gamma only on background as HDR is used

        ostringstream osstream;
        osstream << this->getFolderPath() << "/Results/office_room/" << m_object.toStdString() << "_" << m_lightType.toStdString() << "_" << m_environmentMapName.toStdString() << "_offset" << l << ".jpg";

        this->saveResult(SAVE_8BITS, osstream.str());

        emit updateImage(QString(osstream.str().c_str()));
        osstream.str("");

        osstream << "Result " << l << " done";
        progressBarValue += 25/(m_numberOfOffsets);
        this->updateProgressWindow(QString(osstream.str().c_str()), progressBarValue);
        osstream.str("");
    }

    this->updateProgressWindow(QString("Done"), 100);

    delete[] startingPointArray;
}

/**
 * Virtual pure function.
 * Loads the reflectance field of the object.
 * @brief loadReflectanceField
 * @return
 */
bool OfficeRoomRelighting::loadReflectanceField()
{
    //Only allocate memory if it has not been already allocated
    if(m_reflectanceField == NULL)
        m_reflectanceField = new Mat[m_numberOfLightingConditions];

    string file;
    string extension;

    if(m_object == "Egg")
    {
        file = string("office_room/EggCropped_");
        extension = string(".TIF");
    }
    else if(m_object == "Bird")
    {
        file = string("office_room/BirdCropped_");
        extension = string(".TIF");

    }
    else if(m_object == "Egg_bedroom")
    {
        file = string("office_room/Egg_bedroom");
        extension = string(".TIF");
    }
    else if(m_object == "Bird_bedroom")
    {
        file = string("office_room/Bird_bedroom");
        extension = string(".pfm");
    }
    else if(m_object == "Egg_bedroom45")
    {
        file = string("office_room/Egg_bedroom45_");
        extension = string(".TIF");
    }
    else if(m_object == "Bird_bedroom45")
    {
        file = string("office_room/Bird_bedroom45_");
        extension = string(".TIF");
    }
    else if(m_object == "PlantOR")
    {
        file = string("office_room/plantOR_");
        extension = string(".png");
    }
    else if(m_object == "HelmetOR")
    {
        file = string("office_room/helmetOR_");
        extension = string(".png");
    }
    else
    {
        cerr << "Object does not exist : " << m_object.toStdString() << endl;
        return EXIT_FAILURE;
    }

    ostringstream osstream;

    //Load the files
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

       if(extension == ".pfm")
       {
           m_reflectanceField[i] = loadPFM(osstream.str());

           if(!m_reflectanceField[i].data)
           {
              cerr << "Couldn't open the file : " << osstream.str() << endl;
              return EXIT_FAILURE;
           }

           m_reflectanceField[i].convertTo(m_reflectanceField[i], CV_32FC3, 1.0/65535.0);
       }
       else
       {
           //For 16 bits TIF HDR images, use CV_LOAD_IMAGE_ANYDEPTH to load the image correctly (with values between 0 and 65535)
           m_reflectanceField[i] = imread(osstream.str(), CV_LOAD_IMAGE_ANYDEPTH | CV_LOAD_IMAGE_COLOR);

           if(!m_reflectanceField[i].data)
           {
              cerr << "Couldn't open the file : " << osstream.str() << endl;
              return EXIT_FAILURE;
           }

           m_reflectanceField[i].convertTo(m_reflectanceField[i], CV_32FC3, 1.0/65535.0);
       }

       osstream.str("");
    }

    //Load the mask
    osstream << this->getFolderPath() << "/images/office_room/" << m_object.toStdString() << "_mask.png";

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
 * Function that stores optimisation weights for a given environment map and offset.
 * Scaling factors computed with 1024x512 environment maps.
 * @brief weightsTableOptimisation
 * @param offset
 * @return
 */
bool OfficeRoomRelighting::weightsTableOptimisation(int offset)
{
    bool found = false;
    float *scalingFactors = new float[m_numberOfLightingConditions];

    if(m_environmentMapName == "grace_latlong")
    {

        if(m_roomType == "office" && offset == 0)
        {
            // Grace cathedral scaling factors office
            scalingFactors[0] = 0.0000502968;
            scalingFactors[1] = 0.0000492904;
            scalingFactors[2] = 0.000056814;
            scalingFactors[3] = 0.0000508221;
            scalingFactors[4] = 0.0000169376;
            scalingFactors[5] = 0.0000159368;
            scalingFactors[6] = 0.0000235687;
            scalingFactors[7] = 0.0000201205;
            scalingFactors[8] = 0.0000205601;
            found = true;
        }
        else if(m_roomType == "bedroom" && offset == 0)
        {

            scalingFactors[0] = 2.40093e-006;
            scalingFactors[1] = 5.73275e-005;
            scalingFactors[2] = 0.000143763;
            scalingFactors[3] = 0.000472368;
            scalingFactors[4] = 0.000101153;
            scalingFactors[5] = 0.000346131;
            scalingFactors[6] = 0.000308545;
            scalingFactors[7] = 5.82419e-005;
            scalingFactors[8] = 6.04429e-005;
            scalingFactors[9] = 6.97165e-005;
            scalingFactors[10] = 0.000100591;
            scalingFactors[11] = 0.000888963;
            found = true;
        }
        else if(m_roomType == "bedroom45" && offset == 0)
        {
            scalingFactors[0] = 2.13515e-006;
            scalingFactors[1] = 0.000126759;
            scalingFactors[2] = 0.000101177;
            scalingFactors[3] = 0.00761379;
            scalingFactors[4] = 0.00170077;
            scalingFactors[5] = 0.00127038;
            scalingFactors[6] = 0.00110342;
            scalingFactors[7] = 0.000431537;
            scalingFactors[8] = 5.83877e-005;
            scalingFactors[9] = 0.000120979;
            scalingFactors[10] = 0.000280371;
            scalingFactors[11] = 0.000124229;
            found = true;
        }

    }
    else if(m_environmentMapName == "stpeters_probe")
    {

        // St Peters scaling factors bedroom
        if(m_roomType == "bedroom45" && offset == 0)
        {
            scalingFactors[0] = 2.15422e-006;
            scalingFactors[1] = 0.000110989;
            scalingFactors[2] = 9.67531e-005;
            scalingFactors[3] = 0.000998374;
            scalingFactors[4] = 0.000508237;
            scalingFactors[5] = 0.0010361;
            scalingFactors[6] = 0.000199304;
            scalingFactors[7] = 0.000112372;
            scalingFactors[8] = 9.40383e-005;
            scalingFactors[9] = 0.0001952;
            scalingFactors[10] = 0.00017135;
            scalingFactors[11] = 0.000178673;
            found = true;
        }
    }
    else if(m_environmentMapName == "uffizi")
    {
        if(m_roomType == "bedroom45" && offset == 0)
        {
            scalingFactors[0] = 2.14806e-006;
            scalingFactors[1] = 0.000137978;
            scalingFactors[2] = 7.80083e-005;
            scalingFactors[3] = 0.00270032;
            scalingFactors[4] = 0.00145579;
            scalingFactors[5] = 0.0114177;
            scalingFactors[6] = 0.001374;
            scalingFactors[7] = 0.00015886;
            scalingFactors[8] = 9.51125e-005;
            scalingFactors[9] = 0.000194578;
            scalingFactors[10] = 0.00028165;
            scalingFactors[11] = 0.000326;
            found = true;
        }


    }
    else if(m_environmentMapName == "eucalyptus_grove")
    {
        if(m_roomType == "bedroom45" && offset == 0)
        {
            scalingFactors[0] = 2.14263e-006;
            scalingFactors[1] = 0.000133766;
            scalingFactors[2] = 0.000114081;
            scalingFactors[3] = 0.000555755;
            scalingFactors[4] = 0.000682996;
            scalingFactors[5] = 0.00118779;
            scalingFactors[6] = 0.00388733;
            scalingFactors[7] = 9.45236e-005;
            scalingFactors[8] = 9.68602e-005;
            scalingFactors[9] = 0.0001993;
            scalingFactors[10] = 0.000129835;
            scalingFactors[11] = 0.00018034;
            found = true;
        }
    }
    else if(m_environmentMapName == "pisa_courtyard")
    {
        if(m_roomType == "bedroom45" && offset == 0)
        {
            scalingFactors[0] = 2.14388e-006;
            scalingFactors[1] = 0.000109126;
            scalingFactors[2] = 0.000110366;
            scalingFactors[3] = 0.000614845;
            scalingFactors[4] = 0.000744723;
            scalingFactors[5] = 0.0010236;
            scalingFactors[6] = 0.000664646;
            scalingFactors[7] = 0.000107445;
            scalingFactors[8] = 0.000110655;
            scalingFactors[9] = 0.000201834;
            scalingFactors[10] = 0.000181071;
            scalingFactors[11] = 0.00018569;
            found = true;
        }
        else if(m_roomType == "bedroom45" && offset == 1)
        {
            scalingFactors[0] = 2.15585e-006;
            scalingFactors[1] = 0.000109048;
            scalingFactors[2] = 0.000110205;
            scalingFactors[3] = 0.000603626;
            scalingFactors[4] = 0.000743255;
            scalingFactors[5] = 0.00102554;
            scalingFactors[6] = 0.000660404;
            scalingFactors[7] = 0.000107443;
            scalingFactors[8] = 0.000110609;
            scalingFactors[9] = 0.000201807;
            scalingFactors[10] = 0.000181097;
            scalingFactors[11] = 0.000185565;
            found = true;
        }
        else if(m_roomType == "bedroom45" && offset == 2)
        {
            scalingFactors[0] = 2.1557e-006;
            scalingFactors[1] = 0.000110102;
            scalingFactors[2] = 0.000102752;
            scalingFactors[3] = 0.000641798;
            scalingFactors[4] = 0.000753351;
            scalingFactors[5] = 0.000926588;
            scalingFactors[6] = 0.000556786;
            scalingFactors[7] = 0.000108266;
            scalingFactors[8] = 9.92855e-005;
            scalingFactors[9] = 0.000195801;
            scalingFactors[10] = 0.000182268;
            scalingFactors[11] = 0.000181242;
            found = true;
        }
        else if(m_roomType == "bedroom45" && offset == 3)
        {
            scalingFactors[0] = 2.16667e-006;
            scalingFactors[1] = 0.000110258;
            scalingFactors[2] = 0.000102754;
            scalingFactors[3] = 0.000641814;
            scalingFactors[4] = 0.000751065;
            scalingFactors[5] = 0.000922956;
            scalingFactors[6] = 0.00056237;
            scalingFactors[7] = 0.000108262;
            scalingFactors[8] = 9.92736e-005;
            scalingFactors[9] = 0.000195387;
            scalingFactors[10] = 0.000181666;
            scalingFactors[11] = 0.000181573;
            found = true;
        }
    }

    //Apply the factors
    if(found)
    {
        for(unsigned int k = 0 ; k<m_weightsRGB.size() ; k++)
        {
            for(unsigned int l = 0 ; l<m_weightsRGB[l].size() ; l++)
            {
                 m_weightsRGB[k][l] *= scalingFactors[k];
            }
        }
    }

    delete[] scalingFactors;

    return found;
}

/**
 * Saves the Voronoi diagram with each cell painted as its corresponding RGB weight.
 * @brief saveVoronoiWeights
 * @param offset
 */
void OfficeRoomRelighting::saveVoronoiWeights(int offset)
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
    osstream << this->getFolderPath() << "/Results/office_room/voronoi_diagram_avg_" << m_lightType.toStdString() << "_" << m_environmentMapName.toStdString() << "_offset" << offset << ".jpg";
    imwrite(osstream.str(), EMWeights);
}

/**
 * Method to identify the incoming light directions using the inverse CDF for environment map sampling combined with the k means method.
 * @brief identifyLightsAutomatically
 */
void OfficeRoomRelighting::identifyLightsAutomatically()
{
    ostringstream osstream;
    int cellNumber = 0;
    int numberOfClusters = 1;
    std::vector<std::vector<int> > cellNumberPerPicture;
    Mat lightingCondition;

    //Find the light sources for each of the lighting conditions
    for(unsigned int i = 0 ; i<m_numberOfLightingConditions ; i++)
    {
        osstream << this->getFolderPath();
        //Load the pfm environment map i containing the direct lighting only (for calculation)
        if(i != m_indirectLightPicture)
        {
            if(i<10)
            {
                osstream << "/lighting_conditions/office_room/" << m_roomType << "/directLight0" << i << ".pfm";
            }
            else
            {
                osstream << "/lighting_conditions/office_room/" << m_roomType << "/directLight" << i << ".pfm";
            }
        }
        else
        {
            if(i<10)
            {
                osstream << "/lighting_conditions/office_room/" << m_roomType << "/directLight0" << m_indirectLightPicture << ".pfm";
            }
            else
            {
                osstream << "/lighting_conditions/office_room/" << m_roomType << "/directLight" << m_indirectLightPicture << ".pfm";
            }
        }

        lightingCondition = loadPFM(osstream.str());

        if(!lightingCondition.data)
        {
            cerr << "Could not load : " << osstream.str() << endl;
        }

        osstream.str("");

        //Load the ppm environment map i containing the direct lighting only (to display the result)
        osstream << this->getFolderPath();
        osstream << "/lighting_conditions/office_room/" << m_roomType << "/condition0" << i << ".ppm";

        Mat environmentMap = imread(osstream.str(), CV_LOAD_IMAGE_COLOR);

        if(!environmentMap.data)
        {
            cerr << "Could not load : " << osstream.str() << endl;
        }

        osstream.str("");

        //tmpBasis is only used to show which lights have been identified in which latitude longitude map afterwards
        LightingBasis* tmpBasis = new LightingBasis();

        Rect samplesLocationRect(0,0,2, m_numberOfSamplesInverseCDF);
        Mat samplesLocation(samplesLocationRect.size(), CV_32F);

        //A slight gaussian blur may improve the result
        //gaussianBlur2D(lightingCondition, m_environmentMapWidth, m_environmentMapHeight,m_numberOfComponents, 5, lightingCondition);

        //Inverse CDF algorithm to find the point of high energy in the environment map
        inverseCDFAlgorithm(lightingCondition, m_environmentMapWidth, m_environmentMapHeight, m_numberOfComponents, m_numberOfSamplesInverseCDF, samplesLocation);

        //Paint the samples (points of high energy) found in the environment map
        paintSamples(lightingCondition, m_environmentMapWidth, m_environmentMapHeight, samplesLocation);

        if(i == 5 || i == 6)
        {
            numberOfClusters = 2;
        }
        else
        {
            numberOfClusters = 1;
        }

        Mat labels; //Matrix that contains which point belongs to which cluster
        int numberOfAttempts = 5; //number of attempts (with different centroids as an initialisation)
        Mat centers; //Centers of the clusters
        int maxIteration = 10000;
        float epsilon = 0.0001;

        //k-means algorithm : create clusters with the points
        //TermCriteria contains the criterion that will stop the k means algorithm
        //Number max of iterations 10000
        kmeans(samplesLocation, numberOfClusters, labels, TermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS, maxIteration, epsilon), numberOfAttempts, KMEANS_PP_CENTERS, centers);
        std::vector<int> cellsForImagei;

        //Add the center of the clusters to the lighting basis
        for(int k = 0 ; k<centers.rows ; k++)
        {
            if(centers.at<float>(k,0)<511 && centers.at<float>(k,1)<1023)
            {
                tmpBasis->addPointLight(Point2i(centers.at<float>(k,1), centers.at<float>(k,0)));
                m_voronoi->addPointLight(Point2i(centers.at<float>(k,1), centers.at<float>(k,0)));
            }
            //Increase the number of the cell depending on the number of clusters returned by the k means algorithm.
            //cell numbers denoted from 0 to the number of points in the voronoi diagram-1
            cellsForImagei.push_back(cellNumber);
            cellNumber++;
        }

        cellNumberPerPicture.push_back(cellsForImagei);
        environmentMap.convertTo(environmentMap,CV_32FC3);
        environmentMap /= 255.0;
        gammaCorrectionImage(environmentMap,environmentMap,1.8);
        environmentMap *=255;
        environmentMap.convertTo(environmentMap,CV_8UC3);
        //tmpBasis is only used to show which lights have been identified in which latitude longitude map
        tmpBasis->paintPointLights(environmentMap);
        osstream.str("");

        osstream << this->getFolderPath();
        osstream << "/Results/office_room/Result0" << i << ".jpg";

        imwrite(osstream.str(), environmentMap);
        cout << "Condition " << i << " done" << endl;

        delete tmpBasis;
        osstream.str("");
    }

    m_voronoi->setCellNumberPerPicture(cellNumberPerPicture);
}


/**
 * Method that chooses the centroid of the energy as the incoming light direction for each lighting condition.
 * @brief identifyMedianEnergy
 */
void OfficeRoomRelighting::identifyMedianEnergy()
{
    ostringstream osstream;
    Mat lightingCondition;

    float sumEnergy = 0.0;
    float totalEnergy = 0.0;
    float medianEnergy = 0;
    bool isAlreadyGreater = false;
    float R = 0.0, G = 0.0, B = 0.0;

    std::vector<std::vector<int> > cellNumberPerPicture;
    int cellNumber = 0;

    for(unsigned int k = 0 ; k<m_numberOfLightingConditions ; k++)
    {
        LightingBasis* tmpBasis = new LightingBasis();
        std::vector<int> cellsForImagek;

        osstream << this->getFolderPath() << "/lighting_conditions/office_room/" << m_roomType << "/directLight0" << k << ".pfm";
        lightingCondition = loadPFM(osstream.str());
        osstream.str("");

        osstream << this->getFolderPath() << "/lighting_conditions/office_room/" << m_roomType << "/condition0" << k << ".ppm";
        Mat lightingConditionToSave = imread(osstream.str(), CV_LOAD_IMAGE_COLOR);
        osstream.str("");

        for(unsigned int i = 0 ; i<m_environmentMapHeight ; i++)
        {
            for(unsigned int j = 0 ; j<m_environmentMapWidth ; j++)
            {
                R = lightingCondition.at<Vec3f>(i,j).val[2];
                G = lightingCondition.at<Vec3f>(i,j).val[1];
                B = lightingCondition.at<Vec3f>(i,j).val[0];
                totalEnergy += (R+G+B)/3.0;
            }
        }

        medianEnergy = totalEnergy/2.0;

        for(unsigned int i = 0 ; i<m_environmentMapHeight ; i++)
        {
            for(unsigned int j = 0 ; j<m_environmentMapWidth ; j++)
            {
                R = lightingCondition.at<Vec3f>(i,j).val[2];
                G = lightingCondition.at<Vec3f>(i,j).val[1];
                B = lightingCondition.at<Vec3f>(i,j).val[0];

                sumEnergy += (R+G+B)/3.0;

                if( (sumEnergy > medianEnergy) && !isAlreadyGreater)
                {
                    m_voronoi->addPointLight(Point2i(j,i));
                    tmpBasis->addPointLight(Point2i(j,i));
                    isAlreadyGreater = true;
                }
            }
        }
        cellsForImagek.push_back(cellNumber);
        cellNumber++;
        cellNumberPerPicture.push_back(cellsForImagek);
        tmpBasis->paintPointLights(lightingConditionToSave);

        osstream << this->getFolderPath() << "/Results/office_room/Centroid " << k << ".png";
        imwrite(osstream.str(), lightingConditionToSave);
        osstream.str("");

       // imshow("Centroid", lightingConditionToSave);
       // waitKey(0);
        delete tmpBasis;
        sumEnergy = 0.0;
        medianEnergy = 0.0;
        totalEnergy = 0.0;
        isAlreadyGreater = false;
    }

    destroyWindow("Centroid"); //Destroys the window
    m_voronoi->setCellNumberPerPicture(cellNumberPerPicture);
}

/**
 * Method to manually select the incoming light directions in the environment map of the office.
 * @brief identifyLightsUser
 * @param INPUT : mouseParameters parameters that are sent to the callback function to manually select light sources.
 */
void OfficeRoomRelighting::identifyLightsUser(MouseParameters& mouseParameters)
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
        if(i<10)
            osstream << this->getFolderPath() << "/lighting_conditions/office_room/" << m_roomType << "/condition0" << i << ".pfm";
        else
            osstream << this->getFolderPath() << "/lighting_conditions/office_room/" << m_roomType << "/condition" << i << ".pfm";

        lightingCondition = loadPFM(osstream.str());

        if(!lightingCondition.data)
        {
            cerr << "Could not load : " << osstream.str() << endl;
        }

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
 * Method that prepares the basis of the office room before the computation. It removes the indirect lighting and the overlaps.
 * @brief prepareBasis_office
 */
void OfficeRoomRelighting::prepareBasis_office()
{
    ostringstream osstream, osstream2;
    Mat lightingCondition;

    float* globalScalingFactorMirrorBall = new float[m_numberOfLightingConditions];

    //Global scaling factor to meet the shutter speed reference of 1/15
    globalScalingFactorMirrorBall[0] = 2.0;
    globalScalingFactorMirrorBall[1] = pow(2.0, (double) +5/3);
    globalScalingFactorMirrorBall[2] = pow(2.0, (double) -2/3);
    globalScalingFactorMirrorBall[3] = pow(2.0, (double) -1/3);
    globalScalingFactorMirrorBall[4] = pow(2.0, (double) -7/3);
    globalScalingFactorMirrorBall[5] = pow(2.0, (double) 0);
    globalScalingFactorMirrorBall[6] = pow(2.0, (double) -1/3);
    globalScalingFactorMirrorBall[7] = pow(2.0, (double) -2);
    globalScalingFactorMirrorBall[8] = pow(2.0, (double) -7/3);

    //First loop change the exposure of each lighting condition
    //First exposure is taken as a reference
    for(unsigned int i = 0 ; i<m_numberOfLightingConditions ; i++)
    {
        if(i<10)
        {
            osstream << this->getFolderPath() << "/lighting_conditions/office_room/" << m_roomType << "/condition0" << i << ".pfm";
        }
        else
        {
            osstream << this->getFolderPath() << "/lighting_conditions/office_room/" << m_roomType << "/condition" << i << ".pfm";
        }

        lightingCondition = loadPFM(osstream.str());

        if(!lightingCondition.data)
        {
            cerr << "Could not load " << osstream.str() << endl;
        }

        lightingCondition *= globalScalingFactorMirrorBall[i];
        osstream.str("");

        if(i<10)
        {
            osstream << this->getFolderPath() << "/lighting_conditions/office_room/" << m_roomType << "/directLight0" << i << ".pfm";
        }
        else
        {
            osstream << this->getFolderPath() << "/lighting_conditions/office_room/" << m_roomType << "/directLight" << i << ".pfm";
        }

        savePFM(lightingCondition, osstream.str());
        osstream.str("");
    }

    //Load the dark room
    osstream << this->getFolderPath() << "/lighting_conditions/office_room/" << m_roomType << "/directLight0" << m_indirectLightPicture << ".pfm";
    Mat darkRoom = loadPFM(osstream.str());
    osstream.str("");

    //Second loop remove indirect light using the picture of the dark room
    for(unsigned int i = 0 ; i<m_numberOfLightingConditions ; i++)
    {
        osstream << this->getFolderPath() << "/lighting_conditions/office_room/" << m_roomType << "/directLight0" << i << ".pfm";
        lightingCondition = loadPFM(osstream.str());

        if(i != m_indirectLightPicture)
        {
            lightingCondition -= darkRoom;
            osstream.str("");

            osstream << this->getFolderPath() << "/lighting_conditions/office_room/" << m_roomType << "/directLight0" << i << ".pfm";
            savePFM(lightingCondition, osstream.str());
        }

        osstream.str("");
        osstream2.str("");
    }

    //Avoid overlap in the lighting basis by removing a part of the window
    //First window
    osstream << this->getFolderPath() << "/lighting_conditions/office_room/" << m_roomType << "/directLight00.pfm";
    osstream2 << this->getFolderPath() << "/lighting_conditions/office_room/" << m_roomType << "/directLight01.pfm";

    Mat windowFullOpenedPFM = loadPFM(osstream.str());
    Mat windowHalfOpenedPFM = loadPFM(osstream2.str());

    windowFullOpenedPFM -= windowHalfOpenedPFM;
    osstream.str("");

    //SavePFM First window
    osstream << this->getFolderPath() << "/lighting_conditions/office_room/" << m_roomType << "/directLight00.pfm";
    savePFM(windowFullOpenedPFM, osstream.str().c_str());

    //Second window
    osstream.str("");
    osstream2.str("");

    osstream << this->getFolderPath() << "/lighting_conditions/office_room/" << m_roomType << "/directLight02.pfm";
    osstream2 << this->getFolderPath()<< "/lighting_conditions/office_room/" << m_roomType << "/directLight03.pfm";

    windowFullOpenedPFM = loadPFM(osstream.str());
    windowHalfOpenedPFM = loadPFM(osstream2.str());

    windowFullOpenedPFM -= windowHalfOpenedPFM;

    osstream.str("");

    //SavePFM second window
    osstream << this->getFolderPath() << "/lighting_conditions/office_room/" << m_roomType << "/directLight02.pfm";
    savePFM(windowFullOpenedPFM, osstream.str());

    delete[] globalScalingFactorMirrorBall;
}

/**
 * Method that prepares the basis of the bedroom before the computation. It removes the indirect lighting and the overlaps.
 * @brief prepareBasis_office
 */
void OfficeRoomRelighting::prepareBasis_bedroom()
{
    ostringstream osstream;
    Mat *lightingConditions = new Mat[m_numberOfLightingConditions];
    Mat darkRoom;

    //Load the dark room
    osstream << this->getFolderPath() << "/lighting_conditions/office_room/" << m_roomType << "/condition0" << m_indirectLightPicture << ".pfm";

    darkRoom = loadPFM(osstream.str());
    osstream.str("");

    //First remove the indirect light (dark room)
    for(unsigned int i = 0 ; i<m_numberOfLightingConditions ; i++)
    {
         osstream << this->getFolderPath();

        if(i<10)
        {
            osstream << "/lighting_conditions/office_room/" << m_roomType << "/condition0" << i << ".pfm";
        }
        else
        {
            osstream << "/lighting_conditions/office_room/" << m_roomType << "/condition" << i << ".pfm";
        }

        lightingConditions[i] = loadPFM(osstream.str());

        if(!lightingConditions[i].data)
        {
            cerr << "Could not open the lighting condition : " << osstream.str() << endl;
        }

        osstream.str("");

        if(i != m_indirectLightPicture)
        {
            lightingConditions[i] -= darkRoom;
        }
    }

    //Second avoid overlap in the lighting basis by removing a half of the full opened windows
    for(unsigned int i = 1 ; i<m_numberOfLightingConditions-1 ; i+=2)
    {
        //Full window - half window
        lightingConditions[i] = lightingConditions[i]-lightingConditions[i+1];
    }

    //Set negative values to 0.0
    for(unsigned int k = 0 ; k<m_numberOfLightingConditions ; k++)
    {
        int width = lightingConditions[k].cols;
        int height = lightingConditions[k].rows;

        for(int i = 0 ; i<height ; i++)
        {
            for(int j = 0 ; j<width ; j++)
            {
                if(lightingConditions[k].at<Vec3f>(i,j).val[0]<0)
                    lightingConditions[k].at<Vec3f>(i,j).val[0] = 0.0;

                if(lightingConditions[k].at<Vec3f>(i,j).val[1]<0)
                    lightingConditions[k].at<Vec3f>(i,j).val[1] = 0.0;

                if(lightingConditions[k].at<Vec3f>(i,j).val[2]<0)
                    lightingConditions[k].at<Vec3f>(i,j).val[2] = 0.0;
            }
        }

        osstream << this->getFolderPath();

        if(k<10)
        {
            osstream << "/lighting_conditions/office_room/" << m_roomType << "/directLight0" << k << ".pfm";
        }
        else
        {
            osstream << "/lighting_conditions/office_room/" << m_roomType << "/directLight" << k << ".pfm";
        }

        savePFM(lightingConditions[k], osstream.str());

        osstream.str("");
    }

    delete[] lightingConditions;
}

/**
 * Method that computes the residual mask associated with the picture of the dark room.
 * @brief prepareMasks
 */
void OfficeRoomRelighting::prepareMasks()
{

    ostringstream osstream;
    Mat residualMask, residualMaskNot;
    Mat currentMask;
    Mat resultNotAnotB, resultAnotB, resultAB, intermediateResult, result;

    Mat allMasksNot;
    Mat allMasks(Rect(0,0,m_environmentMapWidth, m_environmentMapHeight).size(), CV_8UC3);
    allMasks = 255.0*Scalar::all(1.0);
    allMasks.convertTo(allMasks, CV_8UC3);

    osstream << this->getFolderPath() << "/lighting_conditions/office_room/" << m_roomType << "/" << m_masksType.toStdString() << "/condition_mask0" << m_indirectLightPicture << ".png";

    residualMask = imread(osstream.str(), CV_LOAD_IMAGE_COLOR);
    bitwise_not(residualMask, residualMaskNot);
    osstream.str("");

    //First step assemble all the masks into one mask
    for(unsigned int i = 0 ; i<m_numberOfLightingConditions ; i++)
    {

        if(i != m_indirectLightPicture)
        {

            osstream << this->getFolderPath();

            if(i<10)
                osstream << "/lighting_conditions/office_room/" << m_roomType << "/" << m_masksType.toStdString() << "/condition_mask0" << i << ".png";
            else
                osstream << "/lighting_conditions/office_room/" << m_roomType << "/" << m_masksType.toStdString() << "/condition_mask" << i << ".png";

            currentMask = imread(osstream.str(), CV_LOAD_IMAGE_COLOR);

            osstream.str("");
            bitwise_and(allMasks, currentMask, allMasks);

            osstream << this->getFolderPath() << "/lighting_conditions/office_room/" << m_roomType << "/" << m_masksType.toStdString() << "/allMasks.png";

            imwrite(osstream.str(), allMasks);
            osstream.str("");
        }
    }

    //Second step compute the result
    //Residual = black(0) Mask=black(0) result is white 1
    //Residual = white(1) Mask=black(0) result is white 1
    //Residual = black(0) Mask=white(1) result is black 0
    //Residual = white(1) Mask=white(1) result is white 1
    //Hence the logic formula is AND(notA,notB)+AND(A,notB)+AND(A,B)
    bitwise_not(allMasks, allMasksNot);
    bitwise_and(residualMaskNot, allMasksNot, resultNotAnotB);
    bitwise_and(residualMask, allMasksNot, resultAnotB);
    bitwise_and(residualMask, allMasks, resultAB);
    bitwise_or(resultNotAnotB, resultAnotB, intermediateResult);
    bitwise_or(intermediateResult, resultAB, result);

    osstream << this->getFolderPath() << "/lighting_conditions/office_room/" << m_roomType << "/" << m_masksType.toStdString() << "/residualMask.png";

    imwrite(osstream.str(), result);
}

/**
 * Method to prepare the Reflectance field before the relighting computation (office room).
 * @brief prepareReflectanceField_office
 */
void OfficeRoomRelighting::prepareReflectanceField_office()
{
    //Set the global scaling factors for the chosen object
    float* globalScalingFactor = new float[m_numberOfLightingConditions];

    if(m_object == "Bird")
    {
        globalScalingFactor[0] = 1.0;
        globalScalingFactor[1] = 1.0;
        globalScalingFactor[2] = pow(2.0, (double) -2/3);
        globalScalingFactor[3] = pow(2.0, (double) -5/3);
        globalScalingFactor[4] = pow(2.0, (double) -4);
        globalScalingFactor[5] = pow(2.0, (double) -2);
        globalScalingFactor[6] = pow(2.0, (double) -2);
        globalScalingFactor[7] = pow(2.0, (double) -10/3);
        globalScalingFactor[8] = pow(2.0, (double) -10/3);
    }
    else if(m_object == "Egg")
    {
        globalScalingFactor[0] = 1.0;
        globalScalingFactor[1] = 1.0;
        globalScalingFactor[2] = pow(2.0, (double) -1/3);
        globalScalingFactor[3] = pow(2.0, (double) -5/3);
        globalScalingFactor[4] = pow(2.0, (double) -13/3);
        globalScalingFactor[5] = pow(2.0, (double) -3);
        globalScalingFactor[6] = pow(2.0, (double) -3);
        globalScalingFactor[7] = pow(2.0, (double) -14/3);
        globalScalingFactor[8] = pow(2.0, (double) -14/3);
    }
    else
    {
        for(unsigned int k = 0 ; k<m_numberOfLightingConditions ; ++k)
        {
            globalScalingFactor[k] = 1.0;
        }
    }

    //Multiply each picture of the reflectance field by its scaling factor
    for(unsigned int i = 0 ; i<m_numberOfLightingConditions ; i++)
    {
       m_reflectanceField[i] *= globalScalingFactor[i];
    }

    Mat channel[3], channel32F[3];

    //Apply house light scaling factors to pictures
    //Scaling factors for images 5 and 6
    float scalingFactorLightHouseRGB[3];

    scalingFactorLightHouseRGB[0] = 0.58696;
    scalingFactorLightHouseRGB[1] = 0.6471;
    scalingFactorLightHouseRGB[2] = 0.780822;

    for(int i = 5 ; i<=6 ; i++)
    {
        split(m_reflectanceField[i], channel);

        channel[0].convertTo(channel32F[0], CV_32F);
        channel[1].convertTo(channel32F[1], CV_32F);
        channel[2].convertTo(channel32F[2], CV_32F);

        //Channel32F are matrices. Each element is a float between 0 and 255
        channel32F[0] *= scalingFactorLightHouseRGB[2];
        channel32F[1] *= scalingFactorLightHouseRGB[1];
        channel32F[2] *= scalingFactorLightHouseRGB[0];

        channel32F[0].convertTo(channel32F[0], CV_32F);
        channel32F[1].convertTo(channel32F[1], CV_32F);
        channel32F[2].convertTo(channel32F[2], CV_32F);

        merge(channel32F,3,m_reflectanceField[i]);
   }

   for(unsigned int i = 0 ; i<m_numberOfLightingConditions ; i++)
   {
       if(i != m_indirectLightPicture)
           m_reflectanceField[i]-= m_reflectanceField[4];
   }

   m_reflectanceField[0]-= m_reflectanceField[1];
   m_reflectanceField[2]-= m_reflectanceField[3];

   //Set negative values to 0.0
   for(unsigned int k = 0 ; k<m_numberOfLightingConditions ; k++)
   {
       int width = m_reflectanceField[k].cols;
       int height = m_reflectanceField[k].rows;

       for(int i = 0 ; i<height ; i++)
       {
           for(int j = 0 ; j<width ; j++)
           {
               if(m_reflectanceField[k].at<Vec3f>(i,j).val[0]<0)
                   m_reflectanceField[k].at<Vec3f>(i,j).val[0] = 0.0;

               if(m_reflectanceField[k].at<Vec3f>(i,j).val[1]<0)
                   m_reflectanceField[k].at<Vec3f>(i,j).val[1] = 0.0;

               if(m_reflectanceField[k].at<Vec3f>(i,j).val[2]<0)
                   m_reflectanceField[k].at<Vec3f>(i,j).val[2] = 0.0;
           }
       }
   }

   delete[] globalScalingFactor;
}

/**
 * Method to prepare the Reflectance field before the relighting computation (bedroom).
 * @brief prepareReflectanceField_office
 */
void OfficeRoomRelighting::prepareReflectanceField_bedroom()
{
   //The indirect light picture has been stored with +3 stops
   if(m_object != "Bird_bedroom")
      m_reflectanceField[m_indirectLightPicture] *= pow(2.0,-3.0);

   Mat channel[3], channel32F[3];

   //Apply scaling factor for house lights (picture 11 reflectance field)
   float scalingFactorLightHouseRGB[3];

   scalingFactorLightHouseRGB[0] = 0.6549/0.7783;
   scalingFactorLightHouseRGB[1] = 0.7448/0.7153;
   scalingFactorLightHouseRGB[2] = 0.6739/0.5513;

   split(m_reflectanceField[11], channel);

   channel[0].convertTo(channel32F[0], CV_32F);
   channel[1].convertTo(channel32F[1], CV_32F);
   channel[2].convertTo(channel32F[2], CV_32F);

   //Channel32F are matrices. Each element is a float between 0 and 255
   channel32F[0] *= scalingFactorLightHouseRGB[2];
   channel32F[1] *= scalingFactorLightHouseRGB[1];
   channel32F[2] *= scalingFactorLightHouseRGB[0];

   channel32F[0].convertTo(channel32F[0], CV_32F);
   channel32F[1].convertTo(channel32F[1], CV_32F);
   channel32F[2].convertTo(channel32F[2], CV_32F);

   merge(channel32F,3,m_reflectanceField[11]);

   for(unsigned int i = 0 ; i<m_numberOfLightingConditions ; i++)
   {
        if(i != m_indirectLightPicture)
            m_reflectanceField[i]-= m_reflectanceField[m_indirectLightPicture];
   }

   for(unsigned int i = 1 ; i<m_numberOfLightingConditions-1 ; i+=2)
   {
        if(i != m_indirectLightPicture)
            m_reflectanceField[i] -= m_reflectanceField[i+1];
   }

    //Set negative values to 0.0
    for(unsigned int k = 0 ; k<m_numberOfLightingConditions ; k++)
    {
        int width = m_reflectanceField[k].cols;
        int height = m_reflectanceField[k].rows;

        for(int i = 0 ; i<height ; i++)
        {
            for(int j = 0 ; j<width ; j++)
            {
                if(m_reflectanceField[k].at<Vec3f>(i,j).val[0]<0)
                    m_reflectanceField[k].at<Vec3f>(i,j).val[0] = 0.0;

                if(m_reflectanceField[k].at<Vec3f>(i,j).val[1]<0)
                    m_reflectanceField[k].at<Vec3f>(i,j).val[1] = 0.0;

                if(m_reflectanceField[k].at<Vec3f>(i,j).val[2]<0)
                    m_reflectanceField[k].at<Vec3f>(i,j).val[2] = 0.0;
            }
        }
    }
}

/**
 * Normalize the energy of each picture in the reflectance field to 1.0.
 * @brief normalizeEnergyBasis
 * @param reflectanceField
 */
void OfficeRoomRelighting::normalizeEnergyBasis(Mat reflectanceField[])
{
    Mat currentMask;
    Mat currentLightingCondition;
    ostringstream osstream;
    float *energy = new float[m_numberOfLightingConditions];

    for(unsigned int i = 0 ; i<m_numberOfLightingConditions ; i++)
    {
        energy[i] = 0.0;
        if(i != m_indirectLightPicture)
        {

            osstream << this->getFolderPath() << "/lighting_conditions/office_room/" << m_roomType << "/" << m_masksType.toStdString() << "/condition_mask0" << i << ".png";
            currentMask = imread(osstream.str(), CV_LOAD_IMAGE_GRAYSCALE);
            osstream.str("");

            osstream << this->getFolderPath() << "/lighting_conditions/office_room/" << m_roomType << "/directLight0" << i << ".pfm";
            currentLightingCondition = loadPFM(osstream.str());

            int width = currentMask.cols;
            int height = currentMask.rows;

            for(int k = 0 ; k<height ; ++k)
            {
                for(int l = 0 ; l<width ; ++l)
                {
                    if(currentMask.at<uchar>(k,l)<127)
                    {
                        energy[i] += currentLightingCondition.at<float>(k,l);
                    }
                }
            }
            cout << "condition " << i << " - Energy " << energy[i] << endl;
            osstream.str("");
        }

    }

    //Normalize the energy to 1.0
    for(unsigned int i = 0 ; i<m_numberOfLightingConditions ; ++i)
    {
        if(i != m_indirectLightPicture)
           reflectanceField[i] *= 1.0/energy[i];
    }

    delete[] energy;
}

/**
 * Method to compute the weights using the masks.
 * @brief computeWeightsMasks
 * @param INPUT : environmentMap is an OpenCV Mat of floats containing the HDR values of the environment map.
 * @param INPUT : offset is a float corresponding to the offset that is added to the rotation of the environment map (phi angle).
 * @return the weights of each lighting condition as a vector<vector<float> >. vector[i] is a vector containing three values R, G, B that corresponds to the weights for each color channel.
 */
std::vector<std::vector<float> > OfficeRoomRelighting::computeWeightsMasks(Mat &environmentMap, const float offset)
{
    float R = 0.0, G = 0.0, B = 0.0;
    float RMask = 0.0, GMask = 0.0, BMask = 0.0;
    std::vector<std::vector<float> > rgbWeights;
    Mat currentMask;
    ostringstream osstream;

    //Initialisation
    for(unsigned int k = 0 ; k<m_numberOfLightingConditions ; k++)
    {
        std::vector<float> weightsImageK(3,0.0);
        rgbWeights.push_back(weightsImageK);
    }

    int jOffset = floor(offset*m_environmentMapWidth/(2.0*M_PI));

    for(unsigned int k = 0 ; k<m_numberOfLightingConditions ; k++)
    {
        //Load the correct mask : residual mask for the dark room (indirect light only)
        osstream << this->getFolderPath();

        if(k != m_indirectLightPicture)
        {
            if(k<10)
                osstream << "/lighting_conditions/office_room/" << m_roomType << "/" << m_masksType.toStdString() << "/condition_mask0" << k << ".png";
            else
                osstream << "/lighting_conditions/office_room/" << m_roomType << "/" << m_masksType.toStdString() << "/condition_mask" << k << ".png";

            currentMask = imread(osstream.str(), CV_LOAD_IMAGE_COLOR);
        }
        else
        {
            osstream << "/lighting_conditions/office_room/" << m_roomType << "/" << m_masksType.toStdString() << "/residualMask.png";
            currentMask = imread(osstream.str(), CV_LOAD_IMAGE_COLOR);
        }
        currentMask.convertTo(currentMask, CV_32FC3); //Convert the matrix to CV_32FC3 to be able to read the values
        osstream.str("");

        //Read the pixels of the masks
        for(unsigned int i = 0 ; i<m_environmentMapHeight ; i++)
        {
            for(unsigned int j = 0 ; j<m_environmentMapWidth ; j++)
            {

                int jModulus = (j+jOffset)%m_environmentMapWidth;

                BMask = currentMask.at<Vec3f>(i,j).val[0];
                GMask = currentMask.at<Vec3f>(i,j).val[1];
                RMask = currentMask.at<Vec3f>(i,j).val[2];

                //OpenCV uses BGR components
                //If it's black the weights are calculated, otherwise

               if(RMask<127.0 && GMask<127.0 && BMask<127.0)
               {
                    //OpenCV uses BGR
                    R = environmentMap.at<Vec3f>(i,jModulus).val[2];
                    G = environmentMap.at<Vec3f>(i,jModulus).val[1];
                    B = environmentMap.at<Vec3f>(i,jModulus).val[0];

                    if(!(isnan(R) && isnan(G) && isnan(B))) //Values in the environment map could be NaN.
                    {
                        rgbWeights[k][0] += R*sin((float) i*M_PI/m_environmentMapHeight); //Multiply the intensity by the solid angle
                        rgbWeights[k][1] += G*sin((float) i*M_PI/m_environmentMapHeight); //Multiply the intensity by the solid angle
                        rgbWeights[k][2] += B*sin((float) i*M_PI/m_environmentMapHeight); //Multiply the intensity by the solid angle
                    }
                }
             }//END LOOP j
           }//End Loop i
    }//End Loop lighting conditions

    return rgbWeights;
}


/**
 * Sets the room and the mask types.
 * @brief setMaskAndRoomTypes
 */
void OfficeRoomRelighting::setMaskAndRoomTypes()
{
    //Set room type and masks type
    if(m_object == "Egg_bedroom" || m_object == "Bird_bedroom")
    {
        m_roomType = "bedroom";
    }
    else if(m_object == "Egg_bedroom45" || m_object == "Bird_bedroom45")
    {
        m_roomType = "bedroom45";
    }
    else
    {
        m_roomType = "office";
    }

    if(m_masksType == "High Frequency")
    {
       m_masksType = "high_freq";
    }
    else
    {
        m_masksType = "low_freq";
    }
}

/**
 * Setter that sets the method to select light sources.
 * @brief setIdentificationMethod
 * @param INPUT : identificationMethod method to select the light sources.
 */
void OfficeRoomRelighting::setIdentificationMethod(QString& identificationMethod)
{
    m_identificationMethod = identificationMethod;
}

/**
 * Setter to change the number of samples in the environment map sampling (inverse CDF algorithm).
 * @brief setNumberOfSamplesInverseCDF
 * @param INPUT : numberOfSamplesInverseCDF is the number of samples.
 */
void OfficeRoomRelighting::setNumberOfSamplesInverseCDF(unsigned int numberOfSamplesInverseCDF)
{
    m_numberOfSamplesInverseCDF = numberOfSamplesInverseCDF;
}

/**
 * Setter to change the number of the picture containing the indirect light.
 * @brief setIndirectLightPicture
 * @param INPUT : indirectLightPicture
 */
void OfficeRoomRelighting::setIndirectLightPicture(unsigned int indirectLightPicture)
{
    m_indirectLightPicture = indirectLightPicture;
}

/**
 * Setter to change the exposure of the final result.
 * @brief setExposure
 * @param INPUT : exposure in f-stops. The exposure of the final result is increased by a value of exposure f-stops.
 */
void OfficeRoomRelighting::setExposure(double exposure)
{
    m_exposure = exposure;
}

/**
 * Setter to change the optimisation method for the environment map.
 * @brief setOptimisationMethod
 * @param INPUT : optimisationMethod
 */
void OfficeRoomRelighting::setOptimisationMethod(QString optimisationMethod)
{
    m_optimisationMethod = optimisationMethod;
}

/**
 * Setter to change the type of masks used for the relighting.
 * @brief setMasksType
 * @param INPUT : maskType
 */
void OfficeRoomRelighting::setMasksType(QString masksType)
{
    m_masksType = masksType;
}

/**
 * Method that sets all the parameters for the office room relighting.
 * @brief setRelighting
 * @param INPUT : object Name of the object that will be relit.
 * @param INPUT : environmentMap Name of the environment map.
 * @param INPUT : lightType Type of light sources used.
 * @param INPUT : numberOfLightingConditions Number of lighting conditions.
 * @param INPUT : numberOfOffsets Number of rotation of the environment map.
 * @param INPUT : identificationMethod Name of the method to identify the light sources.
 * @param INPUT : masksType Name of the type of masks used : low or high frequency.
 * @param INPUT : optimisationMethod Name of the method for the optimisation.
 * @param INPUT : numberOfSamplesInverseCDF Number of samples in the inverse CDF algorithm.
 * @param INPUT : indirectLightPicture Number corresponding to the indirect light picture.
 * @param INPUT : exposure exposure of the final result.
 */
void OfficeRoomRelighting::setRelighting(QString &object, QString &environmentMap, QString &lightType, unsigned int numberOfLightingConditions,
                              unsigned int numberOfOffsets, QString identificationMethod, QString masksType, QString optimisationMethod, unsigned int numberOfSamplesInverseCDF, unsigned int indirectLightPicture,
                              bool computeBasisMasks, double exposure)
{
    m_voronoi->clearVoronoi();
    this->setObject(object);
    this->setEnvironmentMap(environmentMap);
    this->setLightType(lightType);
    this->setNumberOfOffsets(numberOfOffsets);
    this->setIdentificationMethod(identificationMethod);
    this->setNumberOfSamplesInverseCDF(numberOfSamplesInverseCDF);
    this->setNumberOfLightingConditions(numberOfLightingConditions);
    this->setIndirectLightPicture(indirectLightPicture);
    this->setExposure(exposure);
    this->setOptimisationMethod(optimisationMethod);
    this->setMasksType(masksType);
    this->m_computeBasisMasks = computeBasisMasks;
}

/**
 * Restart the relighting by reinitialising all the variables.
 * @brief clearRelighting
 */
void OfficeRoomRelighting::clearRelighting()
{
    m_voronoi->clearVoronoi();
    m_object = QString("");
    m_environmentMapName = QString("");
    m_lightType = QString("");
    m_numberOfOffsets = 0;
    m_numberOfLightingConditions = 1;
    m_indirectLightPicture = 1;
    m_exposure = 0.0;

    //Environment Map parameters
    m_environmentMapWidth = 1024;
    m_environmentMapHeight = 512;
    m_numberOfComponents = 3;

    //Office Room relighting
    m_identificationMethod = QString(""); //Manual, Median Energy, Inverse CDF
    m_masksType = QString("");
    m_optimisationMethod = QString("");
    m_numberOfSamplesInverseCDF = 0;

}

/**
 * Virtual method to update the progress window.
 * @brief updateProgressWindow
 * @param updateText
 * @param progressBarValue
 */
void OfficeRoomRelighting::updateProgressWindow(QString updateText, int progressBarValue)
{
    emit statusUpdate(updateText);
    emit updateProgressBar(progressBarValue);
    qApp->processEvents(); //Refresh the main window to process the signals
}
