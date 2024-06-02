#ifndef _SM_IO_FILEUTIL
#define _SM_IO_FILEUTIL
#include "IO/ActiveStreamReader.h"
#include "IO/ProgressHandler.h"
#include "Text/CString.h"

namespace IO
{
	class FileUtil
	{
	public:
		enum class FileExistAction
		{
			Fail,
			Overwrite,
			Continue
		};
	public:
		static Bool DeleteFile(Text::CStringNN file, Bool deleteRdonlyFile);
//		static Bool DeleteFile(const WChar *file, Bool deleteRdonlyFile);
		static Bool RenameFile(UnsafeArray<const UTF8Char> srcFile, UnsafeArray<const UTF8Char> destFile);
//		static Bool RenameFile(const WChar *srcFile, const WChar *destFile);
		static UnsafeArrayOpt<UTF8Char> GetMountPoint(UnsafeArray<UTF8Char> buff, UnsafeArray<const UTF8Char> fileName);
//		static WChar *GetMountPoint(WChar *buff, const WChar *fileName);
		static Bool IsSamePartition(UnsafeArray<const UTF8Char> file1, UnsafeArray<const UTF8Char> file2);
//		static Bool IsSamePartition(const WChar *file1, const WChar *file2);
		static Bool CopyFile(Text::CStringNN file1, Text::CStringNN file2, FileExistAction fea, Optional<IO::ProgressHandler> progHdlr, OptOut<IO::ActiveStreamReader::BottleNeckType> bnt);
		static Bool CopyDir(Text::CStringNN srcDir, Text::CStringNN destDir, FileExistAction fea, Optional<IO::ProgressHandler> progHdlr, OptOut<IO::ActiveStreamReader::BottleNeckType> bnt);
		static Bool MoveFile(Text::CStringNN srcFile, Text::CStringNN destFile, FileExistAction fea, Optional<IO::ProgressHandler> progHdlr, OptOut<IO::ActiveStreamReader::BottleNeckType> bnt);
		static Bool MoveDir(Text::CStringNN srcDir, Text::CStringNN destDir, FileExistAction fea, Optional<IO::ProgressHandler> progHdlr, OptOut<IO::ActiveStreamReader::BottleNeckType> bnt);
	private:
		static void __stdcall CopyHdlr(const UInt8 *buff, UOSInt buffSize, AnyType userData);
		static Bool DeleteDir(UnsafeArray<UTF8Char> dir, UnsafeArray<UTF8Char> dirEnd, Bool deleteRdonlyFile);
	};
}
#endif
