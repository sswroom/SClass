#ifndef _SM_MAP_MAPBAR_MAPBARREVGEO
#define _SM_MAP_MAPBAR_MAPBARREVGEO
#include "IO/Writer.h"
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
			IO::Writer *errWriter;
			Map::MapBar::MapBarAdjuster *adjuster;
			Int32 imgWidth;
			Int32 imgHeight;

		public:
			MapBarRevGeo(Net::SocketFactory *sockf, IO::Writer *errWriter, Map::MapBar::MapBarAdjuster *adjuster, Int32 imgWidth, Int32 imgHeight);
			virtual ~MapBarRevGeo();

			virtual UTF8Char *SearchName(UTF8Char *buff, UOSInt buffSize, Double lat, Double lon, Int32 lcid);
			virtual UTF8Char *CacheName(UTF8Char *buff, UOSInt buffSize, Double lat, Double lon, Int32 lcid);
			UTF8Char *SearchNameAdjusted(UTF8Char *buff, UOSInt buffSize, Double adjLat, Double adjLon, Int32 lcid);
		};
	}
}
#endif
