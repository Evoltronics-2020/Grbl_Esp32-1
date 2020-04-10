/*
    i2s_ioexpander.h
    Part of Grbl_ESP32

    Header for basic GPIO expander using the ESP32 I2S peripheral

    2020    - Michiyasu Odaki

    Grbl_ESP32 is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Grbl is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Grbl_ESP32.  If not, see <http://www.gnu.org/licenses/>.
*/
/**
 * Marlin 3D Printer Firmware
 * Copyright (c) 2020 MarlinFirmware [https://github.com/MarlinFirmware/Marlin]
 *
 * Based on Sprinter and grbl.
 * Copyright (c) 2011 Camiel Gubbels / Erik van der Zalm
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef i2s_ioexpander_h
#define i2s_ioexpander_h

#ifdef USE_I2S_IOEXPANDER
#include <stdint.h>

#define I2S_IOEXP_PIN_BASE 128

/* 1000000 usec / ((160000000 Hz) / 5 / 2) x 32 bit/pulse x 2(stereo) = 4 usec/pulse */
#define I2S_IOEXP_USEC_PER_PULSE 4

typedef void (*i2s_ioexpander_pulse_phase_func_t)(void);

typedef struct {
    /*
        I2S bitstream (32-bits):
             LEFT Channel                    Right Channel                   LEFT Channel
        ws   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~________________________________~~~~...
        bck  _~_~_~_~_~_~_~_~_~_~_~_~_~_~_~_~_~_~_~_~_~_~_~_~_~_~_~_~_~_~_~_~_~_~...
        data vutsrqponmlkjihgfedcba9876543210vutsrqponmlkjihgfedcba9876543210____...
             dummy(same as right channel)    XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX^
                                                                             latch X bits
        0:Extended GPIO 128, 1: Extended GPIO 129, ..., v: Extended GPIO 159
        (data at LEFT Channel will ignored by shift-register IC)
    */
    uint8_t ws_pin;
    uint8_t bck_pin;
    uint8_t data_pin;
    i2s_ioexpander_pulse_phase_func_t pulse_phase_func;
    uint32_t pulse_period; // aka step rate.
} i2s_ioexpander_init_t;

/*
  Initialize I2S and DMA for the stepper bitstreamer
  use I2S0, I2S0 isr, DMA, and FIFO(xQueue).
*/
int i2s_ioexpander_init(i2s_ioexpander_init_t &init_param);

/*
  Get a bit state from the internal pin state var.

  pin: expanded pin No. (0..31)
*/
uint8_t i2s_ioexpander_state(uint8_t pin);

/*
   Set a bit in the internal pin state var. (not written electrically)

   pin: extended pin No. (0..31)
   val: bit value(0 or not 0)
*/
void i2s_ioexpander_write(uint8_t pin, uint8_t val);

/*
    Set current pin state to the I2S bitstream buffer
    (This call will generate a future 4μs x N bitstream)

    num: Number of samples to be generated

    return: number of puhsed samples
            0 .. no space for push
 */
uint32_t i2s_ioexpander_push_sample(uint32_t num);

int i2s_ioexpander_start();
int i2s_ioexpander_stop();
int i2s_ioexpander_set_pulse_period(uint64_t period);

#endif

/*
   Reference: "ESP32 Technical Reference Manual" by Espressif Systems
     https://www.espressif.com/sites/default/files/documentation/esp32_technical_reference_manual_en.pdf
 */
#endif
