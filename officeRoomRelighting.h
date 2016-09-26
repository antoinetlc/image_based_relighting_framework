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

#ifndef OFFICEROOMRELIGHTING_H
#define OFFICEROOMRELIGHTING_H

#define _USE_MATH_DEFINES
#define GAMMA 2.2

#include "loadFiles.h"
#include "mathsFunctions.h"
#include "voronoi.h"
#include "imageProcessing.h"
#include "relighting.h"
#include "LightingBasis.h"
#include "optimisation.h"
#include "manualSelection.h"
#include "PFMReadWrite.h"

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

class OfficeRoomRelighting : public Relighting
{
    Q_OBJECT

    public:
       /**
        * Constructor of the OfficeRoomRelighting class.
        * @brief LightStageRelighting
        */
        OfficeRoomRelighting();

        /**
          * Destructor of the OfficeRoomRelighting class.
          */
        ~OfficeRoomRelighting();

        /**
         * Computes the relighting of the object using the office room method.
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
         * Function that stores optimisation weights for a given environment map and offset.
         * Scaling factors computed with 1024x512 environment maps.
         * @brief weightsTableOptimisation
         * @param offset
         * @return
         */
        bool weightsTableOptimisation(int offset);

        /**
         * Saves the Voronoi diagram with each cell painted as its corresponding RGB weight.
         * @brief saveVoronoiWeights
         * @param offset
         */
        void saveVoronoiWeights(int offset);

        /**
         * Method to identify the incoming light directions using the inverse CDF for environment map sampling combined with the k means method.
         * @brief identifyLightsAutomatically
         */
        void identifyLightsAutomatically();

        /**
         * Method to manually select the incoming light directions in the environment map of the office.
         * @brief identifyLightsUser
         * @param INPUT : mouseParameters parameters that are sent to the callback function to manually select light sources.
         */
        void identifyLightsUser(MouseParameters& mouseParameters);

        /**
         * Method that chooses the centroid of the energy as the incoming light direction for each lighting condition.
         * @brief identifyMedianEnergy
         */
        void identifyMedianEnergy();

        /**
         * Method that prepares the basis of the office room before the computation. It removes the indirect lighting and the overlaps.
         * @brief prepareBasis_office
         */
        void prepareBasis_office();

        /**
         * Method that prepares the basis of the bedroom before the computation. It removes the indirect lighting and the overlaps.
         * @brief prepareBasis_office
         */
        void prepareBasis_bedroom();

        /**
         * Method that computes the residual mask associated with the picture of the dark room.
         * @brief prepareMasks
         */
        void prepareMasks();

        /**
         * Method to prepare the Reflectance field before the relighting computation (office room).
         * @brief prepareReflectanceField_office
         */
        void prepareReflectanceField_office();

        /**
         * Method to prepare the Reflectance field before the relighting computation (bedroom).
         * @brief prepareReflectanceField_office
         */
        void prepareReflectanceField_bedroom();

        /**
         * Normalize the energy of each picture in the reflectance field to 1.0.
         * @brief normalizeEnergyBasis
         * @param reflectanceField
         */
        void normalizeEnergyBasis(cv::Mat reflectanceField[]);

        /**
         * Method to compute the weights using the masks.
         * @brief computeWeightsMasks
         * @param INPUT : environmentMap is an OpenCV Mat of floats containing the HDR values of the environment map.
         * @param INPUT : offset is a float corresponding to the offset that is added to the rotation of the environment map (phi angle).
         * @return the weights of each lighting condition as a vector<vector<float> >. vector[i] is a vector containing three values R, G, B that corresponds to the weights for each color channel.
         */
        std::vector<std::vector<float> > computeWeightsMasks(cv::Mat &environmentMap, const float offset);

        /**
         * Sets the room and the mask types.
         * @brief setMaskAndRoomTypes
         */
        void setMaskAndRoomTypes();

        /**
         * Setter that sets the method to select light sources.
         * @brief setIdentificationMethod
         * @param INPUT : identificationMethod method to select the light sources.
         */
        void setIdentificationMethod(QString& identificationMethod);

        /**
         * Setter to change the number of samples in the environment map sampling (inverse CDF algorithm).
         * @brief setNumberOfSamplesInverseCDF
         * @param INPUT : numberOfSamplesInverseCDF is the number of samples.
         */
        void setNumberOfSamplesInverseCDF(unsigned int numberOfSamplesInverseCDF);

        /**
         * Setter to change the number of the picture containing the indirect light.
         * @brief setIndirectLightPicture
         * @param INPUT : indirectLightPicture
         */
        void setIndirectLightPicture(unsigned int indirectLightPicture);

        /**
         * Setter to change the exposure of the final result.
         * @brief setExposure
         * @param INPUT : exposure in f-stops. The exposure of the final result is increased by a value of exposure f-stops.
         */
        void setExposure(double exposure);

        /**
         * Setter to change the optimisation method for the environment map.
         * @brief setOptimisationMethod
         * @param INPUT : optimisationMethod
         */
        void setOptimisationMethod(QString optimisationMethod);

        /**
         * Setter to change the type of masks used for the relighting.
         * @brief setMasksType
         * @param INPUT : maskType
         */
        void setMasksType(QString maskType);

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
        void setRelighting(QString &object, QString &environmentMap, QString &lightType, unsigned int numberOfLightingConditions,
                           unsigned int numberOfOffsets, QString identificationMethod, QString masksType, QString optimisationMethod, unsigned int numberOfSamplesInverseCDF,
                           unsigned int indirectLightPicture, bool computeBasisMasks, double exposure);

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
        Voronoi* m_voronoi;/*!< Object that performs the voronoi tesselation*/

        //Office Room relighting
        std::string m_roomType;
        unsigned int m_indirectLightPicture; /*!< Number of the picture of the dark room*/
        QString m_identificationMethod; /*!< Method to select light sources*/ //Manual, Median Energy, Inverse CDF, Masks
        QString m_masksType; /*!< Type of masks used : adapted to high or low frequency lighting*/
        QString m_optimisationMethod; /*!< Optimisation method*/
        unsigned int m_numberOfSamplesInverseCDF; /*!< Number of samples used in the environment map sampling (see identifyLightsAutomatically)*/
        bool m_computeBasisMasks;
        double m_exposure; /*!< Exposure of the final result*/

};

#endif // OFFICEROOMRELIGHTING_H
