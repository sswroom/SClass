#ifndef _SM_MAP_MAPDRAWLAYER
#define _SM_MAP_MAPDRAWLAYER
#include "AnyType.h"
#include "Data/ArrayListInt64.h"
#include "Data/ArrayListNative.hpp"
#include "Data/ArrayListNN.hpp"
#include "Data/ArrayListString.h"
#include "DB/DBReader.h"
#include "DB/ReadingDB.h"
#include "IO/ParsedObject.h"
#include "Map/MapSearchLayer.h"
#include "Map/MapView.h"
#include "Math/GeographicCoordinateSystem.h"
#include "Math/RectArea.hpp"
#include "Math/Geometry/Vector2D.h"
#include "Media/SharedImage.h"
#include "Text/SearchIndexer.h"

namespace Map
{
	class VectorLayer;
	struct GetObjectSess;
	struct NameArray;

	typedef enum
	{
		DRAW_LAYER_UNKNOWN = 0,
		DRAW_LAYER_POINT = 1,
		DRAW_LAYER_POLYLINE = 3,
		DRAW_LAYER_POLYGON = 5,
		DRAW_LAYER_POINT3D = 11,
		DRAW_LAYER_POLYLINE3D = 13,
		DRAW_LAYER_IMAGE = 100,
		DRAW_LAYER_MIXED = 101
	} DrawLayerType;

	class MapDrawLayer : public DB::ReadingDB, public Map::MapSearchLayer
	{
	public:
		struct ObjectInfo
		{
			Math::Coord2DDbl objPos;
			Double objY;
			Int64 objId;
			Double objDist;
		};
	protected:
		UIntOS nameCol;
		NN<Math::CoordinateSystem> csys;
		Optional<Text::String> layerName;

		UInt32 pgColor;
		UInt32 lineColor;
		Double lineWidth;
		Optional<Media::SharedImage> iconImg;
		IntOS iconSpotX;
		IntOS iconSpotY;
		Int32 flags;

		static IntOS __stdcall ObjectCompare(NN<ObjectInfo> obj1, NN<ObjectInfo> obj2);
	public:
		enum class MixedData
		{
			AllData,
			PointOnly,
			NonPointOnly
		};

		enum class FailReason
		{
			IdNotFound,
			ItemLoading
		};

		typedef enum
		{
			OC_GPS_TRACK,
			OC_CIP_LAYER,
			OC_SPD_LAYER,
			OC_SHP_DATA,
			OC_VECTOR_LAYER,
			OC_TILE_MAP_LAYER,
			OC_NETWORKLINK_LAYER,
			OC_WEB_IMAGE_LAYER,
			OC_ESRI_MDB_LAYER,
			OC_MAP_LAYER_COLL,
			OC_ORUX_DB_LAYER,
			OC_HKTRAFFIC_LAYER,
			OC_HKTRAFFIC_LAYER2,
			OC_DB_MAP_LAYER,
			OC_DRAW_MAP_SERVICE_LAYER,
			OC_GEOPACKAGE,
			OC_OSMDATA,
			OC_OTHER
		} ObjectClass;

		typedef void (CALLBACKFUNC UpdatedHandler)(AnyType userObj);

		MapDrawLayer(NN<Text::String> sourceName, UIntOS nameCol, Optional<Text::String> layerName, NN<Math::CoordinateSystem> csys);
		MapDrawLayer(Text::CStringNN sourceName, UIntOS nameCol, Text::CString layerName, NN<Math::CoordinateSystem> csys);
		virtual ~MapDrawLayer();

		virtual void SetCurrScale(Double scale);
		virtual void SetCurrTimeTS(Int64 timeStamp);
		virtual Int64 GetTimeStartTS() const;
		virtual Int64 GetTimeEndTS() const;
		virtual NN<Map::MapView> CreateMapView(Math::Size2DDbl scnSize);

		virtual DrawLayerType GetLayerType() const = 0;
		virtual void SetMixedData(MixedData MixedData);
		virtual UIntOS GetAllObjectIds(NN<Data::ArrayListInt64> outArr, OptOut<Optional<NameArray>> nameArr) = 0;
		virtual UIntOS GetObjectIds(NN<Data::ArrayListInt64> outArr, OptOut<Optional<NameArray>> nameArr, Double mapRate, Math::RectArea<Int32> rect, Bool keepEmpty) = 0;
		virtual UIntOS GetObjectIdsMapXY(NN<Data::ArrayListInt64> outArr, OptOut<Optional<NameArray>> nameArr, Math::RectAreaDbl rect, Bool keepEmpty) = 0;
		virtual Int64 GetObjectIdMax() const = 0;
		virtual UIntOS GetRecordCnt() const = 0;
		virtual void ReleaseNameArr(Optional<NameArray> nameArr) = 0;
		virtual Bool GetString(NN<Text::StringBuilderUTF8> sb, Optional<NameArray> nameArr, Int64 id, UIntOS strIndex) = 0;
		virtual UIntOS GetColumnCnt() const = 0;
		virtual UnsafeArrayOpt<UTF8Char> GetColumnName(UnsafeArray<UTF8Char> buff, UIntOS colIndex) const = 0;
		virtual DB::DBUtil::ColType GetColumnType(UIntOS colIndex, OptOut<UIntOS> colSize) const = 0;
		virtual Bool GetColumnDef(UIntOS colIndex, NN<DB::ColDef> colDef) const = 0;
		virtual UInt32 GetCodePage() const = 0;
		virtual Bool GetBounds(OutParam<Math::RectAreaDbl> rect) const = 0;
		virtual void SetDispSize(Math::Size2DDbl size, Double dpi);
		virtual Optional<DB::TableDef> CreateLayerTableDef() const;
		void AddColDefs(NN<DB::TableDef> tableDef) const;

		virtual NN<GetObjectSess> BeginGetObject() = 0;
		virtual void EndGetObject(NN<GetObjectSess> session) = 0;
		virtual Optional<Math::Geometry::Vector2D> GetNewVectorById(NN<GetObjectSess> session, Int64 id) = 0;
		virtual void AddUpdatedHandler(UpdatedHandler hdlr, AnyType obj);
		virtual void RemoveUpdatedHandler(UpdatedHandler hdlr, AnyType obj);
		virtual FailReason GetFailReason() const = 0;
		virtual void WaitForLoad(Data::Duration maxWaitTime) = 0;

		virtual UIntOS QueryTableNames(Text::CString schemaName, NN<Data::ArrayListStringNN> names);
		virtual Optional<DB::DBReader> QueryTableData(Text::CString schemaName, Text::CStringNN tableName, Optional<Data::ArrayListStringNN> columnNames, UIntOS ofst, UIntOS maxCnt, Text::CString ordering, Optional<Data::QueryConditions> condition);
		virtual Optional<DB::TableDef> GetTableDef(Text::CString schemaName, Text::CStringNN tableName);
		virtual void CloseReader(NN<DB::DBReader> r);
		virtual void GetLastErrorMsg(NN<Text::StringBuilderUTF8> str);
		virtual void Reconnect();

		virtual UIntOS GetNameCol() const;
		virtual void SetNameCol(UIntOS nameCol);
		virtual UIntOS GetGeomCol() const = 0;

		virtual ObjectClass GetObjectClass() const = 0;
		NN<Text::String> GetName() const;
		virtual IO::ParserType GetParserType() const;
		virtual NN<Math::CoordinateSystem> GetCoordinateSystem() const;
		virtual void SetCoordinateSystem(NN<Math::CoordinateSystem> csys);

		Int32 CalBlockSize();
		void SetLayerName(Text::CString name);

		virtual Bool IsError() const;
		virtual Bool GetPGLabel(NN<Text::StringBuilderUTF8> sb, Math::Coord2DDbl coord, OptOut<Math::Coord2DDbl> outCoord, UIntOS strIndex);
		virtual Bool GetPLLabel(NN<Text::StringBuilderUTF8> sb, Math::Coord2DDbl coord, OutParam<Math::Coord2DDbl> outCoord, UIntOS strIndex);
		virtual Bool CanQuery();
		virtual Bool QueryInfos(Math::Coord2DDbl coord, NN<Data::ArrayListNN<Math::Geometry::Vector2D>> vecList, NN<Data::ArrayListNative<UIntOS>> valueOfstList, NN<Data::ArrayListStringNN> nameList, NN<Data::ArrayListNN<Text::String>> valueList);

		Int64 GetNearestObjectId(NN<GetObjectSess> session, Math::Coord2DDbl pt, OptOut<Math::Coord2DDbl> nearPt);
		void GetNearestObjectIds(NN<GetObjectSess> session, Math::Coord2DDbl pt, NN<Data::ArrayListNative<Int64>> ids, OptOut<Math::Coord2DDbl> nearPt);
		IntOS GetNearObjects(NN<GetObjectSess> session, NN<Data::ArrayListNN<ObjectInfo>> objList, Math::Coord2DDbl pt, Double maxDist); //return nearest object if no object within distance
		void FreeObjects(NN<Data::ArrayListNN<ObjectInfo>> objList);
		NN<Map::VectorLayer> CreateEditableLayer();

		Optional<Text::SearchIndexer> CreateSearchIndexer(NN<Text::TextAnalyzer> ta, UIntOS strIndex);
		UIntOS SearchString(NN<Data::ArrayListString> outArr, NN<Text::SearchIndexer> srchInd, Optional<NameArray> nameArr, UnsafeArray<const UTF8Char> srchStr, UIntOS maxResult, UIntOS strIndex);
		void ReleaseSearchStr(NN<Data::ArrayListString> strArr);
		Optional<Math::Geometry::Vector2D> GetVectorByStr(NN<Text::SearchIndexer> srchInd, Optional<NameArray> nameArr, NN<GetObjectSess> session, Text::CStringNN srchStr, UIntOS strIndex);

		Bool HasLineStyle();
		Bool HasPGStyle();
		Bool HasIconStyle();
		void SetLineStyle(UInt32 lineColor, Double lineWidth);
		void SetPGStyle(UInt32 pgColor);
		void SetIconStyle(NN<Media::SharedImage> iconImg, IntOS iconSpotX, IntOS iconSpotY);
		UInt32 GetLineStyleColor();
		Double GetLineStyleWidth();
		UInt32 GetPGStyleColor();
		Optional<Media::SharedImage> GetIconStyleImg();
		IntOS GetIconStyleSpotX();
		IntOS GetIconStyleSpotY();
		Bool IsLabelVisible();
		void SetLabelVisible(Bool labelVisible);

		static Map::DrawLayerType VectorType2LayerType(Math::Geometry::Vector2D::VectorType vtype);
	};

	class MapLayerReader : public DB::DBReader
	{
	protected:
		NN<MapDrawLayer> layer;
		Data::ArrayListInt64 objIds; 
		Optional<NameArray> nameArr;
		IntOS currIndex;

		Int64 GetCurrObjId();
		UIntOS RemapColIndex(UIntOS colIndex);
	public:
		MapLayerReader(NN<MapDrawLayer> layer);
		~MapLayerReader();

		virtual Bool ReadNext();
		virtual UIntOS ColCount();
		virtual IntOS GetRowChanged();

		virtual Int32 GetInt32(UIntOS colIndex);
		virtual Int64 GetInt64(UIntOS colIndex);
		virtual UnsafeArrayOpt<WChar> GetStr(UIntOS colIndex, UnsafeArray<WChar> buff);
		virtual Bool GetStr(UIntOS colIndex, NN<Text::StringBuilderUTF8> sb);
		virtual Optional<Text::String> GetNewStr(UIntOS colIndex);
		virtual UnsafeArrayOpt<UTF8Char> GetStr(UIntOS colIndex, UnsafeArray<UTF8Char> buff, UIntOS buffSize);
		virtual Data::Timestamp GetTimestamp(UIntOS colIndex);
		virtual Double GetDblOrNAN(UIntOS colIndex);
		virtual Bool GetBool(UIntOS colIndex);
		virtual UIntOS GetBinarySize(UIntOS colIndex);
		virtual UIntOS GetBinary(UIntOS colIndex, UnsafeArray<UInt8> buff);
		virtual Optional<Math::Geometry::Vector2D> GetVector(UIntOS colIndex);
		virtual Bool GetUUID(UIntOS colIndex, NN<Data::UUID> uuid);

		virtual Bool IsNull(UIntOS colIndex);
		virtual UnsafeArrayOpt<UTF8Char> GetName(UIntOS colIndex, UnsafeArray<UTF8Char> buff);
		virtual DB::DBUtil::ColType GetColType(UIntOS colIndex, OptOut<UIntOS> colSize);
		virtual Bool GetColDef(UIntOS colIndex, NN<DB::ColDef> colDef);

		static void GetShapeColDef(NN<DB::ColDef> colDef, NN<const Map::MapDrawLayer> layer);
		static UIntOS GetShapeColSize(Map::DrawLayerType layerType);
		static Bool GetColDefV(UIntOS colIndex, NN<DB::ColDef> colDef, NN<Map::MapDrawLayer> layer);
	};
}
#endif
