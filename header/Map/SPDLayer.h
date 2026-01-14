#ifndef _SM_MAP_SPDLAYER
#define _SM_MAP_SPDLAYER
#include "Data/ArrayListInt64.h"
#include "Map/MapDrawLayer.h"
#include "Sync/Mutex.h"

namespace Map
{
	class SPDLayer : public MapDrawLayer
	{
	private:
		typedef struct
		{
			UInt32 objCnt;
			Math::Coord2D<Int32> blk;
			UInt32 sofst;
			Int32 *ids;
		} SPDBlock;

		//IO::FileStream *cip;
		UInt32 *ofsts;
		UInt32 nblks;
		Int32 blkScale;
		Bool missFile;
		SPDBlock *blks;
		UOSInt maxTextSize;
		Int64 maxId;
		Map::DrawLayerType lyrType;
//		Sync::Mutex mut;
		const UTF8Char *layerName;

	public:
		SPDLayer(Text::CStringNN layerName);
		virtual ~SPDLayer();

		Bool IsError() const;

		virtual DrawLayerType GetLayerType() const;
		virtual UOSInt GetAllObjectIds(NN<Data::ArrayListInt64> outArr, OptOut<Optional<NameArray>> nameArr);
		virtual UOSInt GetObjectIds(NN<Data::ArrayListInt64> outArr, OptOut<Optional<NameArray>> nameArr, Double mapRate, Math::RectArea<Int32> rect, Bool keepEmpty);
		virtual UOSInt GetObjectIdsMapXY(NN<Data::ArrayListInt64> outArr, OptOut<Optional<NameArray>> nameArr, Math::RectAreaDbl rect, Bool keepEmpty);
		virtual Int64 GetObjectIdMax() const;
		virtual UOSInt GetRecordCnt() const;
		virtual void ReleaseNameArr(Optional<NameArray> nameArr);
		virtual Bool GetString(NN<Text::StringBuilderUTF8> sb, Optional<NameArray> nameArr, Int64 id, UOSInt strIndex);
		virtual UOSInt GetColumnCnt() const;
		virtual UnsafeArrayOpt<UTF8Char> GetColumnName(UnsafeArray<UTF8Char> buff, UOSInt colIndex) const;
		virtual DB::DBUtil::ColType GetColumnType(UOSInt colIndex, OptOut<UOSInt> colSize) const;
		virtual Bool GetColumnDef(UOSInt colIndex, NN<DB::ColDef> colDef) const;
		virtual Int32 GetBlockSize() const;
		virtual UInt32 GetCodePage() const;
		virtual Bool GetBounds(OutParam<Math::RectAreaDbl> bounds) const;

		virtual NN<GetObjectSess> BeginGetObject();
		virtual void EndGetObject(NN<GetObjectSess> session);
		virtual Optional<Math::Geometry::Vector2D> GetNewVectorById(NN<GetObjectSess> session, Int64 id);
		virtual UOSInt GetGeomCol() const;

		virtual ObjectClass GetObjectClass() const;
	};
}
#endif
