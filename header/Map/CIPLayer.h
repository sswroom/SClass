#ifndef _SM_MAP_CIPLAYER
#define _SM_MAP_CIPLAYER
#include "Data/FastMapNN.hpp"
#include "Map/MapDrawLayer.h"
#include "Sync/Mutex.h"

namespace Map
{
	class CIPLayer : public MapDrawLayer
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
			UnsafeArrayOpt<UInt32> parts;
			Int32 nPoints;
			UnsafeArray<Int32> points;
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
		NN<Text::String> layerName;
		Int64 maxId;

		NN<Data::Int32FastMapObj<Optional<CIPFileObject>>> lastObjs;
		NN<Data::Int32FastMapObj<Optional<CIPFileObject>>> currObjs;

		Sync::Mutex mut;
	public:
		CIPLayer(Text::CStringNN layerName);
		virtual ~CIPLayer();

		Bool IsError();
		virtual NN<Text::String> GetName();

		virtual DrawLayerType GetLayerType();
		virtual UIntOS GetAllObjectIds(NN<Data::ArrayListInt64> outArr, OptOut<Optional<NameArray>> nameArr);
		virtual UIntOS GetObjectIds(NN<Data::ArrayListInt64> outArr, OptOut<Optional<NameArray>> nameArr, Double mapRate, Int32 x1, Int32 y1, Int32 x2, Int32 y2, Bool keepEmpty);
		virtual UIntOS GetObjectIdsMapXY(NN<Data::ArrayListInt64> outArr, OptOut<Optional<NameArray>> nameArr, Double x1, Double y1, Double x2, Double y2, Bool keepEmpty);
		virtual Int64 GetObjectIdMax();
		virtual void ReleaseNameArr(Optional<NameArray> nameArr);
		virtual Bool GetString(NN<Text::StringBuilderUTF8> sb, Optional<NameArray> nameArr, Int64 id, UIntOS strIndex);
		virtual UIntOS GetColumnCnt();
		virtual UnsafeArrayOpt<UTF8Char> GetColumnName(UnsafeArray<UTF8Char> buff, UIntOS colIndex);
		virtual DB::DBUtil::ColType GetColumnType(UIntOS colIndex, OptOut<UIntOS> colSize);
		virtual Bool GetColumnDef(UIntOS colIndex, DB::ColDef *colDef);
		virtual Int32 GetBlockSize();
		virtual UInt32 GetCodePage();
		virtual Bool GetBoundsDbl(Double *minX, Double *minY, Double *maxX, Double *maxY);

	private:
		Optional<CIPFileObject> GetFileObject(NN<GetObjectSess> session, Int32 id);
		void ReleaseFileObjs(NN<Data::Int32FastMapObj<Optional<CIPFileObject>>> objs);

	public:
		virtual NN<GetObjectSess> BeginGetObject();
		virtual void EndGetObject(NN<GetObjectSess> session);
		virtual Optional<Math::Geometry::Vector2D> GetNewVectorById(NN<GetObjectSess> session, Int64 id);

		virtual ObjectClass GetObjectClass();
	};
}
#endif
