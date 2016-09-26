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
 * \file mathsFunctions.cpp
 * \brief Header that contains maths functions
 * \author Antoine Toisoul Le Cann
 * \date August, 7th, 2014
 *
 * Several mathematical functions are implemented, including cartesian to spherical coordinates transformation, modulo with real numbers, clamp...
 */

#include "mathsFunctions.h"

using namespace std;
using namespace cv;

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
void cartesianToSpherical(const float &x, const float &y, const float &z, float &r, float &theta, float &phi)
{
    r = sqrt(x*x + y*y + z*z);
    //theta = M_PI-acos(y/r);
    theta = acos(y/r);
    //atan2 compute the the correct value for phi depending on the quadrant (x,z) are in
    //The value return is in the range -Pi ; Pi. Hence Pi is added for the result to be in the range 0 2Pi.
    phi = moduloRealNumber(atan2(x, z), 2.0*M_PI); //Phi is between 0 and 2*PI
}

/**
* Function that converts cartesian coordinates to spherical coordinates for an entire vector
* theta is in the range [0:Pi]
* phi is in the range [0:2Pi]
* @param INPUT : Each element of cartesianVector is a vector that contains 3 elements : x,y,z
* @param OUTPUT : Each element of sphericalVector is a vector that contains 3 elements : r, phi, theta
* @param INPUT : offset is a const float that adds an offset to the angle phi.
*/
void cartesianToSphericalVector(const vector<vector<float> > &cartesianVector, vector<vector<float> > &sphericalVector)
{

    float r = 0.0;
    float theta = 0.0;
    float phi = 0.0;

    sphericalVector.clear();
    for(unsigned int i = 0 ; i<cartesianVector.size() ; i++)
    {
        vector<float> currentSpherical;
        cartesianToSpherical(cartesianVector[i][0],cartesianVector[i][1],cartesianVector[i][2],r,theta,phi);
        currentSpherical.push_back(r);
        currentSpherical.push_back(phi);
        currentSpherical.push_back(theta);
        sphericalVector.push_back(currentSpherical);
    }
}

/**
* Function that converts cartesian coordinates to latitude longitude coordinates for an entire vector.
* @param INPUT : Each element of cartesianVector is a vector that contains 3 elements : x,y,z
* @param OUTPUT : Each element of latLongVector is a point2f that corresponds the pixel in the latlong map
* @param INPUT : width is the width of the lat long map
* @param INPUT : height is the height of the lat long map
* @param INPUT : offset is a const float that is added to the angle phi.
*/
void cartesianToLatLongVector2i(const vector<vector<float> > &cartesianVector, vector<Point2i> &latLongVector, const unsigned int width, const unsigned int height)
{

    float r = 0.0;
    float theta = 0.0;
    float phi = 0.0;

    latLongVector.clear();
    for(unsigned int i = 0 ; i<cartesianVector.size() ; i++)
    {
        vector<Point2i> currentSpherical;

        cartesianToSpherical(cartesianVector[i][0],cartesianVector[i][1],cartesianVector[i][2],r,theta,phi);
        Point2i currentPoint(floor((float) width*phi/(2.0*M_PI)), floor((float) height*theta/M_PI));
        latLongVector.push_back(currentPoint);
    }
}

/**
* Compute the maximum value of a vector of floats
* Also outputs its position in the vector. The vector must not be empty.
* @param INPUT : vec is a vector of floats
* @param OUTPUT : maxValue is the maximum value of the vector
* @param OUTPUT : position is an integer that gives the position of the maximum
*/
void maxVector(vector<float> &vec, float &maxValue, int &position)
{

    float maxSoFar = 0.0;
    for(unsigned int i = 0 ; i< vec.size() ; i++)
    {
        if(vec[i] > maxSoFar)
        {
            maxSoFar = vec[i];
            position = i;
        }
    }

    maxValue = maxSoFar;
}

/**
* Divides each element of a vector by its highest value
* @param INPUT : vec is a vector of floats
*/
void normalizeVector(vector<float> &vec)
{
    float maxValue = 0.0;
    int position = 0;
    maxVector(vec, maxValue, position);

    for(unsigned int i = 0 ; i< vec.size() ; i++)
    {
        vec[i] /= maxValue;
    }
}

/**
* First compute the sum of each channel : sum of the red channel, sum of the green channel, sum of the blue channel.
* Then divide each element by the max of (sumRed, sumGreen, sumBlue)
* @param INPUT/OUTPUT : Vector of a vector of floats. 3 floats per vector corresponding to the red, green and blue channels
*/
void normalizeWeightsRGB(vector<vector<float> >& rgbWeights)
{

    float sumR = 0.0;
    float sumG = 0.0;
    float sumB = 0.0;
    float sum = 0.0;

    //Divide R, G,B values by the same amount to avoid a shift in the colors
    for(unsigned int i = 0 ; i< rgbWeights.size() ; i++)
    {
        sumR += rgbWeights[i][0];
        sumG += rgbWeights[i][1];
        sumB += rgbWeights[i][2];
    }

    sum = max(sumR, max(sumG, sumB));

    //Normalize by sum
    if(sum != 0)
    {
        for(unsigned int i = 0 ; i< rgbWeights.size() ; i++)
        {
            rgbWeights[i][0]/=sum;
            rgbWeights[i][1]/=sum;
            rgbWeights[i][2]/=sum;
        }
    }
}

/**
* Calculate number mod(modulo) where modulo and number are real numbers
* @param INPUT : number is a float
* @param INPUT : modulo is a float
* @return outputs the result of number mod(modulo)
*/
float moduloRealNumber(float number, float modulo)
{
    int quotient = 0;
    float result = 0.0;

    quotient = floor(number/modulo);
    result = number-quotient*modulo;

    return result;
}

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
float clamp(float value, float inf, float sup)
{
	float result = 0.0f;

    if(inf<=sup)
    {
        if(value < inf)
            result = inf;
        else if(value > sup)
            result = sup;
        else
            result = value;
    }
    else
    {
        cerr << "sup must be greater than inf !" << endl;
        return value;
    }

    return result;
}

/**
* Outputs the value of a 2 dimensional Gaussian in (x,y) with mean (meanX, meanY) and variance (varianceX, varianceY).
* Return exp(-(x-meanX)^2/(2*varianceX)-(y-meanY)^2/(2*varianceY))
* @param INPUT : x coordinate of the point in which we want to calculate the Gaussian.
* @param INPUT : y coordinate of the point in which we want to calculate the Gaussian.
* @param INPUT : meanX is the mean of the gaussian in the first dimension.
* @param INPUT : meanY is the mean of the gaussian in the second dimension.
* @param INPUT : varianceX is the variance of the gaussian in the first dimension.
* @param INPUT : varianceY is the mean of the gaussian in the second dimension.
* @return The value of the two dimensional gaussian at point (x,y).
*/
double gaussian2D(double x, double y, double meanX, double meanY, double varianceX, double varianceY)
{
     double result = exp(-(x-meanX)*(x-meanX)/(2*varianceX)-(y-meanY)*(y-meanY)/(2*varianceY));
     return result;
}

/**
* Given two opposite points defining a rectangle, the function reorientate the rectangle such as the two opposite points are the upper left corner and the bottom right corner.
* @param INPUT : startingPoint is a Point2i& corresponding to one corner of a rectangle.
* @param INPUT : endingPoint is a Point2i& corresponding to the opposite corner of a rectangle.
* @param OUTPUT : upperLeft is a Point2i& corresponding to the upper left corner of the rectangle.
* @param OUTPUT : bottomRight is a Point2i& corresponding to the bottom right corner of the rectangle.
*/
void reorientateRectangle(Point2i& startingPoint, Point2i& endingPoint, Point2i& upperLeft, Point2i& bottomRight)
{
        if((startingPoint.x<endingPoint.x) && (startingPoint.y<endingPoint.y))//starting point = upper left ; ending point = bottom right ;
        {
            upperLeft = startingPoint;
            bottomRight = endingPoint;
        }
        else if((startingPoint.x>endingPoint.x) && (startingPoint.y<endingPoint.y)) //starting point = upper right ; ending point = bottom left ;
        {
            upperLeft.x = endingPoint.x;
            upperLeft.y = startingPoint.y;
            bottomRight.x = startingPoint.x;
            bottomRight.y = endingPoint.y;
        }
        else if((startingPoint.x<endingPoint.x) && (startingPoint.y>endingPoint.y)) //starting point = bottom left ; ending point = upper right ;
        {
            upperLeft.x = startingPoint.x;
            upperLeft.y = endingPoint.y;
            bottomRight.x = endingPoint.x;
            bottomRight.y = startingPoint.y;
        }
        else //if((startingPoint.x>endingPoint.x) && (startingPoint.y>endingPoint.y)) //starting point = bottom right ; ending point = upper left ;
        {
            upperLeft.x = endingPoint.x;
            upperLeft.y = endingPoint.y;
            bottomRight.x = startingPoint.x;
            bottomRight.y = startingPoint.y;
        }
}

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
void compute2DDistributionFunction(const Mat &image, unsigned int& width, unsigned int& height, float* pdf, float* cdf)
{
    float intensity = 0.0f;
	float pixelR = 0.0f;
	float pixelG = 0.0f;
	float pixelB = 0.0f;
    float normalisedConstant = 0.0f;

    //Compute the 2D probability density function (pdf) of the image (taking into account the solid angle)
	for(unsigned int i = 0 ; i<height ; i++)
	{
		for(unsigned int j = 0 ; j<width ; j++)
		{
            //OpenCV uses BGR
            pixelR = image.at<Vec3f>(i,j).val[2];
            pixelG = image.at<Vec3f>(i,j).val[1];
            pixelB = image.at<Vec3f>(i,j).val[0];
			intensity = (pixelR+pixelG+pixelB)/3.0;

			// i = height is PI
			if(!(isnan(pixelR) && isnan(pixelG) && isnan(pixelB)))
			{
				pdf[i*width+j] = intensity * sin(i*M_PI/height);
				normalisedConstant += pdf[i*width+j];
			}
			else
			{
				pdf[i*width+j] = 0.0;
			}
		}

	}

    //Normalise the 2D probability density function so it sums to one
    //Also compute the cumulative distribution function given the pdf
	if(normalisedConstant != 0.0)
	{
		for(unsigned int i = 0 ; i<height ; i++)
		{
			for(unsigned int j = 0 ; j<width ; j++)
			{
				pdf[i*width+j] /= normalisedConstant; //Create a probability distribution

				if((i == 0) && (j == 0))
					cdf[0] = pdf[0]; //Create the cdf function
				else
					cdf[i*width+j] = cdf[i*width+j-1] + pdf[i*width+j]; //Create the cdf function
			}
		}
	}

}

