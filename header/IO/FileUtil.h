#ifndef _SM_IO_FILEUTIL
#define _SM_IO_FILEUTIL
#include "IO/ActiveStreamReader.h"
#include "IO/IProgressHandler.h"
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
		static Bool DeleteFile(const UTF8Char *file, Bool deleteRdonlyFile);
//		static Bool DeleteFile(const WChar *file, Bool deleteRdonlyFile);
		static Bool RenameFile(const UTF8Char *srcFile, const UTF8Char *destFile);
//		static Bool RenameFile(const WChar *srcFile, const WChar *destFile);
		static UTF8Char *GetMountPoint(UTF8Char *buff, const UTF8Char *fileName);
//		static WChar *GetMountPoint(WChar *buff, const WChar *fileName);
		static Bool IsSamePartition(const UTF8Char *file1, const UTF8Char *file2);
//		static Bool IsSamePartition(const WChar *file1, const WChar *file2);
		static Bool CopyFile(Text::CString file1, Text::CString file2, FileExistAction fea, IO::IProgressHandler *progHdlr, IO::ActiveStreamReader::BottleNeckType *bnt);
//		static Bool CopyFile(const WChar *file1, const WChar *file2, FileExistAction fea, IO::IProgressHandler *progHdlr, IO::ActiveStreamReader::BottleNeckType *bnt);
		static Bool CopyDir(const UTF8Char *srcDir, const UTF8Char *destDir, FileExistAction fea, IO::IProgressHandler *progHdlr, IO::ActiveStreamReader::BottleNeckType *bnt);
//		static Bool CopyDir(const WChar *srcDir, const WChar *destDir, FileExistAction fea, IO::IProgressHandler *progHdlr, IO::ActiveStreamReader::BottleNeckType *bnt);
		static Bool MoveFile(Text::CString srcFile, Text::CString destFile, FileExistAction fea, IO::IProgressHandler *progHdlr, IO::ActiveStreamReader::BottleNeckType *bnt);
//		static Bool MoveFile(const WChar *srcFile, const WChar *destFile, FileExistAction fea, IO::IProgressHandler *progHdlr, IO::ActiveStreamReader::BottleNeckType *bnt);
		static Bool MoveDir(const UTF8Char *srcDir, const UTF8Char *destDir, FileExistAction fea, IO::IProgressHandler *progHdlr, IO::ActiveStreamReader::BottleNeckType *bnt);
//		static Bool MoveDir(const WChar *srcDir, const WChar *destDir, FileExistAction fea, IO::IProgressHandler *progHdlr, IO::ActiveStreamReader::BottleNeckType *bnt);
	private:
		static void __stdcall CopyHdlr(const UInt8 *buff, UOSInt buffSize, void *userData);
		static Bool DeleteDir(UTF8Char *dir, Bool deleteRdonlyFile);
	};
}
#endif
