#ifndef _SM_MAP_VECTORLAYER
#define _SM_MAP_VECTORLAYER
#include "Map/IMapDrawLayer.h"
#include "Text/PString.h"
#include "Text/String.h"

namespace Map
{
	class VectorLayer : public Map::IMapDrawLayer
	{
	private:
		typedef struct
		{
			DB::DBUtil::ColType colType;
			UOSInt colSize;
			UOSInt colDP;
		} ColInfo;

	private:
		Map::DrawLayerType layerType;
		const UTF8Char **colNames;
		UOSInt strCnt;
		UOSInt *maxStrLen;
		UOSInt *thisStrLen;
		Data::ArrayList<Math::Geometry::Vector2D*> vectorList;
		Data::ArrayList<const UTF8Char **> strList;
		Math::Coord2DDbl min;
		Math::Coord2DDbl max;
		ColInfo *cols;
		Double mapRate;
		MixedData mixedData;

	private:
		const UTF8Char **CopyStrs(const UTF8Char **strs);
		const UTF8Char **CopyStrs(Text::String **strs);
		const UTF8Char **CopyStrs(Text::PString *strs);
		void UpdateMapRate();
	public:
		VectorLayer(Map::DrawLayerType layerType, Text::String *sourceName, UOSInt strCnt, const UTF8Char **colNames, Math::CoordinateSystem *csys, UOSInt nameCol, Text::String *layerName);
		VectorLayer(Map::DrawLayerType layerType, Text::CString sourceName, UOSInt strCnt, const UTF8Char **colNames, Math::CoordinateSystem *csys, UOSInt nameCol, Text::CString layerName);
		VectorLayer(Map::DrawLayerType layerType, Text::String *sourceName, UOSInt strCnt, const UTF8Char **colNames, Math::CoordinateSystem *csys, DB::DBUtil::ColType *colTypes, UOSInt *colSize, UOSInt *colDP, UOSInt nameCol, Text::String *layerName);
		VectorLayer(Map::DrawLayerType layerType, Text::CString sourceName, UOSInt strCnt, const UTF8Char **colNames, Math::CoordinateSystem *csys, DB::DBUtil::ColType *colTypes, UOSInt *colSize, UOSInt *colDP, UOSInt nameCol, Text::CString layerName);
		virtual ~VectorLayer();

		virtual DrawLayerType GetLayerType();
		virtual void SetMixedData(MixedData mixedData);
		virtual UOSInt GetAllObjectIds(Data::ArrayListInt64 *outArr, void **nameArr);
		virtual UOSInt GetObjectIds(Data::ArrayListInt64 *outArr, void **nameArr, Double mapRate, Math::RectArea<Int32> rect, Bool keepEmpty);
		virtual UOSInt GetObjectIdsMapXY(Data::ArrayListInt64 *outArr, void **nameArr, Math::RectAreaDbl rect, Bool keepEmpty);
		virtual Int64 GetObjectIdMax();
		virtual void ReleaseNameArr(void *nameArr);
		virtual UTF8Char *GetString(UTF8Char *buff, UOSInt buffSize, void *nameArr, Int64 id, UOSInt colIndex);
		virtual UOSInt GetColumnCnt();
		virtual UTF8Char *GetColumnName(UTF8Char *buff, UOSInt colIndex);
		virtual DB::DBUtil::ColType GetColumnType(UOSInt colIndex, UOSInt *colSize);
		virtual Bool GetColumnDef(UOSInt colIndex, DB::ColDef *colDef);
		virtual UInt32 GetCodePage();
		virtual Bool GetBounds(Math::RectAreaDbl *rect);

		virtual void *BeginGetObject();
		virtual void EndGetObject(void *session);
		virtual Math::Geometry::Vector2D *GetNewVectorById(void *session, Int64 id);

		virtual ObjectClass GetObjectClass();
		Bool VectorValid(Math::Geometry::Vector2D *vec);
		Bool AddVector(Math::Geometry::Vector2D *vec, Text::String **strs);
		Bool AddVector(Math::Geometry::Vector2D *vec, Text::PString *strs);
		Bool AddVector(Math::Geometry::Vector2D *vec, const UTF8Char **strs);
		Bool SplitPolyline(Math::Coord2DDbl pt);
		void OptimizePolylinePath();
		void ReplaceVector(Int64 id, Math::Geometry::Vector2D *vec);
		void ConvCoordinateSystem(Math::CoordinateSystem *csys);
		void SwapXY();
	};
}
#endif
