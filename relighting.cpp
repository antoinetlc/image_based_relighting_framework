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
 * \file relighting.cpp
 * \brief Abstract class that contains methods and attributes that can be used for any form of relighting (free form, light stage...)
 * \author Antoine Toisoul Le Cann
 * \date September, 26th, 2016
 *
 *  Abstract class that contains methods and attributes that can be used for any form of relighting (free form, light stage...)
 */

#include "relighting.h"

using namespace std;
using namespace cv;

/**
 * Main constructor of the Relighting class.
 * @brief Relighting
 */
Relighting::Relighting(): m_object(QString()), m_environmentMapName(QString()), m_lightType(QString()),
    m_numberOfOffsets(1), m_reflectanceField(NULL), m_numberOfLightingConditions(1),  m_objectMask(Mat()),
    m_environmentMap(Mat()), m_environmentMapWidth(1024), m_environmentMapHeight(512), m_numberOfComponents(3),
    m_weightsRGB(std::vector<std::vector<float> >()), m_relitResult(Mat())
{

}

/**
  * Destructor of the relighting class
  */
Relighting::~Relighting()
{
    delete[] m_reflectanceField;
}

/**
 * Sets the correct environment map name and loads the HDR image.
 * @brief loadEnvironmentMap
 */
void Relighting::loadEnvironmentMap()
{
    if(m_environmentMapName == "Grace Cathedral")
    {
        m_environmentMapName = "grace_latlong";
    }
    else if(m_environmentMapName == "St. Peter's Basilica")
    {
        m_environmentMapName = "stpeters_probe";
    }
    else if(m_environmentMapName == "The Uffizi Gallery")
    {
        m_environmentMapName = "uffizi";
    }
    else if(m_environmentMapName == "Eucalyptus Grove")
    {
        m_environmentMapName = "eucalyptus_grove";
    }
    else if(m_environmentMapName == "Pisa courtyard")
    {
       m_environmentMapName = "pisa_courtyard";
    }

    m_environmentMap = loadPFM(this->getFolderPath() + "/environment_maps/" + m_environmentMapName.toStdString() + ".pfm");

    if(!m_environmentMap.data)
    {
        cerr << "Could not load : " << this->getFolderPath() + "/environment_maps/" + m_environmentMapName.toStdString() + ".pfm" << endl;
    }

    m_environmentMapWidth = m_environmentMap.cols;
    m_environmentMapHeight = m_environmentMap.rows;
    m_numberOfComponents = 3;
}

/**
 * Function to compute the final relighting (linear combination) from the reflectance field and the RGB weights.
 * @brief computeFinalRelighting
 */
void Relighting::computeFinalRelighting()
{
    m_relitResult = Mat::zeros(m_reflectanceField[0].rows, m_reflectanceField[0].cols, CV_32FC3);

    Mat channel[3];
    Mat currentImage;

    for(unsigned int i = 0 ; i<m_numberOfLightingConditions ; ++i)
    {
        split(m_reflectanceField[i], channel);

        //OpenCV uses images in BGR format
        channel[0] *= m_weightsRGB[i][2];
        channel[1] *= m_weightsRGB[i][1];
        channel[2] *= m_weightsRGB[i][0];

        merge(channel,3, currentImage);
        m_relitResult += currentImage;
    }
}

/**
 * Function to raytrace the background in the final relit result
 * Applies gamma to background independently if bool parameter is set to true.
 * @brief rayTraceBackground
 * @param offset
 * @param applyGamma
 */
void Relighting::rayTraceBackground(const float offset, bool applyGamma, double gamma)
{
    ostringstream osstream;
    Mat environmentMap;

//Load the environment map
#if defined(__APPLE__) && defined(__MACH__)
    osstream << qApp->applicationDirPath().toStdString() << "/../../..";
#else
    osstream << qApp->applicationDirPath().toStdString();
#endif
    osstream << "/environment_maps/" << m_environmentMapName.toStdString() << ".pfm";

    environmentMap = loadPFM(osstream.str());
    osstream.str("");

    //Information about the image
    int width = m_relitResult.cols;
    int height = m_relitResult.rows;
    float halfHeight = height/2;
    float halfWidth = width/2;

    for(int i = 0 ; i<height ; i++)
    {
        for(int j = 0 ; j<width; j++)
        {
            //OpenCV uses BGR components
            if(m_objectMask.at<Vec3f>(i,j).val[0]>0.5 && m_objectMask.at<Vec3f>(i,j).val[1]>0.5 && m_objectMask.at<Vec3f>(i,j).val[2]>0.5)//If it's white, it is the background
            {
                //Calculate the direction
                float x = (float) (j-halfWidth)/halfWidth;
                float y = (float) -(i-halfHeight)/halfHeight;

                float z = -1.0;

                //Normalize the direction
                float r = sqrt(x*x + y*y + z*z);

                x /=r;
                y /=r;
                z /=r;

                //Convert the direction to spherical coordinates
                float theta = 0.0, phi = 0.0;
                cartesianToSpherical(x,y,z,r,theta,phi);

                phi += offset;
                phi = moduloRealNumber(phi, 2.0*M_PI);

                //Convert the spherical coordinates to latitude longitude map
                int I = floor(m_environmentMapHeight*theta/M_PI);
                int J = floor(m_environmentMapWidth*phi/(2.0*M_PI));

                //Results are between 0 and 255 for 8 bits images
                m_relitResult.at<Vec3f>(i,j).val[0] = environmentMap.at<Vec3f>(I,J).val[0];
                m_relitResult.at<Vec3f>(i,j).val[1] = environmentMap.at<Vec3f>(I,J).val[1];
                m_relitResult.at<Vec3f>(i,j).val[2] = environmentMap.at<Vec3f>(I,J).val[2];

                if(applyGamma)
                {
                    m_relitResult.at<Vec3f>(i,j).val[0] = pow(m_relitResult.at<Vec3f>(i,j).val[0], 1.0/gamma);
                    m_relitResult.at<Vec3f>(i,j).val[1] = pow(m_relitResult.at<Vec3f>(i,j).val[1], 1.0/gamma);
                    m_relitResult.at<Vec3f>(i,j).val[2] = pow(m_relitResult.at<Vec3f>(i,j).val[2], 1.0/gamma);
                }
             }
        }
    }
}

/**
 * Apply a gamma correction to the relit result.
 * @brief gammaCorrection
 * @param gamma
 */
void Relighting::gammaCorrection(double gamma)
{

    Mat channel[3], channelWithGamma[3];

    split(m_relitResult, channel);

    channel[0].convertTo(channel[0], CV_32F);
    channel[1].convertTo(channel[1], CV_32F);
    channel[2].convertTo(channel[2], CV_32F);

    pow(channel[0], 1.0/gamma, channelWithGamma[0]);
    pow(channel[1], 1.0/gamma, channelWithGamma[1]);
    pow(channel[2], 1.0/gamma, channelWithGamma[2]);

    merge(channelWithGamma,3, m_relitResult);
}

/**
 * Removes the gamma correction applied of each picture of the reflectance field.
 * @brief removeGammaReflectanceField
 */
void Relighting::removeGammaReflectanceField(double gamma)
{
    Mat channel[3], channelWithoutGamma[3];

    for(unsigned int k = 0 ; k<m_numberOfLightingConditions ; ++k)
    {
        split(m_reflectanceField[k], channel);

        channel[0].convertTo(channel[0], CV_32F);
        channel[1].convertTo(channel[1], CV_32F);
        channel[2].convertTo(channel[2], CV_32F);

        pow(channel[0], gamma, channelWithoutGamma[0]);
        pow(channel[1], gamma, channelWithoutGamma[1]);
        pow(channel[2], gamma, channelWithoutGamma[2]);

        merge(channelWithoutGamma, 3, m_reflectanceField[k]);
    }

}

/**
 * Changes the exposure of the relit result.
 * @brief changeExposure
 * @param exposure
 */
void Relighting::changeExposure(double exposure)
{
    m_relitResult.convertTo(m_relitResult,-1, pow(2.0, exposure));
}

/**
 * Save the relit result in the correct format.
 * @brief saveResult
 */
bool Relighting::saveResult(saveFileType fileType, string filePath)
{

    if(fileType == SAVE_8BITS)
    {
        m_relitResult *= 255.0;
        m_relitResult.convertTo(m_relitResult, CV_8UC3);
        imwrite(filePath, m_relitResult);
    }
    else if(fileType == SAVE_16BITS)
    {
        m_relitResult *= 65535.0;
        m_relitResult.convertTo(m_relitResult, CV_16UC3);
        imwrite(filePath, m_relitResult);
    }
    else
    {
        cerr << "Could not save the file : unknown file type" << endl;
        return EXIT_FAILURE;
    }


    return EXIT_SUCCESS;
}

/**
 * Display the final relighting.
 * @brief displayRelighting
 */
void Relighting::displayRelighting()
{
    imshow("Relighting", m_relitResult);
    waitKey(0);
}

/**
 * Methods that sets the name of the object to be relit.
 * @brief setObject
 * @param INPUT : object Name of the object to be relit.
 */
void Relighting::setObject(QString &object)
{
    m_object = object;
}

/**
 * Methods that sets the name of the environment map used for the relighting.
 * @brief setEnvironmentMap
 * @param INPUT : environmentMap Name of the environment map in which the object is relit.
 */
void Relighting::setEnvironmentMap(QString &environmentMap)
{
    m_environmentMapName = environmentMap;
}

/**
 * Methods that sets the type of light sources used for the relighting.
 * @brief setLightType
 * @param INPUT : lightType Name of the type of light sources used.
 */
void Relighting::setLightType(QString &lightType)
{
    m_lightType = lightType;
}

/**
 * Methods that sets the number of rotations of the environment map. These are used to relit the object under different angles.
 * @brief setNumberOfOffsets
 * @param INPUT : numberOfOffsets Number of rotation of the environment map.
 */
void Relighting::setNumberOfOffsets(unsigned int numberOfOffsets)
{
    m_numberOfOffsets = numberOfOffsets;
}

/**
 * Methods that sets the number of lighting conditions used for the rellighting.
 * @brief setNumberOfLightingConditions
 * @param INPUT : numberOfLightingConditions number of lighting condition used for the relighting.
 */
void Relighting::setNumberOfLightingConditions(unsigned int numberOfLightingConditions)
{
    m_numberOfLightingConditions = numberOfLightingConditions;
}

/**
 * Method that returns the path where the folders are depending on the OS.
 * @brief updateProgressWindow
 */
std::string Relighting::getFolderPath()
{
    string path;
    #if defined(__APPLE__) && defined(__MACH__)
        path += qApp->applicationDirPath().toStdString() + "/../../..";
    #else
        path += qApp->applicationDirPath().toStdString();
    #endif

    return path;
}
