#ifndef _SM_IO_DEVICESTREAM
#define _SM_IO_DEVICESTREAM
#include "IO/Stream.h"

namespace IO
{
	class DeviceStream : public IO::Stream
	{
	private:
		void *hand;

	public:
		DeviceStream(Text::CStringNN devPath);
		virtual ~DeviceStream();

		virtual Bool IsDown() const;
		virtual UOSInt Read(const Data::ByteArray &buff);
		virtual UOSInt Write(const UInt8 *buff, UOSInt size);
		virtual Int32 Flush();
		virtual void Close();
		virtual Bool Recover();
		virtual IO::StreamType GetStreamType() const;
	};
}
#endif
