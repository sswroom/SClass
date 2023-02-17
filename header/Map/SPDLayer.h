#ifndef _SM_MAP_SPDLAYER
#define _SM_MAP_SPDLAYER
#include "Data/ArrayList.h"
#include "Data/ArrayListInt32.h"
#include "Map/IMapDrawLayer.h"
#include "Sync/Mutex.h"

namespace Map
{
	class SPDLayer : public IMapDrawLayer
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
		SPDLayer(Text::CString layerName);
		virtual ~SPDLayer();

		Bool IsError();

		virtual DrawLayerType GetLayerType();
		virtual UOSInt GetAllObjectIds(Data::ArrayListInt64 *outArr, NameArray **nameArr);
		virtual UOSInt GetObjectIds(Data::ArrayListInt64 *outArr, NameArray **nameArr, Double mapRate, Math::RectArea<Int32> rect, Bool keepEmpty);
		virtual UOSInt GetObjectIdsMapXY(Data::ArrayListInt64 *outArr, NameArray **nameArr, Math::RectAreaDbl rect, Bool keepEmpty);
		virtual Int64 GetObjectIdMax();
		virtual void ReleaseNameArr(NameArray *nameArr);
		virtual UTF8Char *GetString(UTF8Char *buff, UOSInt buffSize, NameArray *nameArr, Int64 id, UOSInt strIndex);
		virtual UOSInt GetColumnCnt();
		virtual UTF8Char *GetColumnName(UTF8Char *buff, UOSInt colIndex);
		virtual DB::DBUtil::ColType GetColumnType(UOSInt colIndex, UOSInt *colSize);
		virtual Bool GetColumnDef(UOSInt colIndex, DB::ColDef *colDef);
		virtual Int32 GetBlockSize();
		virtual UInt32 GetCodePage();
		virtual Bool GetBounds(Math::RectAreaDbl *bounds);

		virtual GetObjectSess *BeginGetObject();
		virtual void EndGetObject(GetObjectSess *session);
		virtual Math::Geometry::Vector2D *GetNewVectorById(GetObjectSess *session, Int64 id);

		virtual ObjectClass GetObjectClass();
	};
}
#endif
