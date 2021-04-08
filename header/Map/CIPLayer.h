#ifndef _SM_MAP_CIPLAYER
#define _SM_MAP_CIPLAYER
#include "Map/IMapDrawLayer.h"
#include "Sync/Mutex.h"

namespace Map
{
	class CIPLayer : public IMapDrawLayer
	{
	private:
		typedef struct
		{
			Int32 objCnt;
			Int32 xblk;
			Int32 yblk;
			Int32 sofst;
			Int32 *ids;
		} CIPBlock;

		typedef struct
		{
			Int32 id;
			Int32 nParts;
			Int32 *parts;
			Int32 nPoints;
			Int32 *points;
		} CIPFileObject;

		//IO::FileStream *cip;
		Int32 *ids;
		Int32 *ofsts;
		UInt32 nblks;
		Int32 blkScale;
		Bool missFile;
		CIPBlock *blks;
		Int32 maxTextSize;
		Map::DrawLayerType lyrType;
		const UTF8Char *layerName;
		Int64 maxId;

		Data::Int32Map<CIPFileObject*> *lastObjs;
		Data::Int32Map<CIPFileObject*> *currObjs;

		Sync::Mutex *mut;
	public:
		CIPLayer(const UTF8Char *layerName);
		virtual ~CIPLayer();

		Bool IsError();
		virtual const UTF8Char *GetName();

		virtual DrawLayerType GetLayerType();
		virtual UOSInt GetAllObjectIds(Data::ArrayListInt64 *outArr, void **nameArr);
		virtual UOSInt GetObjectIds(Data::ArrayListInt64 *outArr, void **nameArr, Double mapRate, Int32 x1, Int32 y1, Int32 x2, Int32 y2, Bool keepEmpty);
		virtual UOSInt GetObjectIdsMapXY(Data::ArrayListInt64 *outArr, void **nameArr, Double x1, Double y1, Double x2, Double y2, Bool keepEmpty);
		virtual Int64 GetObjectIdMax();
		virtual void ReleaseNameArr(void *nameArr);
		virtual WChar *GetString(WChar *buff, void *nameArr, Int64 id, UOSInt strIndex);
		virtual UOSInt GetColumnCnt();
		virtual WChar *GetColumnName(WChar *buff, UOSInt colIndex);
		virtual DB::DBUtil::ColType GetColumnType(UOSInt colIndex, UOSInt *colSize);
		virtual Bool GetColumnDef(UOSInt colIndex, DB::ColDef *colDef);
		virtual Int32 GetBlockSize();
		virtual Int32 GetCodePage();
		virtual void GetBoundsDbl(Double *minX, Double *minY, Double *maxX, Double *maxY);

	private:
		CIPFileObject *GetFileObject(void *session, Int32 id);
		void ReleaseFileObjs(Data::Int32Map<CIPFileObject*> *objs);

	public:
		virtual void *BeginGetObject();
		virtual void EndGetObject(void *session);
		virtual DrawObjectL *GetObjectByIdN(void *session, Int64 id);
		virtual Math::Vector2D *GetVectorById(void *session, Int64 id);
		virtual void ReleaseObject(void *session, DrawObjectL *obj);

		virtual ObjectClass GetObjectClass();
	};
}
#endif
