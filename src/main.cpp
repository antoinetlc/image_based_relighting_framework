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
 * \file main.cpp
 * \brief Image-based relighting using room lighting basis
 * \author Antoine Toisoul Le Cann
 * \date May, 23rd, 2014
 *
 * The program relights an object using an environment map and the reflectance field of the object, captured with a light stage, a free-form acquisition or a regular room. It uses OpenCV library version 2.4.2.
 */

#include <iostream>

#include <QApplication>
#include "mainWindow.h"

int main (int argc, char* argv[])
{

    QApplication app(argc, argv);
    MainWindow window(750,600);

    window.show();
    return app.exec();
}

