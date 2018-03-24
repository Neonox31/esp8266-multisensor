#include <Homie.h>
#include <SimpleDHT.h>

#define FW_NAME "multisensor-firmware"
#define FW_VERSION "1.0.0"

const int PIN_LDR = A0;
const int PIN_PIR = D6;
const int PIN_DHT22 = D4;

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
    int luminosity = (analogRead(PIN_LDR) * 100) / 1024;

    luminosityNode.setProperty("state").send(String(luminosity));
    lastLuminositySent = millis();
  }

  if (millis() - lastMotionSent >= MOTION_INTERVAL * 1000UL || lastMotionSent == 0) {
    String motion = digitalRead(PIN_PIR) == 1 ? "true" : "false";

    motionNode.setProperty("state").send(motion);
    lastMotionSent = millis();
  }

  if (millis() - lastTemperatureHumiditySent >= TEMPERATURE_HUMIDITY_INTERVAL * 1000UL || lastTemperatureHumiditySent == 0) {
    float temperature = 0;
    float humidity = 0;

    int err = SimpleDHTErrSuccess;
    if ((err = dht22.read2(PIN_DHT22, &temperature, &humidity, NULL)) != SimpleDHTErrSuccess) {
      Homie.getLogger() << "read DHT22 failed, err= " << err << endl;
    }

    temperatureNode.setProperty("state").send(String(temperature));
    humidityNode.setProperty("state").send(String(humidity));
    lastTemperatureHumiditySent = millis();
  }
}

void setupHandler() {
  luminosityNode.setProperty("unit").send("%");
  temperatureNode.setProperty("unit").send("°C");
  humidityNode.setProperty("unit").send("%");
}

void setup() {
  Serial.begin(115200);
  Serial << endl << endl;

  Homie_setFirmware(FW_NAME, FW_VERSION);
  Homie.setSetupFunction(setupHandler).setLoopFunction(loopHandler);
  
  luminosityNode.advertise("unit");
  temperatureNode.advertise("unit");
  humidityNode.advertise("unit");

  luminosityNode.advertise("state");
  motionNode.advertise("state");
  temperatureNode.advertise("state");
  humidityNode.advertise("state");

  Homie.setup();
}

void loop() {
  Homie.loop();
}
