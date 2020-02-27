#-------------------------------------------------------------------------------
# Name:        module1
# Purpose:
#
# Author:      odonnellm
#
# Created:     16/10/2015
# Copyright:   (c) odonnellm 2015
# Licence:     <your licence>
#-------------------------------------------------------------------------------
#!/usr/bin/env python

"""
Create a time-stamp forecasted oil and gas well pad based on output generated from
autopadc.exe.

PadShapefile() uses the following steps:
    1. derives the inputs to this script using the following hardcoded file names:
        npads.csv: polygon vertices and centroid of new pads  -> e.g., if working
            on rep25 year 2030, npads252030.csv is renamed npads.csv before calling
            this script
        centr.csv: centroids of new pads -> e.g., if working on rep25 year 2030,
            centr252030.csv is renamed centr.csv before calling this script
        dodo.csv: start, stop year, No. of active, inactive wells,etc  for ALL
            pads up to the year being processed -> "  "  "  "
    2. calls this script
    3. renames the output to thepadsXXYYY where XX is the rep and YYYY is the year
    4. deletes everything called new* (to avoid any problems)

basepadv5.shp -> this is the 2012 baseline pad shapefile stripped of all info
    except the accession no.  Is merged with new pads.
"""

# Standard librariees
import os, sys, time

# Non-standard libraries
import arcpy
arcpy.env.overwriteOutput = True

# Arguments
# ------------------------------------------------------------------------------ Typically hardcoded
# Root workspace
root_ws = r"f:\netau\efm"
# out_ws = os.path.join(root_ws, "output")
work = "f:/netau/efm/"
out_ws = work

# Input polygon vertices for well pads
in_vertices = os.path.join(root_ws, "npads.csv")

# Input attributes for all features
in_attribute = os.path.join(root_ws, "dodo.csv")

# Baseline well pads for time zero.
in_base_pads = os.path.join(root_ws, "basepadv5.shp")

# Output time-stamped well pads
out_pads = os.path.join(out_ws, "thepads.shp")


# ------------------------------------------------------------------------------ SRS requires update for output
# Spatial reference for input and output GIS data sets.
# http://edndoc.esri.com/arcobjects/9.0/ComponentHelp/esriGeometry/esriSRProjCSType.htm
# in_spref = r"Coordinate Systems\Projected Coordinate Systems\UTM\NAD 1983\NAD 1983 UTM Zone 12N.prj"
in_spref = arcpy.SpatialReference(26912) # NAD 1983 UTM Zone 12N.prj
out_spref = work + "WLCI_IBM_Albers_Conical_Equal_Area.prj"
geotrans = "NAD_1983_To_WGS_1984_5"

# ------------------------------------------------------------------------------
# Temporary data sets
temp_fc1 = os.path.join(out_ws, "temp_fc1.shp")
temp_fc2 = os.path.join(out_ws, "temp_fc2.shp")
temp_list = [temp_fc1, temp_fc2]


def main():
    """
    Update the baseline well pads with the new pads generated from xx.exe, add
    the appropriate attributes, and use the appropriate map projection.

    1. Create a copy of the baseline well pads.
    2. Define the map projection (UTM)
    3. Use an update cursor and insert new pads via the npads25YYYY.csv (do not
       include features with an ACCESS == 0)
    4. Join the attributes from the dodo CSV to the pads shapefile based on the ACCESS field
    5. Use a cursor and add acres (HA) and calculate value.
    6. Remove unwanted field "Access_1" due to join.
    7. Project the data set to AEA.



    Geoprocessing functions:
        CopyFeatures_management (in_features, out_feature_class, {config_keyword},
            {spatial_grid_1}, {spatial_grid_2}, {spatial_grid_3})
        DefineProjection_management (in_dataset, coor_system)
        UpdateCursor (in_table, field_names, {where_clause}, {spatial_reference},
            {explode_to_points}, {sql_clause})
        MakeFeatureLayer_management (in_features, out_layer, {where_clause}, {workspace},
            {field_info})
        AddJoin_management (in_layer_or_view, in_field, join_table, join_field,
            {join_type})
        CopyFeatures_management (in_features, out_feature_class, {config_keyword},
            {spatial_grid_1}, {spatial_grid_2}, {spatial_grid_3})
        AddField_management (in_table, field_name, field_type, {field_precision},
            {field_scale}, {field_length}, {field_alias}, {field_is_nullable},
            {field_is_required}, {field_domain})
        CalculateField_management (in_table, field, expression, {expression_type},
            {code_block})
        DeleteField_management (in_table, drop_field)
        Project_management (in_dataset, out_dataset, out_coor_system,
            {transform_method}, {in_coor_system})
    """

    # Start time
    startTime = time.time()

    # 1.
    arcpy.CopyFeatures_management(in_features=in_base_pads,
        out_feature_class=temp_fc1)

    # 2.
    arcpy.DefineProjection_management(in_dataset=temp_fc1,
        coor_system=in_spref)

    # 3.
    # Insert new polygons to baseline time-stamp data set
    fields = ["ACCESS", "SHAPE@"]
    cursor = arcpy.da.InsertCursor(in_table=temp_fc1, field_names=fields)

    # Open CSV storing the vertices for each polygon, create polygon, then insert
    # into new data set.
    array = arcpy.Array()
    f = open(in_vertices, "r")
    header = f.readline() # Skip header
    # Read records
    ID = -1
    for line in f:
        # Parse CSV ("ACCESS", "YR", "N", "E", "NN", "EE")
        line = line.replace(" ", "") # Remove white space because inconsistent
        line2 = line.split(",")

        # ID for first record in CSV
        if ID == -1:
            ID = line2[0]

        # If the ID has changed, create a new feature with array
        if ID != line2[0] and line2[0] > 0:
            cursor.insertRow([ID, arcpy.Polygon(array)])
            array.removeAll()

        # Add the point to the feature's array which make up a single polygon
        if line2[0] > 0:
            array.add(arcpy.Point(line2[3], line2[2], ID=line2[0]))
        ID = line2[0]

    # Add the last feature
    cursor.insertRow([ID, arcpy.Polygon(array)])

    # Clean up
    if cursor:
        del cursor
    f.close()
    del f
    del array

    # 4.
    arcpy.env.qualifiedFieldNames = False
    temp_layer = arcpy.MakeFeatureLayer_management(temp_fc1, "temp_layer")
    arcpy.AddJoin_management(in_layer_or_view="temp_layer",
        in_field="ACCESS",
        join_table=in_attribute,
        join_field="ACCESS",
        join_type="KEEP_COMMON")
    arcpy.CopyFeatures_management("temp_layer", temp_fc2)
  #  if arcpy.Exists(temp_layer):
    #    arcpy.Delete_management(temp_layer)

    # 5.
    arcpy.AddField_management(in_table=temp_fc2,
        field_name="Area_Ha",
        field_type="FLOAT")
    arcpy.CalculateField_management(in_table=temp_fc2,
        field="Area_Ha",
        expression='!shape.area@HECTARES!',
        expression_type="PYTHON_9.3")

    # 6.
    arcpy.DeleteField_management(in_table=temp_fc2, drop_field="ACCESS_1")

    # 7.
    arcpy.Project_management(in_dataset=temp_fc2,
        out_dataset=out_pads,
        out_coor_system=out_spref,
        transform_method=geotrans,
        in_coor_system=in_spref)

    # Clean up
    for i in temp_list:
        if arcpy.Exists(i):
            arcpy.Delete_management(i)

    print "\n", round((time.time() - startTime) / 60, 0), "minutes"


if __name__ == '__main__':
    main()
