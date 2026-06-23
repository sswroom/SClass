#ifndef _SM_MAP_MAPBAR_MAPBARREVGEO
#define _SM_MAP_MAPBAR_MAPBARREVGEO
#include "IO/Writer.h"
#include "Net/SocketFactory.h"
#include "Map/ReverseGeocoder.h"
#include "Map/MapBar/MapBarAdjuster.h"

namespace Map
{
	namespace MapBar
	{
		class MapBarRevGeo : public Map::ReverseGeocoder
		{
		private:
			NN<Net::SocketFactory> sockf;
			NN<IO::Writer> errWriter;
			NN<Map::MapBar::MapBarAdjuster> adjuster;
			Int32 imgWidth;
			Int32 imgHeight;

		public:
			MapBarRevGeo(NN<Net::SocketFactory> sockf, NN<IO::Writer> errWriter, NN<Map::MapBar::MapBarAdjuster> adjuster, Int32 imgWidth, Int32 imgHeight);
			virtual ~MapBarRevGeo();

			virtual UnsafeArrayOpt<UTF8Char> SearchName(UnsafeArray<UTF8Char> buff, UIntOS buffSize, Double lat, Double lon, Int32 lcid);
			virtual UnsafeArrayOpt<UTF8Char> CacheName(UnsafeArray<UTF8Char> buff, UIntOS buffSize, Double lat, Double lon, Int32 lcid);
			UnsafeArrayOpt<UTF8Char> SearchNameAdjusted(UnsafeArray<UTF8Char> buff, UIntOS buffSize, Double adjLat, Double adjLon, Int32 lcid);
		};
	}
}
#endif
