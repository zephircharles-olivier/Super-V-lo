#include <Arduino.h>

#include <LED_Animations.h>
#include <WifiMQTT.h>
#include <HeartrateDetection.h>

//---------Heartrate Pin--------//
#define VIBRATION_PIN 14//J4
// -----------------------------
// Switch Pins
// -----------------------------
#define LEFT_SWITCH_PIN 4 //SW1
#define RIGHT_SWITCH_PIN 5

// -----------------------------
// Hall Sensor Pin
// -----------------------------
#define HALL_SENSOR_PIN 47
#define HALL_SENSOR_PIN2 16

// -----------------------------
// Speed Sensor Setup
// -----------------------------
#define SPEED_SENSOR_PIN 16
volatile int wheelRotations = 0;
unsigned long lastSpeedCalcTime = 0;
const unsigned long speedInterval = 1000; // calculate speed every 1 second
float speed = 0.0;
unsigned long lastActionTime = 0;
const unsigned long actionInterval = 200; // 200 ms

void IRAM_ATTR onSpeedSensorTrigger()
{
  wheelRotations++;
}



// -----------------------------
// LED Animation Based on Switch
// -----------------------------
void updateFrontPanelWithHallSensor()
{
  frontStrip.clear();
  for (int i = 0; i < LED_COUNT; i++)
  {
    if (i == 4)
    {
      if (digitalRead(HALL_SENSOR_PIN) == LOW)
        frontStrip.setPixelColor(i, redFront);
      else
        frontStrip.setPixelColor(i, whiteFront);
    }
    else
    {
      frontStrip.setPixelColor(i, whiteFront);
    }
  }
  frontStrip.show();
}

void updateBackPanel()
{
  bool hallActive = (digitalRead(HALL_SENSOR_PIN) == LOW);
  uint32_t color = hallActive ? redBackBright : redBackDim;
  for (int i = 0; i < LED_COUNT; i++)
  {
    backStrip.setPixelColor(i, color);
  }
  backStrip.show();
}

// -----------------------------
// Setup and Loop
// -----------------------------
void setup()
{
  Serial.begin(115200);
  pinMode(VIBRATION_PIN, INPUT);
  pinMode(LEFT_SWITCH_PIN, INPUT);
  pinMode(RIGHT_SWITCH_PIN, INPUT);
  pinMode(HALL_SENSOR_PIN, INPUT);
  pinMode(SPEED_SENSOR_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(SPEED_SENSOR_PIN), onSpeedSensorTrigger, FALLING);

  leftStrip.begin();
  rightStrip.begin();
  frontStrip.begin();
  backStrip.begin();

  leftStrip.fill(leftStrip.Color(0, 255, 0));
  leftStrip.show();
  delay(2000);
  leftStrip.clear();
  leftStrip.show();

  rightStrip.fill(rightStrip.Color(0, 255, 0));
  rightStrip.show();
  delay(2000);
  rightStrip.clear();
  rightStrip.show();

  frontStrip.clear();
  frontStrip.show();

  backStrip.clear();
  backStrip.show();

  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  Serial.println("Ready to detect BPM and animate LEDs...");
}

void loop()
{
  if (online)
  {
    if (!client.connected())
    {
      reconnect();
    }
    client.loop();
  }

  unsigned long currentMillis = millis();
  if (currentMillis - lastActionTime >= actionInterval)
  {
    lastActionTime = currentMillis;
    processHeartRate();
    handleLEDAnimations();
    updateFrontPanelWithHallSensor();
    updateBackPanel();
  }
  // Speed calculation logic
  if (currentMillis - lastSpeedCalcTime >= speedInterval)
  {
    lastSpeedCalcTime = currentMillis;
    speed = (wheelRotations * 60.0) / (speedInterval / 1000.0);
    Serial.print("Wheel Speed: ");
    Serial.print(speed);
    Serial.println(" RPM");
    char speedMsg[8];
    sprintf(speedMsg, "RPM: %d", speed);
    if (online)
    {
      client.publish("esp32/rpm", speedMsg);
      Serial.print("published:");
    }
    Serial.println(speedMsg);
    wheelRotations = 0;
  }

  delay(20);
}
