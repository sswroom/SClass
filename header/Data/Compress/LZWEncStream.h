#ifndef _SM_DATA_COMPRESS_LZWENCSTREAM
#define _SM_DATA_COMPRESS_LZWENCSTREAM
#include "IO/Stream.h"
#include "IO/BitWriter.h"

namespace Data
{
	namespace Compress
	{
		class LZWEncStream : public IO::Stream
		{
		private:
			typedef struct
			{
				Int32 prevCode;
				UInt8 outByte;
				UInt8 outLen;
				UInt8 firstByte;
				UInt8 reserved2;
			} TableItem;
		private:
			OSInt minCodeSize;
			OSInt maxCodeSize;
			OSInt currCodeSize;
			OSInt currTableSize;
			OSInt nextTableSize;
			TableItem *lzwTable;
			UInt8 *encBuff;
			OSInt buffSize;
			Int32 resetCode;
			Int32 endCode;
			OSInt tableSize;
			OSInt codeSizeAdj;
			IO::BitWriter *writer;
			Bool toRelease;
			OSInt maxCodeLeng;


		private:
			void ResetTable();
		public:
			LZWEncStream(IO::Stream *stm, Bool lsb, OSInt minCodeSize, OSInt maxCodeSize, OSInt codeSizeAdj);
			virtual ~LZWEncStream();

			virtual OSInt Read(UInt8 *buff, OSInt size);
			virtual OSInt Write(const UInt8 *buff, OSInt size);

			virtual Int32 Flush();
			virtual void Close();
			virtual Bool Recover();
		};
	};
};

#endif
