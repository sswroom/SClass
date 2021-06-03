#ifndef _SM_MAP_CIPLAYER2
#define _SM_MAP_CIPLAYER2
#include "Sync/Mutex.h"
#include "Map/IMapDrawLayer.h"
#include "Data/Int32Map.h"

namespace Map
{
	class CIPLayer2 : public IMapDrawLayer
	{
	private:
		typedef struct
		{
			UInt32 objCnt;
			Int32 xblk;
			Int32 yblk;
			Int32 sofst;
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
		Int32 *ofsts;
		UInt32 nblks;
		Int32 blkScale;
		Bool missFile;
		CIPBlock *blks;
		UOSInt maxTextSize;
		Map::DrawLayerType lyrType;
		const UTF8Char *layerName;
		Int64 maxId;

		Data::Int32Map<CIPFileObject*> *lastObjs;
		Data::Int32Map<CIPFileObject*> *currObjs;

		Sync::Mutex *mut;
	public:
		CIPLayer2(const UTF8Char *layerName);
		virtual ~CIPLayer2();

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

	private:
		CIPFileObject *GetFileObject(void *session, Int32 id);
		void ReleaseFileObjs(Data::Int32Map<CIPFileObject*> *objs);

	public:
		virtual void *BeginGetObject();
		virtual void EndGetObject(void *session);
		virtual DrawObjectL *GetObjectByIdD(void *session, Int64 id);
		virtual Math::Vector2D *GetVectorById(void *session, Int64 id);
		virtual void ReleaseObject(void *session, DrawObjectL *obj);

		virtual ObjectClass GetObjectClass();

	};
}
#endif
