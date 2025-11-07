#ifndef _SM_MAP_VECTORLAYER
#define _SM_MAP_VECTORLAYER
#include "Data/ArrayListArr.h"
#include "Data/ArrayListNN.h"
#include "Map/MapDrawLayer.h"
#include "Text/PString.h"
#include "Text/String.h"
#include <cstddef>

namespace Map
{
	class VectorLayer : public Map::MapDrawLayer
	{
	public:
		struct ColInfo
		{
			DB::DBUtil::ColType colType;
			UOSInt colSize;
			UOSInt colDP;

			ColInfo(std::nullptr_t)
			{
				this->colType = DB::DBUtil::CT_VarUTF8Char;
				this->colSize = 256;
				this->colDP = 0;
			}

			ColInfo(DB::DBUtil::ColType colType, UOSInt colSize, UOSInt colDP)
			{
				this->colType = colType;
				this->colSize = colSize;
				this->colDP = colDP;
			}

			Bool operator==(const ColInfo &v)
			{
				return this->colType == v.colType && this->colSize == v.colSize && this->colDP == v.colDP;
			}
		};

	private:
		Map::DrawLayerType layerType;
		UnsafeArray<NN<Text::String>> colNames;
		UOSInt strCnt;
		UOSInt *maxStrLen;
		UOSInt *thisStrLen;
		Data::ArrayListNN<Math::Geometry::Vector2D> vectorList;
		Data::ArrayListArr<UnsafeArrayOpt<const UTF8Char>> strList;
		Math::Coord2DDbl min;
		Math::Coord2DDbl max;
		ColInfo *cols;
		Double mapRate;
		MixedData mixedData;
		Text::String *tableName;

	private:
		UnsafeArray<UnsafeArrayOpt<const UTF8Char>> CopyStrs(UnsafeArray<UnsafeArrayOpt<const UTF8Char>> strs);
		UnsafeArray<UnsafeArrayOpt<const UTF8Char>> CopyStrs(Text::String **strs);
		UnsafeArray<UnsafeArrayOpt<const UTF8Char>> CopyStrs(Text::PString *strs);
		UnsafeArray<UnsafeArrayOpt<const UTF8Char>> CopyStrs(NN<Data::ArrayListStringNN> strs);
		void UpdateMapRate();
	public:
		VectorLayer(Map::DrawLayerType layerType, NN<Text::String> sourceName, NN<Math::CoordinateSystem> csys, Optional<Text::String> layerName);
		VectorLayer(Map::DrawLayerType layerType, Text::CStringNN sourceName, NN<Math::CoordinateSystem> csys, Text::CString layerName);
		VectorLayer(Map::DrawLayerType layerType, NN<Text::String> sourceName, UOSInt strCnt, UnsafeArray<UnsafeArrayOpt<const UTF8Char>> colNames, NN<Math::CoordinateSystem> csys, UOSInt nameCol, Optional<Text::String> layerName);
		VectorLayer(Map::DrawLayerType layerType, Text::CStringNN sourceName, UOSInt strCnt, UnsafeArray<UnsafeArrayOpt<const UTF8Char>> colNames, NN<Math::CoordinateSystem> csys, UOSInt nameCol, Text::CString layerName);
		VectorLayer(Map::DrawLayerType layerType, NN<Text::String> sourceName, UOSInt strCnt, UnsafeArray<UnsafeArrayOpt<const UTF8Char>> colNames, NN<Math::CoordinateSystem> csys, DB::DBUtil::ColType *colTypes, UOSInt *colSize, UOSInt *colDP, UOSInt nameCol, Optional<Text::String> layerName);
		VectorLayer(Map::DrawLayerType layerType, Text::CStringNN sourceName, UOSInt strCnt, UnsafeArray<UnsafeArrayOpt<const UTF8Char>> colNames, NN<Math::CoordinateSystem> csys, DB::DBUtil::ColType *colTypes, UOSInt *colSize, UOSInt *colDP, UOSInt nameCol, Text::CString layerName);
		VectorLayer(Map::DrawLayerType layerType, Text::CStringNN sourceName, NN<Data::ArrayListStringNN> colNames, NN<Math::CoordinateSystem> csys, NN<Data::ArrayList<ColInfo>> colInfos, UOSInt nameCol, Text::CString layerName);
		virtual ~VectorLayer();

		virtual DrawLayerType GetLayerType() const;
		virtual void SetMixedData(MixedData mixedData);
		virtual UOSInt GetAllObjectIds(NN<Data::ArrayListInt64> outArr, OptOut<Optional<NameArray>> nameArr);
		virtual UOSInt GetObjectIds(NN<Data::ArrayListInt64> outArr, OptOut<Optional<NameArray>> nameArr, Double mapRate, Math::RectArea<Int32> rect, Bool keepEmpty);
		virtual UOSInt GetObjectIdsMapXY(NN<Data::ArrayListInt64> outArr, OptOut<Optional<NameArray>> nameArr, Math::RectAreaDbl rect, Bool keepEmpty);
		virtual Int64 GetObjectIdMax() const;
		virtual UOSInt GetRecordCnt() const;
		virtual void ReleaseNameArr(Optional<NameArray> nameArr);
		virtual Bool GetString(NN<Text::StringBuilderUTF8> sb, Optional<NameArray> nameArr, Int64 id, UOSInt colIndex);
		virtual UOSInt GetColumnCnt() const;
		virtual UnsafeArrayOpt<UTF8Char> GetColumnName(UnsafeArray<UTF8Char> buff, UOSInt colIndex);
		virtual DB::DBUtil::ColType GetColumnType(UOSInt colIndex, OptOut<UOSInt> colSize);
		virtual Bool GetColumnDef(UOSInt colIndex, NN<DB::ColDef> colDef);
		virtual UInt32 GetCodePage() const;
		virtual Bool GetBounds(OutParam<Math::RectAreaDbl> rect) const;

		virtual NN<GetObjectSess> BeginGetObject();
		virtual void EndGetObject(NN<GetObjectSess> session);
		virtual Optional<Math::Geometry::Vector2D> GetNewVectorById(NN<GetObjectSess> session, Int64 id);

		void SetTableName(Text::String *tableName);
		virtual UOSInt QueryTableNames(Text::CString schemaName, NN<Data::ArrayListStringNN> names);
		virtual UOSInt GetGeomCol() const;

		virtual ObjectClass GetObjectClass() const;
		Bool VectorValid(NN<Math::Geometry::Vector2D> vec);
		Int64 AddVector2(NN<Math::Geometry::Vector2D> vec, Text::String **strs); //-1 = fail
		Int64 AddVector2(NN<Math::Geometry::Vector2D> vec, Text::PString *strs); //-1 = fail
		Int64 AddVector2(NN<Math::Geometry::Vector2D> vec, UnsafeArray<UnsafeArrayOpt<const UTF8Char>> strs); //-1 = fail
		Int64 AddVector2(NN<Math::Geometry::Vector2D> vec, NN<Data::ArrayListStringNN> strs); //-1 = fail
		Bool SplitPolyline(Math::Coord2DDbl pt);
		void OptimizePolylinePath();
		void ReplaceVector(Int64 id, NN<Math::Geometry::Vector2D> vec);
		Bool DeleteVector(Int64 id);
		void ConvCoordinateSystem(NN<Math::CoordinateSystem> csys);
		void SwapXY();
	};
}
#endif
