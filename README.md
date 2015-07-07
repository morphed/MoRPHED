MoRPHED
=================

The MoRPHED Morphodynamic Model. Some brief notes on building from source are here:

1. MoRPHED has been successfully built on Windows 7 and 8. The software is written in Qt, and as such cross-platform building on Linux should also be possible, but may take some effort.

2. We build MoRPHED using Qt with the MinGW compiler (g++; Qt 5.3.0, MinGW, 32 bit).

3. MoRPHED leverages Geospatial Data Abstraction Libraries (GDAL) for geoprocessing routines. GDAL must be built from source; for directions see http://trac.osgeo.org/gdal/wiki/BuildingOnWindows and/or http://trac.osgeo.org/gdal/wiki/BuildHints.

4. To build MoRPHED, select 'Release' mode and build "MORPHED_GUI". Once built, put a copy of libgdal-1.dll (obtained in Step 3) in MORPHED_GUTS/release.

5. MoRPHED integrates with the freely-available Delft3D hydaulic model, which must be built by following the instructions at http://oss.deltares.nl/web/delft3d. When prompted in MoRPHED, provide the top-level directory of Delft (e.g. C:/delft3d).

#Contributors
[Konrad Hafen](https://github.com/khafen74)
[Alan Kasprak](www.alankasprak.org)
