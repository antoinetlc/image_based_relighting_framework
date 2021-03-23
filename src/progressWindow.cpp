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
 * \file progressWindow.cpp
 * \brief Header for the progress window of the GUI.
 * \author Antoine Toisoul Le Cann
 * \date August, 7th, 2014
 *
 * Class that contains the information about the progress window (widgets).
 */

#include "progressWindow.h"

/**
 * Constructor of the progress window.
 * @brief ProgressWindow
 * @param INPUT : parent of the progress window (main window).
 */
ProgressWindow::ProgressWindow(QWidget *parent) :
    QDialog(parent),
    m_gridLayout(new QGridLayout()), m_progressBar(new QProgressBar(this)), m_scrollBar(new QScrollBar()),
    m_textArea(new QPlainTextEdit()), m_imageResult(new QLabel()), m_closeButton(new QPushButton("Close"))
{
    buildWindow();
}

/**
 * Destructor of the main window.
 * @brief ~ProgressWindow
 */
ProgressWindow::~ProgressWindow()
{
    delete m_progressBar;
    delete m_scrollBar;
    delete m_textArea;
    delete m_imageResult;
    delete m_closeButton;
    delete m_gridLayout;

}

/**
 * Method to build the progress window (layout, position of widgets and connection between widgets)
 * @brief buildWindow
 */
void ProgressWindow::buildWindow()
{

    //Put the image in the center of the label
    m_imageResult->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    m_imageResult->setFixedHeight(300);
    m_imageResult->setFixedWidth(300);

    m_textArea->setFixedHeight(200);
    m_gridLayout->addWidget(m_imageResult, 0,0,2,1);
    m_gridLayout->addWidget(m_progressBar, 0,1);
    m_gridLayout->addWidget(m_textArea,1,1);
    m_gridLayout->addWidget(m_closeButton, 2,1);

    m_progressBar->setMinimum(0);
    m_progressBar->setMaximum(100);

    this->setLayout(m_gridLayout);

    QObject::connect(m_closeButton, SIGNAL(clicked()), this, SLOT(close()));
}

/**
 * Qt slot that adds text to the text area.
 * @brief addText
 * @param INPUT : text that will be added to the text area.
 */
void ProgressWindow::addText(QString text)
{
    m_textArea->appendPlainText(text);
}

/**
 * Qt slot that changes the value of the progress bar.
 * @brief setValueProgressBar
 * @param INPUT : value to which the progress bar will be set.
 */
void ProgressWindow::setValueProgressBar(int value)
{
    m_progressBar->setValue(value);
}

/**
 * Method that delete all the text in the text area.
 * @brief clearText
 */
void ProgressWindow::clearText()
{
    m_textArea->clear();
}

/**
 * Method that remove the image displayed in the progress window.
 * @brief clearImage
 */
void ProgressWindow::clearImage()
{
    m_imageResult->setPixmap(QPixmap());
}

/**
 * Method that clears the text area, the image and the progress bar.
 * @brief clear
 */
void ProgressWindow::clear()
{
    this->clearImage();
    this->clearText();
    this->setValueProgressBar(0);
}

/**
 * Method that changes the image displayed in the progress window.
 * @brief updateImage
 * @param image path of the image that will be displayed.
 */
void ProgressWindow::updateImage(QString image)
{
    //Load the image and get the dimensions of the QLabel
    QPixmap qImage(image);
    int width = m_imageResult->width();
    int height = m_imageResult->height();

    //Update the QLabel using the dimensions
    m_imageResult->setPixmap(qImage.scaled(width,height,Qt::KeepAspectRatio));
}
