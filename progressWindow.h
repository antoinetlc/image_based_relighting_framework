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
 * \file progressWindow.h
 * \brief Header for the progress window of the GUI.
 * \author Antoine Toisoul Le Cann
 * \date August, 7th, 2014
 *
 * Class that contains the information about the progress window (widgets).
 */

#ifndef PROGRESSWINDOW_H
#define PROGRESSWINDOW_H

#include <QDialog>
#include <QWidget>
#include <QLabel>
#include <QProgressBar>
#include <QScrollBar>
#include <QPlainTextEdit>
#include <QGridLayout>
#include <QPushButton>

class ProgressWindow : public QDialog
{
    Q_OBJECT

    public:

        /**
         * Constructor of the progress window.
         * @brief ProgressWindow
         * @param INPUT : parent of the progress window (main window).
         */
        ProgressWindow(QWidget *parent = 0);

        /**
         * Destructor of the main window.
         * @brief ~ProgressWindow
         */
        virtual ~ProgressWindow();

        /**
         * Method to build the progress window (layout, position of widgets and connection between widgets)
         * @brief buildWindow
         */
        void buildWindow();
    
    signals:
    
    public slots:
        /**
         * Qt slot that adds text to the text area.
         * @brief addText
         * @param INPUT : text that will be added to the text area.
         */
        void addText(QString text);

        /**
         * Qt slot that changes the value of the progress bar.
         * @brief setValueProgressBar
         * @param INPUT : value to which the progress bar will be set.
         */
        void setValueProgressBar(int value);

        /**
         * Method that delete all the text in the text area.
         * @brief clearText
         */
        void clearText();

        /**
         * Method that remove the image displayed in the progress window.
         * @brief clearImage
         */
        void clearImage();

        /**
         * Method that clears the text area, the image and the progress bar.
         * @brief clear
         */
        void clear();

        /**
         * Method that changes the image displayed in the progress window.
         * @brief updateImage
         * @param image path of the image that will be displayed.
         */
        void updateImage(QString image);

    private:
        QGridLayout* m_gridLayout; /*!< Grid layout of the progress window*/
        QProgressBar* m_progressBar; /*!< Progress bar*/
        QScrollBar* m_scrollBar; /*!< Vertical scroll bar for the text area*/
        QPlainTextEdit* m_textArea; /*!< Text area in which the text is displayed*/
        QLabel* m_imageResult; /*!< QLabel that displays an image*/
        QPushButton* m_closeButton; /*!< Button to close the window*/
};

#endif // PROGRESSWINDOW_H
