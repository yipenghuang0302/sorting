class Fabric::Chip::Tile::Slice::Fanout : public Fabric::Chip::Tile::Slice::FunctionUnit {
	friend Slice;

	public:
		void setEnable ( bool enable );
		void setThird (
			bool third // whether third output is on
		);
		void setRange (
			bool hiRange // 20uA mode
			// 20uA mode results in more ideal behavior in terms of phase shift but consumes more power
			// this setting should match the unit that gives the input to the fanout
		);
	private:
		class FanoutOut;
		Fanout (Slice * parentSlice, unit unitId);
		~Fanout () {
			delete in0;
			delete out0;
			delete out1;
			delete out2;
		};
		/*Set enable, range*/
		void setParam0 ();
		/*Set calDac1, invert output 1*/
		void setParam1 ();
		/*Set calDac2, invert output 2*/
		void setParam2 ();
		/*Set calDac3, invert output 3, enable output 3*/
		void setParam3 ();
		void setParam4 () {};
		void setParam5 () {};
		/*Helper function*/
		void setParamHelper (
			unsigned char selLine,
			unsigned char cfgTile
		);
		bool calibrate ();
		void setAnaIrefDacNmos (
			bool decrement,
			bool increment
		);
		void setAnaIrefNmos (
		);
		bool findBias (
			unsigned char & gainCode
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
		bool third = false;
		const unsigned char anaIrefNmos = 4;
};

class Fabric::Chip::Tile::Slice::Fanout::FanoutOut : public Fabric::Chip::Tile::Slice::FunctionUnit::Interface  {
	friend Fanout;

	public:
		void setInv ( bool inverse );
	private:
		FanoutOut (Fanout * parentFu, ifc ifcId) :
			Interface(parentFu, ifcId),
			parentFanout(parentFu)
		{};
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
		void setRange (
			bool loRange, // 0.2uA mode
			bool hiRange // 20 uA mode
		) { error("not implemented"); };
		void calibrate() {};
		Fanout * parentFanout;
};