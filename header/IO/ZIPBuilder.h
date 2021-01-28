#ifndef _SM_IO_ZIPBUILDER
#define _SM_IO_ZIPBUILDER
#include "Crypto/Hash/CRC32R.h"
#include "Data/ArrayList.h"
#include "IO/SeekableStream.h"

namespace IO
{
	class ZIPBuilder
	{
	private:
		typedef struct
		{
			const UTF8Char *fileName;
			UInt64 fileOfst;
			Int64 fileTimeTicks;
			UInt32 crcVal;
			UOSInt uncompSize;
			UOSInt compSize;
			UInt16 compMeth;
		} FileInfo;
		
	private:
		IO::SeekableStream *stm;
		Crypto::Hash::CRC32R *crc;
		UInt64 baseOfst;
		UInt64 currOfst;
		Data::ArrayList<FileInfo*> *files;

	public:
		ZIPBuilder(IO::SeekableStream *stm);
		~ZIPBuilder();

		Bool AddFile(const UTF8Char *fileName, const UInt8 *fileContent, UOSInt fileSize, Int64 fileTimeTicks, Bool storeOnly);
	};
}
#endif
