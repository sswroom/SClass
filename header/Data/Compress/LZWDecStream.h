#ifndef _SM_DATA_COMPRESS_LZWDECSTREAM
#define _SM_DATA_COMPRESS_LZWDECSTREAM
#include "IO/Stream.h"
#include "IO/BitReader.h"

namespace Data
{
	namespace Compress
	{
		class LZWDecStream : public IO::Stream
		{
		private:
			UOSInt minCodeSize;
			UOSInt maxCodeSize;
			UOSInt currCodeSize;
			UOSInt currTableSize;
			UOSInt nextTableSize;
			UInt32 resetCode;
			UInt32 endCode;
			UInt8 *lzwTable;
			UOSInt tableSize;
			UInt8 *decBuff;
			UOSInt decBuffSize;
			UOSInt codeSizeAdj;
			UInt32 localCode;
			IO::BitReader *reader;
			Bool toRelease;

		private:
			void ResetTable();
		public:
			LZWDecStream(IO::Stream *stm, Bool lsb, UOSInt minCodeSize, UOSInt maxCodeSize, UOSInt codeSizeAdj);
			LZWDecStream(IO::BitReader *reader, Bool toRelease, UOSInt minCodeSize, UOSInt maxCodeSize, UOSInt codeSizeAdj);
			virtual ~LZWDecStream();

			virtual Bool IsDown() const;
			virtual UOSInt Read(const Data::ByteArray &buff);
			virtual UOSInt Write(UnsafeArray<const UInt8> buff, UOSInt size);

			virtual Int32 Flush();
			virtual void Close();
			virtual Bool Recover();
			virtual IO::StreamType GetStreamType() const;
		};
	}
}

#endif
