#include "HCDC_DEMO_API.h"
#include <float.h>

void Fabric::Chip::Tile::Slice::Integrator::setEnable (
	bool enable
) {
	this->enable = enable;
	setParam0 ();
	setParam1 ();
	setParam3 ();
	setParam4 ();
}

void Fabric::Chip::Tile::Slice::Integrator::IntegratorOut::setInv (
	bool inverse // whether output is negated
) {
	this->inverse = inverse;
	parentFu->setEnable (
		parentFu->enable
	);
}

void Fabric::Chip::Tile::Slice::Integrator::IntegratorOut::setRange (
	bool loRange, // 0.2uA mode
	bool hiRange // 20 uA mode
	// not both of the range settings should be true
	// default is 2uA mode
) {
	/*check*/
	if (loRange&&hiRange) {
		error ("INT low and high range cannot be selected at the same time");
	}
	this->loRange = loRange;
	this->hiRange = hiRange;
	if (loRange) {
		if (parentFu->in0->loRange) {
			parentIntegrator->loGain = false;
			parentIntegrator->hiGain = false;
		} else if (parentFu->in0->hiRange) {
			error ("cannot set integrator output loRange when input hiRange");
		} else {
			parentIntegrator->loGain = true;
			parentIntegrator->hiGain = false;
		}
	} else if (hiRange) {
		if (parentFu->in0->loRange) {
			error ("cannot set integrator output hiRange when input loRange");
		} else if (parentFu->in0->hiRange) {
			parentIntegrator->loGain = false;
			parentIntegrator->hiGain = false;
		} else {
			parentIntegrator->loGain = false;
			parentIntegrator->hiGain = true;
		}
	} else {
		if (parentFu->in0->loRange) {
			parentIntegrator->loGain = false;
			parentIntegrator->hiGain = true;
		} else if (parentFu->in0->hiRange) {
			parentIntegrator->loGain = true;
			parentIntegrator->hiGain = false;
		} else {
			parentIntegrator->loGain = false;
			parentIntegrator->hiGain = false;
		}
	}
	parentFu->setEnable (
		parentFu->enable
	);
}

void Fabric::Chip::Tile::Slice::Integrator::IntegratorIn::setRange (
	bool loRange, // 0.2uA mode
	bool hiRange // 20 uA mode
	// not both of the range settings should be true
	// default is 2uA mode
) {
	/*check*/
	if (loRange&&hiRange) {
		error ("INT low and high range cannot be selected at the same time");
	}
	this->loRange = loRange;
	this->hiRange = hiRange;
	if (loRange) {
		if (parentFu->out0->loRange) {
			parentIntegrator->loGain = false;
			parentIntegrator->hiGain = false;
		} else if (parentFu->out0->hiRange) {
			error ("cannot set integrator input loRange when output hiRange");
		} else {
			parentIntegrator->loGain = false;
			parentIntegrator->hiGain = true;
		}
	} else if (hiRange) {
		if (parentFu->out0->loRange) {
			error ("cannot set integrator input hiRange when output loRange");
		} else if (parentFu->out0->hiRange) {
			parentIntegrator->loGain = false;
			parentIntegrator->hiGain = false;
		} else {
			parentIntegrator->loGain = true;
			parentIntegrator->hiGain = false;
		}
	} else {
		if (parentFu->out0->loRange) {
			parentIntegrator->loGain = true;
			parentIntegrator->hiGain = false;
		} else if (parentFu->out0->hiRange) {
			parentIntegrator->loGain = false;
			parentIntegrator->hiGain = true;
		} else {
			parentIntegrator->loGain = false;
			parentIntegrator->hiGain = false;
		}
	}
	parentFu->setEnable (
		parentFu->enable
	);
}

void Fabric::Chip::Tile::Slice::Integrator::setInitial (
	unsigned char initialCondition // fixed point representation of initial condition
) {
	this->initialCondition = initialCondition;
	setParam2 ();
}

void Fabric::Chip::Tile::Slice::Integrator::setException (
	bool exception // turn on overflow detection
	// turning false overflow detection saves power if it is known to be unnecessary
) {
	this->exception = exception;
	setParam1 ();
}

Fabric::Chip::Tile::Slice::Integrator::Integrator (
	Chip::Tile::Slice * parentSlice
) :
	FunctionUnit(parentSlice, unitInt)
{
	in0 = new IntegratorIn (this);
	tally_dyn_mem <IntegratorIn> ("IntegratorIn");
	out0 = new IntegratorOut (this);
	tally_dyn_mem <IntegratorOut> ("IntegratorOut");
	setAnaIrefDacNmos( false, false );
	setAnaIrefPmos();
}

/*Set enable, invert, range*/
void Fabric::Chip::Tile::Slice::Integrator::setParam0 () {
	intRange intRange;
	if (in0->loRange) {
		if (loGain) {
			error ("INT low range low gain setting unavailable");
		}
		else if (hiGain) intRange = lRngHGain;
		else intRange = lRngMGain;
	} else if (in0->hiRange) {
		if (loGain) intRange = hRngLGain;
		else if (hiGain) {
			error ("INT high range high gain setting unavailable");
		}
		else intRange = hRngMGain;
	} else {
		if (loGain) intRange = mRngLGain;
		else if (hiGain) intRange = mRngHGain;
		else intRange = mRngMGain;
	}

	unsigned char cfgTile = 0;
	cfgTile += enable ? 1<<7 : 0;
	cfgTile += (out0->inverse) ? 1<<6 : 0;
	cfgTile += intRange<<3;
	setParamHelper (0, cfgTile);
}

/*Set calIc, overflow enable*/
void Fabric::Chip::Tile::Slice::Integrator::setParam1 () {
	unsigned char cfgCalIc = loGain ? loNegGainCode : ( hiGain ? hiNegGainCode : midNegGainCode );
	if (cfgCalIc<0||63<cfgCalIc) error ("cfgCalIc out of bounds");
	unsigned char cfgTile = 0;
	cfgTile += cfgCalIc<<2;
	cfgTile += (exception) ? 1<<1 : 0;
	setParamHelper (1, cfgTile);
}

/*Set initial condition*/
void Fabric::Chip::Tile::Slice::Integrator::setParam2 () {
	setParamHelper (2, initialCondition);
}

/*Set calOutOs, calOutEn*/
void Fabric::Chip::Tile::Slice::Integrator::setParam3 () {
	unsigned char calOutOs = out0->loRange ? out0->loOffsetCode : (out0->hiRange ? out0->hiOffsetCode : out0->midOffsetCode);
	if (calOutOs<0||63<calOutOs) error ("calOutOs out of bounds");
	unsigned char cfgTile = 0;
	cfgTile += calOutOs<<2;
	cfgTile += (out0->calEn) ? 1<<1 : 0;
	setParamHelper (3, cfgTile);
}

/*Set calInOs, calInEn*/
void Fabric::Chip::Tile::Slice::Integrator::setParam4 () {
	unsigned char calInOs = in0->loRange ? in0->loOffsetCode : (in0->hiRange ? in0->hiOffsetCode : in0->midOffsetCode);
	if (calInOs<0||63<calInOs) error ("calInOs out of bounds");
	unsigned char cfgTile = 0;
	cfgTile += calInOs<<2;
	cfgTile += (in0->calEn) ? 1<<1 : 0;
	setParamHelper (4, cfgTile);
}

/*Helper function*/
void Fabric::Chip::Tile::Slice::Integrator::setParamHelper (
	unsigned char selLine,
	unsigned char cfgTile
) {
	if (selLine<0||4<selLine) error ("selLine out of bounds");

	/*DETERMINE SEL_ROW*/
	unsigned char selRow;
	switch (parentSlice->sliceId) {
		case slice0: selRow = 2; break;
		case slice1: selRow = 3; break;
		case slice2: selRow = 4; break;
		case slice3: selRow = 5; break;
		default: error ("invalid slice. Only slices 0 through 3 have INTs"); break;
	}

	Chip::Vector vec = Vector (
		*this,
		selRow,
		2,
		selLine,
		endian (cfgTile)
	);

	parentSlice->parentTile->parentChip->cacheVec (
		vec
	);
}


bool Fabric::Chip::Tile::Slice::Integrator::calibrate () {
	Serial.println("\nIntegrator Calibration");

	setEnable(true);
	setInitial(128);
	Chip::Connection conn0 = Fabric::Chip::Connection ( out0, parentSlice->tileOuts[0].in0 );
	conn0.setConn();
	Chip::Connection conn1 = Fabric::Chip::Connection ( parentSlice->tileOuts[0].out0, parentSlice->parentTile->parentChip->tiles[3].slices[3].chipOutput->in0 );
	conn1.setConn();
	setParam1();

	out0->calibrate();
	in0->calibrate();

	bool biasStable = false;
	while (!biasStable) {
		biasStable = findBias(midNegGainCode);
	}

	conn0.brkConn();
	conn1.brkConn();
	setEnable(false);
	parentSlice->parentTile->parentChip->parentFabric->cfgCommit();

	return biasStable;
}

void Fabric::Chip::Tile::Slice::Integrator::IntegratorInterface::calibrate () {
	Serial.print("\nIntegrator interface calibration");
	calEn = true;
	// setRange(true, false);
	// binarySearch ( 0, 63, 7, loOffsetCode );
	// if ( loOffsetCode<1 || loOffsetCode>62 ) error ("INT offset failure");
	// setRange(false, true);
	// binarySearch ( 0, 63, 7, hiOffsetCode );
	// if ( hiOffsetCode<1 || hiOffsetCode>62 ) error ("INT offset failure");
	setRange(false, false);
	binarySearch ( 0, FLT_MAX, 63, FLT_MAX, midOffsetCode );
	if ( midOffsetCode<1 || midOffsetCode>62 ) error ("INT offset failure");
	calEn = false;
}

void Fabric::Chip::Tile::Slice::Integrator::IntegratorInterface::binarySearch (
	unsigned char minOffsetCode,
	float minBest,
	unsigned char maxOffsetCode,
	float maxBest,
	unsigned char & finalOffsetCode
) {
	if (binarySearchAvg (minOffsetCode, minBest, maxOffsetCode, maxBest, finalOffsetCode)) return;

	parentFu->setEnable(true);
	parentFu->parentSlice->parentTile->parentChip->parentFabric->cfgCommit();

	if (ifcId==in0Id) parentFu->parentSlice->parentTile->parentChip->parentFabric->execStart();
	float voltageDiff = binarySearchMeas();
	if (ifcId==in0Id) parentFu->parentSlice->parentTile->parentChip->parentFabric->execStop();

	float target = 0.0;
	if (voltageDiff > target) {
		return binarySearch ( minOffsetCode, minBest, finalOffsetCode, fabs(voltageDiff-target), finalOffsetCode);
	} else {
		return binarySearch ( finalOffsetCode, fabs(voltageDiff-target), maxOffsetCode, maxBest, finalOffsetCode);
	}
}

bool Fabric::Chip::Tile::Slice::Integrator::findBias (
	unsigned char & gainCode
) {
	Serial.print("\nIntegrator gain calibration ");
		
	setEnable (true);
	setInitial (0);

	bool success = findBiasHelper (gainCode);

	setEnable (false);

	return success;
}

void Fabric::Chip::Tile::Slice::Integrator::binarySearch (
	unsigned char minGainCode,
	float minBest,
	unsigned char maxGainCode,
	float maxBest,
	unsigned char & finalGainCode
) {
	if (binarySearchAvg (minGainCode, minBest, maxGainCode, maxBest, finalGainCode)) return;

	setParam1();
	parentSlice->parentTile->parentChip->parentFabric->cfgCommit();

	delayMicroseconds(CAL_REPS);
	float voltageDiff = binarySearchMeas();

	float target = hiGain ? parentSlice->dac->hiNegTarget : parentSlice->dac->midNegTarget;
	if (voltageDiff < target) {
		return binarySearch (minGainCode, minBest, finalGainCode, fabs(voltageDiff-target), finalGainCode);
	} else {
		return binarySearch (finalGainCode, fabs(voltageDiff-target), maxGainCode, maxBest, finalGainCode);
	}
}

void Fabric::Chip::Tile::Slice::Integrator::setAnaIrefDacNmos (
	bool decrement,
	bool increment
) {
	setAnaIrefDacNmosHelper (decrement, increment);

	unsigned char selRow=0;
	unsigned char selCol=2;
	unsigned char selLine;
	switch (parentSlice->sliceId) {
		case slice0: selLine=1; break;
		case slice1: selLine=2; break;
		case slice2: selLine=0; break;
		case slice3: selLine=3; break;
		default: error ("INT invalid slice"); break;
	}
	unsigned char cfgTile = endian(parentSlice->parentTile->parentChip->cfgBuf[parentSlice->parentTile->tileRowId][parentSlice->parentTile->tileColId][selRow][selCol][selLine]);
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

void Fabric::Chip::Tile::Slice::Integrator::setAnaIrefPmos () {

	unsigned char selRow=0;
	unsigned char selCol;
	unsigned char selLine;
	switch (parentSlice->sliceId) {
		case slice0: selCol=3; selLine=4; break;
		case slice1: selCol=3; selLine=5; break;
		case slice2: selCol=4; selLine=3; break;
		case slice3: selCol=4; selLine=4; break;
		default: error ("INT invalid slice"); break;
	}
	unsigned char cfgTile = endian(parentSlice->parentTile->parentChip->cfgBuf[parentSlice->parentTile->tileRowId][parentSlice->parentTile->tileColId][selRow][selCol][selLine]);
	switch (parentSlice->sliceId) {
		case slice0: cfgTile = (cfgTile & 0b00111000) + (anaIrefPmos & 0b00000111); break;
		case slice1: cfgTile = (cfgTile & 0b00000111) + ((anaIrefPmos<<3) & 0b00111000); break;
		case slice2: cfgTile = (cfgTile & 0b00111000) + (anaIrefPmos & 0b00000111); break;
		case slice3: cfgTile = (cfgTile & 0b00111000) + (anaIrefPmos & 0b00000111); break;
		default: error ("INT invalid slice"); break;
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