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
 * \file imageProcessing.cpp
 * \brief File that contains the functions that are related to image processing
 * \author Antoine Toisoul Le Cann
 * \date August, 7th, 2014
 *
 * The functions are related to image processing using OpenCV or not.
 */

#include "imageProcessing.h"

using namespace std;
using namespace cv;

/**
 * Apply a gamma correction to a RGB image (OpenCV Mat image).
 * @param INPUT : rgbImage is the image to which the gamma correction is applied.
 * @param OUTPUT : rgbImageWithGamma is the rgbImage with the gamma applied.
 * @param INPUT : gamma is a double corresponding to the value of the gamma correction.
 */
void gammaCorrectionImage(const Mat &rgbImage, Mat &rgbImageWithGamma, double gamma)
{
    Mat channel[3],channel32F[3], channelWithGamma[3];

    split(rgbImage, channel);

    channel[0].convertTo(channel32F[0], CV_32F);
    channel[1].convertTo(channel32F[1], CV_32F);
    channel[2].convertTo(channel32F[2], CV_32F);

    pow(channel32F[0], 1.0/gamma, channelWithGamma[0]);
    pow(channel32F[1], 1.0/gamma, channelWithGamma[1]);
    pow(channel32F[2], 1.0/gamma, channelWithGamma[2]);

    merge(channelWithGamma,3,rgbImageWithGamma);
}

/**
 * Apply a gamma correction to a RGB image stored in an array of floats.
 * @param INPUT : image is the image to which the gamma correction is applied. It is an array of floats.
 * @param INPUT : width is the width of the image.
 * @param INPUT : height is the height of the image.
 * @param INPUT : numberOfComponents is the number of components of the image (3 for a RGB image).
 * @param INPUT : gamma is a float corresponding to the value of the gamma correction.
 */
void gammaCorrectionImage(float* image, unsigned int& width, unsigned int& height, unsigned int& numberOfComponents,const float& gamma)
{
	float pixelR = 0.0f, pixelG = 0.0f, pixelB = 0.0f;

	for(unsigned int i = 0 ; i< height; i++)
	{
		for(unsigned int j = 0 ; j< width; j++)
		{
			pixelR = image[(i*width+j)*numberOfComponents];
			pixelG = image[(i*width+j)*numberOfComponents+1];
			pixelB = image[(i*width+j)*numberOfComponents+2];

			image[(i*width+j)*numberOfComponents] = clamp(pow(pixelR, 1.0/gamma), 0.0, 1.0);
			image[(i*width+j)*numberOfComponents+1] = clamp(pow(pixelG, 1.0/gamma), 0.0 , 1.0);
			image[(i*width+j)*numberOfComponents+2] = clamp(pow(pixelB, 1.0/gamma), 0.0, 1.0);
		}
	}

}

/**
 * Remove the gamma correction of a RGB image (OpenCV Mat image).
 * @param INPUT : rgbImage is the image to which the gamma correction is removed. rgbImage is an OpenCV CV_8UC3 matrix. (matrix of 3 channels of 8 bits unsigned char).
 * @param OUTPUT : rgbImageWithGamma is the rgbImage with the gamma removed. It is a CV_32FC3 matrix (matrix of 3 channels of 32 bits floats).
 * @param INPUT : gamma is a double corresponding to the value of the gamma correction.
 */
void removeGammaCorrection(const Mat &rgbImage, Mat &rgbImageWithoutGamma, double gamma)
{
    Mat channel[3], channel32F[3], channelWithoutGamma[3];

    split(rgbImage, channel);

    channel[0].convertTo(channel32F[0], CV_32F);
    channel[1].convertTo(channel32F[1], CV_32F);
    channel[2].convertTo(channel32F[2], CV_32F);

    //Channel32F are matrices. Each element is a float between 0 and 255
    channel32F[0] /= 255.0;
    channel32F[1] /= 255.0;
    channel32F[2] /= 255.0;

    pow(channel32F[0], gamma, channelWithoutGamma[0]);
    pow(channel32F[1], gamma, channelWithoutGamma[1]);
    pow(channel32F[2], gamma, channelWithoutGamma[2]);

    merge(channelWithoutGamma,3,rgbImageWithoutGamma);
}

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
void gaussianBlur2D(float* image, unsigned int& width, unsigned int& height, unsigned int& numberOfComponents ,int blurSize, float* imageBlur)
{
    int halfSize = blurSize/2;
    double* blurCoeficients = new double[blurSize];
    double variance = 100.0;
    double sumR = 0.0;
    double sumG = 0.0;
    double sumB = 0.0;
    float* tmp = new float[width*height*numberOfComponents];

    for(int k = -halfSize ; k<=halfSize; k++)
    {
        blurCoeficients[k+halfSize] = 1.0/sqrt(2*M_PI*variance)*exp(-k*k/(2*variance));
    }

    //Blur along the rows
   	for(unsigned int i =  0; i<height ; i++)
	{
		for(unsigned int j = 0 ; j<width ; j++)
		{
            sumR = 0.0;
            sumG = 0.0;
            sumB = 0.0;

            for(int k = -halfSize ; k<=halfSize; k++)
            {
                    if((j+k)>=0 && (j+k)<width)
                    {
                        sumR += image[numberOfComponents*(i*width+j+k)]*blurCoeficients[k+halfSize];
                        sumG += image[numberOfComponents*(i*width+j+k)+1]*blurCoeficients[k+halfSize];
                        sumB += image[numberOfComponents*(i*width+j+k)+2]*blurCoeficients[k+halfSize];
                    }
            }
            tmp[3*(i*width+j)] = sumR;
            tmp[3*(i*width+j)+1] = sumG;
            tmp[3*(i*width+j)+2] = sumB;
		}

    }


    //Blur along the columns
    for(unsigned int i =  0; i<height ; i++)
	{
		for(unsigned int j = 0 ; j<width ; j++)
		{
            sumR = 0.0;
            sumG = 0.0;
            sumB = 0.0;

            for(int k = -halfSize ; k<=halfSize; k++)
            {
                    if((i+k) > 0 && (i+k) < height)
                    {
                        sumR += tmp[numberOfComponents*((i+k)*width+j)]*blurCoeficients[k+halfSize];
                        sumG += tmp[numberOfComponents*((i+k)*width+j)+1]*blurCoeficients[k+halfSize];
                        sumB += tmp[numberOfComponents*((i+k)*width+j)+2]*blurCoeficients[k+halfSize];
                    }
            }
            imageBlur[numberOfComponents*(i*width+j)] = sumR;
            imageBlur[numberOfComponents*(i*width+j)+1] = sumG;
            imageBlur[numberOfComponents*(i*width+j)+2] = sumB;
		}
    }


    delete[] blurCoeficients;
    delete[] tmp;
}

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
void inverse2DFunction(float& value, float f[], unsigned int& width, unsigned int& height, unsigned int& resultI, unsigned int& resultJ)
{
	float epsilon = 0.01f;
	float energy = 0.0f;

	for(unsigned int i =  0; i<height ; i++)
	{
		for(unsigned int j = 0 ; j<width ; j++)
		{
            energy = f[i*width+j];
            if(energy<(value+epsilon) && energy>(value-epsilon))
			{
				resultI = i;
				resultJ = j;
			}
		}
	}
}

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
void inverseCDFAlgorithm(const Mat &environmentMap, unsigned int& width, unsigned int& height, unsigned int& numberOfComponents,
                         unsigned int& numberOfSamples, Mat &samplesLocation)
{
    float* pXY = new float[width*height];
    float* cdf = new float[width*height];

    compute2DDistributionFunction(environmentMap, width, height, pXY, cdf);

    //Inverse CDF algorithm
	vector<unsigned int> resultsI;
	vector<unsigned int> resultsJ;

    float uniformSample = 0.0f;
	unsigned int resultI = 0;
	unsigned int resultJ = 0;


	for(unsigned int k = 0 ; k<numberOfSamples ; k++)
	{
		uniformSample = (float) k/(numberOfSamples);//uniform samples on the y axis
        inverse2DFunction(uniformSample, cdf, width, height, resultI, resultJ);
		resultsI.push_back(resultI);
		resultsJ.push_back(resultJ);
	}

	for(unsigned int i = 0; i<resultsI.size() ; i++)
	{
		resultI = resultsI[i];
		resultJ = resultsJ[i];
        samplesLocation.at<float>(i,0) = resultsI[i];
        samplesLocation.at<float>(i,1) = resultsJ[i];

	}


    delete[] pXY;
    delete[] cdf;
}

/**
 * Function that paints green points on an image at specific locations.
 * @brief paintSamples
 * @param INPUT/OUTPUT : image on which the green points are painted. It is an OpenCV Mat of floats.
 * @param INPUT : width of the image.
 * @param INPUT : height of the image.
 * @param INPUT : samplesLocation is a matrix that contains the location of the points. Each row corresponds to a point (x,y) that will be painted.
 */
void paintSamples(Mat &image, unsigned int& width, unsigned int& height, Mat &samplesLocation)
{
    int resultI = 0;
    int resultJ = 0;

    for(int i = 0; i<samplesLocation.rows ; i++)
	{
		resultI = samplesLocation.at<float>(i,0);
		resultJ = samplesLocation.at<float>(i,1);

        image.at<Vec3f>(resultI,resultJ).val[2] = 0.0;
        image.at<Vec3f>(resultI,resultJ).val[1] = 1.0;
        image.at<Vec3f>(resultI,resultJ).val[0] = 0.0;

        if((resultI != height-1) && (resultJ != width-1))
		{
            image.at<Vec3f>(resultI,resultJ-1).val[2] = 0.0;
            image.at<Vec3f>(resultI,resultJ-1).val[1] = 1.0;
            image.at<Vec3f>(resultI,resultJ-1).val[0] = 0.0;

            image.at<Vec3f>(resultI,resultJ+1).val[2] = 0.0;
            image.at<Vec3f>(resultI,resultJ+1).val[1] = 1.0;
            image.at<Vec3f>(resultI,resultJ+1).val[0] = 0.0;

            image.at<Vec3f>(resultI-1,resultJ-1).val[2] = 0.0;
            image.at<Vec3f>(resultI-1,resultJ-1).val[1] = 1.0;
            image.at<Vec3f>(resultI-1,resultJ-1).val[0] = 0.0;

            image.at<Vec3f>(resultI-1,resultJ+1).val[2] = 0.0;
            image.at<Vec3f>(resultI-1,resultJ+1).val[1] = 1.0;
            image.at<Vec3f>(resultI-1,resultJ+1).val[0] = 0.0;

            image.at<Vec3f>(resultI-1,resultJ).val[2] = 0.0;
            image.at<Vec3f>(resultI-1,resultJ).val[1] = 1.0;
            image.at<Vec3f>(resultI-1,resultJ).val[0] = 0.0;

            image.at<Vec3f>(resultI+1,resultJ-1).val[2] = 0.0;
            image.at<Vec3f>(resultI+1,resultJ-1).val[1] = 1.0;
            image.at<Vec3f>(resultI+1,resultJ-1).val[0] = 0.0;

            image.at<Vec3f>(resultI+1,resultJ+1).val[2] = 0.0;
            image.at<Vec3f>(resultI+1,resultJ+1).val[1] = 1.0;
            image.at<Vec3f>(resultI+1,resultJ+1).val[0] = 0.0;

            image.at<Vec3f>(resultI+1,resultJ).val[2] = 0.0;
            image.at<Vec3f>(resultI+1,resultJ).val[1] = 1.0;
            image.at<Vec3f>(resultI+1,resultJ).val[0] = 0.0;
        }
	}

    ostringstream osstream;

#if defined(__APPLE__) && defined(__MACH__)
    osstream << qApp->applicationDirPath().toStdString() << "/../../..";
#else
    osstream << qApp->applicationDirPath().toStdString();
#endif

    osstream << "/envMapSamples.pfm";

    savePFM(image, osstream.str());
}

/**
 * Function that rotates a latitude longitude environment map along the y axis (adds an offset to the phi angle).
 * @brief rotateLatLongMap
 * @param INPUT : originalMap is the original environment map.
 * @param INPUT : offset is the offset that is added to the phi angle.
 * @param OUTPUT : result is the result of the rotated environment map.
 */
void rotateLatLongMap(const Mat& originalMap, const float offset, Mat& result)
{
    int width = originalMap.cols;
    int height = originalMap.rows;
    originalMap.convertTo(originalMap, CV_32FC3);


    Rect boundingBoxResult(0,0, width, height);
    result = Mat(boundingBoxResult.size(), CV_32FC3);
    result = Scalar::all(0.0);

    int jOffset = floor(moduloRealNumber(offset, 2.0*M_PI)/(2.0*M_PI)*width);

    for(int i = 0 ; i<height ; i++)
    {
        for(int j = 0 ; j<width; j++)
        {
            int jModulus = (j+jOffset) % width;
            //OpenCV uses BGR components
            result.at<Vec3f>(i,j).val[0] = originalMap.at<Vec3f>(i,jModulus).val[0];
            result.at<Vec3f>(i,j).val[1] = originalMap.at<Vec3f>(i,jModulus).val[1];
            result.at<Vec3f>(i,j).val[2] = originalMap.at<Vec3f>(i,jModulus).val[2];
        }
    }
}

/**
 * Function that reads images (hardcoded name) and crops them in the rectangle defined by (xStart,yStart) and (xEnd,yEnd).
 * @brief cropImages
 * @param INPUT : xStart is the abscissa of the upper right point of the rectangle.
 * @param INPUT : yStart is the ordinate of the upper right point of the rectangle.
 * @param INPUT : xEnd is the abscissa of the upper bottom left of the rectangle.
 * @param INPUT : yEnd is the ordinate of the upper bottom left of the rectangle.
 * @param INPUT : numberOfImages is the number of images to be cropped.
 */
void cropImages(int xStart, int yStart, int xEnd, int yEnd, int numberOfImages)
{
    ostringstream osstream;

    Mat* listOfImages = new Mat[numberOfImages];
    osstream.str("");
    //Load images
    for(int k = 0 ; k<numberOfImages ; k++)
    {
        int imageNumber = k;

#if defined(__APPLE__) && defined(__MACH__)
    osstream << qApp->applicationDirPath().toStdString() << "/../../..";
#else
    osstream << qApp->applicationDirPath().toStdString();
#endif

        if(k<10)
            osstream << "/images/office_room/Egg_bedroom45_000" << imageNumber << ".TIF";
        else
            osstream << "/images/office_room/Egg_bedroom45_00" << imageNumber << ".TIF";

        cout << osstream.str() << endl;
        //listOfImages[k] = loadPFMOpenCV(osstream.str());
        listOfImages[k] = imread(osstream.str(), CV_LOAD_IMAGE_ANYDEPTH | CV_LOAD_IMAGE_COLOR);

        if(!listOfImages[k].data)
        {
           cerr << "Couldn't open the file : " << osstream.str() << endl;
        }

        osstream.str("");
    }

    int width = xEnd-xStart;
    int height = yEnd-yStart;
    Rect boundingBoxResult(xStart,yStart, width, height);

    Mat result;

    for(int k = 0 ; k<numberOfImages ; k++)
    {
        result = Mat(listOfImages[k], boundingBoxResult);

        int imageNumber = k;

#if defined(__APPLE__) && defined(__MACH__)
    osstream << qApp->applicationDirPath().toStdString() << "/../../..";
#else
    osstream << qApp->applicationDirPath().toStdString();
#endif

        if(k<10)
        {
            osstream << "/images/Cropped/Egg_bedroom45_000" << imageNumber << ".TIF";
        }
        else if(k<100)
        {
            osstream << "/images/Cropped/Egg_bedroom45_00" << imageNumber << ".TIF";
        }
        else
        {
            osstream << "/images/Cropped/Egg_bedroom0" << imageNumber << ".TIF";
        }

//        savePFMOpenCV(osstream.str(), result, result.cols, result.rows, 3);
        imwrite(osstream.str(), result);
        osstream.str("");
    }
/*
#if defined(__APPLE__) && defined(__MACH__)
    osstream << qApp->applicationDirPath().toStdString() << "/../../..";
#else
    osstream << qApp->applicationDirPath().toStdString();
#endif

    //Cropping the mask
    osstream << "/images/free_form/" << "EggFF_" << "mask" << ".png";
    cout << osstream.str() << endl;
    Mat mask = imread(osstream.str(), CV_LOAD_IMAGE_COLOR);
    osstream.str("");
    result = Mat(mask, boundingBoxResult);

#if defined(__APPLE__) && defined(__MACH__)
    osstream << qApp->applicationDirPath().toStdString() << "/../../..";
#else
    osstream << qApp->applicationDirPath().toStdString();
#endif

    osstream << "/images/Cropped/EggFF_maskCropped.png";
    imwrite(osstream.str(), result);
    osstream.str("");*/

    delete[] listOfImages;
}

/**
 * Function that crops the mirror balls in each picture taken for the office room relighting
 * Then it takes all the mirror balls and assemble them in a grid
 * @brief cropMirrorBallToMatrix
 */
void cropMirrorBallToMatrix()
{
    ostringstream file;
    file.str("");

    cout << file.str() << endl;

    Mat image;
    Mat aux;

    int imageN = 0;

    for(int k = 0 ; k<143 ; k++)
    {

         file << qApp->applicationDirPath().toStdString() << "/images/";
        imageN = k+416;
        file << "IMG_";
        if(imageN<10)
            file << "000" << imageN <<".JPG";
        else if(imageN<100)
            file << "00" << imageN<<".JPG";
        else if(imageN<1000)
            file << "0" << imageN<<".JPG";

        cout << file.str() << endl;
        image = imread(file.str(), CV_LOAD_IMAGE_COLOR);
        aux = image(Rect(1173,2389,915,915));
        file.str("");
        file << qApp->applicationDirPath().toStdString() << "/cropped/" << k << ".jpg" ;
        imwrite(file.str(), aux);
        file.str("");
    }



    Rect bb = Rect(0,0,6405,4575);
    Rect bb2 = Rect(0,0,915,915);
    Mat matrix = Mat(bb.size(), CV_8UC3);
    Mat r = Mat(bb2.size(), CV_8UC3);

    file.str("");

    for(int i = 5 ; i<10 ; i++)
    {
        for(int j = 7 ; j<14 ; j++)
        {
            file << qApp->applicationDirPath().toStdString() << "/cropped/";
            imageN = i*14+j;

            if(imageN<10)
                file << imageN <<".jpg";
            else if(imageN<100)
                file << imageN <<".jpg";
            else if(imageN<1000)
                file << imageN <<".jpg";
            cout << file.str() << endl;

            r= imread(file.str(), CV_LOAD_IMAGE_COLOR);

            int jm = j-7;
            int im = i-5;
            aux = matrix(Rect(jm*915,im*915,915,915));
            r.copyTo(aux);
            file.str("");
        }
    }
     file << qApp->applicationDirPath().toStdString() << "/matrix4.jpg";
    imwrite(file.str(), matrix);
}
