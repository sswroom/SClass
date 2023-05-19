#ifndef _SM_IO_DEVICE_AXCAN
#define _SM_IO_DEVICE_AXCAN
#include "IO/CANHandler.h"
#include "IO/Reader.h"

namespace IO
{
	namespace Device
	{
		class AXCAN
		{
		private:
			CANHandler *hdlr;
		public:
			AXCAN(CANHandler *hdlr);
			~AXCAN();

			void ParseReader(IO::Reader *reader);
		};
	}
}
#endif
