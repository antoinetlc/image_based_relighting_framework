# Image-Based Relighting Framework

_Image-Based Relighting Framework_ is an open source GUI that implements three types of image-based relighting algorithms using reflectance data captured with either a light stage, a free-form acquisition or a regular room. It was implemented during my MSc thesis at Imperial College London in the Summer 2014. It is distributed under the LGPL license.


For more information on image-based relighting or on this framework please visit :

* http://www.doc.ic.ac.uk/~ast13/relighting.html.
* https://www.doc.ic.ac.uk/~ast13/pdf/MSc_Antoine_Toisoul.pdf

The framework implements the following papers : 
* _Acquiring the Reflectance Field of a Human Face, Paul Debevec_, Tim Hawkins, Chris Tchou, Haarm-Pieter Duiker, Westley Sarokin, and Mark Sagar, SIGGRAPH 2000 Conference Proceedings
* _The free-form light stage_, Vincent Masselus, Philip Dutré, Frederik Anrys In Proc. EGWR, EGRW ’02, 247–256. 
* _Image-Based Relighting using Room Lighting Basis_, Antoine Toisoul, Abhijeet Ghosh, CVMP 2016

###Version
Version 1.0

###Compilation
This program has been compiled and tested both on Windows, Linux and MacOS environments.
It requires the following libraries in order to compile :

* OpenCV (tested with version 2.4.11)
* Qt (tested with version 5.4.2)

A "IBR_Framework.pro" file is provided for compilation with QtCreator IDE. Please update the libraries paths to match your installation.

###Installation

The light stage data can be obtained on USC ICT website :

* http://gl.ict.usc.edu/Data/LightStage/

Please read appendix A of my [MSc thesis](https://www.doc.ic.ac.uk/~ast13/pdf/MSc_Antoine_Toisoul.pdf) for more information on how to setup the software.

####Environment mapping
For the environment mapping to work you will have to download the **latitude longitude maps** of the environment.
Some are available at the following links :

* http://www.pauldebevec.com/Probes/
* http://gl.ict.usc.edu/Data/HighResProbes/

Create a folder called "environment_maps" in the directory where the program is compiled and name the environment maps as follows : 

| Environment  | File Name |
| ------------- | ------------- |
| Grace Cathedral | grace_latlong  |
| Uffizi Gallery  | uffizi  |
| St Peter's Basilica | stpeters_probe  |
| Pisa Courtyard | pisa_courtyard  |
| Eucalyptus Grove | eucalyptus_grove|

###License

Image-Based Relighting framework. Author :  Antoine TOISOUL LE CANN. Copyright © 2016 Antoine TOISOUL LE CANN, Imperial College London. All rights reserved.

Image-Based Relighting framework is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version. Image-Based Relighting framework is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details. You should have received a copy of the GNU Lesser General Public License along with this program. If not, see <http://www.gnu.org/licenses/>.
