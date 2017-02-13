Energy Footprint Model – A Simulation Model for Exploring the Effects of Contemporary and Novel Oil and Gas Development Designs on Biophysical Attributes of Southwestern Wyoming.
 
Contacts:

Steven L. Garman1,2 (sgarman@blm.gov) and Monica Dorning2 (mdorning@usgs.gov)
1Bureau of Land Management, National Operations Center, Denver Federal Center, Denver
2USGS Geosciences and Environmental Change Science Center, Denver Federal Center, Denver

Purpose:
This repository contains the Energy Footprint model developed for southwestern Wyoming, including the source code, executable, data inputs, model documentation, and results of model tests.  The Energy Footprint Model was designed to forecast alternative oil/gas build-out designs across southwestern Wyoming in order to compare their effects on biophysical properties such as surface disturbance, and wildlife populations and habitat.  

Audiences:
Research scientists involved in the evaluation of the effects of oil/gas futures on surface disturbance and wildlife habitat and populations.

Development Status:
The repository version is a fully functional model.  For research purposes, future enhancements are possible, depending on the questions being addressed.  

Model Source Code (description of routines and functions included in source code files): 
Main module
efm.c

Routines to ingest binary, geospatial data layers
readareapatchesb.c, readdemb.c, readfedmb.c, readlekgridb.c, readnontraverseb.c, readpadsb.c, readrdidb.c, readroadsb.c, readsageb.c, readsecpad.c, readsgb.c, readsurfb.c



Routines to ingest ASCII tabular lists
readanchorpts.c, readauslist.c, readfillr.c, readginfo.c, readlek.c, readmsect.c ,readmspace.c , readpadcords.c, readpadinfo.c, readpadinfo2.c, readpatterns.c

Routines to initialize global structures, variables, and files
init.c, reinit.c, openoutputfiles.c


Functions to translate between row and columns of grids and UTM coordinates
convtopts.c, convtorc.c

Routines related to the derivation of new oil/gas roads that connect a new oil/gas pad to the existing road network, and to road deactivation
assignroads.c, checklines.c, checkrd.c, checkrdactive.c, checkrdnew.c, curve.c, datarecord.c, distance.c, donut.c, drawlinemp.c, drawsline.c, drawslinep.c, drawslinepcheck.c, drawslinepmod.c, findnearestcheck.c, findrdcells.c, padrdoverlap.c, processrds.c, rdgrid.c, rdgridup.c, roadskirt.c, setrdcells.c, sort.c, trig.c, updateanchorpts.c 

Routines that control the establishment of pads and roads on the landscape
establish.c, findlekdistance.c, locate.c, locatefunc.c, processcorepad.c, selectsections.c, selsecopts.c, setwells.c, simdevelop.c

Routines to update global variables with new pad and road information.
masterupdate.c, perform.c, printcorearea.c, recordbha.c, sdcorearea.c, storespec.c, updaterdstore.c


Routines that process the oil/gas build-out designs
fillmod.c,  redofill.c

Routines and python scripts used to create the output pad and road shapefiles
padshapefile.c, rdshapefile.c, dopads.py, dordslb.py


Random number generators
ran2.c, ran22.c, ran23.c

Misc. functions
deactivatewells.c, dumppads.c, findactualproj.c, pause.c, valid.c

Include files containing global structures and variables
binary.h, createpads.h, curve.h, emf.h, fnames.h, geo.h, padalloc.h, padpat.h, plss.h, rdend.h, rdgrid.h, rdshapefil.h, reduce.h, roads.h, TBHA.h


Routines to create customized binary files from ArcGIS ASCII files, and to create ArcGIS ASCII files from customized binary files
crinput.c, binascii.c



Copyright and License:
This USGS product is considered to be in the public domain, and is licensed under CC0 1.0.
Although this software program has been used by the U.S. Geological Survey (USGS), no warranty, expressed or implied, is made by the USGS or the U.S. Government as to the accuracy and functioning of the program and related program material nor shall the fact of distribution constitute any such warranty, and no responsibility is assumed by the USGS in connection therewith.

