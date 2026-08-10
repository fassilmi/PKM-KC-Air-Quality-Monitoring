#include <Wire.h>
#include <Adafruit_ADS1X15.h>

Adafruit_ADS1115 ads;

#define SDA_PIN 21
#define SCL_PIN 22
#define CH_MQ131 0
#define CH_MQ136 1

void setup() {
  Serial.begin(115200);
  delay(500);

  Wire.begin(SDA_PIN, SCL_PIN);

  if (!ads.begin(0x48)) {
    Serial.println("Gagal menemukan ADS1115. Cek wiring & alamat I2C.");
    while (1) delay(1000);
  }

  ads.setGain(GAIN_TWOTHIRDS);

  Serial.println("ADS1115 siap. Mulai membaca MQ-131 & MQ-136...");
}

void loop() {
  int16_t raw131 = ads.readADC_SingleEnded(CH_MQ131);
  int16_t raw136 = ads.readADC_SingleEnded(CH_MQ136);

  float volt131 = ads.computeVolts(raw131);
  float volt136 = ads.computeVolts(raw136);

  Serial.print("MQ-131 (ozon) -> raw: ");
  Serial.print(raw131);
  Serial.print("  |  tegangan: ");
  Serial.print(volt131, 3);
  Serial.println(" V");

  Serial.print("MQ-136 (H2S)  -> raw: ");
  Serial.print(raw136);
  Serial.print("  |  tegangan: ");
  Serial.print(volt136, 3);
  Serial.println(" V");

  delay(1000);
}
