#ifndef _SM_DATA_COMPRESS_INFLATESTREAM
#define _SM_DATA_COMPRESS_INFLATESTREAM
#include "IO/Stream.h"

namespace Data
{
	namespace Compress
	{
		class InflateStream : public IO::Stream
		{
		private:
			NN<IO::Stream> outStm;
			void *cmpInfo;
			UInt8 *writeBuff;
			UIntOS headerSize;
		public:
			InflateStream(NN<IO::Stream> outStm, UIntOS headerSize, Bool zlibHeader);
			InflateStream(NN<IO::Stream> outStm, Bool zlibHeader);
			virtual ~InflateStream();

			virtual Bool IsDown() const;
			virtual UIntOS Read(const Data::ByteArray &buff);
			virtual UIntOS Write(Data::ByteArrayR buff);

			virtual Int32 Flush();
			virtual void Close();
			virtual Bool Recover();
			virtual IO::StreamType GetStreamType() const;
		};
	}
}

#endif
