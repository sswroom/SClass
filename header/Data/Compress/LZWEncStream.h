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
			IntOS minCodeSize;
			IntOS maxCodeSize;
			IntOS currCodeSize;
			IntOS currTableSize;
			IntOS nextTableSize;
			UnsafeArray<TableItem> lzwTable;
			UnsafeArray<UInt8> encBuff;
			IntOS buffSize;
			Int32 resetCode;
			Int32 endCode;
			IntOS tableSize;
			IntOS codeSizeAdj;
			NN<IO::BitWriter> writer;
			Bool toRelease;
			IntOS maxCodeLeng;


		private:
			void ResetTable();
		public:
			LZWEncStream(NN<IO::Stream> stm, Bool lsb, UIntOS minCodeSize, UIntOS maxCodeSize, UIntOS codeSizeAdj);
			virtual ~LZWEncStream();

			virtual UIntOS Read(const Data::ByteArray &buff);
			virtual UIntOS Write(Data::ByteArrayR buff);

			virtual Int32 Flush();
			virtual void Close();
			virtual Bool Recover();
		};
	};
};

#endif
