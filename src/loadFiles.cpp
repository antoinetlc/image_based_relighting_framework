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
 * \file loadFiles.cpp
 * \brief file that contains the implementation of the functions that read inputs (images, text files...)
 * \author Antoine Toisoul Le Cann
 * \date May, 23rd, 2014
 *
 * The functions are used to read the input files. Pictures are stored into OpenCV matrices Mat
 */

#include "loadFiles.h"


using namespace std;
using namespace cv;

/**
* This function loads the pictures corresponding to the reflectance field of an object.
* @param listOfImages is an array of OpenCV Mat object. Each element of the array is a picture of the reflectance field.
* @param numberOfImages is an integer corresponding to the number of pictures in the reflectance field.
* @param object is a string which describes the reflectance field to load. It can be either 'plant' or 'helmet'.
* @return returns EXIT_SUCCESS or EXIT_FAILURE if the files cannot be loaded.
*/
int loadImages(Mat listOfImages[], unsigned int numberOfImages, string object)
{
    string file;
    string extension;

    if(object == "Plant")
    {
        file = string("light_stage/plant_left_");
        extension = string(".png");
    }
    else if(object == "Helmet")
    {
        file = string("light_stage/helmet_side_left_");
        extension = string(".png");
    }
    else if(object == "Egg")
    {
        file = string("office_room/EggCropped_");
        extension = string(".TIF");
    }
    else if(object == "Bird")
    {
        file = string("office_room/BirdCropped_");
        extension = string(".TIF");

    }
    else if(object == "PlantOR")
    {
        file = string("office_room/plantOR_");
        extension = string(".png");
    }
    else if(object == "HelmetOR")
    {
        file = string("office_room/helmetOR_");
        extension = string(".png");
    }
    else if(object == "EggFF")
    {
        file = string("free_form/EggFF_");
        extension = string(".png");
    }
    else
    {
        cerr << "Object does not exist : " << object << endl;
        return EXIT_FAILURE;
    }

    ostringstream osstream;

    for(unsigned int i = 0 ; i<numberOfImages ; i++)
    {
#if defined(__APPLE__) && defined(__MACH__)
    osstream << qApp->applicationDirPath().toStdString() << "/../../..";
#else
    osstream << qApp->applicationDirPath().toStdString();
#endif

       if(i<10)
       {
          osstream << "/images/" << file << "000" << i << extension;
       }
       else if(i<100)
       {
          osstream << "/images/" << file << "00" << i << extension;
       }
       else
       {
          osstream << "/images/" << file << "0" << i << extension;
       }

       //For 16 bits TIF HDR images, use CV_LOAD_IMAGE_ANYDEPTH to load the image correctly (with values between 0 and 65535)
     /*  osstream.str("");
       osstream << "/Volumes/My Passport for Mac/Documents/Programmation/C++/build-image-based-relighting-Desktop_Qt_5_3_clang_64bit-Debug/";
       osstream << "images/knight_kneeling_";
       if(i<10)
       {
          osstream << "00" << i << ".png";
       }
       else if(i<100)
       {
          osstream << "0" << i << ".png";
       }
       else
       {
          osstream << i << ".png";
       }
*/
       listOfImages[i] = imread(osstream.str(), CV_LOAD_IMAGE_ANYDEPTH | CV_LOAD_IMAGE_COLOR);

       if(!listOfImages[i].data)
       {
          cerr << "Couldn't open the file : " << osstream.str() << endl;
          return EXIT_FAILURE;
       }

       osstream.str("");
    }


    return EXIT_SUCCESS;
}

/**
* This function reads txt files that have the following format :
* lightNumber1: valueR valueG valueB
* ...
* lightNumberN: valueR valueG valueB
* @param INPUT : fileName a string containing the filename
* @param OUTPUT : components a vector of a vector of floats. components[i] is a vector of floats containing the 3 values corresponding to the red, green and blue components.
*/
void readFile(const string& fileName, vector<vector<float> > &components)
{
        ifstream file(fileName.c_str(), ios::in); //Open the file in read mode

        if(file)
        {
                string lightNumber;
                string value1;
                string value2;
                string value3;

                int i = 0;
                while(file >> lightNumber) //Read the entire file. One loop = one line in the file
                {

                    file >> value1;
                    vector<float> values;
                    components.push_back(values);

                    //Store values 1, 2 and 3
                    //i corresponds to the light number

                    components[i].push_back(atof(value1.c_str()));
                    file >> value2;
                    components[i].push_back(atof(value2.c_str()));
                    file >> value3;
                    components[i].push_back(atof(value3.c_str()));

                    i++;
                }

                file.close();
        }
        else
        {
                cerr << "Cannot open the file " << fileName << endl;
        }
}

