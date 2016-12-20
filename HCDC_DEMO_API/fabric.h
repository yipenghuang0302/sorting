/*Properties of chips.*/
typedef enum {
	chipRow0 = 0
} chipRow;

typedef enum {
	chipCol0 = 0,
	chipCol1 = 1
} chipCol;

class Fabric {
	/*Connection specifies source FU interface and destination FU interface*/
	public:
		class Chip;
		class Vector;

		Fabric();
		~Fabric();
		void reset();
		bool calibrate();
		/*Configuration commit*/
		void cfgStart ();
		void cfgCommit ();
		void cfgStop ();
		void setTimeout ( unsigned int timeout );
		void execStart ();
		void execStop ();
		void serialOutReq ();
		void serialOutStop ();
		void expReq ();
		void expStop ();

		Chip * chips;

	private:
		bool cfgState = false;
		bool execState = false;
		bool dataState = false;
		bool expState = false;
		void stateMachine();
		void controllerHelperFabric (
			unsigned char selLine,
			unsigned char cfgTile
		);
		void spiDriveFabric (
			unsigned char selRow,
			unsigned char selCol,
			unsigned char selLine,
			unsigned char cfgTile
		);
		int spiDriveFabric ( bool * vector );
};