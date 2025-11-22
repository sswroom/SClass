#ifndef _SM_MAP_HKTRAFFICLAYER2
#define _SM_MAP_HKTRAFFICLAYER2
#include "Data/FastMap.hpp"
#include "Map/MapDrawLayer.h"
#include "Net/SocketFactory.h"
#include "Net/SSLEngine.h"
#include "Sync/Mutex.h"
#include "Text/EncodingFactory.h"

namespace Map
{
	class HKRoadNetwork2;

	class HKTrafficLayer2 : public Map::MapDrawLayer
	{
	private:
		struct RoadInfo
		{
			Math::RectAreaDbl bounds;
			Int32 segmentId;
			NN<Math::Geometry::Vector2D> vec;
			Double speed;
			Bool valid;
		};
	private:
		Math::RectAreaDbl bounds;

		NN<Text::String> url;
		NN<Net::TCPClientFactory> clif;
		Optional<Net::SSLEngine> ssl;
		Optional<Text::EncodingFactory> encFact;
		Sync::Mutex roadMut;
		Data::FastMapNN<Int64, RoadInfo> roadMap;
		Data::FastMapNN<Int32, Math::Geometry::Vector2D> vecMap;

		void SetSpeedMap(Int32 segmentId, Double speed, Bool valid);
		Optional<IO::Stream> OpenURLStream();
	public:
		HKTrafficLayer2(NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, Optional<Text::EncodingFactory> encFact, NN<HKRoadNetwork2> rn2);
		virtual ~HKTrafficLayer2();

		void ReloadData();

		virtual DrawLayerType GetLayerType() const;
		virtual UOSInt GetAllObjectIds(NN<Data::ArrayListInt64> outArr, OptOut<Optional<NameArray>> nameArr);
		virtual UOSInt GetObjectIds(NN<Data::ArrayListInt64> outArr, OptOut<Optional<NameArray>> nameArr, Double mapRate, Math::RectArea<Int32> rect, Bool keepEmpty);
		virtual UOSInt GetObjectIdsMapXY(NN<Data::ArrayListInt64> outArr, OptOut<Optional<NameArray>> nameArr, Math::RectAreaDbl rect, Bool keepEmpty);
		virtual Int64 GetObjectIdMax() const;
		virtual UOSInt GetRecordCnt() const;
		virtual void ReleaseNameArr(Optional<NameArray> nameArr);
		virtual Bool GetString(NN<Text::StringBuilderUTF8> sb, Optional<NameArray> nameArr, Int64 id, UOSInt strIndex);
		virtual UOSInt GetColumnCnt() const;
		virtual UnsafeArrayOpt<UTF8Char> GetColumnName(UnsafeArray<UTF8Char> buff, UOSInt colIndex) const;
		virtual DB::DBUtil::ColType GetColumnType(UOSInt colIndex, OptOut<UOSInt> colSize) const;
		virtual Bool GetColumnDef(UOSInt colIndex, NN<DB::ColDef> colDef) const;
		virtual UInt32 GetCodePage() const;
		virtual Bool GetBounds(OutParam<Math::RectAreaDbl> bounds) const;

		virtual NN<GetObjectSess> BeginGetObject();
		virtual void EndGetObject(NN<GetObjectSess> session);
		virtual Optional<Math::Geometry::Vector2D> GetNewVectorById(NN<GetObjectSess> session, Int64 id);
		virtual UOSInt GetGeomCol() const;

		virtual ObjectClass GetObjectClass() const;
	};
}
#endif
