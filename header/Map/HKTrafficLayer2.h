#ifndef _SM_MAP_HKTRAFFICLAYER2
#define _SM_MAP_HKTRAFFICLAYER2
#include "Data/FastMap.h"
#include "Map/IMapDrawLayer.h"
#include "Net/SocketFactory.h"
#include "Net/SSLEngine.h"
#include "Sync/Mutex.h"
#include "Text/EncodingFactory.h"

namespace Map
{
	class HKRoadNetwork2;

	class HKTrafficLayer2 : public Map::IMapDrawLayer
	{
	private:
		struct RoadInfo
		{
			Math::RectAreaDbl bounds;
			Int32 segmentId;
			Math::Geometry::Vector2D *vec;
			Double speed;
			Bool valid;
		};
	private:
		Math::RectAreaDbl bounds;

		Text::String *url;
		Net::SocketFactory *sockf;
		Net::SSLEngine *ssl;
		Text::EncodingFactory *encFact;
		Sync::Mutex roadMut;
		Data::FastMap<Int64, RoadInfo*> roadMap;
		Data::FastMap<Int32, Math::Geometry::Vector2D*> vecMap;

		void SetSpeedMap(Int32 segmentId, Double speed, Bool valid);
		IO::Stream *OpenURLStream();
	public:
		HKTrafficLayer2(Net::SocketFactory *sockf, Net::SSLEngine *ssl, Text::EncodingFactory *encFact, HKRoadNetwork2 *rn2);
		virtual ~HKTrafficLayer2();

		void ReloadData();

		virtual DrawLayerType GetLayerType();
		virtual UOSInt GetAllObjectIds(Data::ArrayListInt64 *outArr, void **nameArr);
		virtual UOSInt GetObjectIds(Data::ArrayListInt64 *outArr, void **nameArr, Double mapRate, Math::RectArea<Int32> rect, Bool keepEmpty);
		virtual UOSInt GetObjectIdsMapXY(Data::ArrayListInt64 *outArr, void **nameArr, Math::RectAreaDbl rect, Bool keepEmpty);
		virtual Int64 GetObjectIdMax();
		virtual void ReleaseNameArr(void *nameArr);
		virtual UTF8Char *GetString(UTF8Char *buff, UOSInt buffSize, void *nameArr, Int64 id, UOSInt strIndex);
		virtual UOSInt GetColumnCnt();
		virtual UTF8Char *GetColumnName(UTF8Char *buff, UOSInt colIndex);
		virtual DB::DBUtil::ColType GetColumnType(UOSInt colIndex, UOSInt *colSize);
		virtual Bool GetColumnDef(UOSInt colIndex, DB::ColDef *colDef);
		virtual UInt32 GetCodePage();
		virtual Bool GetBounds(Math::RectAreaDbl *bounds);

		virtual void *BeginGetObject();
		virtual void EndGetObject(void *session);
		virtual Math::Geometry::Vector2D *GetNewVectorById(void *session, Int64 id);

		virtual ObjectClass GetObjectClass();
	};
}
#endif
