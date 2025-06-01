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
		NN<Text::String> layerName;
		Int64 maxId;

		Optional<Data::FastMapNN<Int32, CIPFileObject>> lastObjs;
		Optional<Data::FastMapNN<Int32, CIPFileObject>> currObjs;

		Sync::Mutex mut;
	public:
		CIPLayer2(Text::CStringNN layerName);
		virtual ~CIPLayer2();

		virtual Bool IsError() const;

		virtual DrawLayerType GetLayerType() const;
		virtual UOSInt GetAllObjectIds(NN<Data::ArrayListInt64> outArr, OptOut<Optional<NameArray>> nameArr);
		virtual UOSInt GetObjectIds(NN<Data::ArrayListInt64> outArr, OptOut<Optional<NameArray>> nameArr, Double mapRate, Math::RectArea<Int32> rect, Bool keepEmpty);
		virtual UOSInt GetObjectIdsMapXY(NN<Data::ArrayListInt64> outArr, OptOut<Optional<NameArray>> nameArr, Math::RectAreaDbl rect, Bool keepEmpty);
		virtual Int64 GetObjectIdMax() const;
		virtual void ReleaseNameArr(Optional<NameArray> nameArr);
		virtual Bool GetString(NN<Text::StringBuilderUTF8> sb, Optional<NameArray> nameArr, Int64 id, UOSInt strIndex);
		virtual UOSInt GetColumnCnt() const;
		virtual UnsafeArrayOpt<UTF8Char> GetColumnName(UnsafeArray<UTF8Char> buff, UOSInt colIndex);
		virtual DB::DBUtil::ColType GetColumnType(UOSInt colIndex, OptOut<UOSInt> colSize);
		virtual Bool GetColumnDef(UOSInt colIndex, NN<DB::ColDef> colDef);
		virtual Int32 GetBlockSize() const;
		virtual UInt32 GetCodePage() const;
		virtual Bool GetBounds(OutParam<Math::RectAreaDbl> bounds) const;

	private:
		Optional<CIPFileObject> GetFileObject(NN<GetObjectSess> session, Int32 id);
		void ReleaseFileObjs(NN<Data::FastMapNN<Int32, CIPFileObject>> objs);

	public:
		virtual NN<GetObjectSess> BeginGetObject();
		virtual void EndGetObject(NN<GetObjectSess> session);
		virtual Optional<Math::Geometry::Vector2D> GetNewVectorById(NN<GetObjectSess> session, Int64 id);
		virtual UOSInt GetGeomCol() const;

		virtual ObjectClass GetObjectClass() const;

	};
}
#endif
