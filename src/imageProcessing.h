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
 * \file imageProcessing.h
 * \brief File that contains the functions that are related to image processing
 * \author Antoine Toisoul Le Cann
 * \date August, 7th, 2014
 *
 * The functions are related to image processing using OpenCV or not.
 */

#ifndef IMAGEPROCESSING_H_INCLUDED
#define IMAGEPROCESSING_H_INCLUDED

#define _USE_MATH_DEFINES //for PI

#include <iostream>
#include <cmath>
#include <vector>
#include <cmath>
#include <opencv2/core/core.hpp>
#include <opencv/highgui.h>

#include <QApplication>

#include "mathsFunctions.h"
#include "PFMReadWrite.h"
#include "loadFiles.h"


/**
 * Apply a gamma correction to a RGB image (OpenCV Mat image).
 * @param INPUT : rgbImage is the image to which the gamma correction is applied.
 * @param OUTPUT : rgbImageWithGamma is the rgbImage with the gamma applied.
 * @param INPUT : gamma is a double corresponding to the value of the gamma correction.
 */
void gammaCorrectionImage(const cv::Mat &rgbImage, cv::Mat &rgbImageWithGamma, double gamma);

/**
 * Apply a gamma correction to a RGB image stored in an array of floats.
 * @param INPUT : image is the image to which the gamma correction is applied. It is an array of floats.
 * @param INPUT : width is the width of the image.
 * @param INPUT : height is the height of the image.
 * @param INPUT : numberOfComponents is the number of components of the image (3 for a RGB image).
 * @param INPUT : gamma is a float corresponding to the value of the gamma correction.
 */
void gammaCorrectionImage(float* image, unsigned int& width, unsigned int& height, unsigned int& numberOfComponents,const float& gamma);

/**
 * Remove the gamma correction of a RGB image (OpenCV Mat image).
 * @param INPUT : rgbImage is the image to which the gamma correction is removed. rgbImage is an OpenCV CV_8UC3 matrix. (matrix of 3 channels of 8 bits unsigned char).
 * @param OUTPUT : rgbImageWithGamma is the rgbImage with the gamma removed. It is a CV_32FC3 matrix (matrix of 3 channels of 32 bits floats).
 * @param INPUT : gamma is a double corresponding to the value of the gamma correction.
 */
void removeGammaCorrection(const cv::Mat &rgbImage, cv::Mat &rgbImageWithoutGamma, double gamma);

/**
 * Calculates a linear combination of the images given in the array images using the weights given in weightsRGB (for each channel).
 * @brief linearCombinationRGB
 * @param images is an array of OpenCV Mat object (CV_32FC3).
 * @param weightsRGB the weights per channel that are used in the linear combination. weightsRGB[i] is a vector containing the weights of image i. weightsRGB[i][k] is the weight for the each channel (k = 0 red ; k = 1 green ; k = 2 blue).
 * @param result is the result of the linear combination.
 */
void linearCombinationRGB(cv::Mat images[], int numberOfImages, std::vector<std::vector<float> > weightsRGB, cv::Mat& result);

/**
 * Apply a 2D gaussian blur to an image.
 * @brief gaussianBlur2D
 * @param INPUT : image is an array of floats that contains the image to which the gaussian blur is applied.
 * @param INPUT : width of the image.
 * @param INPUT : height of the image.
 * @param INPUT : numberOfComponents in the image (3 for a RGB image).
 * @param INPUT : blurSize is the size of the patch used for the Gaussian blur.
 * @param OUTPUT : imageBlur is the blurred image.
 */
void gaussianBlur2D(float* image, unsigned int& width, unsigned int& height, unsigned int& numberOfComponents ,int blurSize, float* imageBlur);

/**
 * Given a 2D function and a value, the function finds resultI and resultJ such as value = f(resultI, resultJ)
 * @brief inverse2DFunction
 * @param value is the value of the function.
 * @param f is an array of floats that correspond to a 2D function.
 * @param width of the f function.
 * @param height of the f function
 * @param resultI is the first component of the result.
 * @param resultJ is the second component of the result.
 */
void inverse2DFunction(float& value, float f[], unsigned int& width, unsigned int& height, unsigned int& resultI, unsigned int& resultJ);

/**
 * Function that applies the inverse CDF algorithm in order to sample an environment map according to the points of high energy.
 * @brief inverseCDFAlgorithm
 * @param INPUT : environmentMap is an array OpenCV Mat of floats that contains the environment map to which the algorithm is applied.
 * @param INPUT : width of the environment map.
 * @param INPUT : height of the environment map.
 * @param INPUT : numberOfComponents of the environment map (3 for a RGB image).
 * @param INPUT : numberOfSamples is the number of samples that are used to sample the environment map.
 * @param OUTPUT : samplesLocation is an OpenCV Mat object. Each row of the matrix contains the location of one sample.
 */
void inverseCDFAlgorithm(const cv::Mat &environmentMap, unsigned int& width, unsigned int& height, unsigned int& numberOfComponents, unsigned int& numberOfSamples, cv::Mat &samplesLocation);

/**
 * Function that paints green points on an image at specific locations.
 * @brief paintSamples
 * @param INPUT/OUTPUT : image on which the green points are painted. It is an OpenCV Mat of floats.
 * @param INPUT : width of the image.
 * @param INPUT : height of the image.
 * @param INPUT : samplesLocation is a matrix that contains the location of the points. Each row corresponds to a point (x,y) that will be painted.
 */
void paintSamples(cv::Mat &image, unsigned int& width, unsigned int& height, cv::Mat &samplesLocation);

/**
 * Function that rotates a latitude longitude environment map along the y axis (adds an offset to the phi angle).
 * @brief rotateLatLongMap
 * @param INPUT : originalMap is the original environment map.
 * @param INPUT : offset is the offset that is added to the phi angle.
 * @param OUTPUT : result is the result of the rotated environment map.
 */
void rotateLatLongMap(const cv::Mat& originalMap, const float offset, cv::Mat& result);

/**
 * Function that reads images (hardcoded name) and crops them in the rectangle defined by (xStart,yStart) and (xEnd,yEnd).
 * @brief cropImages
 * @param INPUT : xStart is the abscissa of the upper right point of the rectangle.
 * @param INPUT : yStart is the ordinate of the upper right point of the rectangle.
 * @param INPUT : xEnd is the abscissa of the upper bottom left of the rectangle.
 * @param INPUT : yEnd is the ordinate of the upper bottom left of the rectangle.
 * @param INPUT : numberOfImages is the number of images to be cropped.
 */
void cropImages(int xStart, int yStart, int xEnd, int yEnd, int numberOfImages);

/**
 * Function that crops the mirror balls in each picture taken for the office room relighting
 * Then it takes all the mirror balls and assemble them in a grid
 * @brief cropMirrorBallToMatrix
 */
void cropMirrorBallToMatrix();


#endif // IMAGEPROCESSING_H_INCLUDED
