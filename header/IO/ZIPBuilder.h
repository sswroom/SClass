#ifndef _SM_IO_ZIPBUILDER
#define _SM_IO_ZIPBUILDER
#include "Crypto/Hash/CRC32RIEEE.h"
#include "Data/ArrayList.h"
#include "Data/Compress/Inflate.h"
#include "IO/SeekableStream.h"
#include "Sync/Mutex.h"

namespace IO
{
	class ZIPBuilder
	{
	private:
		typedef struct
		{
			NotNullPtr<Text::String> fileName;
			UInt64 fileOfst;
			Int64 fileTimeTicks;
			UInt32 crcVal;
			UOSInt uncompSize;
			UOSInt compSize;
			UInt16 compMeth;
		} FileInfo;
		
	private:
		IO::SeekableStream *stm;
		Crypto::Hash::CRC32RIEEE crc;
		UInt64 baseOfst;
		UInt64 currOfst;
		Data::ArrayList<FileInfo*> files;
		Sync::Mutex mut;

	public:
		ZIPBuilder(IO::SeekableStream *stm);
		~ZIPBuilder();

		Bool AddFile(Text::CString fileName, const UInt8 *fileContent, UOSInt fileSize, Int64 fileTimeTicks, Data::Compress::Inflate::CompressionLevel compLevel);
	};
}
#endif
