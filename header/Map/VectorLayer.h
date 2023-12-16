#ifndef _SM_MAP_VECTORLAYER
#define _SM_MAP_VECTORLAYER
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
		Text::String **colNames;
		UOSInt strCnt;
		UOSInt *maxStrLen;
		UOSInt *thisStrLen;
		Data::ArrayListNN<Math::Geometry::Vector2D> vectorList;
		Data::ArrayList<const UTF8Char **> strList;
		Math::Coord2DDbl min;
		Math::Coord2DDbl max;
		ColInfo *cols;
		Double mapRate;
		MixedData mixedData;
		Text::String *tableName;

	private:
		const UTF8Char **CopyStrs(const UTF8Char **strs);
		const UTF8Char **CopyStrs(Text::String **strs);
		const UTF8Char **CopyStrs(Text::PString *strs);
		const UTF8Char **CopyStrs(NotNullPtr<Data::ArrayListStringNN> strs);
		void UpdateMapRate();
	public:
		VectorLayer(Map::DrawLayerType layerType, NotNullPtr<Text::String> sourceName, UOSInt strCnt, const UTF8Char **colNames, NotNullPtr<Math::CoordinateSystem> csys, UOSInt nameCol, Text::String *layerName);
		VectorLayer(Map::DrawLayerType layerType, Text::CStringNN sourceName, UOSInt strCnt, const UTF8Char **colNames, NotNullPtr<Math::CoordinateSystem> csys, UOSInt nameCol, Text::CString layerName);
		VectorLayer(Map::DrawLayerType layerType, NotNullPtr<Text::String> sourceName, UOSInt strCnt, const UTF8Char **colNames, NotNullPtr<Math::CoordinateSystem> csys, DB::DBUtil::ColType *colTypes, UOSInt *colSize, UOSInt *colDP, UOSInt nameCol, Text::String *layerName);
		VectorLayer(Map::DrawLayerType layerType, Text::CStringNN sourceName, UOSInt strCnt, const UTF8Char **colNames, NotNullPtr<Math::CoordinateSystem> csys, DB::DBUtil::ColType *colTypes, UOSInt *colSize, UOSInt *colDP, UOSInt nameCol, Text::CString layerName);
		VectorLayer(Map::DrawLayerType layerType, Text::CStringNN sourceName, NotNullPtr<Data::ArrayListStringNN> colNames, NotNullPtr<Math::CoordinateSystem> csys, NotNullPtr<Data::ArrayList<ColInfo>> colInfos, UOSInt nameCol, Text::CString layerName);
		virtual ~VectorLayer();

		virtual DrawLayerType GetLayerType() const;
		virtual void SetMixedData(MixedData mixedData);
		virtual UOSInt GetAllObjectIds(NotNullPtr<Data::ArrayListInt64> outArr, NameArray **nameArr);
		virtual UOSInt GetObjectIds(NotNullPtr<Data::ArrayListInt64> outArr, NameArray **nameArr, Double mapRate, Math::RectArea<Int32> rect, Bool keepEmpty);
		virtual UOSInt GetObjectIdsMapXY(NotNullPtr<Data::ArrayListInt64> outArr, NameArray **nameArr, Math::RectAreaDbl rect, Bool keepEmpty);
		virtual Int64 GetObjectIdMax() const;
		virtual void ReleaseNameArr(NameArray *nameArr);
		virtual Bool GetString(NotNullPtr<Text::StringBuilderUTF8> sb, NameArray *nameArr, Int64 id, UOSInt colIndex);
		virtual UOSInt GetColumnCnt() const;
		virtual UTF8Char *GetColumnName(UTF8Char *buff, UOSInt colIndex);
		virtual DB::DBUtil::ColType GetColumnType(UOSInt colIndex, OptOut<UOSInt> colSize);
		virtual Bool GetColumnDef(UOSInt colIndex, NotNullPtr<DB::ColDef> colDef);
		virtual UInt32 GetCodePage() const;
		virtual Bool GetBounds(OutParam<Math::RectAreaDbl> rect) const;

		virtual GetObjectSess *BeginGetObject();
		virtual void EndGetObject(GetObjectSess *session);
		virtual Math::Geometry::Vector2D *GetNewVectorById(GetObjectSess *session, Int64 id);

		void SetTableName(Text::String *tableName);
		virtual UOSInt QueryTableNames(Text::CString schemaName, Data::ArrayListStringNN *names);

		virtual ObjectClass GetObjectClass() const;
		Bool VectorValid(NotNullPtr<Math::Geometry::Vector2D> vec);
		Bool AddVector(NotNullPtr<Math::Geometry::Vector2D> vec, Text::String **strs);
		Bool AddVector(NotNullPtr<Math::Geometry::Vector2D> vec, Text::PString *strs);
		Bool AddVector(NotNullPtr<Math::Geometry::Vector2D> vec, const UTF8Char **strs);
		Bool AddVector(NotNullPtr<Math::Geometry::Vector2D> vec, NotNullPtr<Data::ArrayListStringNN> strs);
		Bool SplitPolyline(Math::Coord2DDbl pt);
		void OptimizePolylinePath();
		void ReplaceVector(Int64 id, NotNullPtr<Math::Geometry::Vector2D> vec);
		void ConvCoordinateSystem(NotNullPtr<Math::CoordinateSystem> csys);
		void SwapXY();
	};
}
#endif
