#include <Homie.h>
#include <SimpleDHT.h>

#define FW_NAME "multisensor-firmware"
#define FW_VERSION "1.0.0"

/* Magic sequence for Autodetectable Binary Upload */
const char *__FLAGGED_FW_NAME = "\xbf\x84\xe4\x13\x54" FW_NAME "\x93\x44\x6b\xa7\x75";
const char *__FLAGGED_FW_VERSION = "\x6a\x3f\x3e\x0e\xe1" FW_VERSION "\xb0\x30\x48\xd4\x1a";
/* End of magic sequence for Autodetectable Binary Upload */

const int PIN_LDR = A0;
const int PIN_PIR = D6;
const int PIN_DHT22 = D3;

const int LUMINOSITY_INTERVAL = 1;
const int MOTION_INTERVAL = 1;
const int TEMPERATURE_HUMIDITY_INTERVAL = 5;

unsigned long lastLuminositySent = 0;
unsigned long lastMotionSent = 0;
unsigned long lastTemperatureHumiditySent = 0;

HomieNode luminosityNode("luminosity", "luminosity");
HomieNode motionNode("motion", "motion");
HomieNode temperatureNode("temperature", "temperature");
HomieNode humidityNode("humidity", "humidity");

SimpleDHT22 dht22;

void loopHandler() {
  if (millis() - lastLuminositySent >= LUMINOSITY_INTERVAL * 1000UL || lastLuminositySent == 0) {
    int luminosity = analogRead(PIN_LDR);

    if (!Homie.setNodeProperty(luminosityNode, "state", String(luminosity), true)) {
      Serial.println("Luminosity sending failed");
    } else {
      lastLuminositySent = millis();
    }
  }

  if (millis() - lastMotionSent >= MOTION_INTERVAL * 1000UL || lastMotionSent == 0) {
    int motion = digitalRead(PIN_PIR);

    if (!Homie.setNodeProperty(motionNode, "state", String(motion), true)) {
      Serial.println("Motion sending failed");
    } else {
      lastMotionSent = millis();
    }
  }

  if (millis() - lastTemperatureHumiditySent >= TEMPERATURE_HUMIDITY_INTERVAL * 1000UL || lastTemperatureHumiditySent == 0) {
    float temperature = 0;
    float humidity = 0;

    int err = SimpleDHTErrSuccess;
    if ((err = dht22.read2(PIN_DHT22, &temperature, &humidity, NULL)) != SimpleDHTErrSuccess) {
      Serial.print("Read DHT22 failed, err="); Serial.println(err);
    }

    if (!Homie.setNodeProperty(temperatureNode, "state", String(temperature), true) || !Homie.setNodeProperty(humidityNode, "state", String(humidity), true)) {
      Serial.println("Temperature or humidity sending failed");
    } else {
      lastTemperatureHumiditySent = millis();
    }
  }
}

void setup() {
  Homie.setFirmware(FW_NAME, FW_VERSION);
  Homie.registerNode(luminosityNode);
  Homie.registerNode(motionNode);
  Homie.registerNode(temperatureNode);
  Homie.registerNode(humidityNode);
  Homie.setLoopFunction(loopHandler);
  Homie.setup();
}

void loop() {
  Homie.loop();
}
