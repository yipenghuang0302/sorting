/*Valid options for functional unit interface.*/
typedef enum {
	in0Id,
	in1Id,
	out0Id,
	out1Id,
	out2Id
} ifc;

/*signal range configuration*/
typedef enum {
	fanLo = 0, /*2uA & 200nA*/
 	fanMid = 0, /*2uA & 200nA*/
	fanHi = 1 /*20uA*/
} fanRange;

typedef enum {
	mulMid = 0, /* -2 to 2  uA*/
	mulLo = 1,  /*-.2 to .2 uA*/
	mulHi = 2,  /*-20 to 20 uA*/
} mulRange;

class Fabric::Chip::Tile::Slice::FunctionUnit {
	friend Slice;
	friend Connection;
	friend Vector;

	public:
		class Interface;
		virtual void setEnable ( bool enable )=0;
		virtual void setParallelOut ( bool onOff );
		Interface * in0;
		Interface * in1;
		Interface * out0;
		Interface * out1;
		Interface * out2;
	private:
		class GenericInterface;
		FunctionUnit (
			Slice * parentSlice_,
			unit unitId_
		) :
			parentSlice(parentSlice_),
			unitId(unitId_)
		{};
		virtual void setParam0 ()=0;
		virtual void setParam1 ()=0;
		virtual void setParam2 ()=0;
		virtual void setParam3 ()=0;
		virtual void setParam4 ()=0;
		virtual void setParam5 ()=0;
		virtual void setParamHelper (
			unsigned char selLine,
			unsigned char cfgTile
		)=0;
		virtual bool findBias (
			unsigned char & gainCode
		)=0;
		bool findBiasHelper (
			unsigned char & code
		);
		virtual void setAnaIrefDacNmos (
			bool decrement,
			bool increment
		)=0;
		void setAnaIrefDacNmosHelper (
			bool decrement,
			bool increment
		);
		virtual void binarySearch (
			unsigned char minCode,
			float minBest,
			unsigned char maxCode,
			float maxBest,
			unsigned char & finalCode
		)=0;
		bool binarySearchAvg (
			unsigned char minGainCode,
			float minBest,
			unsigned char maxGainCode,
			float maxBest,
			unsigned char & finalGainCode
		);
		float binarySearchMeas ();
		Slice * parentSlice;
		// used for gain and initial condition range calibration
		unit unitId;
		bool enable = false;
		unsigned char midNegGainCode = 0;
		unsigned char anaIrefDacNmos = 0;
};

class Fabric::Chip::Tile::Slice::FunctionUnit::Interface {
	friend FunctionUnit;
	friend Adc;
	friend Dac;
	friend Fanout;
	friend Integrator;
	friend Multiplier;
	friend Connection;

	public:
		virtual void setInv (
			bool inverse
		)=0; // whether output is negated
		virtual void setRange (
			bool loRange, // 0.2uA mode
			bool hiRange // 20 uA mode
			// default is 2uA mode
		)=0;
		virtual ~Interface () {};
	private:
		Interface (
			FunctionUnit * parentFu,
			ifc ifcId
		) :
			parentFu(parentFu),
			ifcId(ifcId)
		{};
		virtual void calibrate()=0;
		virtual bool findBias (
			unsigned char & offsetCode
		)=0;
		bool findBiasHelper (
			unsigned char & code
		);
		virtual void binarySearch (
			unsigned char minCode,
			float minBest,
			unsigned char maxCode,
			float maxBest,
			unsigned char & finalCode
		)=0;
		bool binarySearchAvg (
			unsigned char minGainCode,
			float minBest,
			unsigned char maxGainCode,
			float maxBest,
			unsigned char & finalGainCode
		);
		float binarySearchMeas ();
		FunctionUnit * parentFu;
		ifc ifcId;

		bool inverse = false;

		bool loRange = false;
		bool hiRange = false;

		// float midOffset = 0.0;
		// float hiOffset = 0.0;

		unsigned char loOffsetCode = 31;
		unsigned char midOffsetCode = 31;
		unsigned char hiOffsetCode = 31;
		bool calEn = false; /*Set high to configure integrator for output offset calibration*/
};

class Fabric::Chip::Tile::Slice::FunctionUnit::GenericInterface : public Fabric::Chip::Tile::Slice::FunctionUnit::Interface {
	friend ChipInput;
	friend ChipOutput;
	friend TileInOut;
	friend Fanout;
	friend Integrator;

	private:
		GenericInterface (
			FunctionUnit * parentFu_,
			ifc ifcId_
		) :
			Interface (parentFu_, ifcId_)
		{};
		~GenericInterface () {};
		void setInv ( bool inverse ) { error("not implemented"); }; // whether output is negated
		void setRange ( bool loRange, bool hiRange ) { error("not implemented"); };
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
		) { error("not implemented"); };
};