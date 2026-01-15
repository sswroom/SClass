#ifndef _SM_IO_SPACKAGEFILE
#define _SM_IO_SPACKAGEFILE
#include "Data/ByteBuffer.h"
#include "Data/BTreeStringMapObj.hpp"
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
		Data::BTreeStringMapObj<FileInfo*> fileMap;
		NN<IO::SeekableStream> stm;
		Bool writeMode;
		Sync::Mutex mut;
		Bool toRelease;
		Bool pauseCommit;
		IO::MemoryStream mstm;
		Int32 flags;
		UInt64 currOfst;
		Int32 customType;
		UIntOS customSize;
		Data::ByteBuffer customBuff;

		void ReadV2DirEnt(UInt64 ofst, UInt64 size);
		void AddPackageInner(NN<IO::PackageFile> pkg, UTF8Char pathSeperator, UnsafeArray<UTF8Char> pathStart, UnsafeArray<UTF8Char> pathEnd);
		Bool OptimizeFileInner(IO::SPackageFile *newFile, UInt64 dirOfst, UInt64 dirSize);
	public:
		SPackageFile(NN<IO::SeekableStream> stm, Bool toRelease);
		SPackageFile(NN<IO::SeekableStream> stm, Bool toRelease, Int32 customType, UIntOS customSize, Data::ByteArrayR customBuff);
		SPackageFile(Text::CStringNN fileName);
		~SPackageFile();

		Bool AddFile(NN<IO::StreamData> fd, Text::CStringNN fileName, const Data::Timestamp &modTime);
		Bool AddFile(UnsafeArray<const UInt8> fileBuff, UIntOS fileSize, Text::CStringNN fileName, const Data::Timestamp &modTime);
		Bool AddPackage(NN<IO::PackageFile> pkg, UTF8Char pathSeperator);
		Bool Commit();
		Bool OptimizeFile(Text::CStringNN newFile);
		void PauseCommit(Bool pauseCommit);

		Optional<IO::StreamData> CreateStreamData(Text::CStringNN fileName);
	};
}
#endif
