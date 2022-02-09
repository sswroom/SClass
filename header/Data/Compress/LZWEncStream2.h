#ifndef _SM_DATA_COMPRESS_LZWENCSTREAM2
#define _SM_DATA_COMPRESS_LZWENCSTREAM2
#include "IO/Stream.h"
#include "IO/BitWriter.h"

namespace Data
{
	namespace Compress
	{
		class LZWEncStream2 : public IO::Stream
		{
		private:
			UOSInt minCodeSize;
			UOSInt maxCodeSize;
			UOSInt currCodeSize;
			UInt8 *encBuff;
			UOSInt buffSize;
			UInt16 currTableSize;
			UInt16 nextTableSize;
			UInt16 *lzwTable;
			UInt16 resetCode;
			UInt16 endCode;
			UInt16 nextTableOfst;
			UOSInt tableSize;
			OSInt codeSizeAdj;
			IO::Stream *stm;
			IO::BitWriter *writer;
			Bool toRelease;

		private:
			void ResetTable();
		public:
			LZWEncStream2(IO::Stream *stm, Bool lsb, UOSInt minCodeSize, UOSInt maxCodeSize, OSInt codeSizeAdj);
			virtual ~LZWEncStream2();

			virtual Bool IsDown();
			virtual UOSInt Read(UInt8 *buff, UOSInt size);
			virtual UOSInt Write(const UInt8 *buff, UOSInt size);

			virtual Int32 Flush();
			virtual void Close();
			virtual Bool Recover();
		};
	}
}

#endif
