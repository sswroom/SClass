#ifndef _SM_MAP_HKTRAFFICLAYER
#define _SM_MAP_HKTRAFFICLAYER
#include "Data/FastMapNN.hpp"
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
			Optional<Math::Geometry::Vector2D> vec;
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

		static Optional<const NodeInfo> GetNodeInfo(Int32 nodeId);
		void SetSpeedMap(Int32 fromId, Int32 toId, SaturationLevel lev, Int32 trafficSpeed);
		Optional<IO::Stream> OpenURLStream();
	public:
		HKTrafficLayer(NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, NN<Text::EncodingFactory> encFact);
		virtual ~HKTrafficLayer();

		void SetURL(NN<Text::String> url);
		Bool AddRoadLayer(NN<Map::MapDrawLayer> roadLayer);
		void EndInit();
		void ReloadData();

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
		virtual UIntOS GetGeomCol() const;

		virtual ObjectClass GetObjectClass() const;

		static Optional<Map::MapDrawLayer> GetNodePoints();
	};
}
#endif
