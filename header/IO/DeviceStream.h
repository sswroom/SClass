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
		virtual UIntOS Read(const Data::ByteArray &buff);
		virtual UIntOS Write(Data::ByteArrayR buff);
		virtual Int32 Flush();
		virtual void Close();
		virtual Bool Recover();
		virtual IO::StreamType GetStreamType() const;
	};
}
#endif
