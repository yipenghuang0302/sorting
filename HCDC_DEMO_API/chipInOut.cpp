#include "HCDC_DEMO_API.h"

/*Measure the reading of an ADC from multiple samples*/
float Fabric::Chip::Tile::Slice::ChipOutput::analogAvg (
	unsigned char samples
) {

	// analog chan7
	// y = 0.000803x + 0.014645
	// analog chan6
	// y = 0.000803x + 0.014928

	float posGain = 0.000804; /*anaOut0Pos*/
	float negGain = 0.000804; /*anaOut0Neg*/
	float posOffset = 0.038951; /*anaOut0Pos*/
	float negOffset = 0.038563; /*anaOut0Neg*/

	unsigned long posSum = 0., negSum = 0.;
	for (unsigned char index = 0; index < samples; index++) {
		posSum += analogRead(posPin);
		negSum += analogRead(negPin);
	}

	return	(
				posGain * ((float)posSum/(float)samples) + posOffset
			) - (
				negGain * ((float)negSum/(float)samples) + negOffset
			);
}