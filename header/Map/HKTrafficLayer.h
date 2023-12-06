#ifndef _SM_MAP_HKTRAFFICLAYER
#define _SM_MAP_HKTRAFFICLAYER
#include "Data/FastMap.h"
#include "Map/MapDrawLayer.h"
#include "Net/SocketFactory.h"
#include "Net/SSLEngine.h"
#include "Sync/Mutex.h"
#include "Text/EncodingFactory.h"

namespace Map
{
	class HKTrafficLayer : public Map::MapDrawLayer
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
			Math::Geometry::Vector2D *vec;
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
			Math::Geometry::Polyline *pl;
		} CenterlineInfo;
	private:
		static const NodeInfo nodeTable[];
		Double minX;
		Double minY;
		Double maxX;
		Double maxY;

		NotNullPtr<Text::String> url;
		NotNullPtr<Net::SocketFactory> sockf;
		Optional<Net::SSLEngine> ssl;
		Text::EncodingFactory *encFact;
		Sync::Mutex roadMut;
		Data::FastMap<Int64, RoadInfo*> roadMap;
		Data::FastMap<Int64, CenterlineInfo*> vecMap;

		static const NodeInfo *GetNodeInfo(Int32 nodeId);
		void SetSpeedMap(Int32 fromId, Int32 toId, SaturationLevel lev, Int32 trafficSpeed);
		IO::Stream *OpenURLStream();
	public:
		HKTrafficLayer(NotNullPtr<Net::SocketFactory> sockf, Optional<Net::SSLEngine> ssl, Text::EncodingFactory *encFact);
		virtual ~HKTrafficLayer();

		void SetURL(Text::String *url);
		Bool AddRoadLayer(Map::MapDrawLayer *roadLayer);
		void EndInit();
		void ReloadData();

		virtual DrawLayerType GetLayerType() const;
		virtual UOSInt GetAllObjectIds(NotNullPtr<Data::ArrayListInt64> outArr, NameArray **nameArr);
		virtual UOSInt GetObjectIds(NotNullPtr<Data::ArrayListInt64> outArr, NameArray **nameArr, Double mapRate, Math::RectArea<Int32> rect, Bool keepEmpty);
		virtual UOSInt GetObjectIdsMapXY(NotNullPtr<Data::ArrayListInt64> outArr, NameArray **nameArr, Math::RectAreaDbl rect, Bool keepEmpty);
		virtual Int64 GetObjectIdMax() const;
		virtual void ReleaseNameArr(NameArray *nameArr);
		virtual Bool GetString(NotNullPtr<Text::StringBuilderUTF8> sb, NameArray *nameArr, Int64 id, UOSInt strIndex);
		virtual UOSInt GetColumnCnt() const;
		virtual UTF8Char *GetColumnName(UTF8Char *buff, UOSInt colIndex);
		virtual DB::DBUtil::ColType GetColumnType(UOSInt colIndex, OptOut<UOSInt> colSize);
		virtual Bool GetColumnDef(UOSInt colIndex, NotNullPtr<DB::ColDef> colDef);
		virtual UInt32 GetCodePage() const;
		virtual Bool GetBounds(OutParam<Math::RectAreaDbl> bounds) const;

		virtual GetObjectSess *BeginGetObject();
		virtual void EndGetObject(GetObjectSess *session);
		virtual Math::Geometry::Vector2D *GetNewVectorById(GetObjectSess *session, Int64 id);

		virtual ObjectClass GetObjectClass() const;

		static Map::MapDrawLayer *GetNodePoints();
	};
}
#endif
