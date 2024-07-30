#ifndef _SM_MAP_HKTRAFFICLAYER
#define _SM_MAP_HKTRAFFICLAYER
#include "Data/FastMapNN.h"
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
			NN<Math::Geometry::Polyline> pl;
		} CenterlineInfo;
	private:
		static const NodeInfo nodeTable[];
		Double minX;
		Double minY;
		Double maxX;
		Double maxY;

		NN<Text::String> url;
		NN<Net::TCPClientFactory> clif;
		Optional<Net::SSLEngine> ssl;
		NN<Text::EncodingFactory> encFact;
		Sync::Mutex roadMut;
		Data::FastMapNN<Int64, RoadInfo> roadMap;
		Data::FastMapNN<Int64, CenterlineInfo> vecMap;

		static const NodeInfo *GetNodeInfo(Int32 nodeId);
		void SetSpeedMap(Int32 fromId, Int32 toId, SaturationLevel lev, Int32 trafficSpeed);
		IO::Stream *OpenURLStream();
	public:
		HKTrafficLayer(NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, NN<Text::EncodingFactory> encFact);
		virtual ~HKTrafficLayer();

		void SetURL(Text::String *url);
		Bool AddRoadLayer(NN<Map::MapDrawLayer> roadLayer);
		void EndInit();
		void ReloadData();

		virtual DrawLayerType GetLayerType() const;
		virtual UOSInt GetAllObjectIds(NN<Data::ArrayListInt64> outArr, OptOut<Optional<NameArray>> nameArr);
		virtual UOSInt GetObjectIds(NN<Data::ArrayListInt64> outArr, OptOut<Optional<NameArray>> nameArr, Double mapRate, Math::RectArea<Int32> rect, Bool keepEmpty);
		virtual UOSInt GetObjectIdsMapXY(NN<Data::ArrayListInt64> outArr, OptOut<Optional<NameArray>> nameArr, Math::RectAreaDbl rect, Bool keepEmpty);
		virtual Int64 GetObjectIdMax() const;
		virtual void ReleaseNameArr(Optional<NameArray> nameArr);
		virtual Bool GetString(NN<Text::StringBuilderUTF8> sb, Optional<NameArray> nameArr, Int64 id, UOSInt strIndex);
		virtual UOSInt GetColumnCnt() const;
		virtual UnsafeArrayOpt<UTF8Char> GetColumnName(UnsafeArray<UTF8Char> buff, UOSInt colIndex);
		virtual DB::DBUtil::ColType GetColumnType(UOSInt colIndex, OptOut<UOSInt> colSize);
		virtual Bool GetColumnDef(UOSInt colIndex, NN<DB::ColDef> colDef);
		virtual UInt32 GetCodePage() const;
		virtual Bool GetBounds(OutParam<Math::RectAreaDbl> bounds) const;

		virtual NN<GetObjectSess> BeginGetObject();
		virtual void EndGetObject(NN<GetObjectSess> session);
		virtual Optional<Math::Geometry::Vector2D> GetNewVectorById(NN<GetObjectSess> session, Int64 id);

		virtual ObjectClass GetObjectClass() const;

		static Map::MapDrawLayer *GetNodePoints();
	};
}
#endif
