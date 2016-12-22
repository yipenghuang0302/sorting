class Fabric::Chip::Tile::Slice::TileInOut : public Fabric::Chip::Tile::Slice::FunctionUnit {
	friend Slice;

	TileInOut ( Slice * parentSlice, unit unitId_ ) : FunctionUnit(parentSlice, unitId_) {
		in0 = new GenericInterface (this, in0Id);
		tally_dyn_mem <GenericInterface> ("GenericInterface");
		out0 = new GenericInterface (this, out0Id);
		tally_dyn_mem <GenericInterface> ("GenericInterface");
	};
	~TileInOut() {
		delete in0;
		delete out0;
	};

	void setEnable ( bool enable ) { error("not implemented"); };
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