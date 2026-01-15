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
			UIntOS minCodeSize;
			UIntOS maxCodeSize;
			UIntOS currCodeSize;
			UnsafeArray<UInt8> encBuff;
			UIntOS buffSize;
			UInt16 currTableSize;
			UInt16 nextTableSize;
			UnsafeArray<UInt16> lzwTable;
			UInt16 resetCode;
			UInt16 endCode;
			UInt16 nextTableOfst;
			UIntOS tableSize;
			IntOS codeSizeAdj;
			NN<IO::Stream> stm;
			NN<IO::BitWriter> writer;
			Bool toRelease;

		private:
			void ResetTable();
		public:
			LZWEncStream2(NN<IO::Stream> stm, Bool lsb, UIntOS minCodeSize, UIntOS maxCodeSize, IntOS codeSizeAdj);
			virtual ~LZWEncStream2();

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
