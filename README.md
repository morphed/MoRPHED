MoRPHED
=================

The MoRPHED Morphodynamic Model. Directions for running the model and other notes below.
For more information view the model website [here](http://morphed.joewheaton.org/).

The MoRPHED Model can be built from source with this repo, or installed with the provided installer. Installers and source code for all versions are available in the [releases](https://github.com/morphed/MoRPHED/releases) section of this repo.
We also provide a .zip folder at the same location that containins example input files. 
Once you have compiled the source code or installed the model (installer [here](https://github.com/morphed/MoRPHED/releases)) follow these directions to use the MoRPHED GUI and run simulations.

##Running a Simulation with the MoRPHED GUI
###1. Start the GUI

This is done by opening the MoRPHED_GUI.exe that will be located at the specified installation path (if using the installer).

###2. Start or Open a Project

From the file menu either start a new project or open an existing project. To start a new project simply select or create a directory (folder) where the model will create its files. To open a project navigate to the directory of an existing project and select the *.morph file. Once a project is created or opened actions in the GUI will be enabled. 

###3. Load Inputs

Click the Inputs button, this will open a new dialog. 

Select a Digital Elevation Model (DEM) representing the initial conditions you wish to model. We suggest using the GeoTiff (.tif) file format, though the model will accept most formats. The upstream and downstream boundaries of the reach to be modeled must occur on the North or South edges of the DEM. Therefore, some preprocessing and data manipulation may be necessary.


Select an input text file. This is a space delimited file created by the user that specifies, the date, discharge, downstream water surface elevation (DSWSE), and imported sediment for each event the user wishes to model. Dates MUST be in the format 'MM/dd/yyyy,hh:mm'. Discharge is cubic meters per second. DSWSE is meters below the maximum elevation of the input DEM, this will always be a negative number. For example, if the DSWSE is 30m and the highest point on the input DEM is 35m the value of the DSWSE will be -5m. This value does not need to be exact for each discharge to be modeled, it just provides a starting point, the MoRPHED Model employs an alorithm to determine the exact DSWSE as changes will occur during the course of modeling. Imported sediment can be represented as an exact volume (m^3) or as a proportion the sediment exported from the reach during an event. The appropriate option must be then be selected in the inputs dialog.

###4. Set the MoRPHED Parameters

Click on the MoRPHED Parameters button.

Set the parameters for the path-length distributions representing sediment deposition.

Slope Threshold, Shear Stress Threshold, and Area Threshold relate to bank erosion. Slope Threshold is the slope a bank must be to experience bank erosion, Area Threshold is the area a bank must be experience bank erosion, and Shear Stress Threshold is the shear stress a bank must experience to undergo erosion. The Bed Erosion Scaling Factor describes the proportion of sediment that is actually eroded at a given location. Grain Size is the average D50 grain size of the reach to be modeled and is used to determine the critical shear stress for bed erosion. To completely eliminate bank erosion from a simulation the Slope Threshold should be set to 91.

###5. Set the Delft3D Parameters

Click on the Delft3D Parameters button.

Select the directory where Delft3D is installed. MoRPHED needs this directory to access the Delft3D executables, the directory must contain the subdirectory 'w32'. Select the location of the upstream and downstream boundaries. For the other parameters the defaults may be used. A better hydraulic solution may be reached by adjusting the values of the additional parameters, however, we recommend reading the [Delft3D documentation](http://oss.deltares.nl/documents/183920/185723/Delft3D-FLOW_User_Manual.pdf) before doing so.

###6. Run

Click Run and the MoRPHED Model will begin. While the model is running the GUI will be unresponsive and the status will read "Not Responding". Once the model has finished the GUI will once again become responsive. You can check the progress of the model by navigating to the working directory for the simulation and observing the outputs for each completed event.

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

##Known bugs

1. Quickplot, the Delft3D application for exporting data, often does not start when called. Hence, on occassion the necessary hydraulic files are not obtained.

##Developers
* [Konrad Hafen](https://github.com/khafen74)
* [Alan Kasprak](http://www.alankasprak.org)
