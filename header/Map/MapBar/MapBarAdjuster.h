#ifndef _SM_MAP_MAPBAR_MAPBARADJUSTER
#define _SM_MAP_MAPBAR_MAPBARADJUSTER
#include "IO/Writer.h"
#include "Net/SocketFactory.h"

namespace Map
{
	namespace MapBar
	{
		class MapBarAdjuster
		{
		private:
			NotNullPtr<Net::SocketFactory> sockf;
			IO::Writer *errWriter;
		public:
			MapBarAdjuster(NotNullPtr<Net::SocketFactory> sockf, IO::Writer *errWriter);
			virtual ~MapBarAdjuster();

			Bool AdjustPoints(Double *srcLatLons, Double *destLatLons, Int32 imgWidth, Int32 imgHeight, OSInt nPoints);
		};
	}
}
#endif
