MoRPHED
=================

The MoRPHED Morphodynamic Model.
For more information view the model website [here](http://morphed.joewheaton.org/).

##Notes

1. MoRPHED has been successfully built on Windows 7 and 8. The software is written in C++ using Qt libraries, so cross-platform building on Linux should be possible.

2. We build MoRPHED using Qt with the MinGW compiler (g++; Qt 5.3.0, MinGW, 32 bit).

3. MoRPHED leverages Geospatial Data Abstraction Libraries (GDAL) for geoprocessing routines. GDAL must be built from source; for directions see http://trac.osgeo.org/gdal/wiki/BuildingOnWindows and/or http://trac.osgeo.org/gdal/wiki/BuildHints.

4. To build MoRPHED, select 'Release' mode and build "MORPHED_GUI". Once built, put a copy of libgdal-1.dll (obtained in Step 3) in MORPHED_GUTS/release.

5. MoRPHED integrates with the freely-available Delft3D hydaulic model, which must be built by following the instructions at http://oss.deltares.nl/web/delft3d. When prompted in MoRPHED, provide the top-level directory of Delft (e.g. C:/delft3d).

##Building from source

1. Clone the repository https://github.com/morphed/MoRPHED.

2. Update the .pro files for the MORPHED_LIB and MORPHED_GUI projects to point to the location of the GDAL DLL and header files.

3. Make sure the MORPHED_GUI .pro is linked to the MORPHED_LIB project.

#Using the MoRPHED Model

The MoRPHED Model can be built from source with this repo, or installed with the provided installer. Installers and source code for all versions are available in the [releases](https://github.com/morphed/MoRPHED/releases) section of this repo.
We also provide a .zip folder at the same location that containins example input files. 
Once you have compiled the source code or installed the model (installer [here](https://github.com/morphed/MoRPHED/releases)) follow these directions to use the MoRPHED GUI and run simulations.

##1. Start the GUI

This is by opening the MoRPHED_GUI.exe that will be located at the specified installation path (if using the installer).

##2.Start or Open a Project

From the file menu either start a new project or open an existing project. To start a new project simply select or create a directory (folder) where the model will create its files. To open a project navigate to the directory of an existing project and select the *.morph file. Once a project is created or opened actions in the GUI will be enabled. 

##3. Load Inputs

Click the Inputs button, this will open a new dialog. 

Select a Digital Elevation Model (DEM) representing the initial conditions you wish to model. We suggest using the GeoTiff (.tif) file format, though the model will accept most formats. The upstream and downstream boundaries of the reach to be modeled must occur on the North or South edges of the DEM. Therefore, some preprocessing and data manipulation may be necessary.


Select an input text file. This is a space delimited file created by the user that specifies, the date, discharge, downstream water surface elevation (DSWSE), and imported sediment for each event the user wishes to model. Dates MUST be in the format 'MM/dd/yyyy,hh:mm'. Discharge is cubic meters per second. DSWSE is meters below the maximum elevation of the input DEM, this will always be a negative number. For example, if the DSWSE is 30m and the highest point on the input DEM is 35m the value of the DSWSE will be -5m. This value does not need to be exact for each discharge to be modeled, it just provides a starting point, the MoRPHED Model employs an alorithm to determine the exact DSWSE as changes will occur during the course of modeling. Imported sediment can be represented as an exact volume (m^3) or as a proportion the sediment exported from the reach during an event. The appropriate option must be then be selected in the inputs dialog.

##Known bugs

1. Quickplot, the Delft3D application for exporting data, often doesn not start when called. Hence, on occassion the necessary hydraulic files are not obtained.

##Developers
* [Konrad Hafen](https://github.com/khafen74)
* [Alan Kasprak](http://www.alankasprak.org)
