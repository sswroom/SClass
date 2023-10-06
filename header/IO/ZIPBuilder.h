#ifndef _SM_IO_ZIPBUILDER
#define _SM_IO_ZIPBUILDER
#include "Crypto/Hash/CRC32RIEEE.h"
#include "Data/ArrayList.h"
#include "Data/Timestamp.h"
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
			Data::Timestamp fileModTime;
			Data::Timestamp fileCreateTime;
			Data::Timestamp fileAccessTime;
			UInt32 crcVal;
			UInt64 uncompSize;
			UInt64 compSize;
			UInt16 compMeth;
		} FileInfo;
		
	private:
		NotNullPtr<IO::SeekableStream> stm;
		Crypto::Hash::CRC32RIEEE crc;
		UInt64 baseOfst;
		UInt64 currOfst;
		Data::ArrayList<FileInfo*> files;
		Sync::Mutex mut;

	public:
		ZIPBuilder(NotNullPtr<IO::SeekableStream> stm);
		~ZIPBuilder();

		Bool AddFile(Text::CStringNN fileName, const UInt8 *fileContent, UOSInt fileSize, Data::Timestamp lastModTime, Data::Timestamp lastAccessTime, Data::Timestamp createTime, Data::Compress::Inflate::CompressionLevel compLevel);
		Bool AddDir(Text::CStringNN dirName, Data::Timestamp lastModTime, Data::Timestamp lastAccessTime, Data::Timestamp createTime);
	};
}
#endif
