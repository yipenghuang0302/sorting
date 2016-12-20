class Fabric::Chip::Tile {
	friend Fabric;
	friend Chip;

	public:
		class Slice;
		bool calibrate();
		void setParallelIn ( bool onOff );
		/*Read serial digital data and the done bit*/
		void readSerial (
			unsigned char & adcData0,
			unsigned char & adcData1,
			bool & done
		);
		/*Read the exception bits*/
		void readExp (
			unsigned char & expVector
		);
		void spiDriveTile (
			unsigned char selRow,
			unsigned char selCol,
			unsigned char selLine,
			unsigned char cfgTile
		);

		Slice * slices;

	private:
		Tile (
			Chip * parentChip,
			tileRow tileRowId,
			tileCol tileColId,
			unsigned char spiSSPin,
			unsigned char spiMisoPin,
			unsigned char anaOutPinBase
		);
		~Tile();

		void controllerHelperTile (unsigned char selLine, unsigned char cfgTile);
		int spiDriveTile ( bool * vector );

		Chip * parentChip;

		tileRow tileRowId;
		tileCol tileColId;
		unsigned char spiSSPin; /*spi slave select*/
		unsigned char spiMisoPin; /*spi master in slave out*/

		bool slice0DacOverride = false;
		bool slice1DacOverride = false;
		bool slice2DacOverride = false;
		bool slice3DacOverride = false;
};