#Toggles 2 Relays at once, for use in OctoPrint Enclosure
#!/bin/sh

Relay1On="http://octopi/plugin/enclosure/setIO?status=true&index_id=2&apikey=YOURAPIKEY"
Relay1Off="http://octopi/plugin/enclosure/setIO?status=false&index_id=2&apikey=YOURAPIKEY"
Relay2On="http://octopi/plugin/enclosure/setIO?status=true&index_id=3&apikey=YOURAPIKEY"
Relay2Off="http://octopi/plugin/enclosure/setIO?status=false&index_id=3&apikey=YOURAPIKEY"

StatusCode=$(curl --silent "http://octopi/plugin/enclosure/getOutputStatus?index_id=3&apikey=YOURAPIKEY")

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
