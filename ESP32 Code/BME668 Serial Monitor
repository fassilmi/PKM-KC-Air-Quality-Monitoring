#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME680.h>
#define SEALEVELPRESSURE_HPA (1013.25)
#define SDA_PIN 21
#define SCL_PIN 22

Adafruit_BME680 bme;

struct SensorData {
    float temperature = 0;
    float humidity = 0;
    float pressure = 0;
    float gasResistance = 0;
    float altitude = 0;
    float dewPoint = 0;
    float airQualityIndex = 0;
    unsigned long lastUpdate = 0;
    bool valid = false;
};

SensorData sensorData;

enum SensorReadState { SENSOR_IDLE, SENSOR_READING };
SensorReadState sensorState = SENSOR_IDLE;
unsigned long sensorReadyAt = 0;
unsigned long lastReadRequest = 0;
const unsigned long READ_INTERVAL_MS = 5000;

void requestSensorRead();
void checkSensorRead();
void printSensorData();
float calculateAirQualityIndex(float gas, float humidity);
float dewPointCalculation(float temperature, float humidity);
float mapFloat(float x, float inMin, float inMax, float outMin, float outMax);

void setup() {
    Serial.begin(115200);
    delay(500);

    Wire.begin(SDA_PIN, SCL_PIN);

    Serial.println();
    Serial.println(F("BME688 Environmental Monitor"));
    int initAttempts = 0;

    while (!bme.begin(0x76, &Wire) && initAttempts < 5) {
        Serial.printf("Mencoba koneksi sensor (%d/5)...\n", initAttempts + 1);
        delay(1000);
        initAttempts++;
    }

    if (initAttempts >= 5) {
        Serial.println("ERROR: Sensor BME688 tidak ditemukan!");
        while (1) {
            delay(1000);
        }
    }

    Serial.println("BME688 berhasil terhubung!");

    bme.setTemperatureOversampling(BME680_OS_8X);
    bme.setHumidityOversampling(BME680_OS_4X);
    bme.setPressureOversampling(BME680_OS_4X);
    bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
    bme.setGasHeater(320, 150);

    requestSensorRead();
}

void loop() {
    checkSensorRead();

    if (sensorState == SENSOR_IDLE && millis() - lastReadRequest >= READ_INTERVAL_MS) {
        lastReadRequest = millis();
        requestSensorRead();
    }
}

void requestSensorRead() {
    if (sensorState != SENSOR_IDLE)
        return;

    uint32_t endTime = bme.beginReading();

    if (endTime == 0) {
        Serial.println("Gagal memulai pembacaan!");
        return;
    }

    sensorReadyAt = endTime;
    sensorState = SENSOR_READING;
}

void checkSensorRead() {
    if (sensorState != SENSOR_READING)
        return;

    if ((int32_t)(millis() - sensorReadyAt) < 0)
        return;

    if (!bme.endReading()) {
        Serial.println("Gagal membaca sensor!");
        sensorState = SENSOR_IDLE;
        return;
    }

    sensorData.temperature = bme.temperature;
    sensorData.humidity = bme.humidity;
    sensorData.pressure = bme.pressure / 100.0;
    sensorData.gasResistance = bme.gas_resistance / 1000.0;

    sensorData.altitude = 44330.0 * (1.0 - pow(sensorData.pressure / SEALEVELPRESSURE_HPA, 0.1903));

    sensorData.dewPoint = dewPointCalculation(sensorData.temperature, sensorData.humidity);
    sensorData.airQualityIndex = calculateAirQualityIndex(sensorData.gasResistance, sensorData.humidity);

    sensorData.lastUpdate = millis();
    sensorData.valid = true;

    printSensorData();

    sensorState = SENSOR_IDLE;
}

void printSensorData() {
    Serial.println("---------- Pembacaan Sensor Baru ----------");
    Serial.printf("Suhu           : %.2f C\n", sensorData.temperature);
    Serial.printf("Kelembapan     : %.2f %%\n", sensorData.humidity);
    Serial.printf("Tekanan        : %.2f hPa\n", sensorData.pressure);
    Serial.printf("Gas Resistance : %.2f KOhm\n", sensorData.gasResistance);
    Serial.printf("Ketinggian     : %.2f m\n", sensorData.altitude);
    Serial.printf("Titik Embun    : %.2f C\n", sensorData.dewPoint);
    Serial.printf("Indeks Kualitas Udara: %.2f (0-100)\n", sensorData.airQualityIndex);
    Serial.printf("Waktu berjalan (ms)  : %lu\n", sensorData.lastUpdate);
    Serial.println("--------------------------------------------\n");
}

float mapFloat(float x, float inMin, float inMax, float outMin, float outMax) {
    return (x - inMin) * (outMax - outMin) / (inMax - inMin) + outMin;
}

float dewPointCalculation(float temperature, float humidity) {
    const float a = 17.271;
    const float b = 237.7;
    float temp = (a * temperature) / (b + temperature) + log(humidity / 100.0);
    return (b * temp) / (a - temp);
}

float calculateAirQualityIndex(float gasResistance, float humidity) {
    float gasScore = mapFloat(gasResistance, 10, 1000, 0, 100);
    gasScore = constrain(gasScore, 0.0f, 100.0f);

    float humidityScore = 100 - fabs(humidity - 50) * 2;
    humidityScore = constrain(humidityScore, 0.0f, 100.0f);

    return gasScore * 0.7 + humidityScore * 0.3;
}
