#include "HCDC_DEMO_API.h"

/*Auxiliary function for converting between endian formats for 8 bit values*/
unsigned char endian (unsigned char input) {
	if (input<0||255<input) error ("endian conversion input out of bounds");
	unsigned char output = 0;
	output += (input&0x00000080)>>7;
	output += (input&0x00000040)>>5;
	output += (input&0x00000020)>>3;
	output += (input&0x00000010)>>1;
	output += (input&0x00000008)<<1;
	output += (input&0x00000004)<<3;
	output += (input&0x00000002)<<5;
	output += (input&0x00000001)<<7;
	if (output<0||255<output) error ("endian conversion output out of bounds");
	return output;
}