#include <Arduino.h>
#include <cstdlib>
#include <mqtt_client.h>
#include <PubSubClient.h>
#include <string.h>
#include <time.h>
#include <WiFi.h>

// Azure IoT SDK for C includes
#include <az_core.h>
#include <az_iot.h>
#include <azure_ca.h>

// Additional sample headers 
#include "AzIoTSasToken.h"
#include "SerialLogger.h"

#include "iot_configs.h"
// Enter your WiFi SSID and password
char ssid[] = "TN-GY0900";         // your network SSID (name)
char pass[] = "Dihytsathut4";    // your network password (use for WPA, or use as key for WEP)

//Azure IOT Hub Setup
static const char* connectionString = "HostName=sajren-hub.azure-devices.net;DeviceId=sajren-esp32;SharedAccessKey=X2IGcH8+ZN+wel8csevmXt7WC1n1HXfXd2ntVE0Fotg=";
static bool hasIoTHub = false;

// Initialize the Ethernet client library
// with the IP address and port of the server
// that you want to connect to (port 80 is default for HTTP):
WiFiClient client;

void connectToSerialPort()
{
    //Initialize serial and wait for port to open:
  Serial.begin(115200);
  while (!Serial) 
  {
    Serial.println(".");
  }// wait for serial port to connect. Needed for native USB port only
}

void connecToIotHub()
{
  if (!Esp32MQTTClient_Init((const uint8_t*)connectionString))
  {
    hasIoTHub = false;
    Serial.println("Initializing IoT hub failed.");
    return;
  }
  hasIoTHub = true;
}

void sendMsgToIotHub()
{
  Serial.println("start sending events.");
  if (hasIoTHub)
  {
    char buff[128];
    Serial.println("Inside");
    // replace the following line with your data sent to Azure IoTHub
    snprintf(buff, 128, "{\"topic\":\"iot\"}");
    
    if (Esp32MQTTClient_SendEvent(buff))
    {
      Serial.println("Sending data succeed");
    }
    else
    {
      Serial.println("Failure...");
    }
    delay(5000);
  }
}

void scanWifiNetwork()
{
  Serial.println("scan start");

  // WiFi.scanNetworks will return the number of networks found
  int n = WiFi.scanNetworks();
  Serial.println("scan done");
  if (n == 0) 
  {
    Serial.println("no networks found");
  } 
  else 
  {
      Serial.print(n);
      Serial.println(" networks found");
      for (int i = 0; i < n; ++i) 
      {
        // Print SSID and RSSI for each network found
        Serial.print(i + 1);
        Serial.print(": ");
        Serial.print(WiFi.SSID(i));
        Serial.print(" (");
        Serial.print(WiFi.RSSI(i));
        Serial.print(")");
        Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN)?" ":"*");
        delay(10);

        Serial.println(WiFi.SSID(i));
        Serial.println(ssid);
        if (WiFi.SSID(i) == ssid)
        {
          Serial.print("Found network ");
          Serial.println(WiFi.SSID(i));
          break;
        }
      }
  }
  Serial.println("");

  // Wait a bit before scanning again
  delay(5000);
}

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

void connectToWifi()
{
    // attempt to connect to Wifi network:
  Serial.print("Attempting to connect to SSID: ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) 
  {
      delay(500);
      Serial.print(".");
  }
  Serial.println("");
  Serial.println("Connected to WiFi");
}

void setupWiFi()
{
  connectToSerialPort();
  delay(1000);
  //scan for wifi networks
  scanWifiNetwork();
  delay(1000);
  // Connect to wifi
  connectToWifi();
  printWifiStatus();
  delay(1000);
}

void runWiFi()
{
  // if there are incoming bytes available
  // from the server, read them and print them:
  while (client.available()) {
    Serial.println("Hej");
    char c = client.read();
    Serial.write(c);
  }

  // if the server's disconnected, stop the client:
  if (!client.connected()) {
    Serial.println();
    Serial.println("disconnecting from server.");
    client.stop();

    // do nothing forevermore:
    while (true) {
      delay(100);
    }
  }
}

void setup() 
{
  setupWiFi();
  connecToIotHub();
  initTime();
}

void loop() 
{
  delay(1000);
  sendMsgToIotHub();
}