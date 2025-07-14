#ifndef _SM_MAP_HKROADNETWORK2
#define _SM_MAP_HKROADNETWORK2
#include "DB/ReadingDB.h"
#include "Map/MapDrawLayer.h"
#include "Math/ArcGISPRJParser.h"
#include "Math/CoordinateSystem.h"
#include "Map/ShortestPath3D.h"
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
		Optional<DB::ReadingDB> fgdb;
	public:
		HKRoadNetwork2(Text::CStringNN fgdbPath, Optional<Math::ArcGISPRJParser> prjParser);
		HKRoadNetwork2(NN<DB::ReadingDB> fgdb);
		~HKRoadNetwork2();

		Bool IsError();
		Optional<DB::ReadingDB> GetDB();
		NN<Math::CoordinateSystem> CreateCoordinateSystem();
		Optional<Map::HKSpeedLimit> CreateSpeedLimit();
		Optional<Map::MapDrawLayer> CreateTonnesSignLayer();
		Optional<Map::HKTrafficLayer2> CreateTrafficLayer(NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, Optional<Text::EncodingFactory> encFact);
		Optional<Map::ShortestPath3D> CreateShortestPath();

		static Text::CStringNN GetDownloadURL();
		static Text::CStringNN GetDefFileName();
	};
}
#endif
