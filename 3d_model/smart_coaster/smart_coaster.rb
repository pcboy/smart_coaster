#!/usr/bin/env ruby
require "rubygems"
require "crystalscad"
require "require_all"
include CrystalScad

require_all "lib/**/*.rb"

# To run this project and refresh any changes to the code, run the following command
# in a terminal (make sure you are in the same directory as this file): 
#  observr smart_coaster.observr
#
# This will generate smart_coaster.scad which you can open in OpenSCAD.
# In OpenSCAD make sure that you have the menu item
# Design -> Automatic Reload and Compile 
# activated. 
 
# Scans every file in lib/**/*.rb for classes and saves them in the output/ directory
save!

# Disabled by default for now since this will give a warning. 
# @@bom.save("bom.txt")


