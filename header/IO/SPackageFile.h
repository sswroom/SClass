#ifndef _SM_IO_SPACKAGEFILE
#define _SM_IO_SPACKAGEFILE
#include "Data/ByteBuffer.h"
#include "Data/BTreeUTF8Map.h"
#include "IO/StreamData.h"
#include "IO/MemoryStream.h"
#include "IO/PackageFile.h"
#include "IO/SeekableStream.h"
#include "Sync/Mutex.h"
#include "Text/CString.h"

namespace IO
{
	class SPackageFile
	{
	private:
		typedef struct
		{
			UInt64 ofst;
			UInt64 size;
		} FileInfo;
	private:
		Data::BTreeUTF8Map<FileInfo*> fileMap;
		NotNullPtr<IO::SeekableStream> stm;
		Bool writeMode;
		Sync::Mutex mut;
		Bool toRelease;
		Bool pauseCommit;
		IO::MemoryStream mstm;
		Int32 flags;
		UInt64 currOfst;
		Int32 customType;
		UOSInt customSize;
		Data::ByteBuffer customBuff;

		void ReadV2DirEnt(UInt64 ofst, UInt64 size);
		void AddPackageInner(IO::PackageFile *pkg, UTF8Char pathSeperator, UTF8Char *pathStart, UTF8Char *pathEnd);
		Bool OptimizeFileInner(IO::SPackageFile *newFile, UInt64 dirOfst, UInt64 dirSize);
	public:
		SPackageFile(NotNullPtr<IO::SeekableStream> stm, Bool toRelease);
		SPackageFile(NotNullPtr<IO::SeekableStream> stm, Bool toRelease, Int32 customType, UOSInt customSize, Data::ByteArrayR customBuff);
		SPackageFile(Text::CString fileName);
		~SPackageFile();

		Bool AddFile(IO::StreamData *fd, Text::CString fileName, const Data::Timestamp &modTime);
		Bool AddFile(const UInt8 *fileBuff, UOSInt fileSize, Text::CString fileName, const Data::Timestamp &modTime);
		Bool AddPackage(IO::PackageFile *pkg, UTF8Char pathSeperator);
		Bool Commit();
		Bool OptimizeFile(Text::CString newFile);
		void PauseCommit(Bool pauseCommit);

		IO::StreamData *CreateStreamData(Text::CString fileName);
	};
}
#endif
