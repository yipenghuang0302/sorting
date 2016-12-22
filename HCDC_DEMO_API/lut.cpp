#include "HCDC_DEMO_API.h"

void Fabric::Chip::Tile::Slice::LookupTable::setSource (
	bool external, // lut takes input from chip parallel input
	bool adc0, // lut takes input from first analog to digital converter
	bool adc1 // lut takes input from second analog to digital converter
	// only one of these should be true
) {
	/*check*/
	if ( (external&&adc0) || (external&&adc1) || (adc0&&adc1) ) error ("LUT only one input can be selected");
	if (external) {
		parentSlice->parentTile->setParallelIn ( external );
	}
	/*set*/
	setParam0 (
		ns_6, /*Trigger output delay*/
		ns2_5, /*pins for programming the delay lines for read operation*/
		ps1060, /*pin for programming the delay lines for write operation*/
		external ? extLut : ( adc0 ? adcL : adcR ), /*clock signal selection*/
		external ? extLut : ( adc0 ? adcL : adcR ) /*input signal selection*/
	);
	/*record*/
	this->external = external;
	this->adc0 = adc0;
	this->adc1 = adc1;
}

/*Put LUT in writing mode*/
void Fabric::Chip::Tile::Slice::LookupTable::setStart (
) {
	unsigned char cfgTile = 0;
	cfgTile += ctlrLut<<2;
	cfgTile += ctlrLut<<0;

	/*DETERMINE SEL_COL*/
	unsigned char selCol;
	switch (parentSlice->sliceId) {
		case slice0: selCol = 1; break;
		case slice2: selCol = 2; break;
		default: error ("invalid slice. Only even slices have LUTs"); break;
	}

	parentSlice->parentTile->spiDriveTile ( 7, selCol, 0, endian(cfgTile) );
	parentSlice->parentTile->spiDriveTile (noOp);

}

/*Set read delay, write delay, clock select, input select*/
void Fabric::Chip::Tile::Slice::LookupTable::setParam0 (
	lutTrigDelay trigDelay, /*Trigger output delay*/
	lutRDelay rDelay, /*pins for programming the delay lines for read operation*/
	lutWDelay wDelay, /*pin for programming the delay lines for write operation*/
	lutSel selClk, /*clock signal selection*/
	lutSel selIn /*input signal selection*/
) {
	unsigned char cfgTile = 0;
	cfgTile += (trigDelay==ns_6) ? 1<<7 : 0;
	cfgTile += rDelay<<5;
	cfgTile += (wDelay==ps1060) ? 1<<4 : 0;
	cfgTile += selClk<<2;
	cfgTile += selIn<<0;
	setParamHelper (0, cfgTile);
}

/*Helper function*/
void Fabric::Chip::Tile::Slice::LookupTable::setParamHelper (
	unsigned char selLine,
	unsigned char cfgTile
) {
	if (selLine<0||1<selLine) error ("selLine out of bounds");
	// Serial.print("LUT setParamHelper parentSlice->sliceId = "); Serial.println(parentSlice->sliceId);

	/*DETERMINE SEL_COL*/
	unsigned char selCol;
	switch (parentSlice->sliceId) {
		case slice0: selCol = 1; break;
		case slice2: selCol = 2; break;
		default: error ("invalid slice. Only even slices have LUTs"); break;
	}

	Chip::Vector vec = Vector (
		*this,
		7,
		selCol,
		selLine,
		endian (cfgTile)
	);

	parentSlice->parentTile->parentChip->cacheVec (
		vec
	);
}

/*Set LUT SRAM contents*/
void Fabric::Chip::Tile::Slice::LookupTable::setLut (
	unsigned char addr,
	unsigned char data
) {

	/*edit addr endianness*/
	unsigned char endianAddr = endian (addr);

	unsigned char selRow;
	switch (parentSlice->sliceId) {
		case slice0 : selRow = 0b1001; break;
		case slice2 : selRow = 0b1010; break;
		default: error ("invalid slice. Only even slices have LUTs"); break;
	}

	Chip::Vector vec = Vector (
		*this,
		selRow,
		(endianAddr&0xf0) >> 4,
		(endianAddr&0x0f) >> 0,
		endian (data)
	);

	parentSlice->parentTile->parentChip->cacheVec (
		vec
	);
}