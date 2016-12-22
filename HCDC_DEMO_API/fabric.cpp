#include "HCDC_DEMO_API.h"

size_t dynamic_memory = 0;

Fabric::Fabric () {

	/*USB connection to PC*/
	Serial.begin(115200);
	/*SET PIN DIRECTIONS*/
	/*CONTROL PINS*/
	/*SPI PINS*/
	pinMode(ctrRstPin, OUTPUT);
	pinMode(spiClkPin, OUTPUT);
	pinMode(spiMosiPin, OUTPUT);
	pinMode(moMiEnPin, OUTPUT);

	digitalWrite(moMiEnPin, LOW); /*scan chain disable*/

	/*ANALOG PINS*/
	#ifdef _DUE
		analogReadResolution(12);
		analogWriteResolution(12);
	#else
		analogReference(DEFAULT);
	#endif

	/*random addr seed*/
	randomSeed(analogRead(0));

	Serial.println("initialized arduino");

	#ifdef _DUE
		// Serial.println("fast reset");
		digitalWriteDirect(ctrRstPin, HIGH);
		digitalWriteDirect(ctrRstPin, LOW);
	#else
		// Serial.println("regular reset");
		digitalWrite(ctrRstPin, HIGH);
		digitalWrite(ctrRstPin, LOW);
	#endif

	/*create chips*/
	chips = new Chip[2] {
		Chip (this, chipRow0, chipCol0, 30, 25, 24, 3, 9, 0),
		Chip (this, chipRow0, chipCol1, 31, 23, 22, 50, 14, 4)
	};
	tally_dyn_mem <Chip[2]> ("Chip[2]");

	Serial.println("allocated hcdc");

	cfgCommit();

	Serial.println("initialized hcdc");
}

Fabric::~Fabric() { 
	Serial.println("Fabric dtor");
	delete[] chips;
};

void Fabric::reset () {
	Serial.begin(115200);
	digitalWrite(moMiEnPin, LOW); /*scan chain disable*/
	Serial.println("initialized arduino");

	#ifdef _DUE
		// Serial.println("fast reset");
		digitalWriteDirect(ctrRstPin, HIGH);
		digitalWriteDirect(ctrRstPin, LOW);
	#else
		// Serial.println("regular reset");
		digitalWrite(ctrRstPin, HIGH);
		digitalWrite(ctrRstPin, LOW);
	#endif

	/*create chips*/
	delete[] chips;
	chips = new Chip[2] {
		Chip (this, chipRow0, chipCol0, 30, 25, 24, 3, 9, 0),
		Chip (this, chipRow0, chipCol1, 31, 23, 22, 50, 14, 4)
	};
	tally_dyn_mem <Chip[2]> ("Chip[2]");

	Serial.println("allocated hcdc");

	cfgCommit();

	Serial.println("initialized hcdc");
}

bool Fabric::calibrate () {
	// TODO: Route to dump
	Serial.println("Calibrating Chip 0");
	chips[0].calibrate();
	// Serial.println("Calibrating Chip 1");
	// chips[0] chipInput 4 to chipOutput 15
	// Fabric::Chip::Connection conn ( chips[0].tiles[2].slices[0].chipInput->out0, chips[0].tiles[3].slices[3].chipOutput->in0 );
	// conn.setConn();
	// chips[1].calibrate();
	// conn.brkConn();
	return true;
}

void Fabric::spiDriveFabric (
	unsigned char selRow,
	unsigned char selCol,
	unsigned char selLine,
	unsigned char cfgTile
) {
	digitalWrite (chips[0].tiles[0].spiSSPin, LOW);
	digitalWrite (chips[0].tiles[1].spiSSPin, LOW);
	digitalWrite (chips[0].tiles[2].spiSSPin, LOW);
	digitalWrite (chips[0].tiles[3].spiSSPin, LOW);
	digitalWrite (chips[1].tiles[0].spiSSPin, LOW);
	digitalWrite (chips[1].tiles[1].spiSSPin, LOW);
	digitalWrite (chips[1].tiles[2].spiSSPin, LOW);
	digitalWrite (chips[1].tiles[3].spiSSPin, LOW);
	spiDrive ( selRow, selCol, selLine, cfgTile );
	digitalWrite (chips[0].tiles[0].spiSSPin, HIGH);
	digitalWrite (chips[0].tiles[1].spiSSPin, HIGH);
	digitalWrite (chips[0].tiles[2].spiSSPin, HIGH);
	digitalWrite (chips[0].tiles[3].spiSSPin, HIGH);
	digitalWrite (chips[1].tiles[0].spiSSPin, HIGH);
	digitalWrite (chips[1].tiles[1].spiSSPin, HIGH);
	digitalWrite (chips[1].tiles[2].spiSSPin, HIGH);
	digitalWrite (chips[1].tiles[3].spiSSPin, HIGH);
}

int Fabric::spiDriveFabric (
	bool * vector
) {
	digitalWrite (chips[0].tiles[0].spiSSPin, LOW);
	digitalWrite (chips[0].tiles[1].spiSSPin, LOW);
	digitalWrite (chips[0].tiles[2].spiSSPin, LOW);
	digitalWrite (chips[0].tiles[3].spiSSPin, LOW);
	digitalWrite (chips[1].tiles[0].spiSSPin, LOW);
	digitalWrite (chips[1].tiles[1].spiSSPin, LOW);
	digitalWrite (chips[1].tiles[2].spiSSPin, LOW);
	digitalWrite (chips[1].tiles[3].spiSSPin, LOW);
	unsigned char result = spiDrive (vector, chips[0].tiles[0].spiMisoPin);
	digitalWrite (chips[0].tiles[0].spiSSPin, HIGH);
	digitalWrite (chips[0].tiles[1].spiSSPin, HIGH);
	digitalWrite (chips[0].tiles[2].spiSSPin, HIGH);
	digitalWrite (chips[0].tiles[3].spiSSPin, HIGH);
	digitalWrite (chips[1].tiles[0].spiSSPin, HIGH);
	digitalWrite (chips[1].tiles[1].spiSSPin, HIGH);
	digitalWrite (chips[1].tiles[2].spiSSPin, HIGH);
	digitalWrite (chips[1].tiles[3].spiSSPin, HIGH);
	return result;
}