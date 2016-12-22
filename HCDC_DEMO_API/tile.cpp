#include "HCDC_DEMO_API.h"

Fabric::Chip::Tile::Tile (
	Chip * parentChip,
	tileRow tileRowId,
	tileCol tileColId,
	unsigned char spiSSPin,
	unsigned char spiMisoPin,
	unsigned char anaOutPinBase
) :
	parentChip (parentChip),
	tileRowId (tileRowId),
	tileColId (tileColId),
	spiSSPin (spiSSPin),
	spiMisoPin (spiMisoPin)
{
	pinMode(spiSSPin, OUTPUT);
	digitalWrite (spiSSPin, HIGH);
	pinMode(spiMisoPin, INPUT);

	slices = new Slice[4] {
		Slice (this, slice0, anaOutPinBase+4, anaOutPinBase+5),
		Slice (this, slice1, anaOutPinBase+6, anaOutPinBase+7),
		Slice (this, slice2, anaOutPinBase+0, anaOutPinBase+1),
		Slice (this, slice3, anaOutPinBase+2, anaOutPinBase+3)
	};
}

Fabric::Chip::Tile::~Tile() { delete[] slices; };

bool Fabric::Chip::Tile::calibrate () {
	slices[0].calibrate();
	slices[1].calibrate();
	slices[2].calibrate();
	slices[3].calibrate();
	return true;
};

/*Internal function*/
void Fabric::Chip::Tile::controllerHelperTile (
	unsigned char selLine,
	unsigned char cfgTile
) {
	if (selLine<7||11<selLine) error ("selLine out of bounds");
	if (cfgTile<0||255<cfgTile) error ("cfgTile out of bounds");
	/*if arduino form, check that sram vector fields are within bounds*/
	// should only be used by controller and lut param writes
	// Serial.print("vec.tileRowId = "); Serial.println(vec.tileRowId);
	// Serial.print("vec.tileColId = "); Serial.println(vec.tileColId);
	// Serial.print("vec.selRow = "); Serial.println(vec.selRow);
	// Serial.print("vec.selCol = "); Serial.println(vec.selCol);
	// Serial.print("vec.cfgTile = "); Serial.println(vec.cfgTile);
	spiDriveTile ( 8, 0, selLine, cfgTile );
	spiDriveTile (noOp);
}

void Fabric::Chip::Tile::spiDriveTile (
	unsigned char selRow,
	unsigned char selCol,
	unsigned char selLine,
	unsigned char cfgTile
) {
	digitalWrite (spiSSPin, LOW);
	spiDrive ( selRow, selCol, selLine, cfgTile );
	digitalWrite (spiSSPin, HIGH);
}

int Fabric::Chip::Tile::spiDriveTile ( bool * vector ) {
	digitalWrite (spiSSPin, LOW);
	// Serial.print("spiMisoPin = "); Serial.println(spiMisoPin);
	unsigned int result = spiDrive ( vector, spiMisoPin );
	// Serial.print("result = "); Serial.println(result);
	digitalWrite (spiSSPin, HIGH);
	return result;
}