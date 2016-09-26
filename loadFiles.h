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
 * \file loadFiles.h
 * \brief Header that contains the functions that read inputs (images, text files...)
 * \author Antoine Toisoul Le Cann
 * \date May, 23rd, 2014
 *
 * The functions are used to read the input files. Pictures are stored into OpenCV matrices Mat.
 */

#ifndef LOADFILES_H_INCLUDED
#define LOADFILES_H_INCLUDED

#include "imageProcessing.h"

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>

#include <opencv2/core/core.hpp>
#include <opencv/highgui.h>

/**
* This function loads the pictures corresponding to the reflectance field of an object.
* @param listOfImages is an array of OpenCV Mat object. Each element of the array is a picture of the reflectance field.
* @param numberOfImages is an integer corresponding to the number of pictures in the reflectance field.
* @param object is a string which describes the reflectance field to load. It can be either 'plant' or 'helmet'.
* @return returns EXIT_SUCCESS or EXIT_FAILURE if the files cannot be loaded.
*/
int loadImages(cv::Mat listOfImages[], unsigned int numberOfImages, std::string object);

/**
* This function reads txt files that have the following format :
* lightNumber1: valueR valueG valueB
* ...
* lightNumberN: valueR valueG valueB
* @param INPUT : fileName a string containing the filename
* @param OUTPUT : components a vector of a vector of floats. components[i] is a vector of floats containing the 3 values corresponding to the red, green and blue components.
*/
void readFile(const std::string& fileName, std::vector<std::vector<float> > &components);

#endif // LOADFILES_H_INCLUDED
