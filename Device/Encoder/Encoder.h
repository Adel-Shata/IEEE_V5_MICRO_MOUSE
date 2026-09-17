/*
 * Encoder.h
 *
 *  Created on: 16 Dec 2026
 *      Author: Adel Shata
 */

#ifndef ENCODER_H_
#define ENCODER_H_

#include <stdint.h>
#include <stdbool.h>
#include "../../config.h"

void encodersBegin(void);
void encodersReset(void);
long encodersLeft(void);
long encodersRight(void);

#endif /* ENCODER_H_ */
