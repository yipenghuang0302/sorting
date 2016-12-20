class Fabric::Chip::Connection {
	friend Vector;

	public:
		Connection (
			Chip::Tile::Slice::FunctionUnit::Interface * sourceIfc,
			Chip::Tile::Slice::FunctionUnit::Interface * destIfc
		) :
			chip(*sourceIfc->parentFu->parentSlice->parentTile->parentChip),
			sourceSlice(*sourceIfc->parentFu->parentSlice),
			sourceUnitId(sourceIfc->parentFu->unitId),
			sourceIfcId(sourceIfc->ifcId),
			destSlice(*destIfc->parentFu->parentSlice),
			destUnitId(destIfc->parentFu->unitId),
			destIfcId(destIfc->ifcId)
		{
			if (
				sourceIfc->parentFu->parentSlice->parentTile->parentChip->chipRowId != destIfc->parentFu->parentSlice->parentTile->parentChip->chipRowId ||
				sourceIfc->parentFu->parentSlice->parentTile->parentChip->chipColId != destIfc->parentFu->parentSlice->parentTile->parentChip->chipColId
			) error("cannot make connections between different chips");
		};
		/*Generate selRow, selCol, selLine, setBit*/
		/*Set connection*/
		void setConn ();
		/*Break connection*/
		void brkConn ();
	private:
		Chip & chip;
		Chip::Tile::Slice & sourceSlice;
		unit sourceUnitId;
		ifc sourceIfcId;

		Chip::Tile::Slice & destSlice;
		unit destUnitId;
		ifc destIfcId;
};