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
 * \file voronoi.cpp
 * \brief Class that computes the voronoi diagram.
 * \author Antoine Toisoul Le Cann
 * \date August, 7th, 2014
 *
 *  Class used to compute the voronoi diagram needed for the relighting.
 */

#include "voronoi.h"

using namespace std;
using namespace cv;

/**
 * Default contructor of the Voronoi class. Set the size of the environment map to 1024x512 by default.
 * @brief Voronoi
 */
Voronoi::Voronoi(): m_basis(LightingBasis()), m_numberOfPixelsInVoronoiCell(vector<int>()), m_voronoiSubdivision(Subdiv2D()),
    m_cellNumberPerPicture(vector<vector<int> >()), m_intensity(vector<float >()), m_rgbWeights(vector<vector<float> >()), m_envMapWidth(1024), m_envMapHeight(512)
{
    //Initialising the voronoi subdivision
    Rect boundingBoxEnvMap(0,0,m_envMapWidth,m_envMapHeight);
    m_voronoiSubdivision = Subdiv2D(boundingBoxEnvMap);
}

/**
 * Constructor that creates a Voronoi diagram given a lighting basis and the cell numbers for each picture of the lighting basis.
 * @brief Voronoi
 * @param basis The lighting basis which is used to create the Voronoi diagram.
 * @param envMapWidth width of the environment map.
 * @param envMapHeight height of the environment map.
 * @param cellNumberPerPicture vector that contains the voronoi cells for each picture (each picture of the reflectance field may have more than one picture). cellNumberPerPicture[i] is the vector containing the cell numbers for picture i of the reflectance field.
 */
Voronoi::Voronoi(LightingBasis& basis, unsigned int envMapWidth, unsigned int envMapHeight, vector<vector<int> >& cellNumberPerPicture):
    m_basis(basis), m_numberOfPixelsInVoronoiCell(vector<int>()), m_voronoiSubdivision(Subdiv2D()),
    m_cellNumberPerPicture(cellNumberPerPicture), m_intensity(vector<float >()),
    m_rgbWeights(vector<vector<float> >()), m_envMapWidth(envMapWidth), m_envMapHeight(envMapHeight)
{
    //Initialising the voronoi subdivision
    Rect boundingBoxEnvMap(0,0,m_envMapWidth,m_envMapHeight);
    m_voronoiSubdivision = Subdiv2D(boundingBoxEnvMap);

    m_numberOfPixelsInVoronoiCell.assign(m_basis.getNumberOfPointLights(), 0);

    vector<Point2i> pointLightSourcePosition = m_basis.getPointLightSourcePosition();
    this->setVoronoi(pointLightSourcePosition);
}

/**
  * Destructor of the Voronoi class
  */
Voronoi::~Voronoi()
{

}

/**
 * Method that adds a point light source to the voronoi diagram.
 * @brief addPointLight
 * @param lightPosition is the position of the point light source.
 */
void Voronoi::addPointLight(Point2i lightPosition)
{

    if(lightPosition.x<m_envMapWidth && lightPosition.y<m_envMapHeight)
    {
        m_basis.addPointLight(lightPosition);
        m_voronoiSubdivision.insert(lightPosition); /*!< The Voronoi subdivision*/

    }
    this->numberOfPixelsPerVoronoiCell();
}

/**
 * Method that adds an area light source to the voronoi diagram (the center of the light source is considered).
 * @brief addAreaLight
 * @param startingPoint
 * @param endingPoint
 */
void Voronoi::addAreaLight(Point2i startingPoint, Point2i endingPoint)
{
    m_basis.addAreaLight(startingPoint, endingPoint);

    Point2i center = (startingPoint+endingPoint)*0.5;
    m_voronoiSubdivision.insert(center);
    this->numberOfPixelsPerVoronoiCell();
}

/**
 * Method that creates the voronoi diagram using the position of the point light sources.
 * @brief setVoronoi
 * @param pointLightSourcePosition vector that contains the position of point light sources (Point2i objects).
 */
void Voronoi::setVoronoi(vector<Point2i> &pointLightSourcePosition)
{
    unsigned int numberOfPointLights = pointLightSourcePosition.size();
    m_basis.addPointLights(pointLightSourcePosition);

    for(unsigned int i = 0 ; i<numberOfPointLights ; i++)
    {
        m_voronoiSubdivision.insert(pointLightSourcePosition[i]);
        vector<int> cellNumbersForImagei;
        cellNumbersForImagei.push_back(i);
        m_cellNumberPerPicture.push_back(cellNumbersForImagei); //Add a cell number to each picture
    }

    if(numberOfPointLights != 0)
    {
        this->numberOfPixelsPerVoronoiCell();
    }
}

/**
 * Method that creates the voronoi diagram using the position of the point light sources. Several point light sources can be associated to one picture in the relighting.
 * The vector cellNumberPerPicture the information for each picture of the reflectance field. cellNumberPerPicture[i] is a vector that contains the point light sources (as a number) for picture i.
 * @brief setVoronoi
 * @param pointLightSourcePosition vector that contains the position (Point2i) of the point light sources.
 * @param cellNumberPerPicture contains the information for each picture of the reflectance field. cellNumberPerPicture[i] is a vector that contains the point light sources (as a number) for picture i.
 */
void Voronoi::setVoronoi(vector<Point2i> &pointLightSourcePosition, vector<vector<int> > &cellNumberPerPicture)
{
    unsigned int numberOfPointLights = pointLightSourcePosition.size();
    m_basis.addPointLights(pointLightSourcePosition);
    m_cellNumberPerPicture = cellNumberPerPicture;

    for(unsigned int i = 0 ; i<numberOfPointLights ; i++)
    {
        m_voronoiSubdivision.insert(pointLightSourcePosition[i]);
        vector<int> cellNumbersForImagei;
        cellNumbersForImagei.push_back(i);
        m_cellNumberPerPicture.push_back(cellNumbersForImagei); //Add a cell number to each picture
    }

    if(numberOfPointLights != 0)
    {
        this->numberOfPixelsPerVoronoiCell();
    }
}

/**
 * Reinitialize the variables of the Voronoi diagram.
 * @brief clearVoronoi
 */
void Voronoi::clearVoronoi()
{
    m_basis.clearBasis();
    m_numberOfPixelsInVoronoiCell = vector<int>();
    Rect boundingBoxEnvMap(0,0,m_envMapWidth,m_envMapHeight);
    m_voronoiSubdivision = Subdiv2D(boundingBoxEnvMap);
    m_cellNumberPerPicture = vector<vector<int> >();

    m_intensity =  vector<float >();
    m_rgbWeights = vector<vector<float> >();
}

/**
 * Method to calculate the number of pixels in each voronoi cell.
 * The result is stored in the numberOfPixelsInVoronoiCellareaLights vector.
 * @brief numberOfPixelsPerVoronoiCell
 */
void Voronoi::numberOfPixelsPerVoronoiCell()
{
        //Initialise the vector with zeros
        m_numberOfPixelsInVoronoiCell.assign(m_basis.getNumberOfPointLights(), 0);

        for(unsigned int k = 0 ; k<m_numberOfPixelsInVoronoiCell.size() ; k++)
        {
            m_numberOfPixelsInVoronoiCell[k] = 0;
        }

        int cellNumber = 0;
        for(unsigned int j = 0 ; j<m_envMapWidth ; j++)
        {
            for(unsigned int i = 0 ; i<m_envMapHeight ; i++)
            {

                cellNumber = this->findNearestLightSource(j, i);
                if(cellNumber != -1)
                {
                    m_numberOfPixelsInVoronoiCell[cellNumber]++;
                }

            }
        }
}

/*********************************
 * Functions related to painting *
 *********************************/

/**
 * Method to paint the point light sources on the environment map.
 * @brief paintPointLights
 * @param environmentMap Mat image that contains the image on which the point light sources are painted.
 */
void Voronoi::paintPointLights(Mat& environmentMap)
{
    m_basis.paintPointLights(environmentMap);
}

/**
 * Method to paint the area light sources on the environment map.
 * @brief paintAreaLights
 * @param environmentMap Mat image that contains the image on which the area light sources (rectangles) are painted.
 */
void Voronoi::paintAreaLights(Mat& environmentMap)
{
    m_basis.paintAreaLights(environmentMap);
}

/**
* Method that draws the Voronoi diagram on an image.
* @param INPUT : img is a OpenCV Mat&. It is the image on which the Voronoi diagram will be drawn.
*/
void Voronoi::paintVoronoi(Mat& img)
{
    vector<vector<Point2f> > facets;
    vector<Point2f> centers;
    m_voronoiSubdivision.getVoronoiFacetList(vector<int>(), facets, centers);

    vector<Point> ifacet;
    vector<vector<Point> > ifacets(1);

    for( size_t i = 0; i < facets.size(); i++ )
    {
        ifacet.resize(facets[i].size());
        for( size_t j = 0; j < facets[i].size(); j++ )
            ifacet[j] = facets[i][j];

        ifacets[0] = ifacet;
        polylines(img, ifacets, true, Scalar(255,0,0), 2, 8, 0);
        circle(img, centers[i], 4, Scalar(0,0,255), -1, 8, 0);
    }
}

/**
 * Method that paints the light stage intensities of each voronoi cell.
 * @brief paintLightStageIntensities
 * @param img is an OpenCV Mat image that contains the image on which the light stage intensities are painted.
 */
void Voronoi::paintLightStageIntensities(Mat& img)
{
    vector<vector<Point2f> > facets;
    vector<Point2f> centers;
    m_voronoiSubdivision.getVoronoiFacetList(vector<int>(), facets, centers);

    vector<Point> ifacet;
    vector<vector<Point> > ifacets(1);

    //Load light intentisities in order to normalize each light by its intensity
    vector<vector<float> > lightIntensities;

    ostringstream osstream;
#if defined(__APPLE__) && defined(__MACH__)
    osstream << qApp->applicationDirPath().toStdString() << "/../../..";
#else
    osstream << qApp->applicationDirPath().toStdString();
#endif
    osstream << "/light_intensities.txt";

    readFile(osstream.str(), lightIntensities);

    for( size_t i = 0; i < facets.size(); i++ )
    {
        ifacet.resize(facets[i].size());
        for( size_t j = 0; j < facets[i].size(); j++ )
            ifacet[j] = facets[i][j];

        ifacets[0] = ifacet;

        //Opencv stores colors in BGR format
        Scalar color;
        color[0] = floor(255*lightIntensities[i][2]);
        color[1] = floor(255*lightIntensities[i][1]);
        color[2] = floor(255*lightIntensities[i][0]);

        fillConvexPoly(img, ifacet, color, 8, 0);

        circle(img, centers[i], 4, Scalar(0,0,0), 1, 8, 0);

    }
}

/**
* Method that paints the Voronoi cells on an image. The color of each Voronoi cell is given by the rgbWeights vector.
* @param INPUT : img is a OpenCV Mat&. It is the image on which the Voronoi cells will be drawn.
*/
void Voronoi::paintVoronoiCells(Mat& img)
{
    vector<vector<Point2f> > facets;
    vector<Point2f> centers;
    m_voronoiSubdivision.getVoronoiFacetList(vector<int>(), facets, centers);

    vector<vector<float> > normalizedWeights = m_rgbWeights;
    normalizeWeightsRGB(normalizedWeights);

    vector<Point> ifacet;
    vector<vector<Point> > ifacets(1);
    int numberOfPointLights = this->m_basis.getNumberOfPointLights();
    for( size_t i = 0; i < facets.size(); i++ )
    {
        ifacet.resize(facets[i].size());
        for( size_t j = 0; j < facets[i].size(); j++ )
            ifacet[j] = facets[i][j];

        ifacets[0] = ifacet;

        //Opencv stores colors in BGR format
        Scalar color;

        //The weights have been normalized first : multiply by the number of cells to display a correct color
        color[0] = floor(255*numberOfPointLights*normalizedWeights[i][2]);
        color[1] = floor(255*numberOfPointLights*normalizedWeights[i][1]);
        color[2] = floor(255*numberOfPointLights*normalizedWeights[i][0]);

        fillConvexPoly(img, ifacet, color, 8, 0);
        circle(img, centers[i], 4, Scalar(0,0,0), 1, 8, 0);

    }
}

/**
 * Method that paints the Voronoi cells on an image depending on the cellNumberPerPicture vector. Each picture in the reflectance field might have several corresponding voronoi cells.
 * The color of each Voronoi cell is given by the rgbWeights vector.
 * @brief paintVoronoiCellsOR
 * @param INPUT : img is a OpenCV Mat&. It is the image on which the Voronoi cells will be drawn.
 */
void Voronoi::paintVoronoiCellsOR(Mat& img)
{
    vector<vector<Point2f> > facets;
    vector<Point2f> centers;
    m_voronoiSubdivision.getVoronoiFacetList(vector<int>(), facets, centers);

    unsigned int numberOfLightingConditions = m_cellNumberPerPicture.size();
    vector<vector<float> > normalizedWeights = m_rgbWeights;
    normalizeWeightsRGB(normalizedWeights);

    vector<Point> ifacet;
    vector<vector<Point> > ifacets(1);
    int imageNumber = -1;
    for( size_t i = 0; i < facets.size(); i++ )
    {
        ifacet.resize(facets[i].size());
        for( size_t j = 0; j < facets[i].size(); j++ )
            ifacet[j] = facets[i][j];

        ifacets[0] = ifacet;

        //Opencv stores colors in BGR format
        Scalar color;
        imageNumber = this->findImageNumber(i);
        if(imageNumber != -1)
        {
            color[0] = floor(255*numberOfLightingConditions*normalizedWeights[imageNumber][2]);
            color[1] = floor(255*numberOfLightingConditions*normalizedWeights[imageNumber][1]);
            color[2] = floor(255*numberOfLightingConditions*normalizedWeights[imageNumber][0]);
        }

        fillConvexPoly(img, ifacet, color, 8, 0);
        circle(img, centers[i], 4, Scalar(0,0,0), 1, 8, 0);
    }
}

/**
* Method that paints the boundary of the given Voronoi cells (only).
* @param INPUT : img is a OpenCV Mat&. It is the image on which the Voronoi diagram will be drawn.
* @param INPUT : voronoiCells is a vector of int containing the Voronoi cells numbers which boundary will be drawn.
*/
void Voronoi::paintSpecificVoronoiCellsBoundary(Mat& img, vector<int> &voronoiCells)
{
    vector<vector<Point2f> > facets;
    vector<Point2f> centers;
    m_voronoiSubdivision.getVoronoiFacetList(vector<int>(), facets, centers);

    vector<Point> ifacet;
    vector<vector<Point> > ifacets(1);

    for( size_t i = 0; i < voronoiCells.size(); i++ )
    {
        int cellNumber = voronoiCells[i];
        ifacet.resize(facets[cellNumber].size());

        for( size_t j = 0; j < facets[cellNumber].size(); j++ )
            ifacet[j] = facets[cellNumber][j];

        ifacets[0] = ifacet;

        //Opencv stores colors in BGR format
        Scalar color(0,0,255);
        polylines(img, ifacets, true, color, 1, 8, 0);
        circle(img, centers[cellNumber], 3, Scalar(), 1, 8, 0);
    }
}

/**
* Method that only paints the given Voronoi cells.
* @param INPUT : img is a OpenCV Mat&. It is the image on which the Voronoi diagram will be drawn.
* @param INPUT : voronoiCells is a vector of int containing the Voronoi cells numbers which boundary will be drawn.
* @param INPUT : greyColor is a vector of float the value of the shade of grey used to paint the voronoi cells. greyColor[i] contains the shade of grey gvoronoiCells.
*/
void Voronoi::paintSpecificVoronoiCells(Mat& img, vector<int> &voronoiCells, vector<float> &greyColor)
{
    vector<vector<Point2f> > facets;
    vector<Point2f> centers;
    m_voronoiSubdivision.getVoronoiFacetList(vector<int>(), facets, centers);

    vector<Point> ifacet;
    vector<vector<Point> > ifacets(1);

    for( size_t i = 0; i < voronoiCells.size(); i++ )
    {
        int cellNumber = voronoiCells[i];
        ifacet.resize(facets[cellNumber].size());

        for( size_t j = 0; j < facets[cellNumber].size(); j++ )
            ifacet[j] = facets[cellNumber][j];

        ifacets[0] = ifacet;

        //Opencv stores colors in BGR format
        Scalar color;
        color[0] = floor(255*greyColor[i]);
        color[1] = floor(255*greyColor[i]);
        color[2] = floor(255*greyColor[i]);

        polylines(img, ifacets, true, color, 1, 8, 0);
        fillConvexPoly(img, ifacet, color, 8, 0);
        circle(img, centers[cellNumber], 3, Scalar(), 1, 8, 0);
    }
}

/**
* Method that paints the given Voronoi cells entirely with the intensity (shade of grey) given in the vector weights.
* @param INPUT : img is a OpenCV Mat&. It is the image on which the Voronoi diagram will be drawn.
*/
void Voronoi::paintVoronoiIntensity(Mat& img)
{
    vector<vector<Point2f> > facets;
    vector<Point2f> centers;
    m_voronoiSubdivision.getVoronoiFacetList(vector<int>(), facets, centers);

    vector<Point> ifacet;
    vector<vector<Point> > ifacets(1);


    for( size_t i = 0; i < facets.size(); i++ )
    {
        ifacet.resize(facets[i].size());
        for( size_t j = 0; j < facets[i].size(); j++ )
            ifacet[j] = facets[i][j];

        ifacets[0] = ifacet;

        //Opencv stores colors in BGR format
        Scalar color;

        color[0] = floor(255*m_intensity[i]);
        color[1] = floor(255*m_intensity[i]);
        color[2] = floor(255*m_intensity[i]);
        fillConvexPoly(img, ifacet, color, 8, 0);
        circle(img, centers[i], 3, Scalar(), 1, 8, 0);
    }
}


/********************************
 *Functions that compute weights*
 ********************************/

/**
* Method that computes the intensity of each Voronoi cell(taking into account the solid angle).
* @param INPUT : environmentMap is an OpenCV Mat of floats containing the HDR values of the environment map.
*/
void Voronoi::computeVoronoiIntensity(Mat &environmentMap)
{
    float R = 0.0, G = 0.0, B = 0.0;
    int numberOfPointLights = m_basis.getNumberOfPointLights();
    int cellNumber = -1;

    //Initialisation
    m_intensity.resize(numberOfPointLights);

    //Load light intentisities in order to normalize each light by its intensity
    vector<vector<float> > lightIntensities;
    ostringstream osstream;

#if defined(__APPLE__) && defined(__MACH__)
    osstream << qApp->applicationDirPath().toStdString() << "/../../..";
#else
    osstream << qApp->applicationDirPath().toStdString();
#endif
    osstream << "/light_intensities.txt";

    readFile(osstream.str(), lightIntensities);

    for(unsigned int i = 0 ; i<m_envMapHeight ; i++)
    {
        for(unsigned int j = 0 ; j< m_envMapWidth ; j++)
        {
            cellNumber = this->findNearestLightSource(j,i); //Finds the cell number corresponding to the nearest light source of point (x,y)= (j,i)

            if(cellNumber != -1)
            {
                //OpenCV uses BGR
                R = environmentMap.at<Vec3f>(i,j).val[2]*lightIntensities[cellNumber][0];
                G = environmentMap.at<Vec3f>(i,j).val[1]*lightIntensities[cellNumber][1];
                B = environmentMap.at<Vec3f>(i,j).val[0]*lightIntensities[cellNumber][2];

                if(!(isnan(R) && isnan(G) && isnan(B))) //Values in the environment map can be NaN.
                {
                    m_intensity[cellNumber] += (R+G+B)/3.0*sin((float) i*M_PI/m_envMapHeight); //Multiply the intensity by the solid angle
                }
            }
        }
    }
}

/**
* Method that computes the RGB intensity of each Voronoi cell (taking into account the solid angle).
* @param INPUT : environmentMap is an OpenCV Mat of floats containing the HDR values of the environment map.
*/
void Voronoi::computeVoronoiWeightsRGB(const Mat &environmentMap, float offset)
{
    float R = 0.0, G = 0.0, B = 0.0;
    int cellNumber = -1;
    int numberOfPointLights = m_basis.getNumberOfPointLights();

    //Initialisation
    for(int k = 0 ; k<numberOfPointLights ; k++)
    {
        vector<float> weightsImageK(3,0.0);
        m_rgbWeights.push_back(weightsImageK);
    }

    int jOffset = floor(offset*m_envMapWidth/(2.0*M_PI));
    //Load light intentisities in order to normalize each light by its intensity
    vector<vector<float> > lightIntensities;
    ostringstream osstream;

#if defined(__APPLE__) && defined(__MACH__)
    osstream << qApp->applicationDirPath().toStdString() << "/../../..";
#else
    osstream << qApp->applicationDirPath().toStdString();
#endif
    osstream << "/light_intensities.txt";

    readFile(osstream.str(), lightIntensities);

    for(unsigned int i = 0 ; i<m_envMapHeight ; i++)
    {
        for(unsigned int j = 0 ; j< m_envMapWidth ; j++)
        {
            cellNumber = this->findNearestLightSource(j,i);
            int jModulus = (j+jOffset) % m_envMapWidth;
            if(cellNumber != -1)
            {
                if(j + jOffset < m_envMapWidth)
                {
                    //OpenCV uses BGR
                    R = environmentMap.at<Vec3f>(i,jModulus).val[2]*lightIntensities[cellNumber][0];
                    G = environmentMap.at<Vec3f>(i,jModulus).val[1]*lightIntensities[cellNumber][1];
                    B = environmentMap.at<Vec3f>(i,jModulus).val[0]*lightIntensities[cellNumber][2];
                 }
                else
                {
                    //OpenCV uses BGR
                    R = environmentMap.at<Vec3f>(i,jModulus).val[2]*lightIntensities[cellNumber][0];
                    G = environmentMap.at<Vec3f>(i,jModulus).val[1]*lightIntensities[cellNumber][1];
                    B = environmentMap.at<Vec3f>(i,jModulus).val[0]*lightIntensities[cellNumber][2];
                }

                if(!(isnan(R) && isnan(G) && isnan(B))) //Values in the environment map could be NaN.
                {
                    m_rgbWeights[cellNumber][0] += R*sin((float) i*M_PI/m_envMapHeight); //Multiply the intensity by the solid angle
                    m_rgbWeights[cellNumber][1] += G*sin((float) i*M_PI/m_envMapHeight); //Multiply the intensity by the solid angle
                    m_rgbWeights[cellNumber][2] += B*sin((float) i*M_PI/m_envMapHeight); //Multiply the intensity by the solid angle
                }

            }
        }
    }
}

/**
* Method that computes the sum of each pixel in each Voronoi cell (taking into account the solid angle).
* Each pixel is multiplied by a gaussian function that depends on its distance to the center of the Voronoi cell.
* @param INPUT : environmentMap is an OpenCV Mat of floats containing the HDR values of the environment map.
*/
void Voronoi::computeVoronoiWeightsGaussian(const Mat &environmentMap, const float offset)
{
    float R = 0.0, G = 0.0, B = 0.0;
    int cellNumber = -1;
    int numberOfPointLights = m_basis.getNumberOfPointLights();
    vector<Point2i> pointLightSourcePosition = m_basis.getPointLightSourcePosition();
    float varianceX = 10.0;
    float varianceY = 10.0;
    float meanX = 0.0;
    float meanY = 0.0;

    //Initialisation
    for(int k = 0 ; k<numberOfPointLights ; k++)
    {
        vector<float> weightsImageK(3,0.0);
        m_rgbWeights.push_back(weightsImageK);
    }

    //Load light intentisities in order to normalize each light by its intensity
    vector<vector<float> > lightIntensities;
    int jOffset = floor(offset*m_envMapWidth/(2.0*M_PI));
    ostringstream osstream;

#if defined(__APPLE__) && defined(__MACH__)
    osstream << qApp->applicationDirPath().toStdString() << "/../../..";
#else
    osstream << qApp->applicationDirPath().toStdString();
#endif
    osstream << "/light_intensities.txt";

    readFile(osstream.str(), lightIntensities);

    Point2i currentPoint;
    Point2i centerCell;

    for(unsigned int i = 0 ; i<m_envMapHeight ; i++)
    {
        for(unsigned int j = 0 ; j< m_envMapWidth ; j++)
        {

            currentPoint = Point2i(j,i);
            cellNumber = this->findNearestLightSource(j,i);
            centerCell = pointLightSourcePosition[cellNumber];

            int jModulus = (j+jOffset)%m_envMapWidth;

            meanX = centerCell.x;
            meanY = centerCell.y;

            if(cellNumber != -1)
            {
                //OpenCV uses BGR
                R = environmentMap.at<Vec3f>(i,jModulus).val[2]*lightIntensities[cellNumber][0];
                G = environmentMap.at<Vec3f>(i,jModulus).val[1]*lightIntensities[cellNumber][1];
                B = environmentMap.at<Vec3f>(i,jModulus).val[0]*lightIntensities[cellNumber][2];

                if(!(isnan(R) && isnan(G) && isnan(B))) //Values in the environment map could be NaN.
                {
                    m_rgbWeights[cellNumber][0] += R*sin((float) i*M_PI/m_envMapHeight)*gaussian2D(currentPoint.x, currentPoint.y, meanX, meanY, varianceX, varianceY); //Multiply the intensity by the solid angle
                    m_rgbWeights[cellNumber][1] += G*sin((float) i*M_PI/m_envMapHeight)*gaussian2D(currentPoint.x, currentPoint.y, meanX, meanY, varianceX, varianceY); //Multiply the intensity by the solid angle
                    m_rgbWeights[cellNumber][2] += B*sin((float) i*M_PI/m_envMapHeight)*gaussian2D(currentPoint.x, currentPoint.y, meanX, meanY, varianceX, varianceY); //Multiply the intensity by the solid angle
                }
            }
        }
    }
}

/**
* Method that compute the weight for each image of the reflectance field.
* Each image might have several corresponding voronoi cells.
* @param INPUT : environmentMap is an OpenCV Mat of floats containing the HDR values of the environment map.
* @param INPUT : offset is the offset added for the rotation of the environment map.
*/
void Voronoi::computeVoronoiWeightsOR(const Mat &environmentMap, const float offset)
{
    float R = 0.0, G = 0.0, B = 0.0;
    int cellNumber = -1;
    int imageNumber = -1;

    //Initialisation
    for(unsigned int k = 0 ; k<m_cellNumberPerPicture.size() ; k++)
    {
        vector<float> weightsImageK(3,0.0);
        m_rgbWeights.push_back(weightsImageK);
    }

    //Load light intentisities in order to normalize each light by its intensity
    Point2i currentPoint;
    Point2i centerCell;

    int jOffset = floor(offset*m_envMapWidth/(2.0*M_PI));

    for(unsigned int i = 0 ; i<m_envMapHeight ; i++)
    {
        for(unsigned int j = 0 ; j<m_envMapWidth ; j++)
        {
            currentPoint = Point2i(j,i);
            cellNumber = this->findNearestLightSource(j,i);

            int jModulus = (j+jOffset)%m_envMapWidth;

            if(cellNumber != -1)
            {
                //Given a cell number, which image does the cell correspond to ?
                imageNumber = this->findImageNumber(cellNumber);

                //OpenCV uses BGR
                R = environmentMap.at<Vec3f>(i,jModulus).val[2];
                G = environmentMap.at<Vec3f>(i,jModulus).val[1];
                B = environmentMap.at<Vec3f>(i,jModulus).val[0];

                if(!(isnan(R) && isnan(G) && isnan(B))) //Values in the environment map could be NaN.
                {
                    m_rgbWeights[imageNumber][0] += R*sin((float) i*M_PI/m_envMapHeight); //Multiply the intensity by the solid angle
                    m_rgbWeights[imageNumber][1] += G*sin((float) i*M_PI/m_envMapHeight); //Multiply the intensity by the solid angle
                    m_rgbWeights[imageNumber][2] += B*sin((float) i*M_PI/m_envMapHeight); //Multiply the intensity by the solid angle
                }
            }
        }
    }

}

/**
 * Method that compute the weight for each image of the reflectance field. Each pixel is multiplied by a gaussian function that depends on its distance to the center of the Voronoi cell.
 * Each image might have several corresponding voronoi cells.
 * @brief computeVoronoiWeightsGaussianOR
 * @param INPUT : environmentMap is an OpenCV Mat of floats containing the HDR values of the environment map.
 * @param INPUT : offset is the offset added for the rotation of the environment map.
 * @param INPUT : varianceX variance of the 2D Gaussian along the first dimension. varianceX[i] contains the varianceX that is used for the cell of picture i.
 * @param INPUT : varianceY variance of the 2D Gaussian along the second dimension. varianceX[i] contains the varianceY that is used for the cell of picture i.
 */
void Voronoi::computeVoronoiWeightsGaussianOR(const Mat &environmentMap, const float offset, float varianceX[], float varianceY[])
{
    float R = 0.0, G = 0.0, B = 0.0;
    int cellNumber = -1;
    int imageNumber = -1;
    unsigned int numberOfImages = m_cellNumberPerPicture.size();
    int* numberOfPixelsPerImage = new int[numberOfImages];
    vector<Point2i> pointLightSourcePosition = m_basis.getPointLightSourcePosition();
/*
    float varianceX[numberOfImages];
    float varianceY[numberOfImages];

    varianceX[0] = 80;
    varianceY[0] = 40;

    varianceX[1] = 80;
    varianceY[1] = 60;

    varianceX[2] = 65;
    varianceY[2] = 35;

    varianceX[3] = 65;
    varianceY[3] = 60;

    varianceX[4] = 50;
    varianceY[4] = 50;

    varianceX[5] = 80;
    varianceY[5] = 40;

    varianceX[6] = 55;
    varianceY[6] = 30;

    varianceX[7] = 15;
    varianceY[7] = 15;

    varianceX[8] = 15;
    varianceY[8] = 15;
*/
    float meanX = 0.0;
    float meanY = 0.0;

    //Initialisation
    for(unsigned int k = 0 ; k<m_cellNumberPerPicture.size() ; k++)
    {
        vector<float> weightsImageK(3,0.0);
        m_rgbWeights.push_back(weightsImageK);
        numberOfPixelsPerImage[k] = 0;
    }

    //Load light intentisities in order to normalize each light by its intensity
    Point2i currentPoint;
    Point2i centerCell;

    int jOffset = floor(offset*m_envMapWidth/(2.0*M_PI));

    for(unsigned int i = 0 ; i<m_envMapHeight ; i++)
    {
        for(unsigned int j = 0 ; j< m_envMapWidth ; j++)
        {
            currentPoint = Point2i(j,i);
            cellNumber = this->findNearestLightSource(j,i);
            centerCell = pointLightSourcePosition[cellNumber];

             int jModulus = (j+jOffset)%m_envMapWidth;

            if(cellNumber != -1)
            {
                //Given a cell number, which image does the cell correspond to ?
                imageNumber = this->findImageNumber(cellNumber);

                meanX = centerCell.x;
                meanY = centerCell.y;

                R = environmentMap.at<Vec3f>(i,jModulus).val[2];
                G = environmentMap.at<Vec3f>(i,jModulus).val[2];
                B = environmentMap.at<Vec3f>(i,jModulus).val[2];

                if(!(isnan(R) && isnan(G) && isnan(B))) //Values in the environment map could be NaN.
                {
                    m_rgbWeights[imageNumber][0] += R*sin((float) i*M_PI/m_envMapHeight)*gaussian2D(currentPoint.x, currentPoint.y, meanX, meanY, varianceX[imageNumber], varianceY[imageNumber]); //Multiply the intensity by the solid angle
                    m_rgbWeights[imageNumber][1] += G*sin((float) i*M_PI/m_envMapHeight)*gaussian2D(currentPoint.x, currentPoint.y, meanX, meanY, varianceX[imageNumber], varianceY[imageNumber]); //Multiply the intensity by the solid angle
                    m_rgbWeights[imageNumber][2] += B*sin((float) i*M_PI/m_envMapHeight)*gaussian2D(currentPoint.x, currentPoint.y, meanX, meanY, varianceX[imageNumber], varianceY[imageNumber]); //Multiply the intensity by the solid angle
                }
                numberOfPixelsPerImage[imageNumber]++;
            }
        }
    }

    //Divide by the number of pixels
 /*   for(int k = 0 ; k<cellNumberPerPicture.size() ; k++)
    {
        if(numberOfPixelsPerImage[k] != 0)
        {
            rgbWeights[k][0] /= (float) numberOfPixelsPerImage[k];
            rgbWeights[k][1] /= (float) numberOfPixelsPerImage[k];
            rgbWeights[k][2] /= (float) numberOfPixelsPerImage[k];
        }

    }
*/
    delete[] numberOfPixelsPerImage;
}

/**
 * Method that sets the vector m_cellNumberPerPicture.
 * @brief setCellNumberPerPicture
 * @param cellNumberPerPicture is the vector used to set m_cellNumberPerPicture.
 */
void Voronoi::setCellNumberPerPicture(vector<vector<int> > &cellNumberPerPicture)
{
    this->m_cellNumberPerPicture = cellNumberPerPicture;
}

/*****************************************************************
 * Functions that makes the link between
 * a point on the environment map, the corresponding Voronoi cell
 * and the corresponding image number
 ******************************************************************/

/**
* Given a centroid of the voronoi cell (a pixel in the environment map), the method returns the number of the corresponding light source.
* @param INPUT : x is the abscissa of the centroid.
* @param INPUT : y is the ordinate of the centroid.
* @return returns the corresponding light source number.
*/
int Voronoi::findLightSource(int x, int y)
{
    Point2i currentPoint;
    vector<Point2i> pointLightSourcePosition = m_basis.getPointLightSourcePosition();

    for(unsigned int i = 0 ; i<pointLightSourcePosition.size() ; i++)
    {
        currentPoint = pointLightSourcePosition[i];
        if((currentPoint.x == x) && (currentPoint.y == y))
        {
           return i;
        }
    }

    return -1;
}

/**
* Given a pixel in the environment map, the method returns the number of the nearest light source.
* @param INPUT : x is the abscissa of the pixel.
* @param INPUT : y is the ordinate of the pixel.
* @return returns the corresponding light source number.
*/
int Voronoi::findNearestLightSource(int x, int y)
{
    Point2f result;
    Point2i currentPoint(x,y);

    m_voronoiSubdivision.findNearest(currentPoint, &result);
    return this->findLightSource(result.x, result.y);

}

/**
 * Given a Voronoi cell number, the function returns the number of corresponding image in the reflectance field.
 * @brief findImageNumber
 * @param cellNumber is the Voronoi cell number.
 * @return returns the number of corresponding image in the reflectance field.
 */
int Voronoi::findImageNumber(int cellNumber)
{
    int imageNumber = -1;
    //Given a cell number, which image does the cell correspond to ?
    for(unsigned int k = 0 ; k<m_cellNumberPerPicture.size() ; k++)
    {
        for(unsigned int l = 0 ; l<m_cellNumberPerPicture[k].size() ; l++)
        {
            if(m_cellNumberPerPicture[k][l] == cellNumber)
            {
                 imageNumber = k;
            }
        }
    }
    return imageNumber;
}

/**
 * Method that saves the voronoi diagram to a file.
 * @brief saveVoronoi
 */
void Voronoi::saveVoronoi()
{
    m_basis.saveBasis();

    ostringstream osstream;

#if defined(__APPLE__) && defined(__MACH__)
    osstream << qApp->applicationDirPath().toStdString() << "/../../..";
#else
    osstream << qApp->applicationDirPath().toStdString();
#endif

    osstream << "/voronoi.txt";

    //Open the file and delete all the text inside
    ofstream file(osstream.str().c_str(), ios::out | ios::trunc);

    if(file)
    {

        for(unsigned int i = 0 ; i<m_cellNumberPerPicture.size() ; i++)
        {
            file << m_cellNumberPerPicture[i].size() << " ";

            for(unsigned int j = 0 ; j<m_cellNumberPerPicture[i].size() ; j++)
            {
                if(j != (m_cellNumberPerPicture[i].size()-1))
                    file << m_cellNumberPerPicture[i][j] << " ";
                 else
                    file << m_cellNumberPerPicture[i][j];
            }
            if(i != (m_cellNumberPerPicture.size()-1))
                file << endl;
        }

        file.close();

    }
    else
    {
        cerr << "Cannot open the file " << osstream.str() << endl;
    }
}

/**
 * Method that load the voronoi diagram (lighting basis and cellNumberPerPicture) from voronoi.txt and basis.txt files.
 * @brief loadVoronoi
 */
void Voronoi::loadVoronoi()
{
    //Load the basis
    m_basis.loadBasis();
    vector<Point2i> pointLightSources = m_basis.getPointLightSourcePosition();
    this->setVoronoi(pointLightSources);

    //Load the voronoi diagram
    ostringstream osstream;

#if defined(__APPLE__) && defined(__MACH__)
    osstream << qApp->applicationDirPath().toStdString() << "/../../..";
#else
    osstream << qApp->applicationDirPath().toStdString();
#endif
    osstream << "/voronoi.txt";

    //Open the file and delete all the text inside
    ifstream file(osstream.str().c_str(), ios::in);
    vector<vector<int> > cellNumberPerPicture;

    if(file)
    {

        string sizeString;
        string value;
        int size;

        while(file >> sizeString) //Read the entire file. One loop = one line in the file
        {
            vector<int> cellNumbers;
            size = atoi(sizeString.c_str());
            //The file has the folowing format
            // size1 value1 value2 ... valueSize1-1
            // size2 value1 value2 ... valueSize2-1
            for(int i = 0 ; i<size ; i++)
            {
                file >> value;
                cellNumbers.push_back(atoi(value.c_str()));
            }
            cellNumberPerPicture.push_back(cellNumbers);
        }

        this->setCellNumberPerPicture(cellNumberPerPicture);

        file.close();
    }
    else
    {
        cerr << "Cannot open the file " << osstream.str() << endl;
    }
}

/**
 * Method that sets the size of the environment map.
 * @brief setEnvironmentMapSize
 * @param width of the environment map.
 * @param height of the environment map.
 */
void Voronoi::setEnvironmentMapSize(unsigned int width, unsigned int height)
{
    this->m_envMapWidth = width;
    this->m_envMapHeight = height;
    Rect boundingBoxEnvMap(0,0,m_envMapWidth,m_envMapHeight);
    m_voronoiSubdivision = Subdiv2D(boundingBoxEnvMap);
}

/**
 * Method that reinitialise the vectors containing the weights.
 * @brief clearWeights
 */
 void Voronoi::clearWeights()
 {
     m_intensity = vector<float>();
     m_rgbWeights = vector<vector<float> >();
 }

/*********
 *Getters*
 ********/

 /**
  * Getter that returns the RGB weights of each voronoi cell.
  * @brief getRGBWeights
  * @return the RGB weights of each voronoi cell.
  */
 vector<vector<float> > Voronoi::getRGBWeights()
 {
     return m_rgbWeights;
 }

 /**
  * Getter that return the intensity of each voronoi cell.
  * @brief getIntensity
  * @return the intensity of each voronoi cell.
  */
 vector<float> Voronoi::getIntensity()
 {
     return m_intensity;
 }
