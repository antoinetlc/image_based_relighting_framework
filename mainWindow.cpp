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
 * \file mainWindow.cpp
 * \brief Implementation of the methods of the mainWindow class for the GUI.
 * \author Antoine Toisoul Le Cann
 * \date July, 28th, 2014
 *
 * Class that contains the information about the main window (widgets, relighting)
 */
#include "mainWindow.h"

/**
 * Default constructor of the main window class.
 * By default the window has a size of 600x650.
 * @brief MainWindow
 */
MainWindow::MainWindow() :
    QWidget(),
    m_tabs(new QTabWidget(this)), m_officeRoomTab(new QWidget()), m_freeFormTab(new QWidget()), m_lightStageTab(new QWidget()),
    m_startButtonOR(new QPushButton("Start")), m_gridLayoutOR(new QGridLayout()), m_objectOR(new QComboBox()),
    m_objectLabelOR(new QLabel("Which object do you want to use ?")), m_envMapLabelOR(new QLabel("Which environment map do you want to use ?")),
    m_lightTypeLabelOR(new QLabel("Integration method")), m_numberOffsetsLabelOR(new QLabel("Number of offsets")), m_envMapOR(new QComboBox()),
    m_lightTypeOR(new QComboBox()), m_numberOffsetsOR(new QSpinBox()), m_numberOfSamplesLabelOR(new QLabel("If Inverse CDF, number of samples for identification ?")), m_numberOfSamplesOR(new QComboBox()),
    m_numberOfLightingConditionsLabelOR(new QLabel("How many lighting conditions are there ?")), m_numberOfLightingConditionsOR(new QSpinBox()),
    m_darkRoomPictureLabelOR(new QLabel("Which picture corresponds to the dark room ?")), m_indirectLightPictureOR(new QSpinBox()),
    m_RadioButtonBoxOR(new QGroupBox("Lights selection")), m_layoutButtonsOR(new QHBoxLayout()), m_manualButtonOR(new QRadioButton("Manually")), m_inverseCDFButtonOR(new QRadioButton("Inverse CDF")),
    m_medianEnergyButtonOR(new QRadioButton("Median Energy")), m_masksButtonOR(new QRadioButton("Masks")), m_exposureLabelOR(new QLabel("Exposure change (f-stops)")), m_exposureSpinBoxOR(new QDoubleSpinBox()),
    m_optimisationGroupBoxOR(new QGroupBox("Optimisation")), m_layoutOptimisationOR(new QHBoxLayout()), m_disabledButtonOR(new QRadioButton("Disabled")), m_originalSpaceButtonOR(new QRadioButton("Original Space")),
    m_PCAButtonOR(new QRadioButton("PCA Space")),
    m_masksGroupBoxOR(new QGroupBox("Type of masks")), m_layoutMasksOR(new QHBoxLayout()), m_highFreqOR(new QRadioButton("High frequency lighting")), m_lowFreqOR(new QRadioButton("Low frequency lighting")),
    m_computeBasisMaskOR(new QCheckBox("Compute the lighting basis and masks and save to files")),
    m_startButtonLS(new QPushButton("Start")), m_gridLayoutLS(new QGridLayout()), m_objectLS(new QComboBox()),
    m_objectLabelLS(new QLabel("Which object do you want to use ?")), m_envMapLabelLS(new QLabel("Which environment map do you want to use ?")),
    m_lightTypeLabelLS(new QLabel("Integration method")), m_numberOffsetsLabelLS(new QLabel("Number of offsets")), m_envMapLS(new QComboBox()),
    m_lightTypeLS(new QComboBox()), m_numberOffsetsLS(new QSpinBox()),
    m_startButtonFF(new QPushButton("Start")), m_gridLayoutFF(new QGridLayout()), m_envMapLabelFF(new QLabel("Which environment map do you want to use ?")),
    m_lightTypeLabelFF(new QLabel("Integration method")), m_numberOffsetsLabelFF(new QLabel("Number of offsets")), m_envMapFF(new QComboBox()), m_lightTypeFF(new QComboBox()),
    m_numberOfLightingConditionsLabelFF(new QLabel("Number of lighting conditions")), m_numberOfLightingConditionsFF(new QSpinBox()),
    m_numberOffsetsFF(new QSpinBox()), m_exposureLabelFF(new QLabel("Exposure change (f-stops)")), m_exposureSpinBoxFF(new QDoubleSpinBox()),
    m_RadioButtonLightsBoxFF(new QGroupBox("Lights selection")), m_layoutButtonsLightsFF(new QHBoxLayout()), m_manualButtonFF(new QRadioButton("Manually")), m_loadButtonFF(new QRadioButton("Load from file")),
    m_saveVoronoiFF(new QCheckBox("Save Voronoi diagram (manual selection only)")),
    m_LSRelighting(new LightStageRelighting()), m_FFRelighting(new FreeFormLightStage()), m_ORRelighting(new OfficeRoomRelighting()), m_progressWindow(new ProgressWindow(this))

{
    this->setGeometry(50,50, 600,750);
    buildWindow();
}

/**
 * Constructor of the Main Window class. Creates a window with a width and height given as an input.
 * @brief MainWindow
 * @param width of the main window.
 * @param height of the main window.
 */
MainWindow::MainWindow(int width, int height) :
    QWidget(),
    m_tabs(new QTabWidget(this)), m_officeRoomTab(new QWidget()), m_freeFormTab(new QWidget()), m_lightStageTab(new QWidget()),
    m_startButtonOR(new QPushButton("Start")), m_gridLayoutOR(new QGridLayout()), m_objectOR(new QComboBox()),
    m_objectLabelOR(new QLabel("Which object do you want to use ?")), m_envMapLabelOR(new QLabel("Which environment map do you want to use ?")),
    m_lightTypeLabelOR(new QLabel("Integration method")), m_numberOffsetsLabelOR(new QLabel("Number of offsets")), m_envMapOR(new QComboBox()),
    m_lightTypeOR(new QComboBox()), m_numberOffsetsOR(new QSpinBox()), m_numberOfSamplesLabelOR(new QLabel("If Inverse CDF, number of samples for identification ?")), m_numberOfSamplesOR(new QComboBox()),
    m_numberOfLightingConditionsLabelOR(new QLabel("How many lighting conditions are there ?")), m_numberOfLightingConditionsOR(new QSpinBox()),
    m_darkRoomPictureLabelOR(new QLabel("Which picture corresponds to the dark room ?")), m_indirectLightPictureOR(new QSpinBox()),
    m_RadioButtonBoxOR(new QGroupBox("Lights identification")), m_layoutButtonsOR(new QHBoxLayout()), m_manualButtonOR(new QRadioButton("Manually")), m_inverseCDFButtonOR(new QRadioButton("Inverse CDF")),
    m_medianEnergyButtonOR(new QRadioButton("Median Energy")), m_masksButtonOR(new QRadioButton("Masks")), m_exposureLabelOR(new QLabel("Exposure change (f-stops)")), m_exposureSpinBoxOR(new QDoubleSpinBox()),   
    m_optimisationGroupBoxOR(new QGroupBox("Optimisation")), m_layoutOptimisationOR(new QHBoxLayout()), m_disabledButtonOR(new QRadioButton("Disabled")), m_originalSpaceButtonOR(new QRadioButton("Original Space")),
    m_PCAButtonOR(new QRadioButton("PCA Space")),
    m_masksGroupBoxOR(new QGroupBox("Type of masks")), m_layoutMasksOR(new QHBoxLayout()), m_highFreqOR(new QRadioButton("High frequency lighting")), m_lowFreqOR(new QRadioButton("Low frequency lighting")),
    m_computeBasisMaskOR(new QCheckBox("Compute the lighting basis and masks and save to files")),
    m_startButtonLS(new QPushButton("Start")), m_gridLayoutLS(new QGridLayout()), m_objectLS(new QComboBox()),
    m_objectLabelLS(new QLabel("Which object do you want to use ?")), m_envMapLabelLS(new QLabel("Which environment map do you want to use ?")),
    m_lightTypeLabelLS(new QLabel("Integration method")), m_numberOffsetsLabelLS(new QLabel("Number of offsets")), m_envMapLS(new QComboBox()),
    m_lightTypeLS(new QComboBox()), m_numberOffsetsLS(new QSpinBox()),
    m_startButtonFF(new QPushButton("Start")), m_gridLayoutFF(new QGridLayout()), m_envMapLabelFF(new QLabel("Which environment map do you want to use ?")),
    m_lightTypeLabelFF(new QLabel("Integration method")), m_numberOffsetsLabelFF(new QLabel("Number of offsets")), m_envMapFF(new QComboBox()), m_lightTypeFF(new QComboBox()),
    m_numberOfLightingConditionsLabelFF(new QLabel("Number of lighting conditions")), m_numberOfLightingConditionsFF(new QSpinBox()),
    m_numberOffsetsFF(new QSpinBox()), m_exposureLabelFF(new QLabel("Exposure change (f-stops)")), m_exposureSpinBoxFF(new QDoubleSpinBox()),
    m_RadioButtonLightsBoxFF(new QGroupBox("Lights selection")), m_layoutButtonsLightsFF(new QHBoxLayout()), m_manualButtonFF(new QRadioButton("Manually")), m_loadButtonFF(new QRadioButton("Load from file")),
    m_saveVoronoiFF(new QCheckBox("Save Voronoi diagram (manual selection only)")),
    m_LSRelighting(new LightStageRelighting()), m_FFRelighting(new FreeFormLightStage()), m_ORRelighting(new OfficeRoomRelighting()), m_progressWindow(new ProgressWindow(this))
{
    this->setGeometry(50,50, width,height);
    this->setFixedSize(width, height);
    buildWindow();
}

/**
 * Destructor of the main window class. Releases the memory.
 * @brief ~MainWindow
 */
MainWindow::~MainWindow()
{  

    //The widget have to be destroyed in the correct order
    //First destruct widgets inside containers then destruct the containers themselves.
    delete m_startButtonOR;
    delete m_gridLayoutOR;
    delete m_objectOR;
    delete m_objectLabelOR;
    delete m_envMapLabelOR;
    delete m_lightTypeLabelOR;
    delete m_numberOffsetsLabelOR;
    delete m_envMapOR;
    delete m_lightTypeOR;
    delete m_numberOffsetsOR;
    delete m_numberOfLightingConditionsLabelOR;
    delete m_numberOfLightingConditionsOR;
    delete m_numberOfSamplesLabelOR;
    delete m_numberOfSamplesOR;
    delete m_darkRoomPictureLabelOR;
    delete m_indirectLightPictureOR;
    delete m_manualButtonOR;
    delete m_inverseCDFButtonOR;
    delete m_medianEnergyButtonOR;
    delete m_masksButtonOR;
    delete m_layoutButtonsOR;
    delete m_RadioButtonBoxOR;
    delete m_exposureLabelOR;
    delete m_exposureSpinBoxOR;
    delete m_disabledButtonOR;
    delete m_originalSpaceButtonOR;
    delete m_PCAButtonOR;
    delete m_layoutOptimisationOR;
    delete m_optimisationGroupBoxOR;  
    delete m_highFreqOR;
    delete m_lowFreqOR;
    delete m_layoutMasksOR;
    delete m_computeBasisMaskOR;
    delete m_masksGroupBoxOR;

    //Light stage
    delete m_startButtonLS;
    delete m_gridLayoutLS;
    delete m_objectLS;
    delete m_objectLabelLS;
    delete m_envMapLabelLS;
    delete m_lightTypeLabelLS;
    delete m_numberOffsetsLabelLS;
    delete m_envMapLS;
    delete m_lightTypeLS;
    delete m_numberOffsetsLS;

    //Free form light stage
    delete m_startButtonFF;
    delete m_gridLayoutFF;
    delete m_envMapLabelFF;
    delete m_lightTypeLabelFF;
    delete m_numberOfLightingConditionsFF;
    delete m_numberOfLightingConditionsLabelFF;
    delete m_numberOffsetsLabelFF;
    delete m_envMapFF;
    delete m_lightTypeFF;
    delete m_numberOffsetsFF;
    delete m_exposureLabelFF;
    delete m_exposureSpinBoxFF;
    delete m_layoutButtonsLightsFF;
    delete m_manualButtonFF;
    delete m_loadButtonFF;
    delete m_saveVoronoiFF;
    delete m_RadioButtonLightsBoxFF;


    //Tabs
    delete m_officeRoomTab;
    delete m_freeFormTab;
    delete m_lightStageTab;
    delete m_tabs;

    delete m_LSRelighting;
    delete m_ORRelighting;
}


/**
 * Function that sets all the parameters of the widgets of the window. The function also applies a layout.
 * @brief buildWindow
 */
void MainWindow::buildWindow()
{

    m_tabs->setGeometry(30, 20, 700, 580);

//Office room relighting tab

    m_numberOffsetsOR->setRange(1,360);//One offset per degree

    //Add items to Combo Boxes
    m_objectOR->addItem("Bird");
    m_objectOR->addItem("Egg");
    m_objectOR->addItem("Bird_bedroom");
    m_objectOR->addItem("Egg_bedroom");
    m_objectOR->addItem("Bird_bedroom45");
    m_objectOR->addItem("Egg_bedroom45");
    m_objectOR->addItem("PlantOR");
    m_objectOR->addItem("HelmetOR");

    m_envMapOR->addItem("Grace Cathedral");
    m_envMapOR->addItem("St. Peter's Basilica");
    m_envMapOR->addItem("The Uffizi Gallery");
    m_envMapOR->addItem("Eucalyptus Grove");
    m_envMapOR->addItem("Pisa courtyard");

    m_lightTypeOR->addItem("Point");
    m_lightTypeOR->addItem("Gaussian");

    m_numberOfSamplesOR->addItem("64");
    m_numberOfSamplesOR->addItem("128");
    m_numberOfSamplesOR->addItem("256");
    m_numberOfSamplesOR->addItem("512");
    m_numberOfSamplesOR->addItem("1024");
    m_numberOfSamplesOR->addItem("2048");
    m_numberOfSamplesOR->addItem("4096");
    m_numberOfSamplesOR->addItem("8192");
    m_numberOfSamplesOR->addItem("16384");
    m_numberOfSamplesOR->addItem("32768");
    m_numberOfSamplesOR->setCurrentIndex(9);

    m_numberOfLightingConditionsOR->setRange(1,1000);
    m_numberOfLightingConditionsOR->setValue(9);

    m_indirectLightPictureOR->setRange(0, m_numberOfLightingConditionsOR->value());
    m_indirectLightPictureOR->setValue(4);

    m_exposureSpinBoxOR->setValue(1.25);
    m_exposureSpinBoxOR->setRange(-10,10);
    m_exposureSpinBoxOR->setSingleStep(0.25);

    //Radio buttons for light identification
    m_masksButtonOR->setChecked(true);
    m_layoutButtonsOR->addWidget(m_manualButtonOR);
    m_layoutButtonsOR->addWidget(m_inverseCDFButtonOR);
    m_layoutButtonsOR->addWidget(m_medianEnergyButtonOR);
    m_layoutButtonsOR->addWidget(m_masksButtonOR);
    m_RadioButtonBoxOR->setLayout(m_layoutButtonsOR);

    //Radio Buttons for optimisation
    m_disabledButtonOR->setChecked(true);
    m_layoutOptimisationOR->addWidget(m_disabledButtonOR);
    m_layoutOptimisationOR->addWidget(m_originalSpaceButtonOR);
    m_layoutOptimisationOR->addWidget(m_PCAButtonOR);
    m_optimisationGroupBoxOR->setLayout(m_layoutOptimisationOR);

    //Radio button for type of masks
    m_lowFreqOR->setChecked(true);
    m_layoutMasksOR->addWidget(m_lowFreqOR);
    m_layoutMasksOR->addWidget(m_highFreqOR);
    m_masksGroupBoxOR->setLayout(m_layoutMasksOR);

    //Check the box
    m_computeBasisMaskOR->setChecked(true);

    //Layout Tab loffice room relighting
    m_gridLayoutOR->addWidget(m_objectLabelOR, 0,0);
    m_gridLayoutOR->addWidget(m_objectOR, 0,1);
    m_gridLayoutOR->addWidget(m_envMapLabelOR, 1,0);
    m_gridLayoutOR->addWidget(m_envMapOR, 1,1);
    m_gridLayoutOR->addWidget(m_lightTypeLabelOR, 2,0);
    m_gridLayoutOR->addWidget(m_lightTypeOR, 2,1);
    m_gridLayoutOR->addWidget(m_numberOffsetsLabelOR, 3,0);
    m_gridLayoutOR->addWidget(m_numberOffsetsOR, 3,1);
    m_gridLayoutOR->addWidget(m_numberOfLightingConditionsLabelOR,4,0);
    m_gridLayoutOR->addWidget(m_numberOfLightingConditionsOR,4,1);
    m_gridLayoutOR->addWidget(m_darkRoomPictureLabelOR, 5,0);
    m_gridLayoutOR->addWidget(m_indirectLightPictureOR, 5,1);
    m_gridLayoutOR->addWidget(m_exposureLabelOR, 6,0);
    m_gridLayoutOR->addWidget(m_exposureSpinBoxOR, 6,1);
    m_gridLayoutOR->addWidget(m_RadioButtonBoxOR, 7,0,1,2);
    m_gridLayoutOR->addWidget(m_numberOfSamplesLabelOR, 8,0);
    m_gridLayoutOR->addWidget(m_numberOfSamplesOR, 8,1);
    m_gridLayoutOR->addWidget(m_masksGroupBoxOR,9,0,1,2);
    m_gridLayoutOR->addWidget(m_optimisationGroupBoxOR,10,0,1,2);
    m_gridLayoutOR->addWidget(m_computeBasisMaskOR, 11,0,1,2);
    m_gridLayoutOR->addWidget(m_startButtonOR, 12, 1);

    m_officeRoomTab->setLayout(m_gridLayoutOR);

//Free form light stage relighting tab
    m_numberOffsetsFF->setRange(1,360);

    m_envMapFF->addItem("Grace Cathedral");
    m_envMapFF->addItem("St. Peter's Basilica");
    m_envMapFF->addItem("The Uffizi Gallery");
    m_envMapFF->addItem("Eucalyptus Grove");
    m_envMapFF->addItem("Pisa courtyard");

    m_lightTypeFF->addItem("Point");
    m_lightTypeFF->addItem("Gaussian");

    m_numberOfLightingConditionsFF->setRange(1,1000);
    m_numberOfLightingConditionsFF->setValue(142);

    m_exposureSpinBoxFF->setValue(0.0);
    m_exposureSpinBoxFF->setRange(-10,10);
    m_exposureSpinBoxFF->setSingleStep(0.25);

    //Radio buttons for light identification
    m_manualButtonFF->setChecked(true);
    m_layoutButtonsLightsFF->addWidget(m_manualButtonFF);
    m_layoutButtonsLightsFF->addWidget(m_loadButtonFF);
    m_RadioButtonLightsBoxFF->setLayout(m_layoutButtonsLightsFF);
    m_RadioButtonLightsBoxFF->setFixedHeight(80);

    //Layout Tab free form room relighting
    m_gridLayoutFF->addWidget(m_envMapLabelFF, 0,0);
    m_gridLayoutFF->addWidget(m_envMapFF, 0,1);
    m_gridLayoutFF->addWidget(m_lightTypeLabelFF, 1,0);
    m_gridLayoutFF->addWidget(m_lightTypeFF, 1,1);
    m_gridLayoutFF->addWidget(m_numberOffsetsLabelFF, 2,0);
    m_gridLayoutFF->addWidget(m_numberOffsetsFF, 2,1);
    m_gridLayoutFF->addWidget(m_numberOfLightingConditionsLabelFF, 3,0);
    m_gridLayoutFF->addWidget(m_numberOfLightingConditionsFF, 3,1);
    m_gridLayoutFF->addWidget(m_exposureLabelFF, 4,0);
    m_gridLayoutFF->addWidget(m_exposureSpinBoxFF, 4,1);
    m_gridLayoutFF->addWidget(m_RadioButtonLightsBoxFF,5,0,1,2);
    m_gridLayoutFF->addWidget(m_saveVoronoiFF, 6,0,1,2);
    m_gridLayoutFF->addWidget(m_startButtonFF, 7, 1);

    m_freeFormTab->setLayout(m_gridLayoutFF);

//Light stage relighting tab

    m_numberOffsetsLS->setRange(1,360);

    //Add items to Combo Boxes
    m_objectLS->addItem("Helmet");
    m_objectLS->addItem("Plant");

    m_envMapLS->addItem("Grace Cathedral");
    m_envMapLS->addItem("St. Peter's Basilica");
    m_envMapLS->addItem("The Uffizi Gallery");
    m_envMapLS->addItem("Eucalyptus Grove");
    m_envMapLS->addItem("Pisa courtyard");

    m_lightTypeLS->addItem("Point");
    m_lightTypeLS->addItem("Gaussian");

    //Layout Tab light stage relighting
    m_gridLayoutLS->addWidget(m_objectLabelLS, 0,0);
    m_gridLayoutLS->addWidget(m_objectLS, 0,1);
    m_gridLayoutLS->addWidget(m_envMapLabelLS, 1,0);
    m_gridLayoutLS->addWidget(m_envMapLS, 1,1);
    m_gridLayoutLS->addWidget(m_lightTypeLabelLS, 2,0);
    m_gridLayoutLS->addWidget(m_lightTypeLS, 2,1);
    m_gridLayoutLS->addWidget(m_numberOffsetsLabelLS, 3,0);
    m_gridLayoutLS->addWidget(m_numberOffsetsLS, 3,1);
    m_gridLayoutLS->addWidget(m_startButtonLS, 4,1);

    m_lightStageTab->setLayout(m_gridLayoutLS);

    m_tabs->addTab(m_officeRoomTab, "Office Room relighting");
    m_tabs->addTab(m_freeFormTab, "Free form light stage");
    m_tabs->addTab(m_lightStageTab, "Light stage relighting");

    //Signals and slots           
    //Depending on the object the parameters of the room change (number of lighting condition...)
    QObject::connect(m_objectOR, SIGNAL(currentIndexChanged(QString)), this, SLOT(updateParametersOR(QString)));

    QObject::connect(m_startButtonLS, SIGNAL(clicked()), this, SLOT(startLightStageRelighting()));
    QObject::connect(m_startButtonFF, SIGNAL(clicked()), this, SLOT(startFreeFormRelighting()));
    QObject::connect(m_startButtonOR, SIGNAL(clicked()), this, SLOT(startOfficeRoomRelighting()));

    QObject::connect(m_LSRelighting, SIGNAL(statusUpdate(QString)), m_progressWindow, SLOT(addText(QString)));
    QObject::connect(m_LSRelighting, SIGNAL(updateProgressBar(int)), m_progressWindow, SLOT(setValueProgressBar(int)));
    QObject::connect(m_LSRelighting, SIGNAL(updateImage(QString)), m_progressWindow, SLOT(updateImage(QString)));

    QObject::connect(m_FFRelighting, SIGNAL(statusUpdate(QString)), m_progressWindow, SLOT(addText(QString)));
    QObject::connect(m_FFRelighting, SIGNAL(updateProgressBar(int)), m_progressWindow, SLOT(setValueProgressBar(int)));
    QObject::connect(m_FFRelighting, SIGNAL(updateImage(QString)), m_progressWindow, SLOT(updateImage(QString)));

    QObject::connect(m_ORRelighting, SIGNAL(statusUpdate(QString)), m_progressWindow, SLOT(addText(QString)));
    QObject::connect(m_ORRelighting, SIGNAL(updateProgressBar(int)), m_progressWindow, SLOT(setValueProgressBar(int)));
    QObject::connect(m_ORRelighting, SIGNAL(updateImage(QString)), m_progressWindow, SLOT(updateImage(QString)));

    //If the number of lighting conditions changes then the range in which the number corresponding to the picture of the dark room changes tooS
    QObject::connect(m_numberOfLightingConditionsOR, SIGNAL(valueChanged(int)), this, SLOT(changeRangeIndirectLightPicture(int)));
}

/**
 * Qt slot that changes the parameters of the room depending on the object.
 * @brief updateParametersOR
 */
void MainWindow::updateParametersOR(const QString &object)
{
    if(object == "Bird_bedroom" || object == "Egg_bedroom" || object == "Egg_bedroom45" || object == "Bird_bedroom45")
    {

        m_numberOfLightingConditionsOR->setValue(12);

        m_indirectLightPictureOR->setRange(0, m_numberOfLightingConditionsOR->value());
        m_indirectLightPictureOR->setValue(0);

    }
    else
    {
        m_numberOfLightingConditionsOR->setValue(9);

        m_indirectLightPictureOR->setRange(0, m_numberOfLightingConditionsOR->value());
        m_indirectLightPictureOR->setValue(4);
    }
}

/**
 * Qt slot that sets and start the light stage relighting with the parameters of the window.
 * @brief startLightStageRelighting
 */
void MainWindow::startLightStageRelighting()
{
    QString object = m_objectLS->currentText();
    QString environmentMap = m_envMapLS->currentText();
    QString lightType = m_lightTypeLS->currentText();
    int numberOfOffsets = m_numberOffsetsLS->value();

    m_LSRelighting->clearRelighting();
    m_LSRelighting->setRelighting(object, environmentMap, lightType, 253, numberOfOffsets);

    m_progressWindow->clear();
    m_progressWindow->open();
    qApp->processEvents(); //Refresh the main window
    m_LSRelighting->relighting();
}

/**
 * Qt slot that sets and start the free form relighting with the parameters of the window.
 * @brief startFreeFormRelighting
 */
void MainWindow::startFreeFormRelighting()
{
    QString environmentMap = m_envMapFF->currentText();
    QString lightType = m_lightTypeFF->currentText();
    unsigned int numberOfOffsets = m_numberOffsetsFF->value();
    unsigned int numberOfLightingConditions = m_numberOfLightingConditionsFF->value();
    double exposure = m_exposureSpinBoxFF->value();
    QString identificationMethod;
    bool save = m_saveVoronoiFF->isChecked();

    //Light selection method
    if(m_manualButtonFF->isChecked())
    {
        identificationMethod = QString("Manual");
    }

    if(m_loadButtonFF->isChecked())
    {
        identificationMethod = QString("Load");
        save = false; //Do not save the file if the manual button is not checked
    }

    m_FFRelighting->clearRelighting();
    m_FFRelighting->setRelighting(environmentMap, lightType, numberOfLightingConditions, numberOfOffsets, exposure, identificationMethod, save);
    m_progressWindow->clear();
    m_progressWindow->open();
    qApp->processEvents(); //Refresh the main window
    m_FFRelighting->relighting();
}

/**
 * Qt slot that sets and start the office room relighting with the parameters of the window.
 * @brief startOfficeRoomRelighting
 */
void MainWindow::startOfficeRoomRelighting()
{
    QString object = m_objectOR->currentText();
    QString environmentMap = m_envMapOR->currentText();
    QString lightType = m_lightTypeOR->currentText();
    unsigned int numberOfSamples = m_numberOfSamplesOR->currentText().toUInt();
    unsigned int numberOfOffsets = m_numberOffsetsOR->value();
    unsigned int numberOfLightingConditions = m_numberOfLightingConditionsOR->value();
    unsigned int indirectLightPicture = m_indirectLightPictureOR->value();
    double exposure = pow(2.0, m_exposureSpinBoxOR->value()); //The slider takes int values between -1000 and 1000 : divide by 100 to scale it to the range -10 ;10 (fstops)
    bool computeMasks = m_computeBasisMaskOR->isChecked();
    QString identificationMethod;
    QString optimisationMethod;
    QString masksType;

    m_ORRelighting->clearRelighting();

    m_progressWindow->clear();
    m_progressWindow->open();

    //Light selection method
    if(m_manualButtonOR->isChecked())
    {
        identificationMethod = QString("Manual");
    }

    if(m_inverseCDFButtonOR->isChecked())
    {
        identificationMethod = QString("Inverse CDF");
    }

    if(m_medianEnergyButtonOR->isChecked())
    {
        identificationMethod = QString("Median Energy");
    }

    if(m_masksButtonOR->isChecked())
    {
        identificationMethod = QString("Masks");
    }

    //Type of masks
    if(m_highFreqOR->isChecked())
    {
        masksType = QString("High Frequency");
    }

    if(m_lowFreqOR->isChecked())
    {
        masksType = QString("Low Frequency");
    }

    if(m_inverseCDFButtonOR->isChecked())
    {
        identificationMethod = QString("Inverse CDF");
    }

    //Optimisation method
    if(m_disabledButtonOR->isChecked())
    {
        optimisationMethod = QString("Disabled");
    }

    if(m_originalSpaceButtonOR->isChecked())
    {
        optimisationMethod = QString("Original Space");
    }

    if(m_PCAButtonOR->isChecked())
    {
        optimisationMethod = QString("PCA Space");
    }


    m_ORRelighting->setRelighting(object, environmentMap, lightType, numberOfLightingConditions, numberOfOffsets, identificationMethod, masksType, optimisationMethod, numberOfSamples, indirectLightPicture, computeMasks,exposure);
    m_ORRelighting->relighting();
}

/**
 * Qt slots that dynamically changes the range in which the picture corresponding to the indirect light is.
 * This range depends on the number of lighting conditions available : [1, numberOflightingConditions]
 * @brief changeRangeIndirectLightPicture
 * @param indirectLightPicture INPUT : upper bound of the range [1, numberOfLightingConditions (or indirectLightPicture)]
 */
void MainWindow::changeRangeIndirectLightPicture(int indirectLightPicture)
{
    m_indirectLightPictureOR->setRange(1, indirectLightPicture);
}
