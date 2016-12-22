typedef enum {
	mRngMGain = 0, /* -2 to 2  uA input, gain = 1 */
	lRngMGain = 1, /*-.2 to .2 uA input, gain = 1 */
	hRngMGain = 2, /*-20 to 20 uA input, gain = 1 */
	mRngHGain = 3, /* -2 to 2  uA input, gain = 10*/
	lRngHGain = 4, /*-.2 to .2 uA input, gain = 10*/
	mRngLGain = 5, /* -2 to 2  uA input, gain = .1*/
	hRngLGain = 6  /*-20 to 20 uA input, gain = .1*/
} intRange;

class Fabric::Chip::Tile::Slice::Integrator : public Fabric::Chip::Tile::Slice::FunctionUnit {
	friend Slice;

	public:
		void setEnable ( bool enable );
		void setGain (
			bool loGain, // *0.1 mode
			bool hiGain // *10 mode
			// not both of the range settings should be true
			// default is *1 mode
			// this setting allows multiplication by a constant coefficient
			// this setting also allows us to speed up or slow down a simulation
		);
		void setInitial (
			unsigned char initialCondition // fixed point representation of initial condition
		);
		void setException (
			bool exception // turn on overflow detection
			// turning false overflow detection saves power if it is known to be unnecessary
		);
	private:
		class IntegratorInterface;
		class IntegratorIn;
		class IntegratorOut;
		Integrator (Slice * parentSlice);
		~Integrator () { delete in0; delete out0; };
		bool calibrate ();
		/*Set enable, invert, range*/
		void setParam0 ();
		/*Set calIc, overflow enable*/
		void setParam1 ();
		/*Set initial condition*/
		void setParam2 ();
		/*Set calOutOs, calOutEn*/
		void setParam3 ();
		/*Set calInOs, calInEn*/
		void setParam4 ();
		void setParam5 () {};
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
			unsigned char minCode,
			float minBest,
			unsigned char maxCode,
			float maxBest,
			unsigned char & finalCode
		);
		unsigned char initialCondition = 128; // fixed point representation of initial condition
		bool exception = false; // turn on overflow detection
		bool loGain = false;
		bool hiGain = false;
		unsigned char loNegGainCode = 31;
		unsigned char hiNegGainCode = 31;
		const unsigned char anaIrefPmos = 6;
};

class Fabric::Chip::Tile::Slice::Integrator::IntegratorInterface : public Fabric::Chip::Tile::Slice::FunctionUnit::Interface {
	friend Integrator;

	private:
		IntegratorInterface (Integrator * parentFu, ifc ifcId) : Interface(parentFu, ifcId), parentIntegrator(parentFu) {};
		void calibrate();
		bool findBias (
			unsigned char & offsetCode
		) {
			error("not implemented");
			return false;
		};
		void binarySearch (
			unsigned char minOffsetCode,
			float minBest,
			unsigned char maxOffsetCode,
			float maxBest,
			unsigned char & finalOffsetCode
		);
		Integrator * parentIntegrator;
};

class Fabric::Chip::Tile::Slice::Integrator::IntegratorIn : public Fabric::Chip::Tile::Slice::Integrator::IntegratorInterface {
	friend Integrator;

	public:
		void setRange (
			bool loRange, // 0.2uA mode
			bool hiRange // 20 uA mode
			// not both of the range settings should be true
			// default is 2uA mode
			// this setting should match the unit that gives the input to the integrator
		);
	private:
		IntegratorIn (Integrator * parentFu) : IntegratorInterface(parentFu, in0Id) {};
		void setInv ( bool inverse ) { error("not implemented"); }; // whether output is negated
};

class Fabric::Chip::Tile::Slice::Integrator::IntegratorOut : public Fabric::Chip::Tile::Slice::Integrator::IntegratorInterface {
	friend Integrator;

	public:
		void setInv ( bool inverse ); // whether output is negated
		void setRange (
			bool loRange, // 0.2uA mode
			bool hiRange // 20 uA mode
			// not both of the range settings should be true
			// default is 2uA mode
			// this setting should match the unit that gives the input to the integrator
		);
	private:
		IntegratorOut (Integrator * parentFu) : IntegratorInterface(parentFu, out0Id) {};
};