#ifndef FreePass_ALERT_POLICY_H
#define FreePass_ALERT_POLICY_H

#include <stdint.h>

namespace FreePass
{

    enum class AlertLevel : uint8_t
    {
        NORMAL = 0,
        ATTENTION = 1,
        CRITICAL = 2
    };

    struct Measurement
    {
        bool distanceValid;
        float distanceCm;
        uint16_t lightAdc;
    };

    struct Decision
    {
        AlertLevel level;
        const char *reason;
        bool lowLight;
    };

    constexpr uint16_t LOW_LIGHT_ADC = 2048;
    constexpr float CRITICAL_DISTANCE_CM = 40.0f;
    constexpr float ATTENTION_DISTANCE_BRIGHT_CM = 100.0f;
    constexpr float ATTENTION_DISTANCE_DARK_CM = 150.0f;

    inline const char *levelName(AlertLevel level)
    {
        switch (level)
        {
        case AlertLevel::NORMAL:
            return "NORMAL";
        case AlertLevel::ATTENTION:
            return "ATENCAO";
        case AlertLevel::CRITICAL:
            return "CRITICO";
        }
        return "DESCONHECIDO";
    }

    inline Decision classify(const Measurement &reading)
    {
        const bool lowLight = reading.lightAdc >= LOW_LIGHT_ADC;

        if (!reading.distanceValid ||
            !(reading.distanceCm >= 2.0f && reading.distanceCm <= 400.0f))
        {
            return {AlertLevel::ATTENTION, "DISTANCIA_INDISPONIVEL", lowLight};
        }

        if (reading.distanceCm <= CRITICAL_DISTANCE_CM)
            return {AlertLevel::CRITICAL, "OBSTACULO_MUITO_PROXIMO", lowLight};

        const float attentionLimit = lowLight
                                         ? ATTENTION_DISTANCE_DARK_CM
                                         : ATTENTION_DISTANCE_BRIGHT_CM;

        if (reading.distanceCm <= attentionLimit)
        {
            return {AlertLevel::ATTENTION,
                    lowLight ? "OBSTACULO_COM_BAIXA_LUZ" : "OBSTACULO_PROXIMO",
                    lowLight};
        }

        return {AlertLevel::NORMAL, "CAMINHO_SEM_ALERTA", lowLight};
    }

    struct AlertSignal
    {
        bool attentionLed;
        bool criticalLed;
        bool buzzerOn;
        unsigned int frequencyHz;
    };

    inline AlertSignal signalFor(AlertLevel level, uint32_t elapsedAtLevelMs)
    {
        switch (level)
        {
        case AlertLevel::NORMAL:
            return {false, false, false, 0};
        case AlertLevel::ATTENTION:
            return {true, false, (elapsedAtLevelMs % 900U) < 120U, 1000};
        case AlertLevel::CRITICAL:
            return {false, true, (elapsedAtLevelMs % 300U) < 160U, 2200};
        }
        return {false, false, false, 0};
    }

}

#endif
