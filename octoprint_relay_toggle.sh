#!/bin/bash

## Toggles 2 Relays at once, for use in OctoPrint Enclosure
##
## NOTE: Octoprint enclosure plugin has some permissions issues. 
## This needs to be set up as a service in order to reliably run
## /lib/systemd/system/octoprint_power_toggle.service
## ...then run as a shell script in the enclosure plugin with "sudo systemctl start octoprint_power_toggle"
## ----
## [Unit]
## Description=Run Printer Power Toggle
## After=octoprint.service
##
## [Service]
## ExecStart=/bin/bash /home/pi/scripts/printer_relays
## Restart=never
## StandardOutput=syslog
## StandardError=syslog
## SyslogIdentifier=Power-Toggle
## User=root
## Group=root
##
## [Install]
## WantedBy=multi-user.target
##

Relay1On="http://localhost/plugin/enclosure/setIO?status=true&index_id=1&apikey=YOURAPIKEY"
Relay1Off="http://localhost/plugin/enclosure/setIO?status=false&index_id=1&apikey=YOURAPIKEY"
Relay2On="http://localhost/plugin/enclosure/setIO?status=true&index_id=2&apikey=YOURAPIKEY"
Relay2Off="http://localhost/plugin/enclosure/setIO?status=false&index_id=2&apikey=YOURAPIKEY"

StatusCode=$(curl --silent "http://localhost/plugin/enclosure/getOutputStatus?index_id=3&apikey=YOURAPIKEY")

if [[ $StatusCode == *"true"* ]]
then
    echo "Relays ON, toggling OFF"
    curl POST "$Relay1Off"
    curl POST "$Relay2Off"
else
    echo "Relays OFF, toggling ON"
    curl POST "$Relay1On"
    curl POST "$Relay2On"
fi
exit
