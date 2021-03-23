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
 * \file mainWindow.h
 * \brief Header for the main window of the GUI.
 * \author Antoine Toisoul Le Cann
 * \date July, 28th, 2014
 *
 * Class that contains the information about the main window (widgets, relighting)
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include "lightStageRelighting.h"
#include "officeRoomRelighting.h"
#include "progressWindow.h"
#include "freeformlightstage.h"

#include <sstream>

#include <QApplication>
#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QComboBox>
#include <QGridLayout>
#include <QTabWidget>
#include <QSpinBox>
#include <QMessageBox>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QRadioButton>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QPlainTextEdit>
#include <QSlider>
#include <QCheckBox>

class MainWindow : public QWidget
{
    Q_OBJECT

    public:

        /**
         * Default constructor of the main window class.
         * By default the window has a size of 600x650.
         * @brief MainWindow
         */
        MainWindow();

        /**
         * Constructor of the Main Window class. Creates a window with a width and height given as an input.
         * @brief MainWindow
         * @param width of the main window.
         * @param height of the main window.
         */
        MainWindow(int width, int height);

        /**
         * Destructor of the main window class. Releases the memory.
         * @brief ~MainWindow
         */
        virtual ~MainWindow();

        /**
         * Function that sets all the parameters of the widgets of the window. The function also applies a layout.
         * @brief buildWindow
         */
        void buildWindow();

    public slots:

        /**
         * Qt slot that changes the parameters of the room depending on the object.
         * @brief updateParametersOR
         */
        void updateParametersOR(const QString &object);

        /**
         * Qt slot that sets and start the light stage relighting with the parameters of the window.
         * @brief startLightStageRelighting
         */
        void startLightStageRelighting();

        /**
         * Qt slot that sets and start the free form relighting with the parameters of the window.
         * @brief startFreeFormRelighting
         */
        void startFreeFormRelighting();

        /**
         * Qt slot that sets and start the office room relighting with the parameters of the window.
         * @brief startOfficeRoomRelighting
         */
        void startOfficeRoomRelighting();

        /**
         * Qt slots that dynamically changes the range in which the picture corresponding to the indirect light is.
         * This range depends on the number of lighting conditions available : [1, numberOflightingConditions]
         * @brief changeRangeIndirectLightPicture
         * @param indirectLightPicture INPUT : upper bound of the range [1, numberOfLightingConditions (or indirectLightPicture)]
         */
        void changeRangeIndirectLightPicture(int indirectLightPicture);

    private:

        QTabWidget* m_tabs; /*!< The main widget containing the three tabs*/
        QWidget* m_officeRoomTab; /*!< The widget containing the office room tab*/
        QWidget* m_freeFormTab; /*!< The widget containing the free form tab*/
        QWidget* m_lightStageTab; /*!< The widget containing the light stage tab*/

        //Office room widgets OR
        QPushButton* m_startButtonOR; /*!< Start button for the office room relighting*/
        QGridLayout* m_gridLayoutOR; /*!< Grid layout for the office room relighting*/
        QComboBox* m_objectOR; /*!< Combo box to choose the object to be relit (office room)*/
        QLabel* m_objectLabelOR; /*!< Text associated with the object combo box*/
        QLabel* m_envMapLabelOR; /*!< Text associated with the environment map combo box*/
        QLabel* m_lightTypeLabelOR; /*!< Text associated with the light type combo box*/
        QLabel* m_numberOffsetsLabelOR; /*!< Text associated with the number of offsets spin box*/
        QComboBox* m_envMapOR; /*!< Combo box to choose the environment map in which the object will be relit (office room)*/
        QComboBox* m_lightTypeOR; /*!< Combo box to choose the type of lights (office room)*/
        QSpinBox* m_numberOffsetsOR; /*!< Spin box to choose the number of rotations of the environment map (office room)*/
        QLabel* m_numberOfSamplesLabelOR; /*!< Text associated with the number of samples combo box*/
        QComboBox* m_numberOfSamplesOR; /*!< Combo box to choose the number of samples in the inverse CDF algorithm (office room)*/
        QLabel* m_numberOfLightingConditionsLabelOR; /*!< Combo box to choose the number of samples in the inverse CDF algorithm (office room)*/
        QSpinBox* m_numberOfLightingConditionsOR; /*!< Spin box to choose the number of lighting conditions (office room)*/
        QLabel* m_darkRoomPictureLabelOR; /*!< Text associated with the indirect light picture spin box (office room)*/
        QSpinBox* m_indirectLightPictureOR; /*!< Spin box to choose the picture containing the indirect light (office room)*/
        QGroupBox* m_RadioButtonBoxOR; /*!< Group box containing the method to select lights (office room)*/
        QHBoxLayout* m_layoutButtonsOR; /*!< Horizontal layout for the radio buttons (for light identification)*/
        QRadioButton* m_manualButtonOR; /*!< Radio button for manual selection of light sources  (office room)*/
        QRadioButton* m_inverseCDFButtonOR; /*!< Radio button for inverse CDF algorithm for light source selection  (office room)*/
        QRadioButton* m_medianEnergyButtonOR; /*!< Radio button for selection of light sources based on the centroid of energy  (office room)*/
        QRadioButton* m_masksButtonOR; /*!< Radio button to use masks to computes the weights of each light sources (office room)*/
        QLabel* m_exposureLabelOR; /*!< Text associated with the exposure spin box  (office room)*/
        QDoubleSpinBox* m_exposureSpinBoxOR; /*!< Double spin box to change the exposure of the final result (office room)*/
        QGroupBox* m_optimisationGroupBoxOR; /*!< Group box containing the method for optimisation (office room)*/
        QHBoxLayout* m_layoutOptimisationOR; /*!< Horizontal layout the radio buttons (for optimisation)*/
        QRadioButton* m_disabledButtonOR; /*!< Radio button to disable the optimisation (office room)*/
        QRadioButton* m_originalSpaceButtonOR; /*!< Radio button to enable the optimisation in the original space (office room)*/
        QRadioButton* m_PCAButtonOR; /*!< Radio button to enable the optimisation in PCA Space (office room)*/
        QGroupBox* m_masksGroupBoxOR; /*!< Group box containing the type of masks to use (office room)*/
        QHBoxLayout* m_layoutMasksOR; /*!< Horizontal layout for radio buttons (for masks)*/
        QRadioButton* m_highFreqOR; /*!< Radio button to use masks adapted to high frequency environment maps (office room)*/
        QRadioButton* m_lowFreqOR; /*!< Radio button to use masks adapted to low frequency environment maps (office room)*/
        QCheckBox* m_computeBasisMaskOR; /*!< Checkbox to compute the basis and the masks if not already computed (office room)*/

        //Light stage widgets LS
        QPushButton* m_startButtonLS; /*!< Start button for the light stage relighting*/
        QGridLayout* m_gridLayoutLS; /*!< Grid layout for the light stage relighting*/
        QComboBox* m_objectLS; /*!< Combo box to choose the object to be relit (light stage)*/
        QLabel* m_objectLabelLS; /*!< Text associated with the object combo box (light stage)*/
        QLabel* m_envMapLabelLS; /*!< Text associated with the environment map combo box (light stage)*/
        QLabel* m_lightTypeLabelLS; /*!< Text associated with the light type combo box (light stage)*/
        QLabel* m_numberOffsetsLabelLS; /*!< Text associated with the number of offsets spin box (light stage)*/
        QComboBox* m_envMapLS; /*!< Combo box to choose the environment map in which the object will be relit (light stage)*/
        QComboBox* m_lightTypeLS; /*!< Combo box to choose the type of lights (light stage)*/
        QSpinBox* m_numberOffsetsLS; /*!< Spin box to choose the number of rotations of the environment map (light stage)*/

        //Free form light stage widgets
        QPushButton* m_startButtonFF; /*!< Start button for the free form relighting*/
        QGridLayout* m_gridLayoutFF; /*!< Grid layout for the free form relighting*/
        QLabel* m_envMapLabelFF; /*!< Text associated with the environment map combo box (free form)*/
        QLabel* m_lightTypeLabelFF; /*!< Combo box to choose the type of lights (free form)*/
        QLabel* m_numberOffsetsLabelFF; /*!< Text associated with the number of offsets spin box (free for)*/
        QComboBox* m_envMapFF; /*!< Combo box to choose the environment map in which the object will be relit (free form)*/
        QComboBox* m_lightTypeFF; /*!< Combo box to choose the type of lights (free form)*/
        QLabel* m_numberOfLightingConditionsLabelFF; /*!< Text associated with the number of lighting conditions spin box (free form)*/
        QSpinBox* m_numberOfLightingConditionsFF; /*!< Spin box to choose the number of lighting conditions (free form)*/
        QSpinBox* m_numberOffsetsFF; /*!< Spin box to choose the number of rotations of the environment map (free form)*/
        QLabel* m_exposureLabelFF; /*!< Text associated with the exposure spin box  (free form)*/
        QDoubleSpinBox* m_exposureSpinBoxFF; /*!< Double spin box to change the exposure of the final result (free form)*/
        QGroupBox* m_RadioButtonLightsBoxFF; /*!< Group box containing the method to select lights (free form)*/
        QHBoxLayout* m_layoutButtonsLightsFF; /*!< Horizontal layout for the radio buttons (for light identification - free form)*/
        QRadioButton* m_manualButtonFF; /*!< Radio button for manual selection of light sources  (free form)*/
        QRadioButton* m_loadButtonFF; /*!< Radio button to load voronoi diagram from a file (free form)*/
        QCheckBox* m_saveVoronoiFF; /*!< Checkbox to save the voronoi diagram to a file (free form)*/

        LightStageRelighting* m_LSRelighting; /*!< Object to compute the light stage relighting*/
        FreeFormLightStage* m_FFRelighting; /*!< Object to compute the free form relighting*/
        OfficeRoomRelighting* m_ORRelighting; /*!< Object to compute the office room relighting*/
        ProgressWindow* m_progressWindow; /*!< Progress Window that display the results of the relighting and the progress of the current relighting*/

};

#endif // MAINWINDOW_H
