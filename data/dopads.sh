#!/usr/bin/env bash
# Written by Jesse Anderson (jesse@csp-inc.org) - 2/27/2020
 

# This is a non-ESRI dependent version of dopads.py. It does require GDAL/OGR 
# built with spatialite support. It does everything that dopads.py does except 
# for the final reprojection, because I do not have the projection parameters
# for the output (referenced in dopads.py as 'WLCI_IBM_Albers_Conical_Equal_Area.prj'.
# Some of the steps are different because I didn't need to e.g. delete empty
# fields, but the ultimate effect is the same. 
# Specific steps are:
# 1. Read in polygons defined in npads.csv. Each set of rows with a common ID
#    apparently defines vertices for a polygon in the correct order. 
# 2. Join these polygons to the information about the "real" well pads they 
#    represent using the ACCESS field. This information is stored in dodo.csv. 
#    Also calculate area in hectares.
# 3. Merge this information with existing polygons contained basepadv5.shp and
#    output as thepads.shp.

# Convert npads.csv to npads_temp.shp & join with dodo data
QUERY="SELECT *, ST_Area(geometry)/10000 as Area_Ha FROM 
  ( SELECT 
      ACCESS, 
      MakePolygon(MakeLine(MakePoint(CAST(E AS float),CAST(N AS float)))) as geometry
    FROM npads 
    GROUP BY ACCESS ) 
  LEFT JOIN 'dodo.csv'.dodo USING (ACCESS)"

ogr2ogr -t_srs EPSG:26912 -s_srs EPSG:26912 -dialect sqlite -sql "$QUERY" npads_temp.shp npads.csv

# Merge wih base pads
ogrmerge.py -overwrite_ds -single -o thepads.shp basepadv5.shp npads_temp.shp

# Remove temporary shp
rm npads_temp.*

