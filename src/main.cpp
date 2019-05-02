#include "Arduino.h"
#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include "ESPmDNS.h"
#include "Update.h"

/* index View */
const char index_html[] PROGMEM = "<!DOCTYPE html><html><head><meta charset='UTF-8'><title>GreenPonik</title></head><body><h1>WebView</h1><a href='/update'>Update firmware</a></body></html>";
/* update View */
const char update_html[] PROGMEM = "<!DOCTYPE html><html><head><meta charset='UTF-8'><title>GreenPonik</title></head><body><h1>Only .bin file</h1><form method='POST' action='/update' enctype='multipart/form-data'><input type='file' name='update' required><input type='submit' value='Run Update'></form></body></html>";

AsyncWebServer server_http(80);

void notFound(AsyncWebServerRequest *request)
{
	Serial.println("Not Found");
	request->send(404, "text/html", "<strong>not found!</strong>");
}

void onUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final)
{
	if (!index)
	{
		Serial.printf("UploadStart: %s\n", filename.c_str());
	}
	for (size_t i = 0; i < len; i++)
	{
		Serial.write(data[i]);
	}
	if (final)
	{
		Serial.printf("UploadEnd: %s, %u B\n", filename.c_str(), index + len);
	}
}

void onBody(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
{
	if (!index)
	{
		Serial.printf("BodyStart: %u B\n", total);
	}
	for (size_t i = 0; i < len; i++)
	{
		Serial.write(data[i]);
	}
	if (index + len == total)
	{
		Serial.printf("BodyEnd: %u B\n", total);
	}
}


bool shouldReboot = false;

void setup()
{
	Serial.begin(115200);

	//MDNS
	if (!MDNS.begin("greenponik-esp32"))
	{
		Serial.println(F("[MDNS]... Error setting up MDNS responder!"));
	}

	//WIFI AP
	int timeout = 0;
	while (!WiFi.softAP("greenponik-esp32", "greenponik"))
	{
		Serial.println(F("."));
		delay(250);
		if (timeout > 40) //delay 250ms so 10s = 40*250ms
		{
			Serial.println(F("[Wifi AP]... not start"));
		}
		timeout++;
	};
	Serial.println("[Wifi AP]... ip : " + WiFi.softAPIP().toString());

	//HTTP SERVER
	server_http.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
		request->send(200, "text/html", index_html);
	});

	server_http.on("/update", HTTP_GET, [](AsyncWebServerRequest *request) {
		request->send(200, "text/html", update_html);
	});
	server_http.on("/update", HTTP_POST, [](AsyncWebServerRequest *request) {
		shouldReboot = !Update.hasError();
		AsyncWebServerResponse *response = request->beginResponse(200, "text/html", shouldReboot ? "<h1><strong>Update DONE</strong></h1>" : "<h1><strong>Update FAILED</strong></h1>");
		response->addHeader("Connection", "close");
		request->send(response); }, [](AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
		if (!index)
		{
			Serial.printf("Update Start: %s\n", filename.c_str());
			if (!Update.begin((ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000))
			{
				Update.printError(Serial);
			}
		}
		if (!Update.hasError())
		{
			if (Update.write(data, len) != len)
			{
				Update.printError(Serial);
			}
		}
		if (final)
		{
			if (Update.end(true))
			{
				Serial.printf("Update Success: %uB\n", index + len);
			}
			else
			{
				Update.printError(Serial);
			}
		} });

	server_http.onNotFound(notFound);
	server_http.onFileUpload(onUpload);
	server_http.onRequestBody(onBody);
	server_http.begin();
}

void loop()
{
	if (shouldReboot)
	{
		Serial.println("[main]... Rebooting...");
		delay(100);
		ESP.restart();
	}
}
