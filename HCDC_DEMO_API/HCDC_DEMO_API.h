#include "Arduino.h"

extern size_t dynamic_memory;

template <typename type>
void tally_dyn_mem(
	const char * name
) {
	dynamic_memory += sizeof(type);
	// Serial.print(sizeof(type));
	// Serial.print('\t');
	// Serial.println(name);
}

static void error (
	const char * message
) {
	Serial.print("ERROR: ");
	Serial.println(message);
	Serial.flush();
	exit(1);
}
// how much to delay before measurement and how many times to measure
#define CAL_REPS 16

#ifndef _PIN
#define _PIN
#include "pin.h"
#endif

#ifndef _FABRIC
#define _FABRIC
#include "fabric.h"
#endif

#ifndef _CHIP
#define _CHIP
#include "chip.h"
#endif

#ifndef _TILE
#define _TILE
#include "tile.h"
#endif

#ifndef _SLICE
#define _SLICE
#include "slice.h"
#endif

#ifndef _FU
#define _FU
#include "fu.h"
#endif

#ifndef _CHIP_INOUT
#define _CHIP_INOUT
#include "chipInOut.h"
#endif

#ifndef _DAC
#define _DAC
#include "dac.h"
#endif

#ifndef _MUL
#define _MUL
#include "mul.h"
#endif

#ifndef _INT
#define _INT
#include "int.h"
#endif

#ifndef _FAN
#define _FAN
#include "fan.h"
#endif

#ifndef _ADC
#define _ADC
#include "adc.h"
#endif

#ifndef _LUT
#define _LUT
#include "lut.h"
#endif

#ifndef _TILE_INOUT
#define _TILE_INOUT
#include "tileInOut.h"
#endif

#ifndef _CONNECTION
#define _CONNECTION
#include "connection.h"
#endif

#ifndef _VECTOR
#define _VECTOR
#include "vector.h"
#endif

#ifndef _SPI
#define _SPI
#include "spi.h"
#endif