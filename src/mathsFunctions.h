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
 * \file mathsFunctions.h
 * \brief Header that contains maths functions
 * \author Antoine Toisoul Le Cann
 * \date August, 7th, 2014
 *
 * Several mathematical functions are implemented, including cartesian to spherical coordinates transformation, modulo with real numbers, clamp...
 */

#ifndef MATHSFUNCTIONS_H_INCLUDED
#define MATHSFUNCTIONS_H_INCLUDED

#define _USE_MATH_DEFINES //for PI

#include <iostream>
#include <cmath>
#include <vector>
#include <cmath>

#include <opencv2/core/core.hpp>
#include <opencv/highgui.h>

/**
 * Function that converts cartesian coordinates to spherical coordinates
 * theta is in the range [0:Pi]
 * phi is in the range [0:2Pi]
 * @param INPUT : x is a const float& corresponding to the x coordinate in the cartesian coordinate system.
 * @param INPUT : y is a const float& corresponding to the y coordinate in the cartesian coordinate system
 * @param INPUT : z is a const float& corresponding to the z coordinate in the cartesian coordinate system
 * @param OUTPUT : r is a float& corresponding to the r coordinate in the spherical coordinate system
 * @param OUTPUT : theta is a float& corresponding to the polar angle theta in the spherical coordinate system
 * @param OUTPUT : phi is a float& corresponding to the azimuthal angle phi in the spherical coordinate system
 * @param INPUT : offset is a const float that adds an offset to the angle phi.
 */
void cartesianToSpherical(const float &x, const float &y, const float &z, float &r, float &theta, float &phi);

/**
* Function that converts cartesian coordinates to spherical coordinates for an entire vector
* theta is in the range [0:Pi]
* phi is in the range [0:2Pi]
* @param INPUT : Each element of cartesianVector is a vector that contains 3 elements : x,y,z
* @param OUTPUT : Each element of sphericalVector is a vector that contains 3 elements : r, phi, theta
* @param INPUT : offset is a const float that adds an offset to the angle phi.
*/
void cartesianToSphericalVector(const std::vector<std::vector<float> > &cartesianVector, std::vector<std::vector<float> > &sphericalVector);

/**
* Function that converts cartesian coordinates to latitude longitude coordinates for an entire vector.
* @param INPUT : Each element of cartesianVector is a vector that contains 3 elements : x,y,z
* @param OUTPUT : Each element of latLongVector is a point2f that corresponds the pixel in the latlong map
* @param INPUT : width is the width of the lat long map
* @param INPUT : height is the height of the lat long map
* @param INPUT : offset is a const float that is added to the angle phi.
*/
void cartesianToLatLongVector2i(const std::vector<std::vector<float> > &cartesianVector, std::vector<cv::Point2i> &latLongVector, const unsigned int width, const unsigned int height);

/**
* Function that converts spherical coordinates to latitude longitude coordinates for an entire vector. The result is stored in OpenCV's Point2f.
* Each point2f corresponds to a pixel in the latlong map.
* @param INPUT : Each element of sphericalVector is a vector that contains 3 elements : r, phi, theta.
* @param OUTPUT : Each element of points2f is a point2f that corresponds the pixel in the latlong map.
* @param INPUT : width is the width of the lat long map.
* @param INPUT : height is the height of the lat long map.
*/
void sphericalToPoint2i(const std::vector<std::vector<float> > &sphericalVector, std::vector<cv::Point2i> &points2i, const unsigned int width, const unsigned int height);

/**
* Compute the maximum value of a vector of floats
* Also outputs its position in the vector. The vector must not be empty.
* @param INPUT : vec is a vector of floats
* @param OUTPUT : maxValue is the maximum value of the vector
* @param OUTPUT : position is an integer that gives the position of the maximum
*/
void maxVector(std::vector<float> &vec, float &maxValue, int &position);

/**
* Divides each element of a vector by its highest value
* @param INPUT : vec is a vector of floats
*/
void normalizeVector(std::vector<float> &vec);

/**
* First compute the sum of each channel : sum of the red channel, sum of the green channel, sum of the blue channel.
* Then divide each element by the max of (sumRed, sumGreen, sumBlue)
* @param INPUT/OUTPUT : Vector of a vector of floats. 3 floats per vector corresponding to the red, green and blue channels
*/
void normalizeWeightsRGB(std::vector<std::vector<float> >& rgbWeights);

/**
* Calculate number mod(modulo) where modulo and number are real numbers
* @param INPUT : number is a float
* @param INPUT : modulo is a float
* @return outputs the result of number mod(modulo)
*/
float moduloRealNumber(float number, float modulo);

/**
 * Clamp a number so it is in the range [inf ; sup].
 * If number<inf, then the function returns inf.
 * If number>sup, then the function returns sup.
 * If number is in the range [inf ; sup] then the function returns the number.
 * @param INPUT : value is a float which will be clamped.
 * @param INPUT : inf is the lower bound of the range.
 * @param INPUT : sup is the upper bound of the range.
 * @return The clamped value in the range [inf ; sup]. If sup < inf then the function returns the number.
 */
float clamp(float value, float inf, float sup);

/**
* Outputs the value of a 2 dimensional Gaussian in (x,y) with mean (meanX, meanY) and variance (varianceX, varianceY).
* @param INPUT : x coordinate of the point in which we want to calculate the Gaussian.
* @param INPUT : y coordinate of the point in which we want to calculate the Gaussian.
* @param INPUT : meanX is the mean of the gaussian in the first dimension.
* @param INPUT : meanY is the mean of the gaussian in the second dimension.
* @param INPUT : varianceX is the variance of the gaussian in the first dimension.
* @param INPUT : varianceY is the mean of the gaussian in the second dimension.
* @return The value of the two dimensional gaussian at point (x,y).
*/
double gaussian2D(double x, double y, double meanX, double meanY, double varianceX, double varianceY);

/**
* Given two opposite points defining a rectangle, the function reorientate the rectangle such as the two opposite points are the upper left corner and the bottom right corner.
* @param INPUT : startingPoint is a Point2i& corresponding to one corner of a rectangle.
* @param INPUT : endingPoint is a Point2i& corresponding to the opposite corner of a rectangle.
* @param OUTPUT : upperLeft is a Point2i& corresponding to the upper left corner of the rectangle.
* @param OUTPUT : bottomRight is a Point2i& corresponding to the bottom right corner of the rectangle.
*/
void reorientateRectangle(cv::Point2i& startingPoint, cv::Point2i& endingPoint, cv::Point2i& upperLeft, cv::Point2i& bottomRight);

/**
 * Given an image, the function computes the 2D probability density function (pdf) and 2D cumulative distribution function (cdf) of the image intensities.
 * @brief compute2DDistributionFunction
 * @param INPUT : image is an OpenCV Mat of floats that contains the image
 * @param INPUT : width of the image
 * @param INPUT : height of the image
 * @param INPUT : numberOfComponents of the image (3 for an RGB image)
 * @param OUTPUT : pdf is the 2D probability density function
 * @param OUTPUT : cdf is the 2D cumulative density function
 */
void compute2DDistributionFunction(const cv::Mat &image, unsigned int& width, unsigned int& height, float* pdf, float* cdf);

#endif // MATHSFUNCTIONS_H_INCLUDED
