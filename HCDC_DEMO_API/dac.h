typedef enum {
	dacLo = 0,		/*the DAC block signal range is 2uA*/
	dacMid = 0,	/*the DAC block signal range is 2uA*/
	dacHi = 1		/*the DAC block signal range is 20uA*/
} dacRange;

typedef enum {
	lutL	= 0, /*signals from lutL are selected*/
	lutR	= 1, /*signals from lutR are selected*/
	extDac	= 2, /*signals from external are selected*/
	adc		= 3  /*signals from ADC are selected*/
} dacSel;

class Fabric::Chip::Tile::Slice::Dac : public Fabric::Chip::Tile::Slice::FunctionUnit {
	friend Slice;

	public:
		void setEnable ( bool enable );
		void setSource (
			bool memory,
			bool external, // digital to analog converter takes input from chip parallel input
			bool lut0, // digital to analog converter takes input from first lookup table
			bool lut1 // digital to analog converter takes input from second lookup table
			// only one of these should be true
		);
		void setConstantCode (
			unsigned char constant // fixed point representation of desired constant
			// 0 to 255 are valid
		);
		void setConstant (
			float constant // floating point representation of desired constant
			// -10.0 to 10.0 are valid
		);
	private:
		class DacOut;
		Dac (Slice * parentSlice);
		~Dac () { delete out0; };
		/*Set enable, invert, range, clock select*/
		void setParam0 ();
		/*Set calDac, input select*/
		void setParam1 ();
		void setParam2 () {};
		void setParam3 () {};
		void setParam4 () {};
		void setParam5 () {};
		void setParamHelper (
			unsigned char selLine,
			unsigned char cfgTile
		);
		bool calibrateRange ( dacRange dacRange );
		void findOffset ( float & offset );
		bool findBias (
			dacRange dacRange,
			unsigned char & gainCode
		);
		bool findBias (
			unsigned char & gainCode
		);
		void setAnaIrefDacNmos (
			bool decrement,
			bool increment
		);
		void binarySearch (
			dacRange dacRange,
			unsigned char minGainCode,
			float minBest,
			unsigned char maxGainCode,
			float maxBest,
			unsigned char & finalGainCode
		);
		void binarySearch (
			unsigned char minGainCode,
			float minBest,
			unsigned char maxGainCode,
			float maxBest,
			unsigned char & finalGainCode
		);
		bool memory=false;
		bool external=false;
		bool lut0=false;
		bool lut1=false;
		float midNegTarget = -1.2;
		// unsigned char hiNegGainCode = 31;
		static constexpr float midPosTarget = 1.190625;
		static constexpr float hiNegTarget = -12.;
		static constexpr float hiPosTarget = 11.90625;
};

class Fabric::Chip::Tile::Slice::Dac::DacOut : public Fabric::Chip::Tile::Slice::FunctionUnit::Interface  {
	friend Dac;

	public:
		void setInv ( bool inverse ); // whether output is negated
	private:
		DacOut (Dac * parentFu) : Interface(parentFu, out0Id) {};
		void setRange (
			bool loRange,
			bool hiRange
		);
		void calibrate() {};
		bool findBias (
			unsigned char & offsetCode
		) {
			error("not implemented");
			return false;
		};
		void binarySearch (
			unsigned char minCode,
			float minBest,
			unsigned char maxCode,
			float maxBest,
			unsigned char & finalCode
		) {};
};