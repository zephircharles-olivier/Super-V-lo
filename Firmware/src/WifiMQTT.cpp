#include <WifiMQTT.h>
#include <LED_Animations.h>

// Now do the actual definitions here:
const char *ssid = "MiPhone";
const char *password = "passwurd";
const char *mqtt_server = "test.mosquitto.org";
bool online = true; // or false, whatever your default
const unsigned Wifi_Delay = 20;
int WifiCounter = 0;

WiFiClient espClient;
PubSubClient client(espClient);

void reconnect()
{
  if (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP32Client-Charles01"))
    {
      Serial.println("connected ✅");
      client.subscribe("esp32/test");
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" — Offline mode activated");
      online = 0;
    }
  }
}
void callback(char *topic, byte *message, unsigned int length)
{
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)message[i]);
  }
  Serial.println();
}
void setup_wifi()
{
  delay(10);
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED)
  {
    if (WifiCounter < Wifi_Delay)
    {
      delay(500);
      Serial.print(".");
      frontStrip.setPixelColor(4, 0, 0, 50);
      frontStrip.show();
      WifiCounter++;
    }
    else
    {
      Serial.println("\nOffline mode activated");
      frontStrip.setPixelColor(4, 50, 0, 50);
      frontStrip.show();
      online = 0;
      break;
    }
    
  }
  if (!online == 0)
  {
    frontStrip.clear();
    frontStrip.setPixelColor(4, 0, 50, 0);
    frontStrip.show();
    Serial.println("\nWiFi connected!");
    online = 1;
  }
}