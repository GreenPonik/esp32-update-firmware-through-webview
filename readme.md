# Update firmware through ESP32 self hosted webview

> <code>git clone https://github.com/GreenPonik/esp32-update-firmware-through-webview.git</code>

## #1/ run : 
> <code>pio run -t upload</code>

## #3/ then run : 
> <code>pio device monitor</code> to live debug the esp32

## #4/ connect you desktop to ESP32's ssid
![esp_ssid](/doc/images/select_ssid.png)

> password is <code>greenponik</code>

## #5/ next open browser and search for esp32's 
> <code>http://greenponik-esp32.local</code> 

or

> <code>192.168.4.1</code>
> 
![search_browser](/doc/images/browser.jpg)

## #5/ select .bin firmware you want to update
![select_bin_file](/doc/images/update_form.jpg)

message after update

![wait_updating](/doc/images/updated.jpg)

watching serial monitor debug

![serial_debug_update](/doc/images/serial_update.jpg)

#### lib dependencies :
    -ESPAsyncWebServer
    -WiFi
    -Update
    -ESPmDNS

#ESP32firmwareupdate #platformio #gpk