#include "HCDC_DEMO_API.h"
#include <float.h>

void Fabric::Chip::Tile::Slice::Dac::setEnable (
	bool enable
) {
	this->enable = enable;
	setParam0 ();
	setParam1 ();
}

void Fabric::Chip::Tile::Slice::Dac::DacOut::setInv (
	bool inverse // whether output is negated
) {
	this->inverse = inverse;
	parentFu->setParam0 ();
}

void Fabric::Chip::Tile::Slice::Dac::DacOut::setRange (
	bool loRange, // 0.2uA mode
	bool hiRange // 20 uA mode
	// default is 2uA mode
) {
	if (loRange) error ("DAC loRange setting not implemented");
	this->hiRange = hiRange;
	parentFu->setEnable (
		parentFu->enable
	);
}

void Fabric::Chip::Tile::Slice::Dac::setSource (
	bool memory,
	bool external, // digital to analog converter takes input from chip parallel input
	bool lut0, // digital to analog converter takes input from first lookup table
	bool lut1 // digital to analog converter takes input from second lookup table
	// only one of these should be true
) {
	/*record*/
	this->memory = memory;
	switch (parentSlice->sliceId) {
		case slice0: parentSlice->parentTile->slice0DacOverride = memory; break;
		case slice1: parentSlice->parentTile->slice1DacOverride = memory; break;
		case slice2: parentSlice->parentTile->slice2DacOverride = memory; break;
		case slice3: parentSlice->parentTile->slice3DacOverride = memory; break;
	}
	this->external = external;
	if (external) {
		parentSlice->parentTile->setParallelIn ( external );
	}
	this->lut0 = lut0;
	this->lut1 = lut1;

	unsigned char cfgTile = 0b00000000;
	cfgTile += parentSlice->parentTile->slice0DacOverride ? 1<<7 : 0;
	cfgTile += parentSlice->parentTile->slice1DacOverride ? 1<<6 : 0;
	cfgTile += parentSlice->parentTile->slice2DacOverride ? 1<<5 : 0;
	cfgTile += parentSlice->parentTile->slice3DacOverride ? 1<<4 : 0;
	parentSlice->parentTile->controllerHelperTile ( 11, cfgTile );

	setEnable (
		enable
	);
}

void Fabric::Chip::Tile::Slice::Dac::setConstantCode (
	unsigned char constant // fixed point representation of desired constant
	// 0 to 255 are valid
) {
	setEnable ( true );
	setSource ( true, false, false, false );
	parentSlice->parentTile->parentChip->parentFabric->cfgCommit();
	unsigned char selLine = 0;
	switch (parentSlice->sliceId) {
		case slice0: selLine = 7; break;
		case slice1: selLine = 8; break;
		case slice2: selLine = 9; break;
		case slice3: selLine = 10; break;
	}
	unsigned char cfgTile = endian (constant);
	parentSlice->parentTile->controllerHelperTile ( selLine, cfgTile );
}

void Fabric::Chip::Tile::Slice::Dac::setConstant (
	float constant // floating point representation of desired constant
	// -10.0 to 10.0 are valid
) {
	if (fabs(constant)<1.0) {
		setConstantCode ( constant*128.0 + 128.0 );
	} else if (fabs(constant)<10.0) {
		out0->setRange ( false, true );
		setConstantCode ( (constant/10.0)*128.0 + 128.0 );
	} else error ("DAC constant bias generation must be between -10.0 and 10.0");
}

Fabric::Chip::Tile::Slice::Dac::Dac (
	Chip::Tile::Slice * parentSlice
) :
	FunctionUnit(parentSlice, unitDac)
{
	out0 = new DacOut (this);
	tally_dyn_mem <DacOut> ("DacOut");
	setAnaIrefDacNmos ( false, false );
}

/*Set enable, invert, range, clock select*/
void Fabric::Chip::Tile::Slice::Dac::setParam0 () {
	unsigned char cfgTile = 0;
	cfgTile += enable ? 1<<7 : 0;
	cfgTile += (out0->inverse) ? 1<<6 : 0;
	cfgTile += (out0->hiRange ? dacHi : dacMid) ? 1<<5 : 0;
	cfgTile += (external||memory) ? extDac : ( lut0 ? lutL : lutR )<<0;
	setParamHelper (0, cfgTile);
}

/*Set calDac, input select*/
void Fabric::Chip::Tile::Slice::Dac::setParam1 () {
	unsigned char calDac = /*out0->hiRange ? hiNegGainCode :*/ midNegGainCode; /*gain calibration code*/
	if (calDac<0||63<calDac) error ("calDac out of bounds");
	unsigned char cfgTile = 0;
	cfgTile += calDac<<2;
	cfgTile += (external||memory) ? extDac : ( lut0 ? lutL : lutR )<<0;
	setParamHelper (1, cfgTile);
}

/*Helper function*/
void Fabric::Chip::Tile::Slice::Dac::setParamHelper (
	unsigned char selLine,
	unsigned char cfgTile
) {
	if (selLine<0||1<selLine) error ("selLine out of bounds");

	/*DETERMINE SEL_COL*/
	unsigned char selCol;
	switch (parentSlice->sliceId) {
		case slice0: selCol = 6; break;
		case slice1: selCol = 3; break;
		case slice2: selCol = 7; break;
		case slice3: selCol = 4; break;
		default: error ("DAC invalid slice"); break;
	}

	Chip::Vector vec = Vector (
		*this,
		6,
		selCol,
		selLine,
		endian (cfgTile)
	);

	parentSlice->parentTile->parentChip->cacheVec (
		vec
	);
}

bool Fabric::Chip::Tile::Slice::Dac::calibrateRange (
	dacRange dacRange
) {
	out0->setRange (false, dacRange==dacHi);
	Chip::Connection conn0 = Connection ( out0, parentSlice->tileOuts[0].in0 );
	conn0.setConn();
	Chip::Connection conn1 = Connection ( parentSlice->tileOuts[0].out0, parentSlice->parentTile->parentChip->tiles[3].slices[3].chipOutput->in0 );
	conn1.setConn();

	bool biasStable = true;
	// findOffset( dacRange ? out0->hiOffset : out0->midOffset );
	biasStable &= findBias( dacRange, /*dacRange ? hiNegGainCode :*/ midNegGainCode );

	setEnable (false);
	conn0.brkConn();
	conn1.brkConn();
	parentSlice->parentTile->parentChip->parentFabric->cfgCommit();

	return biasStable;
}

void Fabric::Chip::Tile::Slice::Dac::findOffset (
	float & offset
) {
	Serial.print("Dac offset measurement tile row ");
	Serial.print(parentSlice->parentTile->tileRowId);
	Serial.print(" tile col ");
	Serial.print(parentSlice->parentTile->tileColId);
	Serial.print(" slice ");
	Serial.print(parentSlice->sliceId);
	Serial.print(" unit ");
	Serial.print(unitId);

	setConstantCode (128);
	parentSlice->parentTile->parentChip->parentFabric->cfgCommit();

	offset = binarySearchMeas();

	if (offset<-0.02||offset>0.02) error ("DAC offset too large");
}

bool Fabric::Chip::Tile::Slice::Dac::findBias (
	dacRange dacRange,
	unsigned char & gainCode
) {
	Serial.println("Dac gain calibration");

	setConstantCode (0);
	parentSlice->parentTile->parentChip->parentFabric->cfgCommit();

	bool biasStable = false;
	while ( !biasStable ) {
		binarySearch ( dacRange, 0, FLT_MAX, 63, FLT_MAX, gainCode );

		Serial.print("\nDac calibration tile row ");
		Serial.print(parentSlice->parentTile->tileRowId);
		Serial.print(" tile col ");
		Serial.print(parentSlice->parentTile->tileColId);
		Serial.print(" slice ");
		Serial.print(parentSlice->sliceId);
		Serial.print(" unit ");
		Serial.print(unitId);
		Serial.print(dacRange==dacHi ? " hiRange" : " loRange");
		Serial.print(" code ");
		Serial.println(gainCode);

		if (gainCode==0 || gainCode==1) {
			setAnaIrefDacNmos(true, false);
			biasStable = false;
		} else if (gainCode==63 || gainCode==62) {
			setAnaIrefDacNmos(false, true);
			biasStable = false;
		} else {
			biasStable = true;
		}
	}

	return biasStable;
}

void Fabric::Chip::Tile::Slice::Dac::setAnaIrefDacNmos (
	bool decrement,
	bool increment
) {
	setAnaIrefDacNmosHelper (decrement, increment);

	// TODO: order is uncertain
	// Tile53_DAC is a "new" DAC; its params are at row 6 (in HCDC v1 this would be row 5) column 6.
	// Tile54_DAC is an "old" DAC; its params are at row 6 (in HCDC v1 this would be row 5) column 3.
	// Tile55_DAC is a "new" DAC; its params are at row 6 (in HCDC v1 this would be row 5) column 7.
	// Tile56_DAC is an "old" DAC; its params are at row 6 (in HCDC v1 this would be row 5) column 4.
	// col
	// case unitDac: switch (sourceSlice) {
	// // HCDC 2 new DAC:
	// case slice0: return (2);
	// case slice1: return (2);
	// // HCDC 2 new DAC:
	// case slice2: return (5);
	// case slice3: return (5);
	// line
	// case unitDac: switch (sourceSlice) {
	// case slice0: return (9);
	// case slice1: return (10);
	// case slice2: return (14);
	// case slice3: return (15);

	unsigned char selRow;
	unsigned char selCol=2;
	unsigned char selLine;
	switch (parentSlice->sliceId) {
		case slice0: selRow=0; selLine=3; break;
		case slice1: selRow=1; selLine=0; break;
		case slice2: selRow=0; selLine=2; break;
		case slice3: selRow=1; selLine=1; break;
		default: error ("DAC invalid slice"); break;
	}
	unsigned char cfgTile = endian(parentSlice->parentTile->parentChip->cfgBuf[parentSlice->parentTile->tileRowId][parentSlice->parentTile->tileColId][selRow][selCol][selLine]);
	cfgTile = (cfgTile & 0b00111000) + (anaIrefDacNmos & 0b00000111);

	Chip::Vector vec = Vector (
		*this,
		selRow,
		selCol,
		selLine,
		endian (cfgTile)
	);

	parentSlice->parentTile->parentChip->cacheVec (
		vec
	);
}

void Fabric::Chip::Tile::Slice::Dac::binarySearch (
	dacRange dacRange,
	unsigned char minGainCode,
	float minBest,
	unsigned char maxGainCode,
	float maxBest,
	unsigned char & finalGainCode
) {

	if (binarySearchAvg (minGainCode, minBest, maxGainCode, maxBest, finalGainCode)) return;

	setParam1 ();
	parentSlice->parentTile->parentChip->parentFabric->cfgCommit();

	float voltageDiff = binarySearchMeas ();

	// Serial.print("dacRange = ");
	// Serial.println(dacRange);

	float target = dacRange ? (hiNegTarget/*+out0->hiOffset*/) : (midNegTarget/*+out0->midOffset*/);
	if (voltageDiff < target) {
		return binarySearch (dacRange, minGainCode, minBest, finalGainCode, fabs(voltageDiff-target), finalGainCode);
	} else {
		return binarySearch (dacRange, finalGainCode, fabs(voltageDiff-target), maxGainCode, maxBest, finalGainCode);
	}

}

// binary search so dac scale matches adc scale
bool Fabric::Chip::Tile::Slice::Dac::findBias (
	unsigned char & gainCode
) {
	Serial.println("Dac gain calibration");

	setConstantCode(2);
	Adc * adc;
	switch (parentSlice->sliceId) {
		case slice0: adc=parentSlice->adc; break;
		case slice1: adc=parentSlice->parentTile->slices[0].adc; break;
		case slice2: adc=parentSlice->adc; break;
		case slice3: adc=parentSlice->parentTile->slices[2].adc; break;
	}
	Chip::Connection conn = Chip::Connection ( out0, adc->in0 );
	conn.setConn();
	adc->setEnable (true);
	parentSlice->parentTile->parentChip->parentFabric->serialOutReq();

	bool biasStable = false;
	while (!biasStable) {
		biasStable = findBiasHelper (gainCode);
	}

	setEnable (false);
	conn.brkConn();
	adc->setEnable (false);
	parentSlice->parentTile->parentChip->parentFabric->serialOutStop();

	// switch to finding full scale
	setConstantCode(0);
	Chip::Connection conn0 = Chip::Connection ( out0, parentSlice->tileOuts[0].in0 );
	conn0.setConn();
	Chip::Connection conn1 = Chip::Connection ( parentSlice->tileOuts[0].out0, parentSlice->parentTile->parentChip->tiles[3].slices[3].chipOutput->in0 );
	conn1.setConn();
	parentSlice->parentTile->parentChip->parentFabric->cfgCommit();

	midNegTarget = binarySearchMeas();
	Serial.print("midNegTarget = ");
	Serial.println(midNegTarget);

	conn0.brkConn();
	conn1.brkConn();
	setEnable(false);

	return biasStable;
}

// binary search so dac scale matches adc scale
void Fabric::Chip::Tile::Slice::Dac::binarySearch (
	unsigned char minGainCode,
	float minBest,
	unsigned char maxGainCode,
	float maxBest,
	unsigned char & finalGainCode
) {
	if (binarySearchAvg (minGainCode, minBest, maxGainCode, maxBest, finalGainCode)) return;

	setParam1 ();
	parentSlice->parentTile->parentChip->parentFabric->cfgCommit();

	Adc * adc;
	switch (parentSlice->sliceId) {
		case slice0: adc=parentSlice->adc; break;
		case slice1: adc=parentSlice->parentTile->slices[0].adc; break;
		case slice2: adc=parentSlice->adc; break;
		case slice3: adc=parentSlice->parentTile->slices[2].adc; break;
	}

	unsigned char adcRead = adc->readHelper();
	Serial.print("adcRead = ");
	Serial.println(adcRead);

	float target = 2.0;
	if ( adcRead < target ) {
		return binarySearch (minGainCode, minBest, finalGainCode, fabs(adcRead-target), finalGainCode);
	} else {
		return binarySearch (finalGainCode, fabs(adcRead-target), maxGainCode, maxBest, finalGainCode);
	}

}