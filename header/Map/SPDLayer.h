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
			Int32 xblk;
			Int32 yblk;
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
		Sync::Mutex *mut;
		const UTF8Char *layerName;

	public:
		SPDLayer(Text::CString layerName);
		virtual ~SPDLayer();

		Bool IsError();

		virtual DrawLayerType GetLayerType();
		virtual UOSInt GetAllObjectIds(Data::ArrayListInt64 *outArr, void **nameArr);
		virtual UOSInt GetObjectIds(Data::ArrayListInt64 *outArr, void **nameArr, Double mapRate, Int32 x1, Int32 y1, Int32 x2, Int32 y2, Bool keepEmpty);
		virtual UOSInt GetObjectIdsMapXY(Data::ArrayListInt64 *outArr, void **nameArr, Double x1, Double y1, Double x2, Double y2, Bool keepEmpty);
		virtual Int64 GetObjectIdMax();
		virtual void ReleaseNameArr(void *nameArr);
		virtual UTF8Char *GetString(UTF8Char *buff, UOSInt buffSize, void *nameArr, Int64 id, UOSInt strIndex);
		virtual UOSInt GetColumnCnt();
		virtual UTF8Char *GetColumnName(UTF8Char *buff, UOSInt colIndex);
		virtual DB::DBUtil::ColType GetColumnType(UOSInt colIndex, UOSInt *colSize);
		virtual Bool GetColumnDef(UOSInt colIndex, DB::ColDef *colDef);
		virtual Int32 GetBlockSize();
		virtual UInt32 GetCodePage();
		virtual Bool GetBoundsDbl(Double *minX, Double *minY, Double *maxX, Double *maxY);

		virtual void *BeginGetObject();
		virtual void EndGetObject(void *session);
		virtual DrawObjectL *GetObjectByIdD(void *session, Int64 id);
		virtual Math::Vector2D *GetVectorById(void *session, Int64 id);
		virtual void ReleaseObject(void *session, DrawObjectL *obj);

		virtual ObjectClass GetObjectClass();
	};
}
#endif
