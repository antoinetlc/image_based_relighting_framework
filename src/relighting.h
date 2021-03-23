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
 * \file relighting.hpp
 * \brief Abstract class that contains methods and attributes that can be used for any form of relighting (free form, light stage...)
 * \author Antoine Toisoul Le Cann
 * \date September, 26th, 2016
 *
 *  Abstract class that contains methods and attributes that can be used for any form of relighting (free form, light stage...)
 */

#ifndef RELIGHTING_H
#define RELIGHTING_H

#include "loadFiles.h"
#include "mathsFunctions.h"
#include "voronoi.h"
#include "PFMReadWrite.h"
#include "imageProcessing.h"
#include "LightingBasis.h"
#include "optimisation.h"

#include <iostream>
#include <string>
#include <sstream>
#include <vector>

#include <opencv2/core/core.hpp>
#include <opencv/highgui.h>
#include <opencv2/imgproc/imgproc.hpp>

#include <QApplication>
#include <QObject>
#include <QString>

enum saveFileType{ SAVE_8BITS, SAVE_16BITS};

class Relighting: public QObject
{
    Q_OBJECT

    public:

        /**
         * Main constructor of the Relighting class.
         * @brief Relighting
         */
        Relighting();

        /**
          * Destructor of the relighting class
          */
        ~Relighting();

        /**
         * Virtual pure method to compute the relighting given an object and an environment.
         * @brief relighting
         */
        void virtual relighting() = 0;

        /**
         * Sets the correct environment map name and loads the HDR image.
         * @brief loadEnvironmentMap
         */
        void loadEnvironmentMap();

        /**
         * Function to compute the final relighting (linear combination) from the reflectance field and the RGB weights.
         * @brief computeFinalRelighting
         */
        void computeFinalRelighting();

        /**
         * Function to raytrace the background in the final relit result
         * Applies gamma to background independently if bool parameter is set to true.
         * @brief rayTraceBackground
         * @param offset
         * @param applyGamma
         */
        void rayTraceBackground(const float offset, bool applyGamma = false, double gamma = 1.0);

        /**
         * Apply a gamma correction to the relit result.
         * @brief gammaCorrection
         * @param gamma
         */
        void gammaCorrection(double gamma);

        /**
         * Removes the gamma correction applied of each picture of the reflectance field.
         * @brief removeGammaReflectanceField
         */
        void removeGammaReflectanceField(double gamma);

        /**
         * Changes the exposure of the relit result.
         * @brief changeExposure
         * @param exposure
         */
        void changeExposure(double exposure);

        /**
         * Virtual pure method to load the reflectance field.
         * @brief loadReflectanceField
         */
        bool virtual loadReflectanceField() = 0;

        /**
         * Save the relit result in the correct format.
         * @brief saveResult
         */
        bool saveResult(saveFileType fileType, std::string filePath);

        /**
         * Display the final relighting.
         * @brief displayRelighting
         */
        void displayRelighting();

        /**
         * Methods that sets the name of the object to be relit.
         * @brief setObject
         * @param INPUT : object Name of the object to be relit.
         */
        void setObject(QString &object);

        /**
         * Methods that sets the name of the environment map used for the relighting.
         * @brief setEnvironmentMap
         * @param INPUT : environmentMap Name of the environment map in which the object is relit.
         */
        void setEnvironmentMap(QString &environmentMap);

        /**
         * Methods that sets the type of light sources used for the relighting.
         * @brief setLightType
         * @param INPUT : lightType Name of the type of light sources used.
         */
        void setLightType(QString &lightType);

        /**
         * Methods that sets the number of rotations of the environment map. These are used to relit the object under different angles.
         * @brief setNumberOfOffsets
         * @param INPUT : numberOfOffsets Number of rotation of the environment map.
         */
        void setNumberOfOffsets(unsigned int numberOfOffsets);

        /**
         * Methods that sets the number of lighting conditions used for the rellighting.
         * @brief setNumberOfLightingConditions
         * @param INPUT : numberOfLightingConditions number of lighting condition used for the relighting.
         */
        void setNumberOfLightingConditions(unsigned int numberOfLightingConditions);

        /**
         * Restart the relighting by reinitialising all the variables.
         * @brief clearRelighting
         */
        void virtual clearRelighting() = 0;

        /**
         * Virtual method to update the progress window.
         * @brief updateProgressWindow
         */
        void virtual updateProgressWindow(QString updateText, int progressBarValue) = 0;

        /**
         * Method that returns the path where the folders are depending on the OS.
         * @brief updateProgressWindow
         */
        std::string getFolderPath();

    protected:
        QString m_object; /*!< Name of the object used for the relighting*/
        QString m_environmentMapName; /*!< Name of the environment map*/
        QString m_lightType; /*!< Name of the type of lights used*/
        unsigned int m_numberOfOffsets; /*!< Number of rotations of the environment map*/

        //Reflectance field parameters
        cv::Mat* m_reflectanceField; /*!< Reflectance field*/
        unsigned int m_numberOfLightingConditions; /*!< Number of lighting conditions*/
        cv::Mat m_objectMask; /*!< Mask of the object (to raytrace background)*/

        //Environment Map parameters
        cv::Mat m_environmentMap;
        unsigned int m_environmentMapWidth; /*!< Width of the environment map*/
        unsigned int m_environmentMapHeight;  /*!< Height of the environment map*/
        unsigned int m_numberOfComponents;  /*!< Number of components of the environment map*/

        //Result
        std::vector<std::vector<float> > m_weightsRGB;
        cv::Mat m_relitResult;
};

#endif // RELIGHTING_H
