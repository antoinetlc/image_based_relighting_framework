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
 * \file lightStageRelighting.h
 * \brief Class to compute the light stage relighting using Paul Debevec's light stage data (www.pauldebevec.com).
 * \author Antoine Toisoul Le Cann
 * \date August, 7th, 2014
 *
 * Class that inherites of the Relighting class. Computes the light stage relighting of an object.
 */

#ifndef LIGHTSTAGERELIGHTING_H
#define LIGHTSTAGERELIGHTING_H

#define _USE_MATH_DEFINES
#define GAMMA 2.2
#define EXPOSURE 1.2

#include "loadFiles.h"
#include "mathsFunctions.h"
#include "voronoi.h"
#include "PFMReadWrite.h"
#include "imageProcessing.h"
#include "LightingBasis.h"
#include "optimisation.h"
#include "relighting.h"


#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <cmath>

#include <opencv2/core/core.hpp>
#include <opencv/highgui.h>
#include <opencv2/imgproc/imgproc.hpp>

#include <QApplication>
#include <QObject>
#include <QString>

class LightStageRelighting : public Relighting
{
    Q_OBJECT

    public:
        /**
         * Constructor of the LightStageRelighting class.
         * @brief LightStageRelighting
         */
        LightStageRelighting();

        /**
          * Destructor of the LightStageRelighting class.
          */
        ~LightStageRelighting();


        /**
         * Computes the relighting of the object using the light stage method.
         * @brief relighting
         */
        void virtual relighting();

        /**
         * Virtual pure function.
         * Loads the reflectance field of the object and stores it as a float image between 0.0 and 1.0.
         * @brief loadReflectanceField
         * @return
         */
        bool virtual loadReflectanceField();

        /**
         * Function that sets all the parameters for the relighting.
         * @brief setRelighting
         * @param INPUT : environmentMap Name of the environment map.
         * @param INPUT : lightType Type of light sources used.
         * @param INPUT : numberOfLightingConditions Number of lighting conditions.
         * @param INPUT : numberOfOffsets number of offsets (rotations of the environment map) under which the object will be relit.
         * @param INPUT : exposure exposure of the final result.
         */
        void setRelighting(QString &object, QString &environmentMap, QString &lightType, unsigned int numberOfLightingConditions,
                           unsigned int numberOfOffsets);

        /**
         * Restart the relighting by reinitialising all the variables.
         * @brief clearRelighting
         */
         void clearRelighting();

         /**
          * Saves the environment map with the voronoi tesselation displayed.
          * @brief saveVoronoiTesselation
          */
         void saveVoronoiTesselation(int offset);


         /**
          * Saves the Voronoi diagram with each cell painted as its corresponding RGB weight.
          * @brief saveVoronoiWeights
          * @param offset
          */
         void saveVoronoiWeights(int offset);

         /**
          * Saves the environment map with the light stage directions mapped on it.
          * @brief saveLightStageDirection
          */
         void saveLightStageDirection();

         /**
          * Saves the intensities of the light stage light sources.
          * @brief saveLightStageIntensities
          */
         void saveLightStageIntensities();

         /**
          * Virtual method to update the progress window.
          * @brief updateProgressWindow
          * @param updateText
          * @param progressBarValue
          */
         void virtual updateProgressWindow(QString updateText, int progressBarValue);

    signals:

       /**
        * Qt signal that sends information to the main window. This information will be displayed in the in-app terminal.
        * @brief statusUpdate
        * @param information INPUT : QString that contains the information.
        */
        void statusUpdate(QString information);

        /**
         * Qt signal that sends the value of the progress of the relighting to the main window.
         * @brief updateProgressBar
         * @param value INPUT : Value (percentage) of the relighting.
         */
        void updateProgressBar(int value);

        /**
         * Qt signal that sends the path of the image that has been created. This image will be displayed in the progress window.
         * @brief updateImage
         * @param imageName INPUT : path of the image.
         */
        void updateImage(QString imageName);

    private:
        Voronoi* m_voronoi;/*!< Object that performs the voronoi tesselation*/

};


#endif // LIGHTSTAGERELIGHTING_H
