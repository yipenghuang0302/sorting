class Fabric::Chip::Tile::Slice::ChipInput : public Fabric::Chip::Tile::Slice::FunctionUnit {
	friend Slice;

	private:
		ChipInput (
			Slice * parentSlice
		) :
			FunctionUnit(parentSlice, chipInp)
		{
			out0 = new GenericInterface (this, out0Id);
			tally_dyn_mem <GenericInterface> ("GenericInterface");
			/*ANALOG INPUT CHANNEL ENABLE PINS*/
		};
		~ChipInput () { delete out0; };

		void setEnable ( bool enable ) {};
		void setParam0 () {};
		void setParam1 () {};
		void setParam2 () {};
		void setParam3 () {};
		void setParam4 () {};
		void setParam5 () {};
		void setParamHelper (
			unsigned char selLine,
			unsigned char cfgTile
		) {};
		bool findBias (
			unsigned char & gainCode
		) {
			error("not implemented");
			return false;
		};
		void setAnaIrefDacNmos (
			bool decrement,
			bool increment
		) {};
		void binarySearch (
			unsigned char minCode,
			float minBest,
			unsigned char maxCode,
			float maxBest,
			unsigned char & finalCode
		) {};
};

// const float resistorValue = 600000.0;
// const float fullScaleCurrent = 0.000001861;

class Fabric::Chip::Tile::Slice::ChipOutput : public Fabric::Chip::Tile::Slice::FunctionUnit {
	friend Slice;

	public:
	/*Measure the reading of an ADC from multiple samples*/
	/*Measure the differential voltage from a pair of ADC from multiple samples*/
	float analogAvg (
		unsigned char samples
	);

	private:
		ChipOutput (
			Slice * parentSlice,
			unsigned char posPin,
			unsigned char negPin
		) :
			FunctionUnit(parentSlice, chipOut),
			posPin (posPin),
			negPin (negPin)
		{
			in0 = new GenericInterface (this, in0Id);
			tally_dyn_mem <GenericInterface> ("GenericInterface");
		};
		~ChipOutput () { delete in0; };

		unsigned char posPin, negPin;

		void setEnable ( bool enable ) {};
		void setParam0 () {};
		void setParam1 () {};
		void setParam2 () {};
		void setParam3 () {};
		void setParam4 () {};
		void setParam5 () {};
		void setParamHelper (
			unsigned char selLine,
			unsigned char cfgTile
		) {};
		bool findBias (
			unsigned char & gainCode
		) {
			error("not implemented");
			return false;
		};
		void setAnaIrefDacNmos (
			bool decrement,
			bool increment
		) {};
		void binarySearch (
			unsigned char minCode,
			float minBest,
			unsigned char maxCode,
			float maxBest,
			unsigned char & finalCode
		) {};
};