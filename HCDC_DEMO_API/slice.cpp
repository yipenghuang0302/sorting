#include "HCDC_DEMO_API.h"

Fabric::Chip::Tile::Slice::Slice (
	Chip::Tile * parentTile,
	slice sliceId,
	unsigned char anaOutPosPin,
	unsigned char anaOutNegPin
) :
	parentTile (parentTile),
	sliceId (sliceId),
	anaOutPosPin (anaOutPosPin),
	anaOutNegPin (anaOutNegPin)
{

	chipInput = new ChipInput (this);
	tally_dyn_mem <ChipInput> ("ChipInput");

	tileInps = new TileInOut[4] {
		TileInOut(this, tileInp0),
		TileInOut(this, tileInp1),
		TileInOut(this, tileInp2),
		TileInOut(this, tileInp3)
	};
	tally_dyn_mem <TileInOut[4]> ("TileInOut[4]");

	muls = new Multiplier[2] {
		Multiplier (this, unitMulL),
		Multiplier (this, unitMulR)
	};
	tally_dyn_mem <Multiplier[2]> ("Multiplier[2]");

	dac = new Dac (this);
	tally_dyn_mem <Dac> ("Dac");

	integrator = new Integrator (this);
	tally_dyn_mem <Integrator> ("Integrator");

	fans = new Fanout[2] {
		Fanout (this, unitFanL),
		Fanout (this, unitFanR)
	};
	tally_dyn_mem <Fanout[2]> ("Fanout[2]");

	if (sliceId==slice0 || sliceId==slice2) {
		adc = new Adc (this);
		tally_dyn_mem <Adc> ("Adc");
		lut = new LookupTable (this);
		tally_dyn_mem <LookupTable> ("LookupTable");
	}

	tileOuts = new TileInOut[4] {
		TileInOut(this, tileOut0),
		TileInOut(this, tileOut1),
		TileInOut(this, tileOut2),
		TileInOut(this, tileOut3)
	};
	tally_dyn_mem <TileInOut[4]> ("TileInOut[4]");

	chipOutput = new ChipOutput (this, anaOutPosPin, anaOutNegPin);
	tally_dyn_mem <ChipOutput> ("ChipOutput");
}

Fabric::Chip::Tile::Slice::~Slice () {
	delete chipInput;
	delete[] tileInps;
	delete dac;
	delete[] muls;
	delete integrator;
	delete[] fans;
	if (sliceId==slice0 || sliceId==slice2) {
		delete adc;
		delete lut;
	}
	delete[] tileOuts;
	delete chipOutput;
};

bool Fabric::Chip::Tile::Slice::calibrate () {
	if (sliceId==slice0 || sliceId==slice2) {
		if (
			sliceId==slice2
			&& parentTile->tileRowId==tileRow0
			&& parentTile->tileColId==tileCol1
			&& parentTile->parentChip->chipRowId==chipRow0
			&& parentTile->parentChip->chipColId==chipCol0
		) Serial.println("SKIPPING THIS ADC");
		// if (
		// 	sliceId==slice2
		// 	&& parentTile->tileRowId==tileRow1
		// 	&& parentTile->tileColId==tileCol0
		// 	// && parentTile->parentChip->chipRowId==chipRow0
		// 	// && parentTile->parentChip->chipColId==chipCol0
		// ) Serial.println("SKIPPING THIS ADC");
		// else if (
		// 	sliceId==slice0
		// 	&& parentTile->tileRowId==tileRow1
		// 	&& parentTile->tileColId==tileCol1
		// 	&& parentTile->parentChip->chipRowId==chipRow0
		// 	&& parentTile->parentChip->chipColId==chipCol1
		// ) Serial.println("SKIPPING THIS ADC");
		// else if (
		// 	sliceId==slice2
		// 	&& parentTile->tileRowId==tileRow1
		// 	&& parentTile->tileColId==tileCol1
		// 	&& parentTile->parentChip->chipRowId==chipRow0
		// 	&& parentTile->parentChip->chipColId==chipCol1
		// ) Serial.println("SKIPPING THIS ADC");
		else if (!adc->calibrate()) return false;
	}
	if (!dac->findBias (dac->midNegGainCode)) return false;
	// if (!dac->calibrateRange (dacMid)) return false;
	// if (!dac->calibrateRange (dacHi)) return false;
	if (!fans[0].calibrate()) return false;
	if (!fans[1].calibrate()) return false;
	if (!muls[0].calibrate()) return false;
	if (!muls[1].calibrate()) return false;
	if (
		sliceId==slice0
		&& parentTile->tileRowId==tileRow1
		&& parentTile->tileColId==tileCol0
		&& parentTile->parentChip->chipRowId==chipRow0
		&& parentTile->parentChip->chipColId==chipCol0
	) Serial.println("SKIPPING THIS INT");
	else if (!integrator->calibrate()) return false;
	return true;
}