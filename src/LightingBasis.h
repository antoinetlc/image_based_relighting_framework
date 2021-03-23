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
 * \file LightingBasis.h
 * \brief Lighting basis object
 * \author Antoine Toisoul Le Cann
 * \date May, 29th, 2014
 *
 * The lighting basis object contains all the information about a lighting basis : light sources directions, types of light sources.
 */

#ifndef LIGHTINGBASIS_H
#define LIGHTINGBASIS_H

#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <fstream>
#include <cstdio>

#include <opencv2/core/core.hpp>
#include <opencv/highgui.h>
#include <opencv2/imgproc/imgproc.hpp>

#include "loadFiles.h"
#include "mathsFunctions.h"

class LightingBasis
{
    public:

        /**
        * Constructor by default.
        */
        LightingBasis();

        /**
        * Constructor that initialises a set of point light sources directions given in the pointLightSourcePosition vector.
        * @param INPUT : pointLightSourcePosition contains the positions of point light sources in the environment map.
        */
        LightingBasis(std::vector<cv::Point2i> &pointLightSourcePosition);

        /**
        * Constructor that initialises a set of area light sources directions given the rectangles rectanglesAreaLights vector.
        * @param INPUT : rectanglesAreaLights contains a set of vectors. Each vector has 2 points corresponding to a rectangle (boundary of the point light source).
        */
        LightingBasis(std::vector<std::vector<cv::Point2i> > &rectanglesAreaLights);

        /**
        * Constructor that initialises a set of point light sources and area light sources.
        * @param INPUT : pointLightSourcePosition contains the positions of point light sources in the environment map.
        * @param INPUT : rectanglesAreaLights contains a set of vectors. Each vector has 2 points corresponding to a rectangle (boundary of the point light source).
        */
        LightingBasis(std::vector<cv::Point2i> &pointLightSourcePosition, std::vector<std::vector<cv::Point2i> > &rectanglesAreaLights);

        /**
        * Empty destructor
        */
        virtual ~LightingBasis();

        /**
        * Add a point light source to the basis.
        * @param INPUT : lightPosition is a Point2i containing the position of the light source on the environment map.
        */
        void addPointLight(cv::Point2i lightPosition);

        /**
         * Add several point light sources to the basis.
         * @brief addPointLights
         * @param INPUT : lightPosition is a vector containing the position of the light sources.
         */
        void addPointLights(std::vector<cv::Point2i> &lightPosition);

        /**
        * Add an area light source to the basis represented by a rectangle.
        * @param INPUT : startingPoint is a Point2i containing the position of one corner of the rectangle.
        * @param INPUT : endingPoint is a Point2i containing the position of the opposite corner of the rectangle.
        */
        void addAreaLight(cv::Point2i startingPoint, cv::Point2i endingPoint);

        /**
        * Add several area light sources to the basis represented by rectangles.
        * @param INPUT : areaLights is a vector<vector<Point2i> >. Each element of the vector is a vector representing one area light source and containing the starting point and the ending point of the rectangle.
        */
        void addAreaLight(std::vector<std::vector<cv::Point2i> > areaLights);

        /**
        * Method that paints the point light sources on an image.
        * @param INPUT : img is a OpenCV Mat&. It is the image on which the point light sources will be drawn.
        */
        void paintPointLights(cv::Mat& img);

        /**
        * Method that paints area light sources on an image (rectangles).
        * @param INPUT : img is a OpenCV Mat&. It is the image on which the area light sources will be drawn.
        */
        void paintAreaLights(cv::Mat& img);

        /**
        * Method that transform an area light source into numberOfSamples^2 point light sources uniformly.
        * @param INPUT : numberOfSamples is the number of samples taken in the rectangle.
        */
        void uniformSamplingAreaLightSources(unsigned int envMapWidth, unsigned int envMapHeight, int numberOfSamples);

        /**
         * Method that saves the position of point light sources to a file.
         * @brief saveBasis
         */
        void saveBasis();

        /**
         * Function that reads voronoi.txt file and load its content to the basis (point light source position).
         * @brief loadBasis
         */
        void loadBasis();

        /**
        * Method that writes information about the lighting basis (number of point/area light sources).
        */
        void printBasis();

//SETTERS AND GETTERS
        /**
        * Getter that returns the vector rectanglesAreaLights.
        * @return vector<vector<Point2i> > rectanglesAreaLights.
        */
        std::vector<std::vector<cv::Point2i> > getRectanglesAreaLights();

        /**
        * Getter that returns the number of point light sources in the basis.
        * @return int numberOfPointLights.
        */
        int getNumberOfPointLights();

        /**
        * Getter that returns the number of area light sources in the basis.
        * @return int numberOfAreaLights.
        */
        int getNumberOfAreaLights();

        /**
         * Getter that return the vector containing the position of all the point light sources of the lighting basis.
         * @brief getPointLightSourcePosition
         * @return vector<Point2i> pointLightSourcePosition.
         */
        std::vector<cv::Point2i> getPointLightSourcePosition();

        /**
         * Function that reinitialises each attribute of the basis.
         * @brief clearBasis
         */
        void clearBasis();

    private:
        std::vector<cv::Point2i> pointLightSourcePosition; /*!< A vector containing the position of point light sources in the environment map */
        std::vector<std::vector<cv::Point2i> > rectanglesAreaLights; /*!< A vector containing the position of area light sources in the environment map (rectangles)*/
        bool areAreaLightsSampled; /*!< A boolean variable set to true if the area light sources are sampled into N point light sources*/
        int numberOfPointLights; /*!< The number of point light sources in the basis*/
        int numberOfAreaLights; /*!< The number of area light sources in the basis*/
};

#endif // LIGHTINGBASIS_H
