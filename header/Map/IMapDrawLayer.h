#ifndef _SM_MAP_IMAPDRAWLAYER
#define _SM_MAP_IMAPDRAWLAYER
#include "Data/ArrayList.h"
#include "Data/ArrayListInt64.h"
#include "Data/ArrayListString.h"
#include "DB/DBReader.h"
#include "DB/ReadingDB.h"
#include "IO/ParsedObject.h"
#include "Map/IMapSearchLayer.h"
#include "Map/MapView.h"
#include "Math/GeographicCoordinateSystem.h"
#include "Math/RectArea.h"
#include "Math/Geometry/Vector2D.h"
#include "Media/SharedImage.h"
#include "Text/SearchIndexer.h"

namespace Map
{
	class VectorLayer;

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

	class IMapDrawLayer : public DB::ReadingDB, public Map::IMapSearchLayer
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
		Math::CoordinateSystem *csys;
		Text::String *layerName;

		UInt32 pgColor;
		UInt32 lineColor;
		UInt32 lineWidth;
		Media::SharedImage *iconImg;
		OSInt iconSpotX;
		OSInt iconSpotY;
		Int32 flags;

		static OSInt __stdcall ObjectCompare(void *obj1, void *obj2);
	public:
		typedef enum
		{
			OC_GPS_TRACK,
			OC_CIP_LAYER,
			OC_SPD_LAYER,
			OC_SHP_DATA,
			OC_VECTOR_LAYER,
			OC_TILE_MAP_LAYER,
			OC_RELOADABLE_LAYER,
			OC_WEB_IMAGE_LAYER,
			OC_ESRI_MDB_LAYER,
			OC_MAP_LAYER_COLL,
			OC_ORUX_DB_LAYER,
			OC_HKTRAFFIC_LAYER,
			OC_DB_MAP_LAYER,
			OC_DRAW_MAP_SERVICE_LAYER,
			OC_OTHER
		} ObjectClass;

		typedef void (__stdcall *UpdatedHandler)(void *userObj);

		IMapDrawLayer(Text::String *sourceName, UOSInt nameCol, Text::String *layerName);
		IMapDrawLayer(Text::CString sourceName, UOSInt nameCol, Text::CString layerName);
		virtual ~IMapDrawLayer();

		virtual void SetCurrScale(Double scale);
		virtual void SetCurrTimeTS(Int64 timeStamp);
		virtual Int64 GetTimeStartTS();
		virtual Int64 GetTimeEndTS();
		virtual Map::MapView *CreateMapView(Math::Size2D<Double> scnSize);

		virtual DrawLayerType GetLayerType() = 0;
		virtual void SetMixedType(Bool pointType);
		virtual UOSInt GetAllObjectIds(Data::ArrayListInt64 *outArr, void **nameArr) = 0;
		virtual UOSInt GetObjectIds(Data::ArrayListInt64 *outArr, void **nameArr, Double mapRate, Math::RectArea<Int32> rect, Bool keepEmpty) = 0;
		virtual UOSInt GetObjectIdsMapXY(Data::ArrayListInt64 *outArr, void **nameArr, Math::RectAreaDbl rect, Bool keepEmpty) = 0;
		virtual Int64 GetObjectIdMax() = 0;
		virtual void ReleaseNameArr(void *nameArr) = 0;
		virtual UTF8Char *GetString(UTF8Char *buff, UOSInt buffSize, void *nameArr, Int64 id, UOSInt strIndex) = 0;
		virtual UOSInt GetColumnCnt() = 0;
		virtual UTF8Char *GetColumnName(UTF8Char *buff, UOSInt colIndex) = 0;
		virtual DB::DBUtil::ColType GetColumnType(UOSInt colIndex, UOSInt *colSize) = 0;
		virtual Bool GetColumnDef(UOSInt colIndex, DB::ColDef *colDef) = 0;
		virtual UInt32 GetCodePage() = 0;
		virtual Bool GetBounds(Math::RectAreaDbl *rect) = 0;
		virtual void SetDispSize(Math::Size2D<Double> size, Double dpi);

		virtual void *BeginGetObject() = 0;
		virtual void EndGetObject(void *session) = 0;
		virtual Math::Geometry::Vector2D *GetNewVectorById(void *session, Int64 id) = 0;
		virtual void AddUpdatedHandler(UpdatedHandler hdlr, void *obj);
		virtual void RemoveUpdatedHandler(UpdatedHandler hdlr, void *obj);

		virtual UOSInt QueryTableNames(Text::CString schemaName, Data::ArrayList<Text::String*> *names);
		virtual DB::DBReader *QueryTableData(Text::CString schemaName, Text::CString tableName, Data::ArrayList<Text::String*> *columnNames, UOSInt ofst, UOSInt maxCnt, Text::CString ordering, Data::QueryConditions *condition);
		virtual void CloseReader(DB::DBReader *r);
		virtual void GetErrorMsg(Text::StringBuilderUTF8 *str);
		virtual void Reconnect();

		virtual UOSInt GetNameCol();
		virtual void SetNameCol(UOSInt nameCol);

		virtual ObjectClass GetObjectClass() = 0;
		Text::String *GetName();
		virtual IO::ParserType GetParserType() const;
		virtual Math::CoordinateSystem *GetCoordinateSystem();
		virtual void SetCoordinateSystem(Math::CoordinateSystem *csys);

		Int32 CalBlockSize();
		void SetLayerName(Text::CString name);

		virtual Bool IsError();
		virtual UTF8Char *GetPGLabel(UTF8Char *buff, UOSInt buffSize, Math::Coord2DDbl coord, Math::Coord2DDbl *outCoord, UOSInt strIndex);
		virtual UTF8Char *GetPLLabel(UTF8Char *buff, UOSInt buffSize, Math::Coord2DDbl coord, Math::Coord2DDbl *outCoord, UOSInt strIndex);
		virtual Bool CanQuery();
		virtual Math::Geometry::Vector2D *QueryInfo(Math::Coord2DDbl coord, Data::ArrayList<Text::String*> *nameList, Data::ArrayList<Text::String*> *valueList);

		Int64 GetNearestObjectId(void *session, Math::Coord2DDbl pt, Math::Coord2DDbl *nearPt);
		OSInt GetNearObjects(void *session, Data::ArrayList<ObjectInfo*> *objList, Math::Coord2DDbl pt, Double maxDist); //return nearest object if no object within distance
		void FreeObjects(Data::ArrayList<ObjectInfo*> *objList);
		Map::VectorLayer *CreateEditableLayer();

		Text::SearchIndexer *CreateSearchIndexer(Text::TextAnalyzer *ta, UOSInt strIndex);
		UOSInt SearchString(Data::ArrayListString *outArr, Text::SearchIndexer *srchInd, void *nameArr, const UTF8Char *srchStr, UOSInt maxResult, UOSInt strIndex);
		void ReleaseSearchStr(Data::ArrayListString *strArr);
		Math::Geometry::Vector2D *GetVectorByStr(Text::SearchIndexer *srchInd, void *nameArr, void *session, const UTF8Char *srchStr, UOSInt strIndex);

		Bool HasLineStyle();
		Bool HasPGStyle();
		Bool HasIconStyle();
		void SetLineStyle(UInt32 lineColor, UInt32 lineWidth);
		void SetPGStyle(UInt32 pgColor);
		void SetIconStyle(Media::SharedImage *iconImg, OSInt iconSpotX, OSInt iconSpotY);
		UInt32 GetLineStyleColor();
		UInt32 GetLineStyleWidth();
		UInt32 GetPGStyleColor();
		Media::SharedImage *GetIconStyleImg();
		OSInt GetIconStyleSpotX();
		OSInt GetIconStyleSpotY();
		Bool IsLabelVisible();
		void SetLabelVisible(Bool labelVisible);

		static Map::DrawLayerType VectorType2LayerType(Math::Geometry::Vector2D::VectorType vtype);
	};

	class MapLayerReader : public DB::DBReader
	{
	protected:
		IMapDrawLayer *layer;
		Data::ArrayListInt64 *objIds; 
		void *nameArr;
		OSInt currIndex;

		Int64 GetCurrObjId();
	public:
		MapLayerReader(IMapDrawLayer *layer);
		~MapLayerReader();

		virtual Bool ReadNext();
		virtual UOSInt ColCount();
		virtual OSInt GetRowChanged();

		virtual Int32 GetInt32(UOSInt colIndex);
		virtual Int64 GetInt64(UOSInt colIndex);
		virtual WChar *GetStr(UOSInt colIndex, WChar *buff);
		virtual Bool GetStr(UOSInt colIndex, Text::StringBuilderUTF8 *sb);
		virtual Text::String *GetNewStr(UOSInt colIndex);
		virtual UTF8Char *GetStr(UOSInt colIndex, UTF8Char *buff, UOSInt buffSize);
		virtual Data::Timestamp GetTimestamp(UOSInt colIndex);
		virtual Double GetDbl(UOSInt colIndex);
		virtual Bool GetBool(UOSInt colIndex);
		virtual UOSInt GetBinarySize(UOSInt colIndex);
		virtual UOSInt GetBinary(UOSInt colIndex, UInt8 *buff);
		virtual Math::Geometry::Vector2D *GetVector(UOSInt colIndex);
		virtual Bool GetUUID(UOSInt colIndex, Data::UUID *uuid);

		virtual Bool IsNull(UOSInt colIndex);
		virtual UTF8Char *GetName(UOSInt colIndex, UTF8Char *buff);
		virtual DB::DBUtil::ColType GetColType(UOSInt colIndex, UOSInt *colSize);
		virtual Bool GetColDef(UOSInt colIndex, DB::ColDef *colDef);
	};
}
#endif
