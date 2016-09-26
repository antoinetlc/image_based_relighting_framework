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
 * \file freeformlightstage.h
 * \brief Class to compute the free form light relighting.
 * \author Antoine Toisoul Le Cann
 * \date August, 7th, 2014
 *
 * Class that inherites of the Relighting class. Performs the Free form relighting of an object.
 */

#ifndef FREEFORMLIGHTSTAGE_H
#define FREEFORMLIGHTSTAGE_H

#define _USE_MATH_DEFINES
#define GAMMA 2.2

#include "loadFiles.h"
#include "mathsFunctions.h"
#include "voronoi.h"
#include "imageProcessing.h"
#include "lightStageRelighting.h"
#include "LightingBasis.h"
#include "relighting.h"
#include "manualSelection.h"

#include <cmath>
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

class FreeFormLightStage : public Relighting
{
    Q_OBJECT

    public:
        /**
         * Constructor of the FreeFormLightStage class.
         * @brief FreeFormLightStage
         */
        FreeFormLightStage();

        /**
          *Destructor of the FreeFormLightStage. Release memory.
          */
        ~FreeFormLightStage();

        /**
         * Computes the relighting of the object using the free form light stage method.
         * @brief relighting
         */
        void virtual relighting();

       /**
         * Virtual pure function.
         * Loads the reflectance field of the object.
         * @brief loadReflectanceField
         * @return
         */
        bool virtual loadReflectanceField();

        /**
         * Removes the dark room measurement from the reflectance field.
         * @brief removeDarkRoom
         */
        void removeDarkRoom();

        /**
         * Method to manually select the incoming light directions in the environment map of the office.
         * @brief identifyLightsUser
         * @param INPUT : mouseParameters parameters that are sent to the callback function to manually select light sources.
         */
        void identifyLightsUser(MouseParameters& mouseParameters);

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
         * Setter to change the exposure of the final result.
         * @brief setExposure
         * @param INPUT : exposure double that increases the final exposure by exposure f-stops.
         */
        void setExposure(double exposure);

        /**
         * Setter that sets the method to select light sources.
         * @brief setIdentificationMethod
         * @param INPUT : identificationMethod method to select the light sources.
         */
        void setIdentificationMethod(QString& identificationMethod);

        /**
         * Setter to change the value of the boolean that saves of the Voronoi diagram to a file.
         * @brief setSaveVoronoiDiagram
         * @param INPUT : save boolean to save the diagram.
         */
        void setSaveVoronoiDiagram(bool save);

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
        void setRelighting(QString &environmentMap, QString &lightType, unsigned int numberOfLightingConditions,
                           unsigned int numberOfOffsets, double exposure, QString identificationMethod, bool save);

        /**
         * Restart the relighting by reinitialising all the variables.
         * @brief clearRelighting
         */
        void clearRelighting();

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
        Voronoi* m_voronoi; /*!< Object that performs the voronoi tesselation*/
        double m_exposure; /*!< Exposure of the final result*/
        QString m_identificationMethod; /*!< Method to select light sources*/
        bool m_saveVoronoi; /*!< Boolean to save the voronoi diagram or not*/
};

#endif // FREEFORMLIGHTSTAGE_H

