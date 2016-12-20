typedef enum {
	adcLo = 0,		/*2uA*/
	adcMid = 0,	/*2uA*/
	adcHi = 1		/*20uA*/
} adcRange;

typedef enum {
	ns11_5 =	0, /*11.5 ns delay (normal operation)*/
	ns9_7 = 	1, /*9.7 ns delay*/
	ns7_8 = 	2, /*7.8 ns delay*/
	ns5_8 = 	3  /*5.8 ns delay*/
} adcDelay;

typedef enum {
	ns3 =		0, /*3ns is the default*/
	ns6 =		1  /*6ns trigger delay*/
} adcTrigDelay;

typedef enum {
	nA100 = 0, /*IFS = 100nA*/
	nA200 = 1, /*IFS = 200nA*/
	nA300 = 2, /*IFS = 300nA*/
	nA400 = 3  /*IFS = 400nA*/
} adcCalCompFs;

class Fabric::Chip::Tile::Slice::Adc : public Fabric::Chip::Tile::Slice::FunctionUnit {
	friend Slice;
	public:
		void setEnable ( bool enable );
		unsigned char readHelper ();
	private:
		class AdcIn;
		Adc (Slice * parentSlice);
		~Adc () { delete in0; };
		/*Set enable, range, delay, decRst*/
		void setParam0 ();
		/*Set calibration enable, calCompUpperEn, calIv*/
		void setParam1 ();
		/*Set calCompLower, calCompLowerFs*/
		void setParam2 ();
		/*Set calCompUpper, calCompUpperFs*/
		void setParam3 ();
		/*Set testEn, testAdc, testIv, testRs, testRsInc*/
		void setParam4 (
			bool testEn, /*Configure the entire block in testing mode so that I2V and A/D can be tested individually*/
			bool testAdc, /*Testing the ADC individually.*/
			bool testIv, /*Testing the I2V individually.*/
			bool testRs, /*Testing the rstring individually.*/
			bool testRsInc /*Configure the counter for upward or downward increments during set up for testing R-string separately (w/ cfgCalEN=1)*/
		);
		void setParam4 () {};
		void setParam5 () {};
		/*Helper function*/
		void setParamHelper (
			unsigned char selLine,
			unsigned char cfgTile
		);
		bool calibrate ();
		bool findCalCompFs ();
		bool checkScale ();
		bool checkSpread (
			unsigned char spread,
			bool lowerPos,
			bool upperPos
		);
		bool checkSteady (
			unsigned char dacCode
		);
		bool findBias (
			unsigned char & gainCode
		) {
			error("not implemented");
			return false;
		};
		void setAnaIrefDacNmos (
			bool decrement,
			bool increment
		);
		void binarySearch (
			unsigned char minCode,
			float minBest,
			unsigned char maxCode,
			float maxBest,
			unsigned char & finalCode
		) {};
		void setAnaIrefPmos ();

		unsigned char calI2V = 31;
		// anaIrefI2V is remapped in SW to AnaIrefDacNmos

		unsigned char calCompLower = 31;
		adcCalCompFs calCompLowerFs = nA100;
		const unsigned char anaIref1Pmos = 4;

		unsigned char calCompUpper = 31;
		adcCalCompFs calCompUpperFs = nA100;
		const unsigned char anaIref2Pmos = 4;
};

class Fabric::Chip::Tile::Slice::Adc::AdcIn : public Fabric::Chip::Tile::Slice::FunctionUnit::Interface {
	friend Adc;

	public:
		void setRange (
			bool hiRange // 20uA mode
			// 20uA mode results in more ideal behavior in terms of phase shift but consumes more power // this setting should match the unit that gives the input to the fanout
		);
	private:
		AdcIn (Adc * parentFu) :
			Interface(parentFu, in0Id),
			parentAdc(parentFu)
		{};
		void setInv ( bool inverse ) { error("not implemented"); }; // whether output is negated
		void setRange ( bool loRange, bool hiRange ) {};
		void calibrate() {};
		bool findBias (
			unsigned char & offsetCode
		);
		void binarySearch (
			unsigned char minCode,
			float minBest,
			unsigned char maxCode,
			float maxBest,
			unsigned char & finalCode
		);
		Adc * parentAdc;
};