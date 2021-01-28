#ifndef _SM_MAP_MAPBAR_MAPBARADJUSTER
#define _SM_MAP_MAPBAR_MAPBARADJUSTER
#include "IO/IWriter.h"
#include "Net/SocketFactory.h"

namespace Map
{
	namespace MapBar
	{
		class MapBarAdjuster
		{
		private:
			Net::SocketFactory *sockf;
			IO::IWriter *errWriter;
		public:
			MapBarAdjuster(Net::SocketFactory *sockf, IO::IWriter *errWriter);
			virtual ~MapBarAdjuster();

			Bool AdjustPoints(Double *srcLatLons, Double *destLatLons, Int32 imgWidth, Int32 imgHeight, OSInt nPoints);
		};
	};
};
#endif
