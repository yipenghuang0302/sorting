#include "HCDC_DEMO_API.h"

void Fabric::Chip::Tile::Slice::Adc::setEnable (
	bool enable
) {
	this->enable = enable;
	setParam0 ();
	setParam1 ();
	setParam2 ();
	setParam3 ();
}

void Fabric::Chip::Tile::Slice::Adc::AdcIn::setRange (
	bool hiRange // 20uA mode
	// this setting should match the unit that gives the input to the analog to digital converter
) {
	this->hiRange = hiRange;
	parentFu->setParam0 ();
}

Fabric::Chip::Tile::Slice::Adc::Adc (
	Chip::Tile::Slice * parentSlice
) :
	FunctionUnit(parentSlice, unitAdc)
{
	in0 = new AdcIn (this);
	tally_dyn_mem <AdcIn> ("AdcIn");
	setAnaIrefDacNmos( false, false );
	setAnaIrefPmos();
}

/*Set enable, range, delay, decRst*/
void Fabric::Chip::Tile::Slice::Adc::setParam0 () {
	unsigned char cfgTile = 0;
	cfgTile += enable ? 1<<7 : 0;
	cfgTile += (in0->hiRange ? adcHi : adcMid) ? 1<<5 : 0;
	cfgTile += ns11_5<<3;
	cfgTile += false ? 1<<2 : 0;
	cfgTile += (ns3==ns6) ? 1 : 0;
	setParamHelper (0, cfgTile);
}

/*Set calibration enable, calCompUpperEn, calI2V*/
void Fabric::Chip::Tile::Slice::Adc::setParam1 () {
	if (calI2V<0||63<calI2V) error ("calI2V out of bounds");
	unsigned char cfgTile = 0;
	cfgTile += false ? 1<<7 : 0;
	cfgTile += false ? 1<<6 : 0;
	cfgTile += calI2V<<0;
	setParamHelper (1, cfgTile);
}

/*Set calCompLower, calCompLowerFs*/
void Fabric::Chip::Tile::Slice::Adc::setParam2 () {
	if (calCompLower<0||63<calCompLower) error ("calCompLower out of bounds");
	unsigned char cfgTile = 0;
	cfgTile += calCompLower<<2;
	cfgTile += calCompLowerFs<<0;
	setParamHelper (2, cfgTile);
}

/*Set calCompUpper, calCompUpperFs*/
void Fabric::Chip::Tile::Slice::Adc::setParam3 () {
	if (calCompUpper<0||63<calCompUpper) error ("calCompUpper out of bounds");
	unsigned char cfgTile = 0;
	cfgTile += calCompUpper<<2;
	cfgTile += calCompUpperFs<<0;
	setParamHelper (3, cfgTile);
}

/*Set testEn, testAdc, testIv, testRs, testRsInc*/
void Fabric::Chip::Tile::Slice::Adc::setParam4 (
	bool testEn, /*Configure the entire block in testing mode so that I2V and A/D can be tested individually*/
	bool testAdc, /*Testing the ADC individually.*/
	bool testIv, /*Testing the I2V individually.*/
	bool testRs, /*Testing the rstring individually.*/
	bool testRsInc /*Configure the counter for upward or downward increments during set up for testing R-string separately (w/ cfgCalEN=1)*/
) {
	unsigned char cfgTile = 0;
	cfgTile += testEn ? 1<<7 : 0;
	cfgTile += testAdc ? 1<<6 : 0;
	cfgTile += testIv ? 1<<5 : 0;
	cfgTile += testRs ? 1<<4 : 0;
	cfgTile += testRsInc ? 1<<3 : 0;
	setParamHelper (4, cfgTile);
}

void Fabric::Chip::Tile::Slice::Adc::setParamHelper (
	unsigned char selLine,
	unsigned char cfgTile
) {
	if (selLine<0||4<selLine) error ("selLine out of bounds");

	/*DETERMINE SEL_COL*/
	unsigned char selCol;
	switch (parentSlice->sliceId) {
		case slice0: selCol = 1; break;
		case slice2: selCol = 2; break;
		default: error ("setParamHelper invalid slice. Only even slices have ADCs"); break;
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

void Fabric::Chip::Tile::Slice::Adc::setAnaIrefDacNmos (
	bool decrement,
	bool increment
) {
	// anaIrefI2V mapped to anaIrefDacNmos
	setAnaIrefDacNmosHelper (decrement, increment);

	unsigned char selRow=0;
	unsigned char selCol=3;
	unsigned char selLine;
	switch (parentSlice->sliceId) {
		case slice0: selLine=0; break;
		case slice2: selLine=4; break;
		default: error ("ADC invalid slice"); break;
	}
	unsigned char cfgTile = endian(
		parentSlice->parentTile->parentChip->cfgBuf
		[parentSlice->parentTile->tileRowId]
		[parentSlice->parentTile->tileColId]
		[selRow]
		[selCol]
		[selLine]
	);
	cfgTile = (cfgTile & 0b00000111) + ((anaIrefDacNmos<<3) & 0b00111000);

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

void Fabric::Chip::Tile::Slice::Adc::setAnaIrefPmos () {
	// anaIref1Pmos
	unsigned char selRow=0;
	unsigned char selCol=3;
	unsigned char selLine;
	switch (parentSlice->sliceId) {
		case slice0: selLine=1; break;
		case slice2: selLine=3; break;
		default: error ("ADC invalid slice"); break;
	}
	unsigned char cfgTile = endian(parentSlice->parentTile->parentChip->cfgBuf[parentSlice->parentTile->tileRowId][parentSlice->parentTile->tileColId][selRow][selCol][selLine]);
	cfgTile = (cfgTile & 0b00000111) + ((anaIref1Pmos<<3) & 0b00111000);

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

	// anaIref2Pmos
	selRow=0;
	selCol=3;
	selLine;
	switch (parentSlice->sliceId) {
		case slice0: selLine=2; break;
		case slice2: selLine=5; break;
		default: error ("ADC invalid slice"); break;
	}
	cfgTile = endian(parentSlice->parentTile->parentChip->cfgBuf[parentSlice->parentTile->tileRowId][parentSlice->parentTile->tileColId][selRow][selCol][selLine]);

	switch (parentSlice->sliceId) {
		case slice0: cfgTile = (cfgTile & 0b00000111) + ((anaIref2Pmos<<3) & 0b00111000);break;
		case slice2: cfgTile = (cfgTile & 0b00111000) + (anaIref2Pmos & 0b00000111);break;
		default: error ("ADC invalid slice"); break;
	}

	vec = Vector (
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

bool Fabric::Chip::Tile::Slice::Adc::calibrate () {

	Chip::Connection conn0 = Fabric::Chip::Connection ( parentSlice->dac->out0, in0 );
	conn0.setConn();
	setEnable (true);
	parentSlice->parentTile->parentChip->parentFabric->serialOutReq();

	if (!findCalCompFs()) return false;
	Serial.print("calCompLower "); Serial.print(calCompLower);
	Serial.print(" calCompUpper "); Serial.println(calCompUpper);
	Serial.println("fullscale and spread and posneg settings found");

	conn0.brkConn();
	parentSlice->dac->setEnable(false);

	// once fullscale and spread and posneg settings found

	// find I2V offset code
	// parentSlice->dac->setConstantCode(128);
	// Fabric::Chip::Connection conn1 = Fabric::Chip::Connection ( parentSlice->dac->out0, in0 );
	// conn1.setConn();

	in0->findBias ( calI2V );

	// parentSlice->dac->setEnable(false);
	// conn1.brkConn();
	setEnable (false);
	parentSlice->parentTile->parentChip->parentFabric->serialOutStop();
	parentSlice->parentTile->parentChip->parentFabric->cfgCommit();
	Serial.println("offset settings found");
	return true;
}

bool Fabric::Chip::Tile::Slice::Adc::findCalCompFs () {

	Serial.println("nA100");
	calCompLowerFs = nA100;
	calCompUpperFs = nA100;
	if (checkScale()) return true;
	Serial.println("nA200");
	calCompLowerFs = nA200;
	calCompUpperFs = nA200;
	if (checkScale()) return true;
	Serial.println("nA300");
	calCompLowerFs = nA300;
	calCompUpperFs = nA300;
	if (checkScale()) return true;
	Serial.println("nA400");
	calCompLowerFs = nA400;
	calCompUpperFs = nA400;
	if (checkScale()) return true;

	return false;
}

bool Fabric::Chip::Tile::Slice::Adc::checkScale () {
	for (unsigned char spread=0; spread<32; spread++) {
		Serial.print("spread = "); Serial.println(spread);
		if ( checkSpread(spread,false,false) ) return true;
		if ( checkSpread(spread,false,true) ) return true;
		if ( checkSpread(spread,true,false) ) return true;
		if ( checkSpread(spread,true,true) ) return true;
	}
	return false;
}

bool Fabric::Chip::Tile::Slice::Adc::checkSpread (
	unsigned char spread,
	bool lowerPos,
	bool upperPos
) {
	calCompLower = lowerPos ? 31+spread : 31-spread;
	setParam2();
	calCompUpper = upperPos ? 31+spread : 31-spread;
	setParam3();

	bool success=true;
	// see if adc code is steady at dac=128
	success &= checkSteady(128);
	// see if adc code is steady at dac=4
	success &= checkSteady(2);
	// see if adc code is steady at dac=251
	success &= checkSteady(253);

	return success;
}

bool Fabric::Chip::Tile::Slice::Adc::checkSteady (
	unsigned char dacCode
) {
	parentSlice->dac->setConstantCode (dacCode);
	parentSlice->parentTile->parentChip->parentFabric->cfgCommit();
	bool success=true;
	unsigned char adcPrev = readHelper();
	for (unsigned char rep=0; success&&rep<16; rep++)
		success &= adcPrev==readHelper();
	return success;
}

unsigned char Fabric::Chip::Tile::Slice::Adc::readHelper () {
	unsigned char adcData0, adcData1;
	bool done;
	parentSlice->parentTile->readSerial ( adcData0, adcData1, done );
	unsigned char result = (parentSlice->sliceId==slice0) ? adcData0 : adcData1;
	// Serial.print(" ");Serial.println(result);
	return result;
}

bool Fabric::Chip::Tile::Slice::Adc::AdcIn::findBias (
	unsigned char & offsetCode
) {
	Serial.print("Adc offset calibration ");

	bool biasStable = false;
	while (!biasStable) {
		biasStable = findBiasHelper (offsetCode);
	}

	return biasStable;
}

void Fabric::Chip::Tile::Slice::Adc::AdcIn::binarySearch (
	unsigned char minI2VCode,
	float minBest,
	unsigned char maxI2VCode,
	float maxBest,
	unsigned char & finalI2VCode
) {

	if (binarySearchAvg (minI2VCode, minBest, maxI2VCode, maxBest, finalI2VCode)) return;

	parentFu->setParam1 ();
	parentFu->parentSlice->parentTile->parentChip->parentFabric->cfgCommit();

	unsigned char adcRead = parentAdc->readHelper();
	Serial.print("finalI2VCode = ");
	Serial.println(finalI2VCode);
	Serial.print("adcRead = ");
	Serial.println(adcRead);
	float target = 128.0;
	if (adcRead < target) {
		return binarySearch (minI2VCode, minBest, finalI2VCode, fabs(adcRead-target), finalI2VCode);
	} else {
		return binarySearch (finalI2VCode, fabs(adcRead-target), maxI2VCode, maxBest, finalI2VCode);
	}

}