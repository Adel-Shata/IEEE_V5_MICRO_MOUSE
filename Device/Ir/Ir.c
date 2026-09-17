/*
 * Ir.c
 *
 *  Created on: 16 Dec 2026
 *      Author: Adel Shata
 */

#include "Ir.h"

static const uint8_t irRxPins[IR_NUM_SENSORS] = {
    PIN_IR_RIGHT, PIN_IR_FRONT, PIN_IR_LEFT, PIN_IR_TOP_LEFT, PIN_IR_TOP_RIGHT
};

static const uint8_t irTxPins[IR_NUM_SENSORS] = {
    PIN_LED_RIGHT, PIN_LED_FRONT, PIN_LED_LEFT, PIN_LED_TOP_LEFT, PIN_LED_TOP_RIGHT
};

static ir_reading_t irData[IR_NUM_SENSORS];

static uint16_t irSampleAvg(uint8_t pin, uint8_t samples) {
    uint32_t sum = 0;
    uint8_t i;
    for (i = 0; i < samples; i++) {
        sum += halAdcRead(pin);
    }
    return (uint16_t)(sum / samples);
}

void irInit(void) {
    uint8_t i;
    for (i = 0; i < IR_NUM_SENSORS; i++) {
        halGpioMode(irTxPins[i], HAL_GPIO_OUTPUT);
        halDigitalWrite(irTxPins[i], HAL_LOW);
        halGpioMode(irRxPins[i], HAL_GPIO_INPUT);
        irData[i].AMBIENT = 0;
        irData[i].RAW = 0;
        irData[i].FILTERED = 0;
    }
}

void irReadAll(void) {
    uint8_t i, s;
    for (i = 0; i < IR_NUM_SENSORS; i++) {
        /* AMBIENT: all LEDs off */
        for (s = 0; s < IR_NUM_SENSORS; s++) {
            halDigitalWrite(irTxPins[s], HAL_LOW);
        }
        halDelayMs(IR_SETTLE_US / 1000 + 1);
        irData[i].AMBIENT = irSampleAvg(irRxPins[i], IR_AMBIENT_SAMPLES);

        /* Active: turn on this sensor's LED only */
        halDigitalWrite(irTxPins[i], HAL_HIGH);
        halDelayMs(IR_SETTLE_US / 1000 + 1);
        irData[i].RAW = irSampleAvg(irRxPins[i], IR_SAMPLES_PER_READ);

        /* Background subtract */
        uint16_t signal = 0;
        if (irData[i].RAW > irData[i].AMBIENT) {
            signal = irData[i].RAW - irData[i].AMBIENT;
        }

        /* EMA filter */
        irData[i].FILTERED = (uint16_t)(IR_EMA_ALPHA * signal +
                                         (1.0f - IR_EMA_ALPHA) * irData[i].FILTERED);

        /* LED off */
        halDigitalWrite(irTxPins[i], HAL_LOW);
    }
}

uint16_t irRead(ir_sensor_id_t sensor) {
    return irData[sensor].FILTERED;
}

/*
 * Fresh single-sensor read: fires only this sensor's LED, updates its
 * cache entry, returns the filtered signal. Used by the wall checks so
 * they never depend on a prior irReadAll().
 */
static uint16_t irReadSingle(ir_sensor_id_t sensor) {
    uint8_t s;
    uint16_t ambient, raw, signal;
    for (s = 0; s < IR_NUM_SENSORS; s++) {
        halDigitalWrite(irTxPins[s], HAL_LOW);
    }
    halDelayMs(IR_SETTLE_US / 1000 + 1);
    ambient = irSampleAvg(irRxPins[sensor], IR_AMBIENT_SAMPLES);
    halDigitalWrite(irTxPins[sensor], HAL_HIGH);
    halDelayMs(IR_SETTLE_US / 1000 + 1);
    raw = irSampleAvg(irRxPins[sensor], IR_SAMPLES_PER_READ);
    halDigitalWrite(irTxPins[sensor], HAL_LOW);

    irData[sensor].AMBIENT = ambient;
    irData[sensor].RAW = raw;
    signal = 0;
    if (raw > ambient) {
        signal = raw - ambient;
    }
    irData[sensor].FILTERED = (uint16_t)(IR_EMA_ALPHA * signal +
                                         (1.0f - IR_EMA_ALPHA) * irData[sensor].FILTERED);
    return irData[sensor].FILTERED;
}

uint8_t irWallFront(void) {
    return irReadSingle(IR_SENSOR_FRONT) >= IR_WALL_THRESHOLD_FRONT ? 1 : 0;
}

uint8_t irWallLeft(void) {
    return irReadSingle(IR_SENSOR_LEFT) >= IR_WALL_THRESHOLD_SIDE ? 1 : 0;
}

uint8_t irWallRight(void) {
    return irReadSingle(IR_SENSOR_RIGHT) >= IR_WALL_THRESHOLD_SIDE ? 1 : 0;
}
