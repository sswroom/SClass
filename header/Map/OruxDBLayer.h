#ifndef _SM_MAP_ORUXDBLAYER
#define _SM_MAP_ORUXDBLAYER
#include "Data/FastMap.h"
#include "DB/DBConn.h"
#include "Map/MapDrawLayer.h"
#include "Parser/ParserList.h"

namespace Map
{
	class OruxDBLayer : public Map::MapDrawLayer
	{
	private:
		typedef struct
		{
			Math::Coord2DDbl mapMin;
			Math::Coord2DDbl mapMax;
			Double projYMin;
			Double projYMax;
			UInt32 layerId;
			Math::Coord2D<UInt32> max;
		} LayerInfo;
	private:
		NN<Parser::ParserList> parsers;
		Data::FastMap<UInt32, LayerInfo*> layerMap;
		UInt32 currLayer;
		UInt32 tileSize;
		DB::DBConn *db;

	public:
		OruxDBLayer(Text::CStringNN sourceName, Text::CString layerName, NN<Parser::ParserList> parsers);
		virtual ~OruxDBLayer();

		Bool IsError() const;
		void AddLayer(UInt32 layerId, Double mapXMin, Double mapYMin, Double mapXMax, Double mapYMax, UInt32 maxX, UInt32 maxY, UInt32 tileSize);

		void SetCurrLayer(UInt32 level);
		virtual void SetCurrScale(Double scale);
		virtual NN<Map::MapView> CreateMapView(Math::Size2DDbl scnSize);

		virtual DrawLayerType GetLayerType() const;
		virtual UOSInt GetAllObjectIds(NN<Data::ArrayListInt64> outArr, NameArray **nameArr);
		virtual UOSInt GetObjectIds(NN<Data::ArrayListInt64> outArr, NameArray **nameArr, Double mapRate, Math::RectArea<Int32> rect, Bool keepEmpty);
		virtual UOSInt GetObjectIdsMapXY(NN<Data::ArrayListInt64> outArr, NameArray **nameArr, Math::RectAreaDbl rect, Bool keepEmpty);
		virtual Int64 GetObjectIdMax() const;
		virtual void ReleaseNameArr(NameArray *nameArr);
		virtual Bool GetString(NN<Text::StringBuilderUTF8> sb, NameArray *nameArr, Int64 id, UOSInt strIndex);
		virtual UOSInt GetColumnCnt() const;
		virtual UTF8Char *GetColumnName(UTF8Char *buff, UOSInt colIndex);
		virtual DB::DBUtil::ColType GetColumnType(UOSInt colIndex, OptOut<UOSInt> colSize);
		virtual Bool GetColumnDef(UOSInt colIndex, NN<DB::ColDef> colDef);
		virtual UInt32 GetCodePage() const;
		virtual Bool GetBounds(OutParam<Math::RectAreaDbl> bounds) const;

		virtual GetObjectSess *BeginGetObject();
		virtual void EndGetObject(GetObjectSess *session);
		virtual Math::Geometry::Vector2D *GetNewVectorById(GetObjectSess *session, Int64 id);

		virtual UOSInt QueryTableNames(Text::CString schemaName, NN<Data::ArrayListStringNN> names);
		virtual Optional<DB::DBReader> QueryTableData(Text::CString schemaName, Text::CString tableName, Data::ArrayListStringNN *columnNames, UOSInt ofst, UOSInt maxCnt, Text::CString ordering, Data::QueryConditions *condition);
		virtual DB::TableDef *GetTableDef(Text::CString schemaName, Text::CString tableName);
		virtual void CloseReader(NN<DB::DBReader> r);
		virtual void GetLastErrorMsg(NN<Text::StringBuilderUTF8> str);
		virtual void Reconnect();

		virtual ObjectClass GetObjectClass() const;

		Bool GetObjectData(Int64 objectId, IO::Stream *stm, Int32 *tileX, Int32 *tileY, Int64 *modTimeTicks);
	};
}
#endif
