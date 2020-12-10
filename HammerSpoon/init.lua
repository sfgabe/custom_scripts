ha_url = 'https://HASS_Address'
ha_auth_headers = { ["content-type"] = "application/json", ["Authorization"] = "Bearer LongHomeAssistantToken" }

function usbDeviceCallbackKB(data)
  data_keyboard_ergodox = '{"state": "ErgoDox"}'
  data_keyboard_macbook = '{"state": "Macbook Keyboard"}'

  if string.match(data["productName"], "ErgoDox EZ") then
    if (data["eventType"] == "added") then
      hs.http.post( ha_url .. '/api/states/sensor.macbook_keyboard', data_keyboard_ergodox, ha_auth_headers )
    elseif (data["eventType"] == "removed") then
      hs.http.post( ha_url .. '/api/states/sensor.macbook_keyboard', data_keyboard_macbook, ha_auth_headers )
    end
  end
end

function usbDeviceCallbackMS(data)
  data_mouse_macbook = '{"state": "Macbook Trackpad"}'
  data_mouse_microsoft = '{"state": "Red Microsoft Mouse"}'

  if string.match(data["productName"], "Microsoft Nano Transceiver 1.1") then
    if (data["eventType"] == "added") then
      hs.http.post( ha_url .. '/api/states/sensor.macbook_mouse', data_mouse_microsoft, ha_auth_headers )
    elseif (data["eventType"] == "removed") then
      hs.http.post( ha_url .. '/api/states/sensor.macbook_mouse', data_mouse_macbook, ha_auth_headers )
    end
  end
end

function usbDeviceCallbackSC(data)
  data_screen_macbook = '{"state": "Macbook Screen"}'
  data_screen_dell = '{"state": "Dell Desk Screen"}'

  if string.match(data["productName"], "USB 2.0 BILLBOARD             ") then
    if (data["eventType"] == "added") then
      hs.http.post( ha_url .. '/api/states/sensor.macbook_screen', data_screen_dell, ha_auth_headers )
    elseif (data["eventType"] == "removed") then
      hs.http.post( ha_url .. '/api/states/sensor.macbook_screen', data_screen_macbook, ha_auth_headers )
    end
  end
end

function hassSensor()
  data_ssid = '{"state": "' .. hs.wifi.currentNetwork() .. '"}'
  data_battery = '{"state": "' .. hs.battery.percentage() .. '"}'
  data_brightness = '{"state": "' .. hs.brightness.get() .. '"}'
  hs.http.post( ha_url .. '/api/states/sensor.macbook_ssid', data_ssid, ha_auth_headers )
  hs.http.post( ha_url .. '/api/states/sensor.macbook_battery', data_battery, ha_auth_headers )
  hs.http.post( ha_url .. '/api/states/sensor.macbook_brightness', data_brightness, ha_auth_headers )
end

hs.battery.watcher.new(hassSensor):start()
hs.usb.watcher.new(usbDeviceCallbackKB):start()
hs.usb.watcher.new(usbDeviceCallbackMS):start()
hs.usb.watcher.new(usbDeviceCallbackSC):start()
