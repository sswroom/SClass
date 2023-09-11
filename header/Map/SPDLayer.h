#ifndef _SM_MAP_SPDLAYER
#define _SM_MAP_SPDLAYER
#include "Data/ArrayList.h"
#include "Data/ArrayListInt32.h"
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
		virtual UOSInt GetAllObjectIds(NotNullPtr<Data::ArrayListInt64> outArr, NameArray **nameArr);
		virtual UOSInt GetObjectIds(NotNullPtr<Data::ArrayListInt64> outArr, NameArray **nameArr, Double mapRate, Math::RectArea<Int32> rect, Bool keepEmpty);
		virtual UOSInt GetObjectIdsMapXY(NotNullPtr<Data::ArrayListInt64> outArr, NameArray **nameArr, Math::RectAreaDbl rect, Bool keepEmpty);
		virtual Int64 GetObjectIdMax() const;
		virtual void ReleaseNameArr(NameArray *nameArr);
		virtual UTF8Char *GetString(UTF8Char *buff, UOSInt buffSize, NameArray *nameArr, Int64 id, UOSInt strIndex);
		virtual UOSInt GetColumnCnt() const;
		virtual UTF8Char *GetColumnName(UTF8Char *buff, UOSInt colIndex);
		virtual DB::DBUtil::ColType GetColumnType(UOSInt colIndex, UOSInt *colSize);
		virtual Bool GetColumnDef(UOSInt colIndex, NotNullPtr<DB::ColDef> colDef);
		virtual Int32 GetBlockSize() const;
		virtual UInt32 GetCodePage() const;
		virtual Bool GetBounds(OutParam<Math::RectAreaDbl> bounds) const;

		virtual GetObjectSess *BeginGetObject();
		virtual void EndGetObject(GetObjectSess *session);
		virtual Math::Geometry::Vector2D *GetNewVectorById(GetObjectSess *session, Int64 id);

		virtual ObjectClass GetObjectClass() const;
	};
}
#endif
