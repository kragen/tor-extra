#!/usr/bin/python
# This program parses /var/lib/tor/cached-routers
# It output data in XML format for use with the googlemaps API
#
# Put this program in a crontable like so (`crontab -l`):
# m h  dom mon dow   command
# http_proxy=http://127.0.0.1:8118/
# PYTHONPATH=~/python-lib/lib/python
# 5,10,15,20,25,30,35,40,45,50,55 * * * * geoip-ironkey-gmaps.py > tor-data.xml
#

import GeoIP
import sys
import re

# You may need to fetch this from maxmind
# http://www.maxmind.com/download/geoip/database/
gi = GeoIP.open("~/GeoLiteCity.dat",GeoIP.GEOIP_STANDARD)

# Operate on a copy unless you have privs to open /var/lib/tor/cached-routers
file = open("/var/lib/tor/cached-routers", "r")
line = file.readline()

print "<markers>"
while line:
    if line.startswith("router "):
        (type, nodeName, ipAddress, OrPort, other, DirPort ) = line.split() 
        gir = gi.record_by_addr(ipAddress)
        if gir != None:
            print "<marker lat=\"" + str(gir['latitude']) + "\" lng=\"" + str(gir['longitude']) + "\" html=\""+ str(nodeName) + " " + str(ipAddress) + " " + str(gir['city']) + ", " + str(gir['country_name']) + " \"/>\n"
#        else:
#            print "Unable to find GeoIP information for ipAddress"
    line = file.readline()
print "</markers>"
