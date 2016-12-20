class Fabric::Chip::Tile::Slice {
	friend Tile;
	friend Connection;
	friend Vector;

	public:
		class FunctionUnit;
		class ChipInput;
		class TileInOut;
		class Dac;
		class Multiplier;
		class Integrator;
		class Fanout;
		class Adc;
		class LookupTable;
		class ChipOutput;

		ChipInput * chipInput;
		TileInOut * tileInps;
		Dac * dac;
		Multiplier * muls;
		Integrator * integrator;
		Fanout * fans;
		Adc * adc;
		LookupTable * lut;
		TileInOut * tileOuts;
		ChipOutput * chipOutput;

	private:
		Slice (
			Tile * parentTile,
			slice sliceId,
			unsigned char anaOutPosPin,
			unsigned char anaOutNegPin
		);
		~Slice ();
		bool calibrate();
		Tile * parentTile;
		slice sliceId;
		/*ANALOG INPUT PINS*/
		unsigned char anaOutPosPin; /*ANALOG OUTAna*/
		unsigned char anaOutNegPin; /*ANALOG OUTAna*/
};