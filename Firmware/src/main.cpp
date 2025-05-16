#include <stdint.h>
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
#define RIGHT_SWITCH_PIN 5//SW1

// -----------------------------
// Hall Sensor Pin
// -----------------------------
#define HALL_SENSOR_PIN 47    //J5
//#define HALL_SENSOR_PIN2 15 //J8

// -----------------------------
// Speed Sensor Setup
// -----------------------------
#define SPEED_SENSOR_PIN 16 //J9
#define MIN_VALID_INTERVAL 170    // Reject anything faster than 170 ms (limit of speed at 47 km/h)
#define WHEEL_CIRCUMFERENCE_M 2.2 // 27.5" wheel

volatile unsigned long lastEdgeTime = 0;
volatile unsigned long interval = 0;
volatile bool edgeDetected = false;
float speed = 0.0;
unsigned long lastEdgeTimeCopy = 0;
bool speedWasZero = false;

/*action modulation*/
unsigned long lastActionTime = 0;
const unsigned long actionInterval = 200; // 200 ms
//timer for speed actualization//
unsigned long lastZeroSpeedSent = 0;  // tracks when speed=0 was last published
const unsigned long zeroSpeedCooldown = 5000;  // only publish once every 5s
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
//interrupt for speed
void IRAM_ATTR handleFallingEdge()
{
  if (digitalRead(SPEED_SENSOR_PIN) == LOW)
  {
    unsigned long currentTime = millis();
    unsigned long newInterval = currentTime - lastEdgeTime;

    if (newInterval >= MIN_VALID_INTERVAL)
    {
      interval = newInterval;
      edgeDetected = true;
      lastEdgeTime = currentTime;
    }
  }
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
  attachInterrupt(digitalPinToInterrupt(SPEED_SENSOR_PIN), handleFallingEdge, FALLING);

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
  noInterrupts();
  lastEdgeTimeCopy = lastEdgeTime; // Safely copy for use outside ISR
  unsigned long now = millis();
  unsigned long timeBetweenEdges = interval;
  edgeDetected = false;
  interrupts();
  if (online)
  {
    if (!client.connected())
    {
      reconnect();
    }
    client.loop();
  }
// -----------------------------------------------------
// Actions And manipulations with LED Panels  
// -----------------------------------------------------
unsigned long currentMillis = millis();
  if (currentMillis - lastActionTime >= actionInterval)
  {
    lastActionTime = currentMillis;
    processHeartRate();
    handleLEDAnimations();
    updateFrontPanelWithHallSensor();
    updateBackPanel();
  }
//-----------------------------------------------------
// Update speed back to zero                    
//-----------------------------------------------------

  if (now - lastEdgeTimeCopy > 10000)
  {
    if (now - lastZeroSpeedSent >= zeroSpeedCooldown)
    {
      //Setting the speed to 0
      speed = 0;
      Serial.print("Speed: ");
      Serial.print(speed, 1);
      Serial.println(" km/h");
      
      
      //Sending to the internet
      char speedMsg[16];
      sprintf(speedMsg, "%.1f km/h", speed);
      client.publish("esp32/rpm", speedMsg);
      Serial.print("published:");
      Serial.println(speedMsg);
      
      lastZeroSpeedSent = now;  // Reset cooldown timer
    }
  }
//-----------------------------------------------------
// Detect a full revolution                       
//-----------------------------------------------------
  if (edgeDetected)
  {

    float speed = 0.0;
    if (timeBetweenEdges > 0)
    {
      speed = (WHEEL_CIRCUMFERENCE_M * 1000.0) / timeBetweenEdges * 3.6; //conversion of m/s accomplished into km/h
    }
    Serial.print("⏱ Time between falling edges: ");
    Serial.print(timeBetweenEdges);
    Serial.println(" ms");
    Serial.print("Speed: ");
    Serial.print(speed, 1);
    Serial.println(" km/h");
    //Upload it to the internet
    if (online){
      char speedMsg[16];
      sprintf(speedMsg, "%.1f km/h", speed);
      client.publish("esp32/rpm", speedMsg);
      Serial.print("published:");
      Serial.println(speedMsg);
    }
  }
}