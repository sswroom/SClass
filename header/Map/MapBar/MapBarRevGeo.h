#ifndef _SM_MAP_MAPBAR_MAPBARREVGEO
#define _SM_MAP_MAPBAR_MAPBARREVGEO
#include "IO/IWriter.h"
#include "Net/SocketFactory.h"
#include "Map/IReverseGeocoder.h"
#include "Map/MapBar/MapBarAdjuster.h"

namespace Map
{
	namespace MapBar
	{
		class MapBarRevGeo : public Map::IReverseGeocoder
		{
		private:
			Net::SocketFactory *sockf;
			IO::IWriter *errWriter;
			Map::MapBar::MapBarAdjuster *adjuster;
			Int32 imgWidth;
			Int32 imgHeight;

		public:
			MapBarRevGeo(Net::SocketFactory *sockf, IO::IWriter *errWriter, Map::MapBar::MapBarAdjuster *adjuster, Int32 imgWidth, Int32 imgHeight);
			virtual ~MapBarRevGeo();

			virtual WChar *SearchName(WChar *buff, Double lat, Double lon, Int32 lcid);
			virtual WChar *CacheName(WChar *buff, Double lat, Double lon, Int32 lcid);
			WChar *SearchNameAdjusted(WChar *buff, Double adjLat, Double adjLon, Int32 lcid);
		};
	};
};
#endif
