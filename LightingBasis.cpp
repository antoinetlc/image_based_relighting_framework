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
 * \file LightingBasis.hpp
 * \brief Lighting basis object
 * \author Antoine Toisoul Le Cann
 * \date May, 29th, 2014
 *
 * The lighting basis object contains all the information about a lighting basis : light sources directions, types of light sources.
 */

#include "LightingBasis.h"

using namespace std;
using namespace cv;


/**
* Constructor by default.
*/
LightingBasis::LightingBasis() :  pointLightSourcePosition(vector<Point2i>()), rectanglesAreaLights(vector<vector<Point2i> >() ), areAreaLightsSampled(false), numberOfPointLights(0), numberOfAreaLights(0)
{

}

/**
* Constructor that initialises a set of point light sources directions given in the pointLightSourcePosition vector.
* @param INPUT : pointLightSourcePosition contains the positions of point light sources in the environment map.
*/
LightingBasis::LightingBasis(vector<Point2i> &pointLightSourcePosition) : pointLightSourcePosition(pointLightSourcePosition),
rectanglesAreaLights(), areAreaLightsSampled(false), numberOfPointLights(pointLightSourcePosition.size()), numberOfAreaLights(0)
{

}

/**
* Constructor that initialises a set of area light sources directions given the rectangles rectanglesAreaLights vector.
* @param INPUT : rectanglesAreaLights contains a set of vectors. Each vector has 2 points corresponding to a rectangle (boundary of the point light source).
*/
LightingBasis::LightingBasis(vector<vector<Point2i> > &rectanglesAreaLights) : pointLightSourcePosition(), rectanglesAreaLights(rectanglesAreaLights),
areAreaLightsSampled(false), numberOfPointLights(0), numberOfAreaLights(rectanglesAreaLights.size())
{

}

/**
* Constructor that initialises a set of point light sources and area light sources.
* @param INPUT : pointLightSourcePosition contains the positions of point light sources in the environment map.
* @param INPUT : rectanglesAreaLights contains a set of vectors. Each vector has 2 points corresponding to a rectangle (boundary of the point light source).
*/
LightingBasis::LightingBasis(vector<Point2i> &pointLightSourcePosition, vector<vector<Point2i> > &rectanglesAreaLights) :
pointLightSourcePosition(pointLightSourcePosition), rectanglesAreaLights(rectanglesAreaLights), areAreaLightsSampled(false), numberOfPointLights(pointLightSourcePosition.size()),
numberOfAreaLights(rectanglesAreaLights.size())
{

}

/**
* Empty destructor
*/
LightingBasis::~LightingBasis()
{

}

/**
* Add a point light source to the basis.
* @param INPUT : lightPosition is a Point2i containing the position of the light source on the environment map.
*/
void LightingBasis::addPointLight(Point2i lightPosition)
{
        pointLightSourcePosition.push_back(lightPosition);
        numberOfPointLights++;
}

/**
 * Add several point light sources to the basis.
 * @brief addPointLights
 * @param INPUT : lightPosition is a vector containing the position of the light sources.
 */
void LightingBasis::addPointLights(vector<Point2i> &lightPosition)
{
    for(unsigned int i = 0 ; i<lightPosition.size() ; i++)
    {
        pointLightSourcePosition.push_back(lightPosition[i]);
        numberOfPointLights++;
    }
}

/**
* Add an area light source to the basis represented by a rectangle.
* @param INPUT : startingPoint is a Point2i containing the position of one corner of the rectangle.
* @param INPUT : endingPoint is a Point2i containing the position of the opposite corner of the rectangle.
*/
void LightingBasis::addAreaLight(Point2i startingPoint, Point2i endingPoint)
{
        vector<Point2i> newRectangle;
        Point2i center = (startingPoint+endingPoint)*0.5;

        newRectangle.push_back(startingPoint);
        newRectangle.push_back(endingPoint);

        rectanglesAreaLights.push_back(newRectangle);
        numberOfAreaLights++;

        //Add the center to a point light source
        pointLightSourcePosition.push_back(center);
        numberOfPointLights++;
}

/**
* Add several area light sources to the basis represented by rectangles.
* @param INPUT : areaLights is a vector<vector<Point2i> >. Each element of the vector is a vector representing one area light source and containing the starting point and the ending point of the rectangle.
*/
void LightingBasis::addAreaLight(vector<vector<Point2i> > areaLights)
{
    for(unsigned int i = 0 ; i<areaLights.size() ; i++)
    {
        Point2i startingPoint = areaLights[i][0];
        Point2i endingPoint = areaLights[i][1];
        this->addAreaLight(startingPoint,endingPoint);
    }
}


/********************
 *Painting functions*
 ********************/

/**
* Method that paints the point light sources on an image.
* @param INPUT : img is a OpenCV Mat&. It is the image on which the point light sources will be drawn.
*/
void LightingBasis::paintPointLights(Mat& img)
{
    for(unsigned int i = 0; i<pointLightSourcePosition.size(); i++ )
    {
        //Opencv stores colors in BGR format
        circle(img, pointLightSourcePosition[i], 4, Scalar(0,0,255), -1, 8, 0);
    }
}

/**
* Method that paints area light sources on an image (rectangles).
* @param INPUT : img is a OpenCV Mat&. It is the image on which the area light sources will be drawn.
*/
void LightingBasis::paintAreaLights(Mat& img)
{
    for(unsigned int i = 0; i<rectanglesAreaLights.size(); i++ )
    {

        Point2i startingPoint = rectanglesAreaLights[i][0];
        Point2i endingPoint = rectanglesAreaLights[i][1];

        //Opencv stores colors in BGR format
        rectangle(img, startingPoint, endingPoint, Scalar(255,0,0), 3, 8, 0);
    }
}

/**
* Method that transform an area light source into numberOfSamples^2 point light sources uniformly.
* @param INPUT : numberOfSamples is the number of samples taken in the rectangle.
*/
void LightingBasis::uniformSamplingAreaLightSources(unsigned int envMapWidth, unsigned int envMapHeight, int numberOfSamples)
{
    int numberOfPixelsBetweenSamples = 25;

    //Transform each area light sources into numberOfSamples point light sources
    for(unsigned int i = 0 ; i<rectanglesAreaLights.size() ; i++)
    {
        Point2i startingPoint = rectanglesAreaLights[i][0];
        Point2i endingPoint = rectanglesAreaLights[i][1];
        Point2i upperLeft;
        Point2i bottomRight;
        int width = abs(startingPoint.x-endingPoint.x);
        int height = abs(startingPoint.y-endingPoint.y);
        int numberOfSamplesX = width/numberOfPixelsBetweenSamples; //One sample every 25 pixels
        int numberOfSamplesY = height/numberOfPixelsBetweenSamples; // One sample every 25 pixels

        //Reorientate the rectangle to know the upper left corner and the bottom right
        reorientateRectangle(startingPoint, endingPoint, upperLeft, bottomRight);

        if((width > numberOfPixelsBetweenSamples) && (height > numberOfPixelsBetweenSamples))
        {

            int stepWidth = width/numberOfSamplesX;
            int stepHeight = height/numberOfSamplesY;
            cout << "Sampling area light source " << i << endl;
            cout << "Width : " << width << " Height : " << height << endl;


            int positionX = upperLeft.x+stepWidth/2;
            int positionY = upperLeft.y+stepHeight/2;

            for(int k = 0 ; k<numberOfSamplesY ; k++)
            {
                for(int l = 0 ; l<numberOfSamplesX ; l++)
                {
                    cout << "positionX : " << positionX << " positionY : " << positionY << endl;
                    if((positionX < envMapWidth) && (positionY < envMapHeight))
                    {
                        Point2i newPointLightSource(positionX, positionY);
                        addPointLight(newPointLightSource);
                        positionX += stepWidth;
                    }

                }
                positionX = upperLeft.x+stepWidth/2;
                positionY += stepHeight;
            }
        }
        else//The light source area is too small it is approximated by a point light source
        {
            Point2i center = (upperLeft+bottomRight)*0.5;
            addPointLight(center);
        }
    }//End loop on area lights
    areAreaLightsSampled = true;
}

/**
 * Method that saves the position of point light sources to a file.
 * @brief saveBasis
 */
void LightingBasis::saveBasis()
{
    ostringstream osstream;
#if defined(__APPLE__) && defined(__MACH__)
    osstream << qApp->applicationDirPath().toStdString() << "/../../..";
#else
    osstream << qApp->applicationDirPath().toStdString();
#endif

    osstream << "/basis.txt";
    //Open the file and delete all the text inside
    ofstream file(osstream.str().c_str(), ios::out | ios::trunc);

          if(file)
          {
              for(unsigned int i = 0 ; i<pointLightSourcePosition.size() ; i++)
              {
                  file << i << ": " << pointLightSourcePosition[i].x << " " << pointLightSourcePosition[i].y << endl;
              }
                  file.close();
          }
          else
          {
              cerr << "Cannot open the file " << osstream.str() << endl;
          }
}


/**
 * Function that reads voronoi.txt file and load its content to the basis (point light source position).
 * @brief loadBasis
 */
void LightingBasis::loadBasis()
{
    ostringstream osstream;
#if defined(__APPLE__) && defined(__MACH__)
    osstream << qApp->applicationDirPath().toStdString() << "/../../..";
#else
    osstream << qApp->applicationDirPath().toStdString();
#endif

    osstream << "/basis.txt";
    //Open the file and delete all the text inside
    ifstream file(osstream.str().c_str(), ios::in);
    vector<Point2i> pointLights;

    if(file)
    {
        string lightNumber;
        string x;
        string y;

        int i = 0;

        while(file >> lightNumber) //Read the entire file. One loop = one line in the file
        {

            file >> x; //Read to the next space
            file >> y;

            pointLights.push_back(Point2i(atoi(x.c_str()), atoi(y.c_str())));
            //i corresponds to the light number

            i++;

        }

        this->addPointLights(pointLights);

        file.close();
    }
    else
    {
        cerr << "Cannot open the file " << osstream.str() << endl;
    }

}

/**
* Method that writes information about the lighting basis (number of point/area light sources).
*/
void LightingBasis::printBasis()
{
        cout << "Number of point light sources : " << numberOfPointLights << endl;
        cout << "Number of area light sources " << numberOfAreaLights << endl;
}

/*
*SETTERS AND GETTERS
*/

/**
* Getter that returns the vector rectanglesAreaLights.
* @return vector<vector<Point2i> > rectanglesAreaLights.
*/
vector<vector<Point2i> > LightingBasis::getRectanglesAreaLights()
{
    return rectanglesAreaLights;
}

/**
* Getter that returns the number of point light sources in the basis.
* @return int numberOfPointLights.
*/
int LightingBasis::getNumberOfPointLights()
{
    return numberOfPointLights;
}

/**
* Getter that returns the number of area light sources in the basis.
* @return int numberOfAreaLights.
*/
int LightingBasis::getNumberOfAreaLights()
{
    return numberOfAreaLights;
}

/**
 * Getter that return the vector containing the position of all the point light sources of the lighting basis.
 * @brief getPointLightSourcePosition
 * @return vector<Point2i> pointLightSourcePosition.
 */
vector<Point2i> LightingBasis::getPointLightSourcePosition()
{
    return pointLightSourcePosition;
}

/**
 * Function that reinitialises each attribute of the basis.
 * @brief clearBasis
 */
void LightingBasis::clearBasis()
{
    pointLightSourcePosition = vector<Point2i>();
    rectanglesAreaLights = vector<vector<Point2i> >();
    areAreaLightsSampled = false;
    numberOfPointLights = 0;
    numberOfAreaLights = 0;
}
