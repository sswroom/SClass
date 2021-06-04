#ifndef _SM_IO_SPACKAGEFILE
#define _SM_IO_SPACKAGEFILE
#include "Data/BTreeUTF8Map.h"
#include "IO/IStreamData.h"
#include "IO/MemoryStream.h"
#include "IO/PackageFile.h"
#include "IO/SeekableStream.h"
#include "Sync/Mutex.h"

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
		Data::BTreeUTF8Map<FileInfo*> *fileMap;
		IO::SeekableStream *stm;
		Bool writeMode;
		Sync::Mutex *mut;
		Bool toRelease;
		Bool pauseCommit;
		IO::MemoryStream *mstm;
		Int32 flags;
		UInt64 currOfst;
		Int32 customType;
		UOSInt customSize;
		UInt8 *customBuff;

		void ReadV2DirEnt(UInt64 ofst, UInt64 size);
		void AddPackageInner(IO::PackageFile *pkg, UTF8Char pathSeperator, UTF8Char *pathStart, UTF8Char *pathEnd);
		Bool OptimizeFileInner(IO::SPackageFile *newFile, UInt64 dirOfst, UInt64 dirSize);
	public:
		SPackageFile(IO::SeekableStream *stm, Bool toRelease);
		SPackageFile(IO::SeekableStream *stm, Bool toRelease, Int32 customType, UOSInt customSize, const UInt8 *customBuff);
		SPackageFile(const UTF8Char *fileName);
		~SPackageFile();

		Bool AddFile(IO::IStreamData *fd, const UTF8Char *fileName, Int64 modTimeTicks);
		Bool AddFile(const UInt8 *fileBuff, UOSInt fileSize, const UTF8Char *fileName, Int64 modTimeTicks);
		Bool AddPackage(IO::PackageFile *pkg, UTF8Char pathSeperator);
		Bool Commit();
		Bool OptimizeFile(const UTF8Char *newFile);
		void PauseCommit(Bool pauseCommit);

		IO::IStreamData *CreateStreamData(const UTF8Char *fileName);
	};
}
#endif
