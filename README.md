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

##Known bugs

1. Quickplot, the Delft3D application for exporting data, often doesn not start when called. Hence, on occassion the necessary hydraulic files are not obtained.

##Developers
* [Konrad Hafen](https://github.com/khafen74)
* [Alan Kasprak](http://www.alankasprak.org)
