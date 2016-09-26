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
 * \file voronoi.h
 * \brief Class that computes the voronoi diagram.
 * \author Antoine Toisoul Le Cann
 * \date August, 7th, 2014
 *
 * Class used to compute the voronoi diagram needed for the relighting.
 */

#ifndef VORONOI_H_INCLUDED
#define VORONOI_H_INCLUDED

#include <cstdio>
#include <vector>

#include "LightingBasis.h"
#include "imageProcessing.h"

class Voronoi
{
    public:

    /**
     * Default contructor of the Voronoi class. Set the size of the environment map to 1024x512 by default.
     * @brief Voronoi
     */
    Voronoi();

    /**
     * Constructor that creates a Voronoi diagram given a lighting basis and the cell numbers for each picture of the lighting basis.
     * @brief Voronoi
     * @param basis The lighting basis which is used to create the Voronoi diagram.
     * @param envMapWidth width of the environment map.
     * @param envMapHeight height of the environment map.
     * @param cellNumberPerPicture vector that contains the voronoi cells for each picture (each picture of the reflectance field may have more than one picture). cellNumberPerPicture[i] is the vector containing the cell numbers for picture i of the reflectance field.
     */
    Voronoi(LightingBasis& basis, unsigned int envMapWidth, unsigned int envMapHeight, std::vector<std::vector<int> > &cellNumberPerPicture);

    /**
      * Destructor of the Voronoi class
      */
    virtual ~Voronoi();

    /**
     * Method that creates the voronoi diagram using the position of the point light sources.
     * @brief setVoronoi
     * @param pointLightSourcePosition vector that contains the position of point light sources (Point2i objects).
     */
    void setVoronoi(std::vector<cv::Point2i> &pointLightSourcePosition);

    /**
     * Method that creates the voronoi diagram using the position of the point light sources. Several point light sources can be associated to one picture in the relighting.
     * The vector cellNumberPerPicture the information for each picture of the reflectance field. cellNumberPerPicture[i] is a vector that contains the point light sources (as a number) for picture i.
     * @brief setVoronoi
     * @param pointLightSourcePosition vector that contains the position (Point2i) of the point light sources.
     * @param cellNumberPerPicture contains the information for each picture of the reflectance field. cellNumberPerPicture[i] is a vector that contains the point light sources (as a number) for picture i.
     */
    void setVoronoi(std::vector<cv::Point2i> &pointLightSourcePosition, std::vector<std::vector<int> > &cellNumberPerPicture);

    /**
     * Reinitialize the variables of the Voronoi diagram.
     * @brief clearVoronoi
     */
    void clearVoronoi();

    /**
     * Method that adds a point light source to the voronoi diagram.
     * @brief addPointLight
     * @param lightPosition is the position of the point light source.
     */
    void addPointLight(cv::Point2i lightPosition);

    /**
     * Method that adds an area light source to the voronoi diagram (the center of the light source is considered).
     * @brief addAreaLight
     * @param startingPoint
     * @param endingPoint
     */
    void addAreaLight(cv::Point2i startingPoint, cv::Point2i endingPoint);

    /**
     * Method to calculate the number of pixels in each voronoi cell.
     * The result is stored in the numberOfPixelsInVoronoiCellareaLights vector.
     * @brief numberOfPixelsPerVoronoiCell
     */
    void numberOfPixelsPerVoronoiCell();

    /**
     * Method to paint the point light sources on the environment map.
     * @brief paintPointLights
     * @param environmentMap Mat image that contains the image on which the point light sources are painted.
     */
    void paintPointLights(cv::Mat& environmentMap);

    /**
     * Method to paint the area light sources on the environment map.
     * @brief paintAreaLights
     * @param environmentMap Mat image that contains the image on which the area light sources (rectangles) are painted.
     */
    void paintAreaLights(cv::Mat& environmentMap);

    /**
    * Method that draws the Voronoi diagram on an image.
    * @param INPUT : img is a OpenCV Mat&. It is the image on which the Voronoi diagram will be drawn.
    */
    void paintVoronoi(cv::Mat& img);

    /**
     * Method that paints the light stage intensities of each voronoi cell.
     * @brief paintLightStageIntensities
     * @param img is an OpenCV Mat image that contains the image on which the light stage intensities are painted.
     */
    void paintLightStageIntensities(cv::Mat& img);

    /**
    * Method that paints the Voronoi cells on an image. The color of each Voronoi cell is given by the rgbWeights vector.
    * @param INPUT : img is a OpenCV Mat&. It is the image on which the Voronoi cells will be drawn.
    */
    void paintVoronoiCells(cv::Mat& img);

    /**
     * Method that paints the Voronoi cells on an image depending on the cellNumberPerPicture vector. Each picture in the reflectance field might have several corresponding voronoi cells.
     * The color of each Voronoi cell is given by the rgbWeights vector.
     * @brief paintVoronoiCellsOR
     * @param INPUT : img is a OpenCV Mat&. It is the image on which the Voronoi cells will be drawn.
     */
    void paintVoronoiCellsOR(cv::Mat& img);

    /**
    * Method that paints the boundary of the given Voronoi cells (only).
    * @param INPUT : img is a OpenCV Mat&. It is the image on which the Voronoi diagram will be drawn.
    * @param INPUT : voronoiCells is a vector of int containing the Voronoi cells numbers which boundary will be drawn.
    */
    void paintSpecificVoronoiCellsBoundary(cv::Mat& img, std::vector<int> &voronoiCells);

    /**
    * Method that only paints the given Voronoi cells.
    * @param INPUT : img is a OpenCV Mat&. It is the image on which the Voronoi diagram will be drawn.
    * @param INPUT : voronoiCells is a vector of int containing the Voronoi cells numbers which boundary will be drawn.
    * @param INPUT : greyColor is a vector of float the value of the shade of grey used to paint the voronoi cells. greyColor[i] contains the shade of grey gvoronoiCells.
    */
    void paintSpecificVoronoiCells(cv::Mat& img, std::vector<int> &voronoiCells, std::vector<float> &greyColor);

    /**
    * Method that paints the given Voronoi cells entirely with the intensity (shade of grey) given in the vector weights.
    * @param INPUT : img is a OpenCV Mat&. It is the image on which the Voronoi diagram will be drawn.
    */
    void paintVoronoiIntensity(cv::Mat& img);

    /**
    * Method that computes the intensity of each Voronoi cell(taking into account the solid angle).
    * @param INPUT : environmentMap is an OpenCV Mat of floats containing the HDR values of the environment map.
    */
    void computeVoronoiIntensity(cv::Mat &environmentMap);

    /**
    * Method that computes the RGB intensity of each Voronoi cell (taking into account the solid angle).
    * @param INPUT : environmentMap is an OpenCV Mat of floats containing the HDR values of the environment map.
    */
    void computeVoronoiWeightsRGB(const cv::Mat &environmentMap, float offset);

    /**
    * Method that computes the sum of each pixel in each Voronoi cell (taking into account the solid angle).
    * Each pixel is multiplied by a gaussian function that depends on its distance to the center of the Voronoi cell.
    * @param INPUT : environmentMap is an OpenCV Mat of floats containing the HDR values of the environment map.
    */
    void computeVoronoiWeightsGaussian(const cv::Mat &environmentMap, const float offset);

    /**
    * Method that compute the weight for each image of the reflectance field.
    * Each image might have several corresponding voronoi cells.
    * @param INPUT : environmentMap is an OpenCV Mat of floats containing the HDR values of the environment map.
    * @param INPUT : offset is the offset added for the rotation of the environment map.
    */
    void computeVoronoiWeightsOR(const cv::Mat &environmentMap, const float offset);

    /**
     * Method that compute the weight for each image of the reflectance field. Each pixel is multiplied by a gaussian function that depends on its distance to the center of the Voronoi cell.
     * Each image might have several corresponding voronoi cells.
     * @brief computeVoronoiWeightsGaussianOR
     * @param INPUT : environmentMap is an OpenCV Mat of floats containing the HDR values of the environment map.
     * @param INPUT : offset is the offset added for the rotation of the environment map.
     * @param INPUT : varianceX variance of the 2D Gaussian along the first dimension. varianceX[i] contains the varianceX that is used for the cell of picture i.
     * @param INPUT : varianceY variance of the 2D Gaussian along the second dimension. varianceX[i] contains the varianceY that is used for the cell of picture i.
     */
    void computeVoronoiWeightsGaussianOR(const cv::Mat &environmentMap, const float offset, float varianceX[], float varianceY[]);

    /**
    * Given a centroid of the voronoi cell (a pixel in the environment map), the method returns the number of the corresponding light source.
    * @param INPUT : x is the abscissa of the centroid.
    * @param INPUT : y is the ordinate of the centroid.
    * @return returns the corresponding light source number.
    */
    int findLightSource(int x, int y);

    /**
    * Given a pixel in the environment map, the method returns the number of the nearest light source.
    * @param INPUT : x is the abscissa of the pixel.
    * @param INPUT : y is the ordinate of the pixel.
    * @return returns the corresponding light source number.
    */
    int findNearestLightSource(int x, int y);

    /**
     * Given a Voronoi cell number, the function returns the number of corresponding image in the reflectance field.
     * @brief findImageNumber
     * @param cellNumber is the Voronoi cell number.
     * @return returns the number of corresponding image in the reflectance field.
     */
    int findImageNumber(int cellNumber);


    /**
     * Method that saves the voronoi diagram to a file.
     * @brief saveVoronoi
     */
    void saveVoronoi();

    /**
     * Method that load the voronoi diagram (lighting basis and cellNumberPerPicture) from voronoi.txt and basis.txt files.
     * @brief loadVoronoi
     */
    void loadVoronoi();

    /**
     * Method that sets the vector m_cellNumberPerPicture.
     * @brief setCellNumberPerPicture
     * @param cellNumberPerPicture is the vector used to set m_cellNumberPerPicture.
     */
    void setCellNumberPerPicture(std::vector<std::vector<int> > &cellNumberPerPicture);

    /**
     * Method that sets the size of the environment map.
     * @brief setEnvironmentMapSize
     * @param width of the environment map.
     * @param height of the environment map.
     */
    void setEnvironmentMapSize(unsigned int width, unsigned int height);

    /**
     * Method that reinitialise the vectors containing the weights.
     * @brief clearWeights
     */
    void clearWeights();

    /**
     * Getter that returns the RGB weights of each voronoi cell.
     * @brief getRGBWeights
     * @return the RGB weights of each voronoi cell.
     */
    std::vector<std::vector<float> > getRGBWeights();

    /**
     * Getter that return the intensity of each voronoi cell.
     * @brief getIntensity
     * @return the intensity of each voronoi cell.
     */
    std::vector<float > getIntensity();


    private:

    LightingBasis m_basis; /*!< The lighting basis corresponding to the Voronoi tesselation*/
    std::vector<int> m_numberOfPixelsInVoronoiCell; /*!< A vector containing the number of pixels in each Voronoi cell*/
    cv::Subdiv2D m_voronoiSubdivision; /*!< The Voronoi subdivision*/
    std::vector<std::vector<int> > m_cellNumberPerPicture; /*!< A vector that contains the cell numbers corresponding to each picture of the relfectance field*/

    std::vector<float> m_intensity; /*!< A vector containing the average intensity of each cell*/
    std::vector<std::vector<float> > m_rgbWeights; /*!< A vector containing the weights (per color channel) of each Voronoi cell*/

    unsigned int m_envMapWidth; /*!< The width of the environment map*/
    unsigned int m_envMapHeight; /*!< The height of the environment map*/
};

#endif // VORONOI_H_INCLUDED
