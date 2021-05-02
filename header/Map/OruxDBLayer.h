#ifndef _SM_MAP_ORUXDBLAYER
#define _SM_MAP_ORUXDBLAYER
#include "DB/DBConn.h"
#include "Map/IMapDrawLayer.h"
#include "Parser/ParserList.h"

namespace Map
{
	class OruxDBLayer : public Map::IMapDrawLayer
	{
	private:
		typedef struct
		{
			Int32 layerId;
			Double mapXMin;
			Double mapXMax;
			Double mapYMin;
			Double mapYMax;
			Double projYMin;
			Double projYMax;
			UInt32 maxX;
			UInt32 maxY;
		} LayerInfo;
	private:
		Parser::ParserList *parsers;
		Data::Int32Map<LayerInfo*> *layerMap;
		Int32 currLayer;
		UInt32 tileSize;
		DB::DBConn *db;

	public:
		OruxDBLayer(const UTF8Char *sourceName, const UTF8Char *layerName, Parser::ParserList *parsers);
		virtual ~OruxDBLayer();

		Bool IsError();
		void AddLayer(Int32 layerId, Double mapXMin, Double mapYMin, Double mapXMax, Double mapYMax, UInt32 maxX, UInt32 maxY, UInt32 tileSize);

		void SetCurrLayer(Int32 level);
		virtual void SetCurrScale(Double scale);
		virtual Map::MapView *CreateMapView(UOSInt width, UOSInt height);

		virtual DrawLayerType GetLayerType();
		virtual UOSInt GetAllObjectIds(Data::ArrayListInt64 *outArr, void **nameArr);
		virtual UOSInt GetObjectIds(Data::ArrayListInt64 *outArr, void **nameArr, Double mapRate, Int32 x1, Int32 y1, Int32 x2, Int32 y2, Bool keepEmpty);
		virtual UOSInt GetObjectIdsMapXY(Data::ArrayListInt64 *outArr, void **nameArr, Double x1, Double y1, Double x2, Double y2, Bool keepEmpty);
		virtual Int64 GetObjectIdMax();
		virtual void ReleaseNameArr(void *nameArr);
		virtual UTF8Char *GetString(UTF8Char *buff, UOSInt buffSize, void *nameArr, Int64 id, UOSInt strIndex);
		virtual UOSInt GetColumnCnt();
		virtual UTF8Char *GetColumnName(UTF8Char *buff, UOSInt colIndex);
		virtual DB::DBUtil::ColType GetColumnType(UOSInt colIndex, UOSInt *colSize);
		virtual Bool GetColumnDef(UOSInt colIndex, DB::ColDef *colDef);
		virtual Int32 GetCodePage();
		virtual Bool GetBoundsDbl(Double *minX, Double *minY, Double *maxX, Double *maxY);

		virtual void *BeginGetObject();
		virtual void EndGetObject(void *session);
		virtual DrawObjectL *GetObjectByIdD(void *session, Int64 id);
		virtual Math::Vector2D *GetVectorById(void *session, Int64 id);
		virtual void ReleaseObject(void *session, DrawObjectL *obj);

		virtual UOSInt GetTableNames(Data::ArrayList<const UTF8Char*> *names);
		virtual DB::DBReader *GetTableData(const UTF8Char *name, UOSInt maxCnt, void *ordering, void *condition);
		virtual void CloseReader(DB::DBReader *r);
		virtual void GetErrorMsg(Text::StringBuilderUTF *str);
		virtual void Reconnect();

		virtual ObjectClass GetObjectClass();

		Bool GetObjectData(Int64 objectId, IO::Stream *stm, Int32 *tileX, Int32 *tileY, Int64 *modTimeTicks);
	};
}
#endif
