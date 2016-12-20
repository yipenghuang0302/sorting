/*Properties of macro tiles.*/
typedef enum {
	tileRow0 = 0,
	tileRow1 = 1
} tileRow;

typedef enum {
	tileCol0 = 0,
	tileCol1 = 1
} tileCol;

/*Valid options for functional unit slice.*/
typedef enum {
	slice0,
	slice1,
	slice2,
	slice3
} slice;

/*Valid options for functional unit.*/
typedef enum {
	chipInp,
	tileInp0,
	tileInp1,
	tileInp2,
	tileInp3,
	unitDac,
	unitMulL,
	unitMulR,
	unitInt,
	unitFanL,
	unitFanR,
	unitAdc,
	unitLut,
	tileOut0,
	tileOut1,
	tileOut2,
	tileOut3,
	chipOut
} unit;

class Fabric::Chip {
	friend Fabric;

	/*Connection specifies source FU interface and destination FU interface*/
	public:
		class Tile;
		class Connection;
		class Vector;

		void reset();
		bool calibrate();

		void writeParallel ( unsigned char data );
		unsigned char readParallel ();

		Tile * tiles;

	private:
		Chip(
			Fabric * parentFabric,
			chipRow chipRowId,
			chipCol chipColId,
			unsigned char tpSelChipSelPin,
			unsigned char tpSel0RowSelPin,
			unsigned char tpSel1ColSelPin,
			unsigned char spiSSPinBase,
			unsigned char spiMisoPinBase,
			unsigned char anaOutPinBase
		);
		~Chip();

		Fabric * parentFabric;
		chipRow chipRowId;
		chipCol chipColId;

		unsigned char cfgTag [2][2][11][16][2]; // bit indicating configuration has changed
		bool cfgLutLTag [2][2]; // bit indicating configuration has changed
		bool cfgLutRTag [2][2]; // bit indicating configuration has changed
		unsigned char cfgBuf [2][2][11][16][16]; // buffer for all the configuration writes
		void cacheVec ( Vector vec );
		void writeVecs ();

		bool parallelInState = false;
		bool parallelOutState = false;
		tileRow parallelOutTileRow = tileRow0;
		tileCol parallelOutTileCol = tileCol0;
		slice parallelOutSlice = slice0;
		unit parallelOutUnit = unitAdc;
		void parallelHelper();
		unsigned char tpSelChipSelPin; /*tile parallel input output column select*/
		unsigned char tpSel1ColSelPin; /*tile parallel input output column select*/
		unsigned char tpSel0RowSelPin; /*tile parallel input output row select*/

		void controllerHelperChip (
			unsigned char selLine,
			unsigned char cfgTile
		);
		void spiDriveChip (
			unsigned char selRow,
			unsigned char selCol,
			unsigned char selLine,
			unsigned char cfgTile
		);
		int spiDriveChip ( bool * vector );

		unsigned char spiSSPinBase;
		unsigned char spiMisoPinBase;
		unsigned char anaOutPinBase;

		// TODO
		/*HCDC DIGITAL INPUT PINS*/
		const unsigned char tdiClkPin = 33; /*trigger signal*/
		const unsigned char tdi0Pin = 35;
		const unsigned char tdi1Pin = 37;
		const unsigned char tdi2Pin = 39;
		const unsigned char tdi3Pin = 41;
		const unsigned char tdi4Pin = 43;
		const unsigned char tdi5Pin = 45;
		const unsigned char tdi6Pin = 47;
		const unsigned char tdi7Pin = 49;

		/*HCDC DIGITAL OUTPUT PINS*/
		const unsigned char tdoClkPin = 32; /*trigger interrupt, so out of order*/
		const unsigned char tdo0Pin = 34;
		const unsigned char tdo1Pin = 36;
		const unsigned char tdo2Pin = 38;
		const unsigned char tdo3Pin = 40;
		const unsigned char tdo4Pin = 42;
		const unsigned char tdo5Pin = 44;
		const unsigned char tdo6Pin = 46;
		const unsigned char tdo7Pin = 48;

};