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
			UIntOS minCodeSize;
			UIntOS maxCodeSize;
			UIntOS currCodeSize;
			UIntOS currTableSize;
			UIntOS nextTableSize;
			UInt32 resetCode;
			UInt32 endCode;
			UnsafeArray<UInt8> lzwTable;
			UIntOS tableSize;
			UnsafeArray<UInt8> decBuff;
			UIntOS decBuffSize;
			UIntOS codeSizeAdj;
			UInt32 localCode;
			NN<IO::BitReader> reader;
			Bool toRelease;

		private:
			void ResetTable();
		public:
			LZWDecStream(NN<IO::Stream> stm, Bool lsb, UIntOS minCodeSize, UIntOS maxCodeSize, UIntOS codeSizeAdj);
			LZWDecStream(NN<IO::BitReader> reader, Bool toRelease, UIntOS minCodeSize, UIntOS maxCodeSize, UIntOS codeSizeAdj);
			virtual ~LZWDecStream();

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
