#ifndef _SM_MAP_ORUXDBLAYER
#define _SM_MAP_ORUXDBLAYER
#include "Data/FastMapNN.hpp"
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
		Data::FastMapNN<UInt32, LayerInfo> layerMap;
		UInt32 currLayer;
		UInt32 tileSize;
		Optional<DB::DBConn> db;

	public:
		OruxDBLayer(Text::CStringNN sourceName, Text::CString layerName, NN<Parser::ParserList> parsers);
		virtual ~OruxDBLayer();

		Bool IsError() const;
		void AddLayer(UInt32 layerId, Double mapXMin, Double mapYMin, Double mapXMax, Double mapYMax, UInt32 maxX, UInt32 maxY, UInt32 tileSize);

		void SetCurrLayer(UInt32 level);
		virtual void SetCurrScale(Double scale);
		virtual NN<Map::MapView> CreateMapView(Math::Size2DDbl scnSize);

		virtual DrawLayerType GetLayerType() const;
		virtual UIntOS GetAllObjectIds(NN<Data::ArrayListInt64> outArr, OptOut<Optional<NameArray>> nameArr);
		virtual UIntOS GetObjectIds(NN<Data::ArrayListInt64> outArr, OptOut<Optional<NameArray>> nameArr, Double mapRate, Math::RectArea<Int32> rect, Bool keepEmpty);
		virtual UIntOS GetObjectIdsMapXY(NN<Data::ArrayListInt64> outArr, OptOut<Optional<NameArray>> nameArr, Math::RectAreaDbl rect, Bool keepEmpty);
		virtual Int64 GetObjectIdMax() const;
		virtual UIntOS GetRecordCnt() const;
		virtual void ReleaseNameArr(Optional<NameArray> nameArr);
		virtual Bool GetString(NN<Text::StringBuilderUTF8> sb, Optional<NameArray> nameArr, Int64 id, UIntOS strIndex);
		virtual UIntOS GetColumnCnt() const;
		virtual UnsafeArrayOpt<UTF8Char> GetColumnName(UnsafeArray<UTF8Char> buff, UIntOS colIndex) const;
		virtual DB::DBUtil::ColType GetColumnType(UIntOS colIndex, OptOut<UIntOS> colSize) const;
		virtual Bool GetColumnDef(UIntOS colIndex, NN<DB::ColDef> colDef) const;
		virtual UInt32 GetCodePage() const;
		virtual Bool GetBounds(OutParam<Math::RectAreaDbl> bounds) const;

		virtual NN<GetObjectSess> BeginGetObject();
		virtual void EndGetObject(NN<GetObjectSess> session);
		virtual Optional<Math::Geometry::Vector2D> GetNewVectorById(NN<GetObjectSess> session, Int64 id);
		virtual FailReason GetFailReason() const;
		virtual void WaitForLoad(Data::Duration maxWaitTime) {};

		virtual UIntOS QueryTableNames(Text::CString schemaName, NN<Data::ArrayListStringNN> names);
		virtual Optional<DB::DBReader> QueryTableData(Text::CString schemaName, Text::CStringNN tableName, Optional<Data::ArrayListStringNN> columnNames, UIntOS ofst, UIntOS maxCnt, Text::CString ordering, Optional<Data::QueryConditions> condition);
		virtual Optional<DB::TableDef> GetTableDef(Text::CString schemaName, Text::CStringNN tableName);
		virtual void CloseReader(NN<DB::DBReader> r);
		virtual void GetLastErrorMsg(NN<Text::StringBuilderUTF8> str);
		virtual void Reconnect();
		virtual UIntOS GetGeomCol() const;

		virtual ObjectClass GetObjectClass() const;

		Bool GetObjectData(Int64 objectId, NN<IO::Stream> stm, OptOut<Int32> tileX, OptOut<Int32> tileY, OptOut<Int64> modTimeTicks);
	};
}
#endif
