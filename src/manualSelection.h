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
 * \file manualSelection.h
 * \brief File that contains the function to manually identify light sources.
 * \author Antoine Toisoul Le Cann
 * \date August, 7th, 2014
 *
 * File that contains the function to manually identify light sources (with OpenCV).
 */

#ifndef MANUALSELECTION_H
#define MANUALSELECTION_H

#include "voronoi.h"

#include <iostream>
#include <string>
#include <sstream>
#include <vector>

#include <opencv2/core/core.hpp>
#include <opencv/highgui.h>
#include <opencv2/imgproc/imgproc.hpp>

struct MouseParameters
{
    std::string environmentMap;/**< The name environment map that is updated when drawing rectangles. */
    unsigned int latLongWidth; /**< Width of the environment map. */
    unsigned int latLongHeight; /**< Height environment map. */
    Voronoi *voronoi; /**< The point light sources and area light sources that are drawn are stored in that lighting basis. */
    int numberOfLightSourcesAdded; /**< The number of light sources added for one picture of the environment. */
    std::string windowName; /**< Name of the window on which the events of the mouse occur.  */
    cv::Point2i startingPoint; /**< Starting point of the rectangle when drawing one.  */
    cv::Point2i endingPoint; /**< Ending point of the rectangle when drawing one.  */
    bool isPressed; /**< Is the right button of the mouse pressed ?  */
};

/**
 * callback function to manually select light sources with OpenCV.
 * @brief callBackMouseEvents
 * @param event the type of event of the mouse.
 * @param x is the x coordinate of the mouse when event occured.
 * @param y is the y coordinate of the mouse when event occured.
 * @param flags flags associated with the mouse event.
 * @param userdata is the data sent to the callback function.
 */
void callBackMouseEvents(int event, int x, int y, int flags, void* userdata);

#endif //MANUALSELECTION_H_INCLUDED
