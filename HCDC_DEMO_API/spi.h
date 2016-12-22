/*high speed digital pin implementation*/
#ifdef _DUE
	static inline void digitalWriteDirect (unsigned char pin, unsigned char val) {
		if (val) g_APinDescription[pin].pPort -> PIO_SODR = g_APinDescription[pin].ulPin;
		else g_APinDescription[pin].pPort -> PIO_CODR = g_APinDescription[pin].ulPin;
	}

	static inline unsigned char digitalReadDirect (unsigned char pin) {
		return !!(g_APinDescription[pin].pPort -> PIO_PDSR & g_APinDescription[pin].ulPin);
	}
#endif

static inline void spiBit (bool sendBit) {
	/*clock low*/
	#ifdef _DUE
		digitalWriteDirect(spiClkPin, LOW);
	#else
		digitalWrite(spiClkPin, LOW);
	#endif
	#ifdef _DUE
		digitalWriteDirect(spiMosiPin, sendBit);
	#else
		digitalWrite(spiMosiPin, sendBit);
	#endif
	/*clock high*/
	#ifdef _DUE
		digitalWriteDirect(spiClkPin, HIGH);
	#else
		digitalWrite(spiClkPin, HIGH);
	#endif
}

static inline void spiDrive (
	unsigned char selRow,
	unsigned char selCol,
	unsigned char selLine,
	unsigned char cfgTile
) {
	/*write the tile signature, 0101*/
	spiBit(false);spiBit(true);spiBit(false);spiBit(true);

	/*from selRow*/
	if ((selRow&0x8)>>3==1) spiBit(true); else spiBit(false);
	if ((selRow&0x4)>>2==1) spiBit(true); else spiBit(false);
	if ((selRow&0x2)>>1==1) spiBit(true); else spiBit(false);
	if ((selRow&0x1)>>0==1) spiBit(true); else spiBit(false);
	/*from selCol*/
	if ((selCol&0x8)>>3==1) spiBit(true); else spiBit(false);
	if ((selCol&0x4)>>2==1) spiBit(true); else spiBit(false);
	if ((selCol&0x2)>>1==1) spiBit(true); else spiBit(false);
	if ((selCol&0x1)>>0==1) spiBit(true); else spiBit(false);
	/*from selLine*/
	if ((selLine&0x8)>>3==1) spiBit(true); else spiBit(false);
	if ((selLine&0x4)>>2==1) spiBit(true); else spiBit(false);
	if ((selLine&0x2)>>1==1) spiBit(true); else spiBit(false);
	if ((selLine&0x1)>>0==1) spiBit(true); else spiBit(false);
	/*from cfgTile*/
	if ((cfgTile&0x80)>>7==1) spiBit(true); else spiBit(false);
	if ((cfgTile&0x40)>>6==1) spiBit(true); else spiBit(false);
	if ((cfgTile&0x20)>>5==1) spiBit(true); else spiBit(false);
	if ((cfgTile&0x10)>>4==1) spiBit(true); else spiBit(false);
	if ((cfgTile&0x08)>>3==1) spiBit(true); else spiBit(false);
	if ((cfgTile&0x04)>>2==1) spiBit(true); else spiBit(false);
	if ((cfgTile&0x02)>>1==1) spiBit(true); else spiBit(false);
	if ((cfgTile&0x01)>>0==1) spiBit(true); else spiBit(false);
}

/*send single bit to HCDC chip*/

/*send binary instruction to HCDC chip*/
static inline int spiDrive (bool * vector, int spiMisoPin) {

	// Serial.print("spiDrive spiMisoPin = "); Serial.println(spiMisoPin);
	int misoBuffer = 0;

	/*loop over bit*/
	for (unsigned char bitIndex=0; bitIndex<24; bitIndex++) {
		/*write output and read input*/
		/*clock low*/
		#ifdef _DUE
			digitalWriteDirect(spiClkPin, LOW);
		#else
			digitalWrite(spiClkPin, LOW);
		#endif

		#ifdef _DUE
			digitalWriteDirect(spiMosiPin, vector[bitIndex]);
		#else
			digitalWrite(spiMosiPin, vector[bitIndex]);
		#endif

		/*read the SPI MISO bit*/
		// if input is high, add to buffer
		/*bits are streamed out lsb first*/
		#ifdef _DUE
			bitWrite(misoBuffer, bitIndex, digitalReadDirect(spiMisoPin));
		#else
			bitWrite(misoBuffer, bitIndex, digitalRead(spiMisoPin));
		#endif
			
		// Serial.print("misoBuffer = "); Serial.println(misoBuffer);

		/*clock high*/
		#ifdef _DUE
			digitalWriteDirect(spiClkPin, HIGH);
		#else
			digitalWrite(spiClkPin, HIGH);
		#endif

	}

	/*print*/
	// Serial.println("MISO from HCDC:");
	// Serial.println(misoBuffer);
	return misoBuffer;
}
