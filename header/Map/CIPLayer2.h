#ifndef _SM_MAP_CIPLAYER2
#define _SM_MAP_CIPLAYER2
#include "Data/FastMap.h"
#include "Map/MapDrawLayer.h"
#include "Sync/Mutex.h"
#include "Text/String.h"

namespace Map
{
	class CIPLayer2 : public MapDrawLayer
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
		NotNullPtr<Text::String> layerName;
		Int64 maxId;

		Data::FastMap<Int32, CIPFileObject*> *lastObjs;
		Data::FastMap<Int32, CIPFileObject*> *currObjs;

		Sync::Mutex mut;
	public:
		CIPLayer2(Text::CStringNN layerName);
		virtual ~CIPLayer2();

		Bool IsError();

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

	private:
		CIPFileObject *GetFileObject(void *session, Int32 id);
		void ReleaseFileObjs(Data::FastMap<Int32, CIPFileObject*> *objs);

	public:
		virtual GetObjectSess *BeginGetObject();
		virtual void EndGetObject(GetObjectSess *session);
		virtual Math::Geometry::Vector2D *GetNewVectorById(GetObjectSess *session, Int64 id);

		virtual ObjectClass GetObjectClass() const;

	};
}
#endif
