#include "HCDC_DEMO_API.h"
#include <float.h>

bool Fabric::Chip::Tile::Slice::FunctionUnit::findBiasHelper (
	unsigned char & code
) {
	binarySearch ( 0, FLT_MAX, 63, FLT_MAX, code );

	Serial.print("\ntile row ");
	Serial.print(parentSlice->parentTile->tileRowId);
	Serial.print(" tile col ");
	Serial.print(parentSlice->parentTile->tileColId);
	Serial.print(" slice ");
	Serial.print(parentSlice->sliceId);
	Serial.print(" unit ");
	Serial.print(unitId);
	Serial.print(" code ");
	Serial.println(code);

	if (code==0 || code==1) {
		setAnaIrefDacNmos(true, false);
		return false;
	} else if (code==63 || code==62) {
		setAnaIrefDacNmos(false, true);
		return false;
	} else {
		return true;
	}
}

bool Fabric::Chip::Tile::Slice::FunctionUnit::Interface::findBiasHelper (
	unsigned char & code
) {
	binarySearch ( 0, FLT_MAX, 63, FLT_MAX, code );

	Serial.print("\ntile row ");
	Serial.print(parentFu->parentSlice->parentTile->tileRowId);
	Serial.print(" tile col ");
	Serial.print(parentFu->parentSlice->parentTile->tileColId);
	Serial.print(" slice ");
	Serial.print(parentFu->parentSlice->sliceId);
	Serial.print(" unit ");
	Serial.print(parentFu->unitId);
	Serial.print(" ifc ");
	Serial.print(ifcId);
	Serial.print(" code ");
	Serial.println(code);

	if (code==0 || code==1) {
		// bipolar so if running out of code also increase
		parentFu->setAnaIrefDacNmos(false, true);
		return false;
	} else if (code==63 || code==62) {
		parentFu->setAnaIrefDacNmos(false, true);
		return false;
	} else {
		return true;
	}
}

bool Fabric::Chip::Tile::Slice::FunctionUnit::binarySearchAvg (
	unsigned char minCode,
	float minBest,
	unsigned char maxCode,
	float maxBest,
	unsigned char & finalCode
) {
	Serial.print(" minCode ");
	Serial.print(minCode);
	Serial.print(" maxCode ");
	Serial.println(maxCode);
	if (minCode+1==maxCode) {
		if (minBest<maxBest) finalCode=minCode;
		else finalCode=maxCode;
		return true;
	} else {
		finalCode = (maxCode + minCode) / 2;
		return false;
	}
}

bool Fabric::Chip::Tile::Slice::FunctionUnit::Interface::binarySearchAvg (
	unsigned char minCode,
	float minBest,
	unsigned char maxCode,
	float maxBest,
	unsigned char & finalCode
) {
	Serial.print(" minCode ");
	Serial.print(minCode);
	Serial.print(" maxCode ");
	Serial.println(maxCode);
	if (minCode+1==maxCode) {
		if (minBest<maxBest) finalCode=minCode;
		else finalCode=maxCode;
		return true;
	} else {
		finalCode = (maxCode + minCode) / 2;
		return false;
	}
}

float Fabric::Chip::Tile::Slice::FunctionUnit::binarySearchMeas () {
	delayMicroseconds(CAL_REPS);
	float voltageDiff = parentSlice->parentTile->parentChip->tiles[3].slices[3].chipOutput->analogAvg(CAL_REPS);
	Serial.print(" voltageDiff ");
	Serial.println(voltageDiff, 6);
	return voltageDiff;
}

float Fabric::Chip::Tile::Slice::FunctionUnit::Interface::binarySearchMeas () {
	delayMicroseconds(CAL_REPS);
	float voltageDiff = parentFu->parentSlice->parentTile->parentChip->tiles[3].slices[3].chipOutput->analogAvg(CAL_REPS);
	Serial.print(" voltageDiff ");
	Serial.println(voltageDiff, 6);
	return voltageDiff;
}

void Fabric::Chip::Tile::Slice::FunctionUnit::setAnaIrefDacNmosHelper (
	bool decrement,
	bool increment
) {
	if (decrement&&increment) error ("Cannot both increment and decrement");
	else if (decrement) {
		if (anaIrefDacNmos==0) error ("Bias already set to extreme value");
		else anaIrefDacNmos--;
	} else if (increment) {
		if (anaIrefDacNmos==7) error ("Bias already set to extreme value");
		else anaIrefDacNmos++;
	}

// 	Serial.print("anaIrefDacNmos = ");
// 	Serial.println(anaIrefDacNmos);
}