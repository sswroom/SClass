#ifndef _SM_MAP_HKROADNETWORK2
#define _SM_MAP_HKROADNETWORK2
#include "DB/ReadingDB.h"
#include "Map/MapDrawLayer.h"
#include "Math/ArcGISPRJParser.h"
#include "Math/CoordinateSystem.h"
#include "Net/SSLEngine.h"
#include "Text/CString.h"
#include "Text/EncodingFactory.h"

namespace Map
{
	class HKSpeedLimit;
	class HKTrafficLayer2;

	class HKRoadNetwork2
	{
	private:
		DB::ReadingDB *fgdb;
	public:
		HKRoadNetwork2(Text::CString fgdbPath, Math::ArcGISPRJParser *prjParser);
		HKRoadNetwork2(DB::ReadingDB *fgdb);
		~HKRoadNetwork2();

		Bool IsError();
		DB::ReadingDB *GetDB();
		Math::CoordinateSystem *CreateCoordinateSystem();
		Map::HKSpeedLimit *CreateSpeedLimit();
		Map::MapDrawLayer *CreateTonnesSignLayer();
		Map::HKTrafficLayer2 *CreateTrafficLayer(Net::SocketFactory *sockf, Net::SSLEngine *ssl, Text::EncodingFactory *encFact);

		static Text::CString GetDownloadURL();
		static Text::CString GetDefFileName();
	};
}
#endif
