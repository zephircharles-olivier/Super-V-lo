#include <Arduino.h>
#include <HeartrateDetection.h>
#include <WifiMQTT.h>

#define VIBRATION_PIN 14 //J4
#define MIN_TAP_INTERVAL 300         // Bounce protection: ignore taps faster than 300ms
#define AVERAGE_WINDOW 10000         // 5 seconds in milliseconds
#define MAX_TAPS 20                  // Max pulses expected in 10s (e.g., 120 BPM = 20 beats in 10s)


struct Tap {
  unsigned long time;
};

Tap taps[MAX_TAPS];
int tapCount = 0;
unsigned long lastTapTime = 0;
bool tapped = false;
unsigned long lastPublishTime = 0;
unsigned long publishInterval = 1000;
float bpm = 0;

void processHeartRate()
{
  unsigned long currentTime = millis();
  int sensorState = digitalRead(VIBRATION_PIN);
  

  // Detect edge with debounce
  if (sensorState == LOW && !tapped) {
    tapped = true;
    if (currentTime - lastTapTime >= MIN_TAP_INTERVAL) {
      lastTapTime = currentTime;

      // Add new tap
      if (tapCount < MAX_TAPS) {
        taps[tapCount++] = {currentTime};
      } else {
        // Shift left if buffer is full
        for (int i = 1; i < MAX_TAPS; i++) {
          taps[i - 1] = taps[i];
        }
        taps[MAX_TAPS - 1] = {currentTime};
      }

      // Remove taps older than 10s
      while (tapCount > 1 && currentTime - taps[0].time > AVERAGE_WINDOW) {
        for (int i = 1; i < tapCount; i++) {
          taps[i - 1] = taps[i];
        }
        tapCount--;
      }

      // Compute average interval
      if (tapCount > 1) {
        unsigned long totalInterval = 0;
        for (int i = 1; i < tapCount; i++) {
          totalInterval += taps[i].time - taps[i - 1].time;
        }
        float avgInterval = totalInterval / float(tapCount - 1);
        bpm = 60000.0 / avgInterval;

        Serial.print("❤️ Avg Interval: ");
        Serial.print(avgInterval);
        Serial.print(" ms | BPM: ");
        Serial.println(bpm);
      }
    }
  }

  if (sensorState == HIGH) {
    tapped = false;
  }

  if (currentTime - lastPublishTime > publishInterval)
  {
    lastPublishTime = currentTime;
    if (bpm > 30 && bpm < 200)
    {
      if (online)
      {
      char bpmMsg[16];
      sprintf(bpmMsg, "BPM: %.1f", bpm);
      client.publish("esp32/bpm", bpmMsg);
      Serial.print("published:");
      Serial.println(bpmMsg);
      }
    }
    else
    {
      client.publish("esp32/bpm", "BPM not detected");
    }
  }
}