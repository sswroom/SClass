#ifndef _SM_MAP_HKTRAFFICLAYER
#define _SM_MAP_HKTRAFFICLAYER
#include "Data/Int64Map.h"
#include "Map/IMapDrawLayer.h"
#include "Net/SocketFactory.h"
#include "Sync/Mutex.h"
#include "Text/EncodingFactory.h"

namespace Map
{
	class HKTrafficLayer : public Map::IMapDrawLayer
	{
	private:
		typedef enum
		{
			SL_GOOD,
			SL_AVERAGE,
			SL_BAD
		} SaturationLevel;

		typedef struct
		{
			Int32 id;
			Double x;
			Double y;
		} NodeInfo;

		typedef struct
		{
			Int64 objId;
			Int32 fromId;
			Int32 toId;
			Math::Vector2D *vec;
			SaturationLevel lev;
			Int32 spd;
			Double minX;
			Double minY;
			Double maxX;
			Double maxY;
		} RoadInfo;

		typedef struct
		{
			Int32 fromId;
			Int32 toId;
			Math::Polyline *pl;
		} CenterlineInfo;
	private:
		static const NodeInfo nodeTable[];
		Double minX;
		Double minY;
		Double maxX;
		Double maxY;

		const UTF8Char *url;
		Net::SocketFactory *sockf;
		Text::EncodingFactory *encFact;
		Sync::Mutex *roadMut;
		Data::Int64Map<RoadInfo*> *roadMap;
		Data::Int64Map<CenterlineInfo*> *vecMap;

		static const NodeInfo *GetNodeInfo(Int32 nodeId);
		void SetSpeedMap(Int32 fromId, Int32 toId, SaturationLevel lev, Int32 trafficSpeed);
		IO::Stream *OpenURLStream();
	public:
		HKTrafficLayer(Net::SocketFactory *sockf, Text::EncodingFactory *encFact);
		virtual ~HKTrafficLayer();

		void SetURL(const UTF8Char *url);
		Bool AddRoadLayer(Map::IMapDrawLayer *roadLayer);
		void EndInit();
		void ReloadData();

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

		virtual ObjectClass GetObjectClass();

		static Map::IMapDrawLayer *GetNodePoints();
	};
}
#endif
