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
		static Bool DeleteFile(Text::CString file, Bool deleteRdonlyFile);
//		static Bool DeleteFile(const WChar *file, Bool deleteRdonlyFile);
		static Bool RenameFile(const UTF8Char *srcFile, const UTF8Char *destFile);
//		static Bool RenameFile(const WChar *srcFile, const WChar *destFile);
		static UTF8Char *GetMountPoint(UTF8Char *buff, const UTF8Char *fileName);
//		static WChar *GetMountPoint(WChar *buff, const WChar *fileName);
		static Bool IsSamePartition(const UTF8Char *file1, const UTF8Char *file2);
//		static Bool IsSamePartition(const WChar *file1, const WChar *file2);
		static Bool CopyFile(Text::CStringNN file1, Text::CStringNN file2, FileExistAction fea, IO::ProgressHandler *progHdlr, IO::ActiveStreamReader::BottleNeckType *bnt);
//		static Bool CopyFile(const WChar *file1, const WChar *file2, FileExistAction fea, IO::ProgressHandler *progHdlr, IO::ActiveStreamReader::BottleNeckType *bnt);
		static Bool CopyDir(Text::CString srcDir, Text::CString destDir, FileExistAction fea, IO::ProgressHandler *progHdlr, IO::ActiveStreamReader::BottleNeckType *bnt);
//		static Bool CopyDir(const WChar *srcDir, const WChar *destDir, FileExistAction fea, IO::ProgressHandler *progHdlr, IO::ActiveStreamReader::BottleNeckType *bnt);
		static Bool MoveFile(Text::CStringNN srcFile, Text::CStringNN destFile, FileExistAction fea, IO::ProgressHandler *progHdlr, IO::ActiveStreamReader::BottleNeckType *bnt);
//		static Bool MoveFile(const WChar *srcFile, const WChar *destFile, FileExistAction fea, IO::ProgressHandler *progHdlr, IO::ActiveStreamReader::BottleNeckType *bnt);
		static Bool MoveDir(Text::CString srcDir, Text::CString destDir, FileExistAction fea, IO::ProgressHandler *progHdlr, IO::ActiveStreamReader::BottleNeckType *bnt);
//		static Bool MoveDir(const WChar *srcDir, const WChar *destDir, FileExistAction fea, IO::ProgressHandler *progHdlr, IO::ActiveStreamReader::BottleNeckType *bnt);
	private:
		static void __stdcall CopyHdlr(const UInt8 *buff, UOSInt buffSize, void *userData);
		static Bool DeleteDir(UTF8Char *dir, UTF8Char *dirEnd, Bool deleteRdonlyFile);
	};
}
#endif
