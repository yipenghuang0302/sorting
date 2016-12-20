class Fabric::Chip::Tile::Slice::Multiplier : public Fabric::Chip::Tile::Slice::FunctionUnit {
	friend Slice;
	public:
		void setEnable ( bool enable );
		void setGainCode (
			unsigned char gain // fixed point representation of desired gain
		);
		void setGain (
			float gain // floating point representation of desired gain
			// -100.0 to 100.0 are valid
		);
		void setVga (
			bool vga // constant coefficient multiplier mode
		);
	private:
		class MultiplierInterface;
		Multiplier (Slice * parentSlice, unit unitId);
		~Multiplier () {
			delete out0;
			delete in0;
			delete in1;
		};
		bool calibrate ();
		/*Set enable, input 1 range, input 2 range, output range*/
		void setParam0 ();
		/*Set calDac, enable variable gain amplifer mode*/
		void setParam1 ();
		/*Set gain if VGA mode*/
		void setParam2 ();
		/*Set calOutOs*/
		void setParam3 ();
		/*Set calInOs1*/
		void setParam4 ();
		/*Set calInOs2*/
		void setParam5 ();
		/*Helper function*/
		void setParamHelper (
			unsigned char selLine,
			unsigned char cfgTile
		);
		bool findBias (
			unsigned char & gainCode
		);
		void setAnaIrefDacNmos (
			bool decrement,
			bool increment
		);
		void setAnaIrefPmos ();
		void binarySearch (
			unsigned char minGainCode,
			float minBest,
			unsigned char maxGainCode,
			float maxBest,
			unsigned char & finalGainCode
		);
		bool vga = false;
		unsigned char gain = 128;
		const unsigned char anaIrefPmos = 4;
};

class Fabric::Chip::Tile::Slice::Multiplier::MultiplierInterface : public Fabric::Chip::Tile::Slice::FunctionUnit::Interface  {
	friend Multiplier;
	public:
		void setRange (
			bool loRange, // 0.2uA mode
			bool hiRange // 20 uA mode
			// default is 2uA mode
			// this setting should match the unit that gives the input to the multiplier
		);
	private:
		MultiplierInterface (Multiplier * parentFu, ifc ifcId) : Interface(parentFu, ifcId), parentMultiplier(parentFu) {};
		void calibrate ();
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
		);
		void setInv ( bool inverse ) { error("not implemented"); }; // whether output is negated
		Multiplier * parentMultiplier;
};