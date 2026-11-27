#include <Arduino.h>
#include "AlertPolicy.h"

namespace Config
{
    constexpr uint8_t TRIGGER_PIN = 26;
    constexpr uint8_t ECHO_PIN = 27;
    constexpr uint8_t LIGHT_PIN = 34;
    constexpr uint8_t ATTENTION_LED_PIN = 18;
    constexpr uint8_t CRITICAL_LED_PIN = 19;
    constexpr uint8_t BUZZER_PIN = 25;

    constexpr uint32_t READ_INTERVAL_MS = 250;
    constexpr uint32_t SERIAL_INTERVAL_MS = 1000;
    constexpr uint32_t ECHO_TIMEOUT_US = 28000;
    constexpr unsigned long SERIAL_BAUD = 115200;
}

FreePass::Measurement measurement = {false, 0.0f, 0};
FreePass::Decision decision = {FreePass::AlertLevel::ATTENTION, "AGUARDANDO_LEITURA", false};

uint32_t lastReadMs = 0;
uint32_t lastReportMs = 0;
uint32_t levelStartedMs = 0;
bool buzzerActive = false;
unsigned int buzzerFrequencyHz = 0;

void readSensors()
{
    digitalWrite(Config::TRIGGER_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(Config::TRIGGER_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(Config::TRIGGER_PIN, LOW);

    const unsigned long echoUs = pulseIn(
        Config::ECHO_PIN, HIGH, Config::ECHO_TIMEOUT_US);

    measurement.distanceCm = echoUs / 58.0f;
    measurement.distanceValid =
        echoUs != 0 &&
        measurement.distanceCm >= 2.0f &&
        measurement.distanceCm <= 400.0f;
    measurement.lightAdc = static_cast<uint16_t>(analogRead(Config::LIGHT_PIN));
}

void printStatus(uint32_t nowMs)
{
    Serial.print("nivel_situacao=");
    Serial.println(FreePass::levelName(decision.level));

    Serial.print("distancia_cm=");

    if (measurement.distanceValid)
        Serial.println(measurement.distanceCm, 1);
    else
        Serial.println("INDISPONIVEL");

    Serial.print("luminosidade_adc=");
    Serial.println(measurement.lightAdc);
    Serial.print("ambiente=");
    Serial.println(decision.lowLight ? "BAIXA_LUZ" : "ILUMINADO");
    Serial.print("sensor_distancia_valido=");
    Serial.println(measurement.distanceValid ? 1 : 0);
    Serial.print("motivo=");
    Serial.println(decision.reason);
    Serial.print("timestamp_ms=");
    Serial.println(nowMs);
    Serial.println("---");
}

void refreshDecision(uint32_t nowMs)
{
    readSensors();
    FreePass::Decision next = FreePass::classify(measurement);
    const bool levelChanged = next.level != decision.level;
    decision = next;

    if (levelChanged)
    {
        levelStartedMs = nowMs;
        printStatus(nowMs);
        lastReportMs = nowMs;
    }
}

void updateLocalAlert(uint32_t nowMs)
{
    const uint32_t elapsedAtLevelMs = nowMs - levelStartedMs;
    const FreePass::AlertSignal signal =
        FreePass::signalFor(decision.level, elapsedAtLevelMs);

    digitalWrite(Config::ATTENTION_LED_PIN, signal.attentionLed ? HIGH : LOW);
    digitalWrite(Config::CRITICAL_LED_PIN, signal.criticalLed ? HIGH : LOW);

    if (signal.buzzerOn)
    {
        if (!buzzerActive || buzzerFrequencyHz != signal.frequencyHz)
        {
            tone(Config::BUZZER_PIN, signal.frequencyHz);
            buzzerActive = true;
            buzzerFrequencyHz = signal.frequencyHz;
        }
    }
    else if (buzzerActive)
    {
        noTone(Config::BUZZER_PIN);
        buzzerActive = false;
        buzzerFrequencyHz = 0;
    }
}

void setup()
{
    Serial.begin(Config::SERIAL_BAUD);
    pinMode(Config::TRIGGER_PIN, OUTPUT);
    pinMode(Config::ECHO_PIN, INPUT);
    pinMode(Config::LIGHT_PIN, INPUT);
    pinMode(Config::ATTENTION_LED_PIN, OUTPUT);
    pinMode(Config::CRITICAL_LED_PIN, OUTPUT);
    pinMode(Config::BUZZER_PIN, OUTPUT);

    digitalWrite(Config::ATTENTION_LED_PIN, LOW);
    digitalWrite(Config::CRITICAL_LED_PIN, LOW);
    digitalWrite(Config::TRIGGER_PIN, LOW);
    analogReadResolution(12);
    analogSetPinAttenuation(Config::LIGHT_PIN, ADC_11db);

    const uint32_t nowMs = millis();
    readSensors();
    decision = FreePass::classify(measurement);
    lastReadMs = nowMs;
    lastReportMs = nowMs;
    levelStartedMs = nowMs;
    updateLocalAlert(nowMs);
    printStatus(nowMs);
}

void loop()
{
    const uint32_t nowMs = millis();

    if (nowMs - lastReadMs >= Config::READ_INTERVAL_MS)
    {
        lastReadMs = nowMs;
        refreshDecision(nowMs);
    }

    updateLocalAlert(nowMs);

    if (nowMs - lastReportMs >= Config::SERIAL_INTERVAL_MS)
    {
        lastReportMs = nowMs;
        printStatus(nowMs);
    }
}
