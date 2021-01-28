#ifndef _SM_MAP_IMAPDRAWLAYER
#define _SM_MAP_IMAPDRAWLAYER
#include "Data/ArrayList.h"
#include "Data/ArrayListInt64.h"
#include "Data/ArrayListStrUTF8.h"
#include "DB/DBReader.h"
#include "DB/ReadingDB.h"
#include "IO/ParsedObject.h"
#include "Map/MapView.h"
#include "Math/Vector2D.h"
#include "Math/GeographicCoordinateSystem.h"
#include "Media/ImageList.h"
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

	typedef struct
	{
		Int64 objId;
		UInt32 nParts;
		UInt32 nPoints;
		UInt32 *parts;
		Double *points;
		Int32 flags; //bit0 = has Line color
		Int32 lineColor;
	} DrawObjectL;

/*	typedef struct
	{
		Int32 objId;
		Int32 nParts;
		Int32 nPoints;
		Int32 *parts;
		Int32 *ipoints;
		Double *points;
	} DrawObjectDbl;*/

	class IMapDrawLayer : public DB::ReadingDB //IO::ParsedObject
	{
	public:
		typedef struct
		{
			Int64 objId;
			Double objX;
			Double objY;
			Double objDist;
		} ObjectInfo;
	protected:
		OSInt nameCol;
		Math::CoordinateSystem *csys;
		const UTF8Char *layerName;

		Int32 pgColor;
		Int32 lineColor;
		Int32 lineWidth;
		Media::ImageList *iconImg;
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
			OC_OTHER
		} ObjectClass;

		typedef void (__stdcall *UpdatedHandler)(void *userObj);

		IMapDrawLayer(const UTF8Char *sourceName, OSInt nameCol, const UTF8Char *layerName);
		virtual ~IMapDrawLayer();

		virtual void SetCurrScale(Double scale);
		virtual void SetCurrTimeTS(Int64 timeStamp);
		virtual Int64 GetTimeStartTS();
		virtual Int64 GetTimeEndTS();
		virtual Map::MapView *CreateMapView(OSInt width, OSInt height);

		virtual DrawLayerType GetLayerType() = 0;
		virtual void SetMixedType(DrawLayerType mixedType);
		virtual UOSInt GetAllObjectIds(Data::ArrayListInt64 *outArr, void **nameArr) = 0;
		virtual UOSInt GetObjectIds(Data::ArrayListInt64 *outArr, void **nameArr, Double mapRate, Int32 x1, Int32 y1, Int32 x2, Int32 y2, Bool keepEmpty) = 0;
		virtual UOSInt GetObjectIdsMapXY(Data::ArrayListInt64 *outArr, void **nameArr, Double x1, Double y1, Double x2, Double y2, Bool keepEmpty) = 0;
		virtual Int64 GetObjectIdMax() = 0;
		virtual void ReleaseNameArr(void *nameArr) = 0;
		virtual UTF8Char *GetString(UTF8Char *buff, UOSInt buffSize, void *nameArr, Int64 id, UOSInt strIndex) = 0;
		virtual UOSInt GetColumnCnt() = 0;
		virtual UTF8Char *GetColumnName(UTF8Char *buff, UOSInt colIndex) = 0;
		virtual DB::DBUtil::ColType GetColumnType(UOSInt colIndex, UOSInt *colSize) = 0;
		virtual Bool GetColumnDef(UOSInt colIndex, DB::ColDef *colDef) = 0;
		virtual Int32 GetCodePage() = 0;
		virtual Bool GetBoundsDbl(Double *minX, Double *minY, Double *maxX, Double *maxY) = 0;

		virtual void *BeginGetObject() = 0;
		virtual void EndGetObject(void *session) = 0;
		virtual DrawObjectL *GetObjectByIdD(void *session, Int64 id) = 0;
		virtual Math::Vector2D *GetVectorById(void *session, Int64 id) = 0;
		virtual void ReleaseObject(void *session, DrawObjectL *obj) = 0;
		virtual void AddUpdatedHandler(UpdatedHandler hdlr, void *obj);
		virtual void RemoveUpdatedHandler(UpdatedHandler hdlr, void *obj);

		virtual UOSInt GetTableNames(Data::ArrayList<const UTF8Char*> *names); // no need to release
		virtual DB::DBReader *GetTableData(const UTF8Char *name, UOSInt maxCnt, void *ordering, void *condition);
		virtual void CloseReader(DB::DBReader *r);
		virtual void GetErrorMsg(Text::StringBuilderUTF *str);
		virtual void Reconnect();

		virtual OSInt GetNameCol();
		virtual void SetNameCol(OSInt nameCol);

		virtual ObjectClass GetObjectClass() = 0;
		const UTF8Char *GetName();
		virtual IO::ParsedObject::ParserType GetParserType();
		virtual Math::CoordinateSystem *GetCoordinateSystem();
		virtual void SetCoordinateSystem(Math::CoordinateSystem *csys);

		Int32 CalBlockSize();

		UTF8Char *GetPGLabelLatLon(UTF8Char *buff, UOSInt buffSize, Double lat, Double lon, Double *outLat, Double *outLon, Int32 strIndex);
		UTF8Char *GetPLLabelLatLon(UTF8Char *buff, UOSInt buffSize, Double lat, Double lon, Double *outLat, Double *outLon, Int32 strIndex);
		Int64 GetNearestObjectId(void *session, Double x, Double y, Double *pointX, Double *pointY);
		OSInt GetNearObjects(void *session, Data::ArrayList<ObjectInfo*> *objList, Double x, Double y, Double maxDist); //return nearest object if no object within distance
		void FreeObjects(Data::ArrayList<ObjectInfo*> *objList);
		Map::VectorLayer *CreateEditableLayer();

		Text::SearchIndexer *CreateSearchIndexer(Text::TextAnalyzer *ta, UOSInt strIndex);
		UOSInt SearchString(Data::ArrayListStrUTF8 *outArr, Text::SearchIndexer *srchInd, void *nameArr, const UTF8Char *srchStr, UOSInt maxResult, UOSInt strIndex);
		void ReleaseSearchStr(Data::ArrayListStrUTF8 *strArr);
		Math::Vector2D *GetVectorByStr(Text::SearchIndexer *srchInd, void *nameArr, void *session, const UTF8Char *srchStr, UOSInt strIndex);

		Bool HasLineStyle();
		Bool HasPGStyle();
		Bool HasIconStyle();
		void SetLineStyle(Int32 lineColor, Int32 lineWidth);
		void SetPGStyle(Int32 pgColor);
		void SetIconStyle(Media::ImageList *iconImg, OSInt iconSpotX, OSInt iconSpotY);
		Int32 GetLineStyleColor();
		Int32 GetLineStyleWidth();
		Int32 GetPGStyleColor();
		Media::ImageList *GetIconStyleImg();
		OSInt GetIconStyleSpotX();
		OSInt GetIconStyleSpotY();
		Bool IsLabelVisible();
		void SetLabelVisible(Bool labelVisible);

		static Map::DrawLayerType VectorType2LayerType(Math::Vector2D::VectorType vtype);
	};

	class MapLayerReader : public DB::DBReader
	{
	protected:
		IMapDrawLayer *layer;
		Data::ArrayListInt64 *objIds; 
		void *nameArr;
		OSInt currIndex;

	public:
		MapLayerReader(IMapDrawLayer *layer);
		~MapLayerReader();

		virtual Bool ReadNext();
		virtual UOSInt ColCount();
		virtual OSInt GetRowChanged();

		virtual Int32 GetInt32(UOSInt colIndex);
		virtual Int64 GetInt64(UOSInt colIndex);
		virtual WChar *GetStr(UOSInt colIndex, WChar *buff);
		virtual Bool GetStr(UOSInt colIndex, Text::StringBuilderUTF *sb);
		virtual const WChar *GetNewStr(UOSInt colIndex);
		virtual UTF8Char *GetStr(UOSInt colIndex, UTF8Char *buff, UOSInt buffSize);
		virtual DateErrType GetDate(UOSInt colIndex, Data::DateTime *outVal); //return 0 = ok, 2 = Error
		virtual Double GetDbl(UOSInt colIndex);
		virtual Bool GetBool(UOSInt colIndex);
		virtual UOSInt GetBinarySize(UOSInt colIndex);
		virtual UOSInt GetBinary(UOSInt colIndex, UInt8 *buff);
		virtual Math::Vector2D *GetVector(UOSInt colIndex);

		virtual Bool IsNull(UOSInt colIndex);
		virtual UTF8Char *GetName(UOSInt colIndex, UTF8Char *buff);
		virtual DB::DBUtil::ColType GetColType(UOSInt colIndex, UOSInt *colSize);
		virtual Bool GetColDef(UOSInt colIndex, DB::ColDef *colDef);
		virtual void DelNewStr(const WChar *s);
	};
}
#endif
