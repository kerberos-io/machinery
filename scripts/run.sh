#!/bin/bash

# -------------------------------------------
# This is an example script which illustrates
# how to use the Script IO device.
#

# --------------------------------------
# The first parameter is the JSON object
#
# e.g. {"regionCoordinates":[308,250,346,329],"numberOfChanges":194,"timestamp":"1486049622","microseconds":"6-161868","token":344,"pathToImage":"1486049622_6-161868_frontdoor_308-250-346-329_194_344.jpg","instanceName":"frontdoor"}

JSON=$1

# -------------------------------------------
# You can use python to parse the JSON object
# and get the required fields

name=$(echo $JSON | python -c "import sys, json; print json.load(sys.stdin)['instanceName']")
coordinates=$(echo $JSON | python -c "import sys, json; print json.load(sys.stdin)['regionCoordinates']")
changes=$(echo $JSON | pythfon -c "import sys, json; print json.load(sys.stdin)['numberOfChanges']")
image=$(echo $JSON | python -c "import sys, json; print json.load(sys.stdin)['pathToImage']")