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
 * \file manualSelection.cpp
 * \brief File that contains the function to manually identify light sources.
 * \author Antoine Toisoul Le Cann
 * \date August, 7th, 2014
 *
 * File that contains the function to manually identify light sources (with OpenCV).
 */

#include "manualSelection.h"

using namespace std;
using namespace cv;

/**
 * callback function to manually select light sources with OpenCV.
 * @brief callBackMouseEvents
 * @param event the type of event of the mouse.
 * @param x is the x coordinate of the mouse when event occured.
 * @param y is the y coordinate of the mouse when event occured.
 * @param flags flags associated with the mouse event.
 * @param userdata is the data sent to the callback function.
 */
void callBackMouseEvents(int event, int x, int y, int flags, void* userdata)
{
    MouseParameters* parameters = static_cast<MouseParameters*>(userdata);
    int width = 0, height = 0;

    //Reload the environment map for rectangle animation when moving the mouse
    Mat environmentMap = loadPFM(parameters->environmentMap);

    //When the right button of the mouse is pressed, we are drawing a rectangle. Hence the ending point of the rectangle is updated
    if(parameters->isPressed)
    {
        if((x>0) && (y>0) && (x<(parameters->latLongWidth)) && (y<(parameters->latLongHeight))) //If the coordinates are positive. Otherwise the program crashes.
        {
            parameters->endingPoint = Point2i(x,y);
            width = abs(parameters->endingPoint.x - parameters->startingPoint.x);
            height = abs(parameters->endingPoint.y - parameters->startingPoint.y);

            cv::rectangle(environmentMap, parameters->startingPoint, parameters->endingPoint, Scalar(255,0,0), 3, 8, 0);
            imshow(parameters->windowName, environmentMap);
        }

    }

    //Events of the mouse
    switch(event)
    {
        case EVENT_LBUTTONDOWN:
            parameters->startingPoint = Point2i(x,y);
            (parameters->voronoi)->addPointLight(parameters->startingPoint); //Add a point light source to the basis
            parameters->numberOfLightSourcesAdded++; //Increase the number of point light sources added
            (parameters->voronoi)->paintPointLights(environmentMap); //Paint the point light source on the image displayed
            (parameters->voronoi)->paintAreaLights(environmentMap); //Paint the area light source on the image displayed
            imshow(parameters->windowName, environmentMap); //Update the image displayed
            break;
        case EVENT_RBUTTONDOWN:
            parameters->startingPoint = Point2i(x,y); //The first time that the right button of the mouse is down : we start drawing a rectangle
            parameters->endingPoint = Point2i(x,y);
            parameters->isPressed = true;
            break;
        case EVENT_RBUTTONUP:
            parameters->isPressed = false;
            if((x>0) && (y>0) && (x<(parameters->latLongWidth)) && (y<(parameters->latLongHeight))) //If the coordinates are positive. Otherwise the program crashes.
            {
                parameters->endingPoint = Point2i(x,y); //When the right button is released
            }
            (parameters->voronoi)->addAreaLight(parameters->startingPoint, parameters->endingPoint); //Add an area light source to the basis
            parameters->numberOfLightSourcesAdded++; //Increase the number of point light sources added
            (parameters->voronoi)->paintAreaLights(environmentMap); //Paint the area light source on the image displayed
            (parameters->voronoi)->paintPointLights(environmentMap);
            imshow(parameters->windowName, environmentMap); //Update the image displayed
            break;

    }
}
