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
			OSInt minCodeSize;
			OSInt maxCodeSize;
			OSInt currCodeSize;
			OSInt currTableSize;
			OSInt nextTableSize;
			Int32 resetCode;
			Int32 endCode;
			UInt8 *lzwTable;
			OSInt tableSize;
			UInt8 *decBuff;
			UOSInt decBuffSize;
			OSInt codeSizeAdj;
			Int16 localCode;
			IO::BitReader *reader;
			Bool toRelease;

		private:
			void ResetTable();
		public:
			LZWDecStream(IO::Stream *stm, Bool lsb, OSInt minCodeSize, OSInt maxCodeSize, OSInt codeSizeAdj);
			LZWDecStream(IO::BitReader *reader, Bool toRelease, OSInt minCodeSize, OSInt maxCodeSize, OSInt codeSizeAdj);
			virtual ~LZWDecStream();

			virtual UOSInt Read(UInt8 *buff, UOSInt size);
			virtual UOSInt Write(const UInt8 *buff, UOSInt size);

			virtual Int32 Flush();
			virtual void Close();
			virtual Bool Recover();
		};
	}
}

#endif
