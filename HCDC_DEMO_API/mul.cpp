#include "HCDC_DEMO_API.h"
#include <float.h>

void Fabric::Chip::Tile::Slice::Multiplier::setEnable (
	bool enable
) {
	this->enable = enable;
	setParam0 ();
	/*establish calibration codes*/
	setParam1 ();
	setParam3 ();
	setParam4 ();
	setParam5 ();
}

void Fabric::Chip::Tile::Slice::Multiplier::setVga (
	bool vga // constant coefficient multiplier mode
) {
	this->vga = vga;
	setParam1 ();
}

void Fabric::Chip::Tile::Slice::Multiplier::setGainCode (
	unsigned char gain // fixed point representation of desired gain
) {
	Serial.println("setGainCode");
	Serial.println(gain);
	setVga (true);
	this->gain = gain;
	setParam2 ();
}

void Fabric::Chip::Tile::Slice::Multiplier::setGain (
	float gain // floating point representation of desired gain
	// -100.0 to 100.0 are valid
) {
	if (fabs(gain) < 0.01) { // enable x0.01 gain using input & output attenuation
		out0->setRange (
			true, // 0.2uA mode
			false // 20 uA mode
		);
		in0->setRange (
			false, // 0.2uA mode
			true // 20 uA mode
		);
		setGainCode ( (gain*10.0)*128.0 + 128.0 );
	} else if (fabs(gain) < 0.1) { // enable x0.1 gain using input attenuation
		in0->setRange (
			false, // 0.2uA mode
			true // 20 uA mode
		);
		setGainCode ( (gain*10.0)*128.0 + 128.0 );
	} else if (fabs(gain) < 1.0) { // regular mode
		out0->setRange (
			false, // 0.2uA mode
			false // 20 uA mode
		);
		setGainCode ( gain*128.0 + 128.0 );
	} else if (fabs(gain) < 10.0) { // enable x10 gain using output gain
		out0->setRange (
			false, // 0.2uA mode
			true // 20 uA mode
		);
		setGainCode ( (gain/10.0)*128.0 + 128.0 );
	} else if (fabs(gain) < 100.0) { // enable x100 gain using both input & output gain
		out0->setRange (
			false, // 0.2uA mode
			true // 20 uA mode
		);
		in0->setRange (
			true, // 0.2uA mode
			false // 20 uA mode
		);
		setGainCode ( (gain/100.0)*128.0 + 128.0 );
	} else error ("VGA gain must be between -100.0 and 100.0");
}

void Fabric::Chip::Tile::Slice::Multiplier::MultiplierInterface::setRange (
	bool loRange, // 0.2uA mode
	bool hiRange // 20 uA mode
	// default is 2uA mode
	// this setting should match the unit that gives the input to the multiplier
) {
	if (loRange&&hiRange) error ("MUL low and high range cannot be selected at the same time");
	this->loRange = loRange;
	this->hiRange = hiRange;
	parentFu->setParam0 ();
	parentFu->setParam3 ();
	parentFu->setParam4 ();
	parentFu->setParam5 ();
}

Fabric::Chip::Tile::Slice::Multiplier::Multiplier (
	Chip::Tile::Slice * parentSlice,
	unit unitId
) :
	FunctionUnit(parentSlice, unitId)
{
	out0 = new MultiplierInterface (this, out0Id);
	tally_dyn_mem <MultiplierInterface> ("MultiplierInterface");
	in0 = new MultiplierInterface (this, in0Id);
	tally_dyn_mem <MultiplierInterface> ("MultiplierInterface");
	in1 = new MultiplierInterface (this, in1Id);
	tally_dyn_mem <MultiplierInterface> ("MultiplierInterface");
	setAnaIrefDacNmos( false, false );
	setAnaIrefPmos();
}

/*Set enable, input 1 range, input 2 range, output range*/
void Fabric::Chip::Tile::Slice::Multiplier::setParam0 () {
	unsigned char cfgTile = 0;
	cfgTile += enable ? 1<<7 : 0;
	cfgTile += (in0->loRange ? mulLo : (in0->hiRange ? mulHi : mulMid))<<4;
	cfgTile += (in1->loRange ? mulLo : (in1->hiRange ? mulHi : mulMid))<<2;
	cfgTile += (out0->loRange ? mulLo : (out0->hiRange ? mulHi : mulMid))<<0;
	setParamHelper (0, cfgTile);
}

/*Set calDac, enable variable gain amplifer mode*/
void Fabric::Chip::Tile::Slice::Multiplier::setParam1 () {
	if (midNegGainCode<0||63<midNegGainCode) error ("midNegGainCode out of bounds");
	unsigned char cfgTile = 0;
	cfgTile += midNegGainCode<<2;
	cfgTile += vga ? 1<<1 : 0;
	setParamHelper (1, cfgTile);
}

/*Set gain if VGA mode*/
void Fabric::Chip::Tile::Slice::Multiplier::setParam2 () {
	if (gain<0||255<gain) error ("gain out of bounds");
	setParamHelper (2, gain);
}

/*Set calOutOs*/
void Fabric::Chip::Tile::Slice::Multiplier::setParam3 () {
	unsigned char calOutOs = out0->loRange ? out0->loOffsetCode : (out0->hiRange ? out0->hiOffsetCode : out0->midOffsetCode);
	if (calOutOs<0||63<calOutOs) error ("calOutOs out of bounds");
	unsigned char cfgTile = calOutOs<<2;
	setParamHelper (3, cfgTile);
}

/*Set calInOs1*/
void Fabric::Chip::Tile::Slice::Multiplier::setParam4 () {
	unsigned char calInOs1 = in0->loRange ? in0->loOffsetCode : (in0->hiRange ? in0->hiOffsetCode : in0->midOffsetCode);
	if (calInOs1<0||63<calInOs1) error ("calInOs1 out of bounds");
	unsigned char cfgTile = calInOs1<<2;
	setParamHelper (4, cfgTile);
}

/*Set calInOs2*/
void Fabric::Chip::Tile::Slice::Multiplier::setParam5 () {
	unsigned char calInOs2 = in1->loRange ? in1->loOffsetCode : (in1->hiRange ? in1->hiOffsetCode : in1->midOffsetCode);
	if (calInOs2<0||63<calInOs2) error ("calInOs2 out of bounds");
	unsigned char cfgTile = calInOs2<<2;
	setParamHelper (5, cfgTile);
}

void Fabric::Chip::Tile::Slice::Multiplier::setParamHelper (
	unsigned char selLine,
	unsigned char cfgTile
) {
	if (selLine<0||5<selLine) error ("selLine out of bounds");

	/*DETERMINE SEL_ROW*/
	unsigned char selRow;
	switch (parentSlice->sliceId) {
		case slice0: selRow = 2; break;
		case slice1: selRow = 3; break;
		case slice2: selRow = 4; break;
		case slice3: selRow = 5; break;
		default: error ("invalid slice. Only slices 0 through 3 have MULs"); break;
	}

	/*DETERMINE SEL_COL*/
	unsigned char selCol;
	switch (unitId) {
		case unitMulL: selCol = 3; break;
		case unitMulR: selCol = 4; break;
		default: error ("invalid unit. Only unitMulL and unitMulR are MULs"); break;
	}

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

bool Fabric::Chip::Tile::Slice::Multiplier::calibrate () {

	setEnable(true);
	Chip::Connection conn0 = Fabric::Chip::Connection ( out0, parentSlice->tileOuts[0].in0 );
	conn0.setConn();
	Chip::Connection conn1 = Fabric::Chip::Connection ( parentSlice->tileOuts[0].out0, parentSlice->parentTile->parentChip->tiles[3].slices[3].chipOutput->in0 );
	conn1.setConn();

	bool biasStable = false;
	while (!biasStable) {
		Serial.println("\nCalibrate output");
		out0->calibrate();
		Serial.println("\nCalibrate input 0");
		in0->calibrate();
		Serial.println("\nCalibrate input 1");
		in1->calibrate();
		/*calibrate VGA gain to negative full scale*/
		biasStable = findBias(midNegGainCode);
	}

	/*teardown*/
	setEnable(false);
	conn0.brkConn();
	conn1.brkConn();
	parentSlice->parentTile->parentChip->parentFabric->cfgCommit();

	return biasStable;
}

void Fabric::Chip::Tile::Slice::Multiplier::MultiplierInterface::calibrate () {

	/*setup*/
	/*the multiplier is used as a VGA during calibration of the output and the first input*/
	/*Set calDac, enable variable gain amplifer mode*/
	/*when calibrating the output offset, the zero value of the VGA, which has a very small offset, is used to provide a reference zero*/
	/*the VGA has to be some large value when calibrating the first input, which is calibrated by tuning the first input to zero*/
	/*Set gain if VGA mode*/
	if (ifcId==out0Id || ifcId==in0Id) {
		parentMultiplier->setGainCode((ifcId==in0Id) ? 255 : 128);
	}

	/*if calibrating the input offset of second input, feed an input to the MUL first input*/
	Chip::Connection conn = Fabric::Chip::Connection ( parentFu->parentSlice->dac->out0, parentFu->in0 );
	if (ifcId==in1Id) {
		conn.setConn();
		parentFu->parentSlice->dac->setConstantCode (255);
	}

	// setRange(true, false);
	// binarySearch ( 0, 63, 7, loOffsetCode );
	// if ( loOffsetCode<2 || loOffsetCode>61 ) error ("MUL offset failure");
	setRange(false, false);
	binarySearch ( 0, FLT_MAX, 63, FLT_MAX, midOffsetCode );
	if ( midOffsetCode<2 || midOffsetCode>61 ) error ("MUL offset failure");
	setRange(false, true);
	binarySearch ( 0, FLT_MAX, 63, FLT_MAX, hiOffsetCode );
	if ( hiOffsetCode<2 || hiOffsetCode>61 ) error ("MUL offset failure");

	/*teardown*/
	parentMultiplier->setVga(false);
	conn.brkConn();
	setRange(false, false);
	parentFu->parentSlice->dac->setEnable(false);

}

void Fabric::Chip::Tile::Slice::Multiplier::MultiplierInterface::binarySearch (
	unsigned char minOffsetCode,
	float minBest,
	unsigned char maxOffsetCode,
	float maxBest,
	unsigned char & finalOffsetCode
) {
	if (binarySearchAvg (minOffsetCode, minBest, maxOffsetCode, maxBest, finalOffsetCode)) return;

	parentFu->setParam3 ();
	parentFu->setParam4 ();
	parentFu->setParam5 ();
	parentFu->parentSlice->parentTile->parentChip->parentFabric->cfgCommit();

	float voltageDiff = binarySearchMeas ();

	float target = 0.0;
	if (voltageDiff > target) {
		return binarySearch ( minOffsetCode, minBest, finalOffsetCode, fabs(voltageDiff-target), finalOffsetCode);
	} else {
		return binarySearch ( finalOffsetCode, fabs(voltageDiff-target), maxOffsetCode, maxBest, finalOffsetCode);
	}
}

bool Fabric::Chip::Tile::Slice::Multiplier::findBias (
	unsigned char & gainCode
) {
	Serial.print("\nMultiplier gain calibration ");

	parentSlice->dac->setConstantCode(0);
	Chip::Connection conn = Fabric::Chip::Connection ( parentSlice->dac->out0, in0 );
	conn.setConn();
	setGainCode(0);

	bool biasStable = findBiasHelper (gainCode);

	conn.brkConn();
	setVga (false);

	return biasStable;
}

void Fabric::Chip::Tile::Slice::Multiplier::binarySearch (
	unsigned char minGainCode,
	float minBest,
	unsigned char maxGainCode,
	float maxBest,
	unsigned char & finalGainCode
) {
	if (binarySearchAvg (minGainCode, minBest, maxGainCode, maxBest, finalGainCode)) return;

	setVga (true);
	parentSlice->parentTile->parentChip->parentFabric->cfgCommit();

	float voltageDiff = binarySearchMeas ();

	float target = - (parentSlice->dac->midNegTarget/*+parentSlice->dac->out0->midOffset*/);
    if (voltageDiff > target) {
		return binarySearch ( minGainCode, minBest, finalGainCode, fabs(voltageDiff-target), finalGainCode );
	} else {
		return binarySearch ( finalGainCode, fabs(voltageDiff-target), maxGainCode, maxBest, finalGainCode );
	}
}

void Fabric::Chip::Tile::Slice::Multiplier::setAnaIrefDacNmos (
	bool decrement,
	bool increment
) {
	setAnaIrefDacNmosHelper (decrement, increment);

	unsigned char selRow;
	unsigned char selCol;
	unsigned char selLine;
	switch (unitId) {
		case unitMulL: switch (parentSlice->sliceId) {
			case slice0: selRow=1; selCol=2; selLine=1; break;
			case slice1: selRow=0; selCol=3; selLine=0; break;
			case slice2: selRow=1; selCol=2; selLine=0; break;
			case slice3: selRow=0; selCol=3; selLine=1; break;
			default: error ("MUL invalid slice"); break;
		} break;
		case unitMulR: switch (parentSlice->sliceId) {
			case slice0: selRow=1; selCol=2; selLine=3; break;
			case slice1: selRow=0; selCol=3; selLine=2; break;
			case slice2: selRow=1; selCol=2; selLine=2; break;
			case slice3: selRow=0; selCol=3; selLine=3; break;
			default: error ("MUL invalid slice"); break;
		} break;
		default: error ("MUL invalid unitId"); break;
	}
	unsigned char cfgTile = endian(parentSlice->parentTile->parentChip->cfgBuf[parentSlice->parentTile->tileRowId][parentSlice->parentTile->tileColId][selRow][selCol][selLine]);
	switch (unitId) {
		case unitMulL: switch (parentSlice->sliceId) {
			case slice0: cfgTile = (cfgTile & 0b00000111) + ((anaIrefDacNmos<<3) & 0b00111000); break;
			case slice1: cfgTile = (cfgTile & 0b00111000) + (anaIrefDacNmos & 0b00000111); break;
			case slice2: cfgTile = (cfgTile & 0b00000111) + ((anaIrefDacNmos<<3) & 0b00111000); break;
			case slice3: cfgTile = (cfgTile & 0b00111000) + (anaIrefDacNmos & 0b00000111); break;
			default: error ("MUL invalid slice"); break;
		} break;
		case unitMulR: switch (parentSlice->sliceId) {
			case slice0: cfgTile = (cfgTile & 0b00111000) + (anaIrefDacNmos & 0b00000111); break;
			case slice1: cfgTile = (cfgTile & 0b00111000) + (anaIrefDacNmos & 0b00000111); break;
			case slice2: cfgTile = (cfgTile & 0b00111000) + (anaIrefDacNmos & 0b00000111); break;
			case slice3: cfgTile = (cfgTile & 0b00111000) + (anaIrefDacNmos & 0b00000111); break;
			default: error ("MUL invalid slice"); break;
		} break;
		default: error ("MUL invalid unitId"); break;
	}

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

void Fabric::Chip::Tile::Slice::Multiplier::setAnaIrefPmos () {

	unsigned char selRow=0;
	unsigned char selCol=4;
	unsigned char selLine;
	switch (unitId) {
		case unitMulL: switch (parentSlice->sliceId) {
			case slice0: selLine=2; break;
			case slice1: selLine=5; break;
			case slice2: selLine=1; break;
			case slice3: selLine=0; break;
			default: error ("MUL invalid slice"); break;
		} break;
		case unitMulR: switch (parentSlice->sliceId) {
			case slice0: selLine=0; break;
			case slice1: selLine=1; break;
			case slice2: selLine=2; break;
			case slice3: selLine=3; break;
			default: error ("MUL invalid slice"); break;
		} break;
		default: error ("MUL invalid unitId"); break;
	}
	unsigned char cfgTile = endian(parentSlice->parentTile->parentChip->cfgBuf[parentSlice->parentTile->tileRowId][parentSlice->parentTile->tileColId][selRow][selCol][selLine]);
	switch (unitId) {
		case unitMulL: switch (parentSlice->sliceId) {
			case slice0: cfgTile = (cfgTile & 0b00111000) + (anaIrefPmos & 0b00000111); break;
			case slice1: cfgTile = (cfgTile & 0b00111000) + (anaIrefPmos & 0b00000111); break;
			case slice2: cfgTile = (cfgTile & 0b00111000) + (anaIrefPmos & 0b00000111); break;
			case slice3: cfgTile = (cfgTile & 0b00000111) + ((anaIrefPmos<<3) & 0b00111000); break;
			default: error ("MUL invalid slice"); break;
		} break;
		case unitMulR: switch (parentSlice->sliceId) {
			case slice0: cfgTile = (cfgTile & 0b00111000) + (anaIrefPmos & 0b00000111); break;
			case slice1: cfgTile = (cfgTile & 0b00000111) + ((anaIrefPmos<<3) & 0b00111000); break;
			case slice2: cfgTile = (cfgTile & 0b00000111) + ((anaIrefPmos<<3) & 0b00111000); break;
			case slice3: cfgTile = (cfgTile & 0b00000111) + ((anaIrefPmos<<3) & 0b00111000); break;
			default: error ("MUL invalid slice"); break;
		} break;
		default: error ("MUL invalid unitId"); break;
	}

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