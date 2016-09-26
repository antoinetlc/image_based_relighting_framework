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
 * \file optimisation.h
 * \brief Class to perform the optimisation process.
 * \author Antoine Toisoul Le Cann
 * \date August, 7th, 2014
 *
 * Class that performs the optimisation process in original space or PCA space.
 * Given a piecewise constant basis that corresponds to an approximation of an environment map,
 * an instantiation of this class associate a weight to each constant of the basis to minimize the error between this basis and the original environment map.
 * This class uses DLib library.
 */

#ifndef OPTIMISATION_H
#define OPTIMISATION_H

#define _USE_MATH_DEFINES

#include <dlib/optimization.h>

#include <cmath>
#include <iostream>
#include <vector>
#include <string>

#include <opencv2/core/core.hpp>
#include <opencv/highgui.h>
#include <opencv2/imgproc/imgproc.hpp>

#include <QApplication>

#include "PFMReadWrite.h"

//Column Vector used with dlib library
typedef dlib::matrix<double,0,1> column_vector;

class Optimisation
{
    public:
        //Use std::vector to avoid conflict between std library and dlib
        /**
         * Default constructor to initialise
         * @brief Optimisation
         */
        Optimisation();

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
        Optimisation(std::string environmentMapName, unsigned int environmentMapWidth, unsigned int environmentMapHeight,
                     unsigned int numberOfComponents, unsigned int numberOfLightingConditions,
                     unsigned int indirectLightPicture, float offset,
                     std::string roomType,
                     std::string masksType, std::vector<std::vector<float> >& rgbWeights);

        /**
         * Method that performs the optimisation process in the original space.
         * @brief environmentMapOptimisation
         * @param INPUT : startingPointArray starting point of the optimisation process (starting point of the function to minimise).
         */
        void environmentMapOptimisation(double startingPointArray[]);

        /**
         * Method that performs the optimisation process in PCA space.
         * @brief environmentMapOptimisation
         * @param INPUT : startingPointArray starting point of the optimisation process (starting point of the function to minimise).
         */
        void environmentMapPCAOptimisation(double startingPointArray[]);

        /**
         * Method that compute the PCA of the environment map and the matrix to project a vector into the PCA space.
         * @brief computePCAMatrix
         */
        void computePCAMatrix();

        /**
         * Method that sets the global variables required for the function to optimise.
         * @brief setGlobalVariables
         */
        void setGlobalVariables();

        /**
         * Method that return the width of the environment map.
         * @brief getEnvironmentMapWidth
         * @return width of the environment map.
         */
        unsigned int getEnvironmentMapWidth();

        /**
         * Method that returns the height of the environment map.
         * @brief getEnvironmentMapHeight
         * @return height of the environment map.
         */
        unsigned int getEnvironmentMapHeight();

        /**
         * Method that returns the number of components of the environment map.
         * @brief getNumberOfComponents
         * @return number of components of the environment map.
         */
        unsigned int getNumberOfComponents();

        /**
         * Method that return the number of lighting conditions used.
         * @brief getNumberOfLightingConditions
         * @return number of lighting conditions.
         */
        unsigned int getNumberOfLightingConditions();

        /**
         * Method that returns the number of the picture that contains the indirect light.
         * @brief getIndirectLightPicture
         * @return  number of the picture that contains the indirect light.
         */
        unsigned int getIndirectLightPicture();

        /**
         * Method that returns the offset (rotation around the phi angle) of the environment map.
         * @brief getOffset
         * @return the offset (rotation around the phi angle) of the environment map.
         */
        float getOffset();

        /**
         * Method that returns the RGB weights of each lighting condition.
         * @brief getRGBWeights
         * @return the RGB weights of each lighting condition.
         */
        std::vector<std::vector<float> > getRGBWeights();

    private:
        std::string m_environmentMapName; /*!< Name of the environment map*/
        unsigned int m_environmentMapWidth; /*!< Width of the environment map*/
        unsigned int m_environmentMapHeight; /*!< Height of the environment map*/
        unsigned int m_numberOfComponents; /*!< Number of components of the environment map*/
        unsigned int m_numberOflightingConditions; /*!< Number of lighting conditions*/
        unsigned int m_indirectLightPicture; /*!< Number of the picture that contains the indirect light*/
        float m_offset; /*!< Offset (rotation around the phi angle) of the environment map*/
        std::string m_roomType; /*!< Type of room used : office or bedroom*/
        std::string m_masksType; /*!< Type of mask used : adapted to high or low frequency lighting*/
        std::vector<std::vector<float> > m_rgbWeights; /*!< RGB weights of each lighting condition*/


};

/**
 * Function that has to be optimised (original space).
 * @brief functionToOptimise
 * @param variablesVector column vector containing the value of the variables that are being optimised.
 * @return the value of the function.
 */
double functionToOptimise(const column_vector &variablesVector);

/**
 * Function that has to be optimised (PCA space).
 * @brief functionToOptimisePCASpace
 * @param variablesVector column vector containing the value of the variables that are being optimised.
 * @return the value of the function.
 */
double functionToOptimisePCASpace(const column_vector &variablesVector);

#endif // OPTIMISATION_H
