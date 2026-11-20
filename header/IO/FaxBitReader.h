#ifndef _SM_IO_FAXBITREADER
#define _SM_IO_FAXBITREADER
#include "Data/ByteArray.h"

namespace IO
{
	struct FaxCode;
	class FaxBitReader
	{
	private:
		static const FaxCode whiteCodes[];
		static const FaxCode blackCodes[];

	private:
		Data::ByteArrayR data;
		UIntOS bitPos;
	public:
		FaxBitReader(Data::ByteArrayR data);

		Bool ReadBit(UInt32 &bit);
		void AlignByte();
		Bool ReadEOL();
		Bool ReadCode(UnsafeArray<const FaxCode> codes, UIntOS codeCnt, UIntOS &run);
		Bool ReadRun(Bool black, UIntOS width, UIntOS &run);
	};
}

#endif
