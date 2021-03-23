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
 * \file optimisation.cpp
 * \brief Class to perform the optimisation process.
 * \author Antoine Toisoul Le Cann
 * \date August, 7th, 2014
 *
 * Class that performs the optimisation process in original space or PCA space.
 * Given a piecewise constant basis that corresponds to an approximation of an environment map, an instantiation of this class associate a weight to each constant of the basis to minimze the error between this basis and the original environment map.
 */

#include "optimisation.h"

using namespace std;
using namespace dlib;
using namespace cv;

static string environmentMapNameGlobal;
static unsigned int environmentMapWidthGlobal;
static unsigned int environmentMapHeightGlobal;
static unsigned int numberOfComponentsGlobal;
static unsigned int numberOflightingConditionsGlobal;
static unsigned int indirectLightPictureGlobal;
static float offsetGlobal;
static string masksTypeGlobal;
static string roomTypeGlobal;
static std::vector<std::vector<float> > rgbWeightsGlobal;
static PCA pcaProjectionMatrix; //PCA of the projection matrix
static Mat envMapPCASpace;

/**
 * Default constructor to initialise
 * @brief Optimisation
 */
Optimisation::Optimisation(): m_environmentMapName(string("")), m_environmentMapWidth(1024), m_environmentMapHeight(512), m_numberOfComponents(3),
    m_numberOflightingConditions(0), m_indirectLightPicture(0),
    m_offset(0.0), m_rgbWeights(std::vector<std::vector<float> >())
{
    this->setGlobalVariables();
}

/**
 * Constructor to initialise the parameters of the optimisation process. Sets the global variables required for the optimisation.
 * @brief Optimisation
 * @param INPUT : environmentMapName Name of the environment map used for the optimisation.
 * @param INPUT : environmentMapWidth Width of the environment map.
 * @param INPUT : environmentMapHeight Height of the environment map.
 * @param INPUT : numberOfComponents Number of components of the environment map
 * @param INPUT : numberOfLightingConditions Number of lighting conditions used in the office room relighting.
 * @param INPUT : indirectLightPicture Number of the picture containing the indirect light (dark room).
 * @param INPUT : offset offset that is added to the rotation of the environment map.
 * @param INPUT : roomType Type of the room
 * @param INPUT : masksType Name of the type of mask used (adapted to high or low frequency)
 * @param INPUT : rgbWeights weights that corresponds to the piecewise constant basis.
 */
Optimisation::Optimisation(string environmentMapName,
                           unsigned int environmentMapWidth, unsigned int environmentMapHeight,
                           unsigned int numberOfComponents, unsigned int numberOfLightingConditions,
                           unsigned int indirectLightPicture, float offset, string roomType, string masksType,
                           std::vector<std::vector<float> >& rgbWeights):
    m_environmentMapName(environmentMapName),
    m_environmentMapWidth(environmentMapWidth), m_environmentMapHeight(environmentMapHeight), m_numberOfComponents(numberOfComponents),
    m_numberOflightingConditions(numberOfLightingConditions), m_indirectLightPicture(indirectLightPicture),
    m_offset(offset), m_roomType(roomType), m_masksType(masksType), m_rgbWeights(rgbWeights)
{
    this->setGlobalVariables();
}

/**
 * Method that performs the optimisation process in the original space.
 * @brief environmentMapOptimisation
 * @param INPUT : startingPointArray starting point of the optimisation process (starting point of the function to minimise).
 */
void Optimisation::environmentMapOptimisation(double startingPointArray[])
{
    column_vector startingPoint(m_numberOflightingConditions);

    for(unsigned int i = 0 ; i<m_numberOflightingConditions ; i++)
    {
        startingPoint(i) = startingPointArray[i];//m_numberOflightingConditions;

    }

    cout << "Starting optimisation" << endl;
    cout << "starting point \n" << startingPoint << endl;
    find_min_box_constrained(lbfgs_search_strategy(10),
                             objective_delta_stop_strategy(1e-9),
                             functionToOptimise, derivative(functionToOptimise), startingPoint, 0.0, 10.0);

    cout << endl << "Solution to the optimisation process \n" << startingPoint << endl;

    for(unsigned int i = 0 ; i<m_rgbWeights.size() ; i++)
    {
        for(unsigned int j = 0 ; j<m_rgbWeights[i].size() ; j++)
        {
            m_rgbWeights[i][j] *= startingPoint(i);
        }
    }

    for(unsigned int i = 0 ; i<m_numberOflightingConditions ; i++)
    {
        startingPointArray[i] = startingPoint(i);
    }
}

/**
 * Method that performs the optimisation process in PCA space.
 * @brief environmentMapOptimisation
 * @param INPUT : startingPointArray starting point of the optimisation process (starting point of the function to minimise).
 */
void Optimisation::environmentMapPCAOptimisation(double startingPointArray[])
{
    this->computePCAMatrix();

    column_vector startingPoint(m_numberOflightingConditions);

    for(unsigned int i = 0 ; i<m_numberOflightingConditions ; i++)
    {
        startingPoint(i) = startingPointArray[i];//m_numberOflightingConditions;

    }

    cout << "Starting optimisation in PCA space" << endl;
    cout << "starting point \n" << startingPoint << endl;

    find_min_box_constrained(lbfgs_search_strategy(10),
                             objective_delta_stop_strategy(1e-9),
                             functionToOptimisePCASpace, derivative(functionToOptimisePCASpace), startingPoint, 0.0, 10.0);

    cout << endl << "Solution to the optimisation process \n" << startingPoint << endl;

    for(unsigned int i = 0 ; i<m_rgbWeights.size() ; i++)
    {
        for(unsigned int j = 0 ; j<m_rgbWeights[i].size() ; j++)
        {
            m_rgbWeights[i][j] *= startingPoint(i);
        }
    }

    for(unsigned int i = 0 ; i<m_numberOflightingConditions ; i++)
    {
        startingPointArray[i] = 1.0; // The next optimisation starts with 1.0 weights
    }
}


/**
 * Method that compute the PCA of the environment map and the matrix to project a vector into the PCA space.
 * @brief computePCAMatrix
 */
void Optimisation::computePCAMatrix()
{
    ostringstream osstream;

    //Variables are along the columns
    Rect boundingBox(0,0,numberOflightingConditionsGlobal,environmentMapHeightGlobal*environmentMapWidthGlobal);
    Mat projectionMatrix(boundingBox.size(),CV_32F);
    Mat currentMask;

    float weightR = 0.0, weightG = 0.0, weightB = 0.0;
    float RMask = 0.0, GMask = 0.0, BMask = 0.0;

    int jOffset = floor(offsetGlobal*environmentMapWidthGlobal/(2.0*M_PI));

    for(unsigned int k = 0 ; k<numberOflightingConditionsGlobal ; k++)
    {

        //Load the correct mask : residual mask for the dark room (indirect light only)
        #if defined(__APPLE__) && defined(__MACH__)
            osstream << qApp->applicationDirPath().toStdString() << "/../../..";
        #else
            osstream << qApp->applicationDirPath().toStdString();
        #endif

        if(k != indirectLightPictureGlobal)
        {
           if(k<10)
               osstream << "/lighting_conditions/office_room/" << masksTypeGlobal << "/condition_mask0" << k << ".png";
           else
               osstream << "/lighting_conditions/office_room/" << masksTypeGlobal << "/condition_mask" << k << ".png";

            currentMask = imread(osstream.str(), CV_LOAD_IMAGE_COLOR);
        }
        else
        {
            if(masksTypeGlobal == "High Frequency")
            {
                osstream << "/lighting_conditions/office_room/high_freq/residualMask.png";
            }

            if(masksTypeGlobal == "Low Frequency")
            {
                osstream << "/lighting_conditions/office_room/high_freq/residualMask.png";
            }
            currentMask = imread(osstream.str(), CV_LOAD_IMAGE_COLOR);
        }
        currentMask.convertTo(currentMask, CV_32FC3); //Convert the matrix to CV_32FC3 to be able to read the values
        osstream.str("");

        //Read the pixels of the masks
        for(unsigned int i = 0 ; i<environmentMapHeightGlobal ; i++)
        {
            for(unsigned int j = 0 ; j<environmentMapWidthGlobal ; j++)
            {

                BMask = currentMask.at<Vec3f>(i,j).val[0];
                GMask = currentMask.at<Vec3f>(i,j).val[1];
                RMask = currentMask.at<Vec3f>(i,j).val[2];

                //OpenCV uses BGR components
                //If it's black the weights are calculated, otherwise
               if(RMask<127.0 && GMask<127.0 && BMask<127.0)
               {
                    weightR = rgbWeightsGlobal[k][0];
                    weightG = rgbWeightsGlobal[k][1];
                    weightB = rgbWeightsGlobal[k][2];

                    projectionMatrix.at<float>(i*environmentMapWidthGlobal+j,k) = (weightR+weightG+weightB)/3.0;
               }

           }//END LOOP j
        }//End Loop i
    }//End Loop lighting conditions

    //Compute PCA of the projection matrix
    Mat mean;
    pcaProjectionMatrix = PCA(projectionMatrix, mean, CV_PCA_DATA_AS_COL);

    //Project the environment map in the PCA space
    boundingBox = Rect(0,0,1, environmentMapWidthGlobal*environmentMapHeightGlobal);//Column vector
    Mat environmentMapIntensity = Mat(boundingBox.size(), CV_32F);;
    envMapPCASpace = Mat(boundingBox.size(), CV_32F);
    float R = 0.0, G = 0.0, B = 0.0;

    #if defined(__APPLE__) && defined(__MACH__)
        osstream << qApp->applicationDirPath().toStdString() << "/../../..";
    #else
        osstream << qApp->applicationDirPath().toStdString();
    #endif

    osstream << "/environment_maps/" <<environmentMapNameGlobal << ".pfm";


    Mat environmentMap = loadPFM(osstream.str());
    osstream.str("");

    // Fills
    for(unsigned int i = 0 ; i<environmentMapHeightGlobal ; i++)
    {
        for(unsigned int j = 0 ; j<environmentMapWidthGlobal ; j++)
        {

           int jModulus = (j+jOffset)%environmentMapWidthGlobal;

           //OpenCV stores in BGR
           R = environmentMap.at<Vec3f>(i,jModulus).val[2]*sin((float) M_PI*i/environmentMapHeightGlobal);
           G = environmentMap.at<Vec3f>(i,jModulus).val[1]*sin((float) M_PI*i/environmentMapHeightGlobal);
           B = environmentMap.at<Vec3f>(i,jModulus).val[0]*sin((float) M_PI*i/environmentMapHeightGlobal);

           if(!(isnan(R) && isnan(G) && isnan(B))) //Values in the environment map could be NaN.
           {
             environmentMapIntensity.at<float>(i*environmentMapWidthGlobal+j,0) = (R+G+B)/3.0;
           }
           else
           {
             environmentMapIntensity.at<float>(i*environmentMapWidthGlobal+j,0) = 0.0;
           }
        }
    }

    //The projection of the environment map in the PCA space is precomputed
    envMapPCASpace = pcaProjectionMatrix.project(environmentMapIntensity.col(0));

}

/**
 * Method that sets the global variables required for the function to optimise.
 * @brief setGlobalVariables
 */
void Optimisation::setGlobalVariables()
{
    environmentMapNameGlobal = m_environmentMapName;
    environmentMapWidthGlobal = m_environmentMapWidth;
    environmentMapHeightGlobal = m_environmentMapHeight;
    numberOfComponentsGlobal = m_numberOfComponents;
    numberOflightingConditionsGlobal = m_numberOflightingConditions;
    indirectLightPictureGlobal = m_indirectLightPicture;
    offsetGlobal = m_offset;
    rgbWeightsGlobal = m_rgbWeights;
    roomTypeGlobal = m_roomType;
    masksTypeGlobal = m_masksType;
}

/**
 * Method that return the width of the environment map.
 * @brief getEnvironmentMapWidth
 * @return width of the environment map.
 */
unsigned int Optimisation::getEnvironmentMapWidth()
{
    return m_environmentMapWidth;
}

/**
 * Method that returns the height of the environment map.
 * @brief getEnvironmentMapHeight
 * @return height of the environment map.
 */
unsigned int Optimisation::getEnvironmentMapHeight()
{
    return m_environmentMapHeight;
}

/**
 * Method that returns the number of components of the environment map.
 * @brief getNumberOfComponents
 * @return number of components of the environment map.
 */
unsigned int Optimisation::getNumberOfComponents()
{
    return m_numberOfComponents;
}

/**
 * Method that return the number of lighting conditions used.
 * @brief getNumberOfLightingConditions
 * @return number of lighting conditions.
 */
unsigned int Optimisation::getNumberOfLightingConditions()
{
    return m_numberOflightingConditions;
}

/**
 * Method that returns the number of the picture that contains the indirect light.
 * @brief getIndirectLightPicture
 * @return  number of the picture that contains the indirect light.
 */
unsigned int Optimisation::getIndirectLightPicture()
{
    return m_indirectLightPicture;
}

/**
 * Method that returns the offset (rotation around the phi angle) of the environment map.
 * @brief getOffset
 * @return the offset (rotation around the phi angle) of the environment map.
 */
float Optimisation::getOffset()
{
    return m_offset;
}


/**
 * Method that returns the RGB weights of each lighting condition.
 * @brief getRGBWeights
 * @return the RGB weights of each lighting condition.
 */
std::vector<std::vector<float> > Optimisation::getRGBWeights()
{
    return m_rgbWeights;
}

/**
 * Function that has to be optimised (original space).
 * @brief functionToOptimise
 * @param variablesVector column vector containing the value of the variables that are being optimised.
 * @return the value of the function.
 */
double functionToOptimise(const column_vector &variablesVector)
{
    float R = 0.0, G = 0.0, B = 0.0, intensityEnvMap = 0.0;
    float RMask = 0.0, GMask = 0.0, BMask = 0.0;
    float intensityWeights = 0.0;
    Mat currentMask;
    ostringstream osstream;
    unsigned int numberOfVariables = variablesVector.size();

    double* variables = new double[numberOfVariables];
    double result = 0.0;

#if defined(__APPLE__) && defined(__MACH__)
    osstream << qApp->applicationDirPath().toStdString() << "/../../..";
#else
    osstream << qApp->applicationDirPath().toStdString();
#endif
       osstream << "/environment_maps/" <<environmentMapNameGlobal << ".pfm";


    Mat environmentMap = loadPFM(osstream.str());
    osstream.str("");


    for(unsigned int i = 0 ; i<numberOfVariables ; i++)
    {
        variables[i] = variablesVector(i);
    }

    int jOffset = floor(offsetGlobal*environmentMapWidthGlobal/(2.0*M_PI));

    for(unsigned int k = 0 ; k<numberOfVariables ; k++)
    {

        //Load the correct mask : residual mask for the dark room (indirect light only)
        #if defined(__APPLE__) && defined(__MACH__)
            osstream << qApp->applicationDirPath().toStdString() << "/../../..";
        #else
            osstream << qApp->applicationDirPath().toStdString();
        #endif
        if(k != indirectLightPictureGlobal)
        {
            //Type of mask
            if(k<10)
                osstream << "/lighting_conditions/office_room/" << roomTypeGlobal << "/" << masksTypeGlobal << "/condition_mask0" << k << ".png";
            else
                osstream << "/lighting_conditions/office_room/" << roomTypeGlobal << "/" << masksTypeGlobal << "/condition_mask" << k << ".png";

            currentMask = imread(osstream.str(), CV_LOAD_IMAGE_COLOR);
        }
        else
        {
            osstream << "/lighting_conditions/office_room/" << roomTypeGlobal << "/" << masksTypeGlobal << "/residualMask.png";

            currentMask = imread(osstream.str(), CV_LOAD_IMAGE_COLOR);
        }
        currentMask.convertTo(currentMask, CV_32FC3); //Convert the matrix to CV_32FC3 to be able to read the values
        osstream.str("");

        //Read the pixels of the masks
        for(unsigned int i = 0 ; i<environmentMapHeightGlobal ; i++)
        {
            for(unsigned int j = 0 ; j<environmentMapWidthGlobal ; j++)
            {

                int jModulus = (j+jOffset)%environmentMapWidthGlobal;

                BMask = currentMask.at<Vec3f>(i,j).val[0];
                GMask = currentMask.at<Vec3f>(i,j).val[1];
                RMask = currentMask.at<Vec3f>(i,j).val[2];

                //OpenCV uses BGR components
                //If it's black the weights are calculated, otherwise
               if(RMask<127.0 && GMask<127.0 && BMask<127.0)
               {
                   //OpenCV stores in BGR
                    R = environmentMap.at<Vec3f>(i,jModulus).val[2]*sin((float) M_PI*i/environmentMapHeightGlobal);
                    G = environmentMap.at<Vec3f>(i,jModulus).val[1]*sin((float) M_PI*i/environmentMapHeightGlobal);
                    B = environmentMap.at<Vec3f>(i,jModulus).val[0]*sin((float) M_PI*i/environmentMapHeightGlobal);

                    intensityEnvMap = (R+G+B)/3.0;

                    if(!(isnan(R) && isnan(G) && isnan(B))) //Values in the environment map could be NaN.
                    {
                        intensityWeights = (rgbWeightsGlobal[k][0]+rgbWeightsGlobal[k][1]+rgbWeightsGlobal[k][2])/3.0;
                        result += pow(variables[k]*intensityWeights-intensityEnvMap, 2.0);
                    }
                }
             }//END LOOP j
           }//End Loop i
    }//End Loop lighting conditions

    delete[] variables;

    return sqrt(result);
}

/**
 * Function that has to be optimised (PCA space).
 * @brief functionToOptimisePCASpace
 * @param variablesVector column vector containing the value of the variables that are being optimised.
 * @return the value of the function.
 */
double functionToOptimisePCASpace(const column_vector &variablesVector)
{
    float RMask = 0.0, GMask = 0.0, BMask = 0.0;
    float intensityWeights = 0.0;
    Mat currentMask;
    ostringstream osstream;
    unsigned int numberOfVariables = variablesVector.size();

    Rect boundingBox(0,0,1,environmentMapWidthGlobal*environmentMapHeightGlobal);
    Mat projectionOnWeightsBasis(boundingBox.size(), CV_32F);

    double* variables = new double[numberOfVariables];
    double result = 0.0;

    for(unsigned int i = 0 ; i<numberOfVariables ; i++)
    {
        variables[i] = variablesVector(i);
    }

    for(unsigned int k = 0 ; k<numberOfVariables ; k++)
    {

        //Load the correct mask : residual mask for the dark room (indirect light only)
        #if defined(__APPLE__) && defined(__MACH__)
            osstream << qApp->applicationDirPath().toStdString() << "/../../..";
        #else
            osstream << qApp->applicationDirPath().toStdString();
        #endif

        if(k != indirectLightPictureGlobal)
        {
            //Type of mask
            if(k<10)
                osstream << "/lighting_conditions/office_room/" << roomTypeGlobal << "/" << masksTypeGlobal << "/condition_mask0" << k << ".png";
            else
                osstream << "/lighting_conditions/office_room/" << roomTypeGlobal << "/" << masksTypeGlobal << "/condition_mask" << k << ".png";

            currentMask = imread(osstream.str(), CV_LOAD_IMAGE_COLOR);
        }
        else
        {
            osstream << "/lighting_conditions/office_room/" << roomTypeGlobal << "/" << masksTypeGlobal << "/residualMask.png";

            currentMask = imread(osstream.str(), CV_LOAD_IMAGE_COLOR);
        }

        currentMask.convertTo(currentMask, CV_32FC3); //Convert the matrix to CV_32FC3 to be able to read the values
        osstream.str("");

        //Read the pixels of the masks
        for(unsigned int i = 0 ; i<environmentMapHeightGlobal ; i++)
        {
            for(unsigned int j = 0 ; j<environmentMapWidthGlobal ; j++)
            {

                BMask = currentMask.at<Vec3f>(i,j).val[0];
                GMask = currentMask.at<Vec3f>(i,j).val[1];
                RMask = currentMask.at<Vec3f>(i,j).val[2];

                //OpenCV uses BGR components
                //If it's black the weights are calculated, otherwise
               if(RMask<127.0 && GMask<127.0 && BMask<127.0)
               {
                   intensityWeights = (rgbWeightsGlobal[k][0]+rgbWeightsGlobal[k][1]+rgbWeightsGlobal[k][2])/3.0;
                   projectionOnWeightsBasis.at<float>(i*environmentMapWidthGlobal+j,0) = intensityWeights*variables[k];

                }
             }//END LOOP j
           }//End Loop i
    }//End Loop lighting conditions


    Mat pcaProjectionOnWeightsBasis = pcaProjectionMatrix.project(projectionOnWeightsBasis.col(0));

    for(int l = 0 ; l< pcaProjectionOnWeightsBasis.cols ; l++)
    {
       result += pow(pcaProjectionOnWeightsBasis.at<float>(l,0)-envMapPCASpace.at<float>(l,0), 2.0);
    }

    delete[] variables;

    return sqrt(result);
}

