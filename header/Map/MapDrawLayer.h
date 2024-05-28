#ifndef _SM_MAP_MAPDRAWLAYER
#define _SM_MAP_MAPDRAWLAYER
#include "AnyType.h"
#include "Data/ArrayList.h"
#include "Data/ArrayListInt64.h"
#include "Data/ArrayListNN.h"
#include "Data/ArrayListString.h"
#include "DB/DBReader.h"
#include "DB/ReadingDB.h"
#include "IO/ParsedObject.h"
#include "Map/MapSearchLayer.h"
#include "Map/MapView.h"
#include "Math/GeographicCoordinateSystem.h"
#include "Math/RectArea.h"
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
		UOSInt nameCol;
		NN<Math::CoordinateSystem> csys;
		Text::String *layerName;

		UInt32 pgColor;
		UInt32 lineColor;
		Double lineWidth;
		Optional<Media::SharedImage> iconImg;
		OSInt iconSpotX;
		OSInt iconSpotY;
		Int32 flags;

		static OSInt __stdcall ObjectCompare(NN<ObjectInfo> obj1, NN<ObjectInfo> obj2);
	public:
		enum class MixedData
		{
			AllData,
			PointOnly,
			NonPointOnly
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
			OC_OTHER
		} ObjectClass;

		typedef void (__stdcall *UpdatedHandler)(AnyType userObj);

		MapDrawLayer(NN<Text::String> sourceName, UOSInt nameCol, Text::String *layerName, NN<Math::CoordinateSystem> csys);
		MapDrawLayer(Text::CStringNN sourceName, UOSInt nameCol, Text::CString layerName, NN<Math::CoordinateSystem> csys);
		virtual ~MapDrawLayer();

		virtual void SetCurrScale(Double scale);
		virtual void SetCurrTimeTS(Int64 timeStamp);
		virtual Int64 GetTimeStartTS() const;
		virtual Int64 GetTimeEndTS() const;
		virtual NN<Map::MapView> CreateMapView(Math::Size2DDbl scnSize);

		virtual DrawLayerType GetLayerType() const = 0;
		virtual void SetMixedData(MixedData MixedData);
		virtual UOSInt GetAllObjectIds(NN<Data::ArrayListInt64> outArr, NameArray **nameArr) = 0;
		virtual UOSInt GetObjectIds(NN<Data::ArrayListInt64> outArr, NameArray **nameArr, Double mapRate, Math::RectArea<Int32> rect, Bool keepEmpty) = 0;
		virtual UOSInt GetObjectIdsMapXY(NN<Data::ArrayListInt64> outArr, NameArray **nameArr, Math::RectAreaDbl rect, Bool keepEmpty) = 0;
		virtual Int64 GetObjectIdMax() const = 0;
		virtual void ReleaseNameArr(NameArray *nameArr) = 0;
		virtual Bool GetString(NN<Text::StringBuilderUTF8> sb, NameArray *nameArr, Int64 id, UOSInt strIndex) = 0;
		virtual UOSInt GetColumnCnt() const = 0;
		virtual UTF8Char *GetColumnName(UTF8Char *buff, UOSInt colIndex) = 0;
		virtual DB::DBUtil::ColType GetColumnType(UOSInt colIndex, OptOut<UOSInt> colSize) = 0;
		virtual Bool GetColumnDef(UOSInt colIndex, NN<DB::ColDef> colDef) = 0;
		virtual UInt32 GetCodePage() const = 0;
		virtual Bool GetBounds(OutParam<Math::RectAreaDbl> rect) const = 0;
		virtual void SetDispSize(Math::Size2DDbl size, Double dpi);

		virtual GetObjectSess *BeginGetObject() = 0;
		virtual void EndGetObject(GetObjectSess *session) = 0;
		virtual Math::Geometry::Vector2D *GetNewVectorById(GetObjectSess *session, Int64 id) = 0;
		virtual void AddUpdatedHandler(UpdatedHandler hdlr, AnyType obj);
		virtual void RemoveUpdatedHandler(UpdatedHandler hdlr, AnyType obj);

		virtual UOSInt QueryTableNames(Text::CString schemaName, NN<Data::ArrayListStringNN> names);
		virtual Optional<DB::DBReader> QueryTableData(Text::CString schemaName, Text::CString tableName, Data::ArrayListStringNN *columnNames, UOSInt ofst, UOSInt maxCnt, Text::CString ordering, Data::QueryConditions *condition);
		virtual Optional<DB::TableDef> GetTableDef(Text::CString schemaName, Text::CString tableName);
		virtual void CloseReader(NN<DB::DBReader> r);
		virtual void GetLastErrorMsg(NN<Text::StringBuilderUTF8> str);
		virtual void Reconnect();

		virtual UOSInt GetNameCol() const;
		virtual void SetNameCol(UOSInt nameCol);

		virtual ObjectClass GetObjectClass() const = 0;
		NN<Text::String> GetName() const;
		virtual IO::ParserType GetParserType() const;
		virtual NN<Math::CoordinateSystem> GetCoordinateSystem();
		virtual void SetCoordinateSystem(NN<Math::CoordinateSystem> csys);

		Int32 CalBlockSize();
		void SetLayerName(Text::CString name);

		virtual Bool IsError() const;
		virtual Bool GetPGLabel(NN<Text::StringBuilderUTF8> sb, Math::Coord2DDbl coord, OptOut<Math::Coord2DDbl> outCoord, UOSInt strIndex);
		virtual Bool GetPLLabel(NN<Text::StringBuilderUTF8> sb, Math::Coord2DDbl coord, OutParam<Math::Coord2DDbl> outCoord, UOSInt strIndex);
		virtual Bool CanQuery();
		virtual Bool QueryInfos(Math::Coord2DDbl coord, NN<Data::ArrayListNN<Math::Geometry::Vector2D>> vecList, NN<Data::ArrayList<UOSInt>> valueOfstList, NN<Data::ArrayListStringNN> nameList, NN<Data::ArrayListNN<Text::String>> valueList);

		Int64 GetNearestObjectId(GetObjectSess *session, Math::Coord2DDbl pt, OptOut<Math::Coord2DDbl> nearPt);
		OSInt GetNearObjects(GetObjectSess *session, NN<Data::ArrayListNN<ObjectInfo>> objList, Math::Coord2DDbl pt, Double maxDist); //return nearest object if no object within distance
		void FreeObjects(NN<Data::ArrayListNN<ObjectInfo>> objList);
		NN<Map::VectorLayer> CreateEditableLayer();

		Text::SearchIndexer *CreateSearchIndexer(Text::TextAnalyzer *ta, UOSInt strIndex);
		UOSInt SearchString(NN<Data::ArrayListString> outArr, Text::SearchIndexer *srchInd, NameArray *nameArr, const UTF8Char *srchStr, UOSInt maxResult, UOSInt strIndex);
		void ReleaseSearchStr(NN<Data::ArrayListString> strArr);
		Math::Geometry::Vector2D *GetVectorByStr(Text::SearchIndexer *srchInd, NameArray *nameArr, GetObjectSess *session, Text::CStringNN srchStr, UOSInt strIndex);

		Bool HasLineStyle();
		Bool HasPGStyle();
		Bool HasIconStyle();
		void SetLineStyle(UInt32 lineColor, Double lineWidth);
		void SetPGStyle(UInt32 pgColor);
		void SetIconStyle(NN<Media::SharedImage> iconImg, OSInt iconSpotX, OSInt iconSpotY);
		UInt32 GetLineStyleColor();
		Double GetLineStyleWidth();
		UInt32 GetPGStyleColor();
		Optional<Media::SharedImage> GetIconStyleImg();
		OSInt GetIconStyleSpotX();
		OSInt GetIconStyleSpotY();
		Bool IsLabelVisible();
		void SetLabelVisible(Bool labelVisible);

		static Map::DrawLayerType VectorType2LayerType(Math::Geometry::Vector2D::VectorType vtype);
	};

	class MapLayerReader : public DB::DBReader
	{
	protected:
		NN<MapDrawLayer> layer;
		Data::ArrayListInt64 objIds; 
		NameArray *nameArr;
		OSInt currIndex;

		Int64 GetCurrObjId();
	public:
		MapLayerReader(NN<MapDrawLayer> layer);
		~MapLayerReader();

		virtual Bool ReadNext();
		virtual UOSInt ColCount();
		virtual OSInt GetRowChanged();

		virtual Int32 GetInt32(UOSInt colIndex);
		virtual Int64 GetInt64(UOSInt colIndex);
		virtual WChar *GetStr(UOSInt colIndex, WChar *buff);
		virtual Bool GetStr(UOSInt colIndex, NN<Text::StringBuilderUTF8> sb);
		virtual Optional<Text::String> GetNewStr(UOSInt colIndex);
		virtual UTF8Char *GetStr(UOSInt colIndex, UTF8Char *buff, UOSInt buffSize);
		virtual Data::Timestamp GetTimestamp(UOSInt colIndex);
		virtual Double GetDbl(UOSInt colIndex);
		virtual Bool GetBool(UOSInt colIndex);
		virtual UOSInt GetBinarySize(UOSInt colIndex);
		virtual UOSInt GetBinary(UOSInt colIndex, UInt8 *buff);
		virtual Optional<Math::Geometry::Vector2D> GetVector(UOSInt colIndex);
		virtual Bool GetUUID(UOSInt colIndex, NN<Data::UUID> uuid);

		virtual Bool IsNull(UOSInt colIndex);
		virtual UTF8Char *GetName(UOSInt colIndex, UTF8Char *buff);
		virtual DB::DBUtil::ColType GetColType(UOSInt colIndex, OptOut<UOSInt> colSize);
		virtual Bool GetColDef(UOSInt colIndex, NN<DB::ColDef> colDef);

		static Bool GetColDefV(UOSInt colIndex, NN<DB::ColDef> colDef, NN<Map::MapDrawLayer> layer);
	};
}
#endif
