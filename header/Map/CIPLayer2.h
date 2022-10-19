#ifndef _SM_MAP_CIPLAYER2
#define _SM_MAP_CIPLAYER2
#include "Data/FastMap.h"
#include "Map/IMapDrawLayer.h"
#include "Sync/Mutex.h"
#include "Text/String.h"

namespace Map
{
	class CIPLayer2 : public IMapDrawLayer
	{
	private:
		typedef struct
		{
			UInt32 objCnt;
			Math::Coord2D<Int32> blk;
			UInt32 sofst;
			Int32 *ids;
		} CIPBlock;

		typedef struct
		{
			Int32 id;
			UInt32 nPtOfst;
			UInt32 *ptOfstArr;
			UInt32 nPoint;
			Int32 *pointArr;
		} CIPFileObject;

		//IO::FileStream *cip;
		Int32 *ids;
		UInt32 *ofsts;
		UInt32 nblks;
		Int32 blkScale;
		Bool missFile;
		CIPBlock *blks;
		UOSInt maxTextSize;
		Map::DrawLayerType lyrType;
		Text::String *layerName;
		Int64 maxId;

		Data::FastMap<Int32, CIPFileObject*> *lastObjs;
		Data::FastMap<Int32, CIPFileObject*> *currObjs;

		Sync::Mutex mut;
	public:
		CIPLayer2(Text::CString layerName);
		virtual ~CIPLayer2();

		Bool IsError();

		virtual DrawLayerType GetLayerType();
		virtual UOSInt GetAllObjectIds(Data::ArrayListInt64 *outArr, void **nameArr);
		virtual UOSInt GetObjectIds(Data::ArrayListInt64 *outArr, void **nameArr, Double mapRate, Math::RectArea<Int32> rect, Bool keepEmpty);
		virtual UOSInt GetObjectIdsMapXY(Data::ArrayListInt64 *outArr, void **nameArr, Math::RectAreaDbl rect, Bool keepEmpty);
		virtual Int64 GetObjectIdMax();
		virtual void ReleaseNameArr(void *nameArr);
		virtual UTF8Char *GetString(UTF8Char *buff, UOSInt buffSize, void *nameArr, Int64 id, UOSInt strIndex);
		virtual UOSInt GetColumnCnt();
		virtual UTF8Char *GetColumnName(UTF8Char *buff, UOSInt colIndex);
		virtual DB::DBUtil::ColType GetColumnType(UOSInt colIndex, UOSInt *colSize);
		virtual Bool GetColumnDef(UOSInt colIndex, DB::ColDef *colDef);
		virtual Int32 GetBlockSize();
		virtual UInt32 GetCodePage();
		virtual Bool GetBounds(Math::RectAreaDbl *bounds);

	private:
		CIPFileObject *GetFileObject(void *session, Int32 id);
		void ReleaseFileObjs(Data::FastMap<Int32, CIPFileObject*> *objs);

	public:
		virtual void *BeginGetObject();
		virtual void EndGetObject(void *session);
		virtual Math::Geometry::Vector2D *GetNewVectorById(void *session, Int64 id);

		virtual ObjectClass GetObjectClass();

	};
}
#endif
