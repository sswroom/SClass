#ifndef _SM_IO_ZIPBUILDER
#define _SM_IO_ZIPBUILDER
#include "Crypto/Hash/CRC32RIEEE.h"
#include "Data/ArrayListObj.hpp"
#include "Data/Timestamp.h"
#include "Data/Compress/Inflate.h"
#include "IO/BufferedOutputStream.h"
#include "IO/SeekableStream.h"
#include "Sync/Mutex.h"

namespace IO
{
	enum class ZIPOS
	{
		MSDOS,
		Amiga,
		OpenVMS,
		UNIX,
		VM_CMS,
		Atari_ST,
		OS2_HPFS,
		Macintosh,
		ZSystem,
		CP_M,
		NTFS,
		MVS,
		VSE,
		AcornRisc,
		VFAT,
		AlternateMVS,
		BeOS,
		Tandem,
		OS400,
		OSX
	};

	class ZIPBuilder
	{
	private:
		typedef struct
		{
			NN<Text::String> fileName;
			UInt64 fileOfst;
			Data::Timestamp fileModTime;
			Data::Timestamp fileCreateTime;
			Data::Timestamp fileAccessTime;
			UInt32 crcVal;
			UInt64 uncompSize;
			UInt64 compSize;
			UInt16 compMeth;
			UInt32 fileAttr;
/*
DOS:
0 - ReadOnly
1 - Unknown
2 - Unknown
3 - Unknown
4 - Directory

Unix:
TTTTsstrwxrwxrwx0000000000ADVSHR
^^^^____________________________ file type as explained above
    ^^^_________________________ setuid, setgid, sticky
       ^^^^^^^^^________________ permissions
                ^^^^^^^^________ This is the "lower-middle byte" your post mentions
                        ^^^^^^^^ DOS attribute bits
*/
		} FileInfo;
		
	private:
		IO::BufferedOutputStream stm;
		NN<IO::SeekableStream> outStm;
		Crypto::Hash::CRC32RIEEE crc;
		UInt64 baseOfst;
		UInt64 currOfst;
		Data::ArrayListObj<FileInfo*> files;
		Sync::Mutex mut;
		ZIPOS osType;

	public:
		ZIPBuilder(NN<IO::SeekableStream> stm, ZIPOS os);
		~ZIPBuilder();

		Bool AddFile(Text::CStringNN fileName, UnsafeArray<const UInt8> fileContent, UIntOS fileSize, Data::Timestamp lastModTime, Data::Timestamp lastAccessTime, Data::Timestamp createTime, Data::Compress::Inflate::CompressionLevel compLevel, UInt32 unixAttr);
		Bool AddDir(Text::CStringNN dirName, Data::Timestamp lastModTime, Data::Timestamp lastAccessTime, Data::Timestamp createTime, UInt32 unixAttr);
		Bool AddDeflate(Text::CStringNN fileName, Data::ByteArrayR buff, UInt64 decSize, UInt32 crcVal, Data::Timestamp lastModTime, Data::Timestamp lastAccessTime, Data::Timestamp createTime, UInt32 unixAttr);
	};
}
#endif
