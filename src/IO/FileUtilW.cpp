#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/DateTime.h"
#include "IO/ActiveStreamReader.h"
#include "IO/FileStream.h"
#include "IO/FileUtil.h"
#include "IO/Path.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include <windows.h>

#ifndef INVALID_FILE_ATTRIBUTES
#define INVALID_FILE_ATTRIBUTES 0xffffffff
#endif

#undef CreateDirectory
#undef DeleteFile
#undef FindNextFile
#undef MoveFile
#undef CopyFile
#undef RemoveDirectory

Bool IO::FileUtil::DeleteFile(const UTF8Char *file, Bool deleteRdonlyFile)
{
	UOSInt fileLen = Text::StrCharCnt(file);
	IO::Path::PathType pt = IO::Path::GetPathType(file, fileLen);
	if (pt == IO::Path::PathType::Directory)
	{
		UTF8Char sbuff[512];
		Text::StrConcatC(sbuff, file, fileLen);
		return DeleteDir(sbuff, deleteRdonlyFile);
	}
	const WChar *wptr = Text::StrToWCharNew(file);
	BOOL ret = ::DeleteFileW(wptr);
	if (ret == 0)
	{
		if (deleteRdonlyFile && GetLastError() == ERROR_ACCESS_DENIED)
		{
			::SetFileAttributesW(wptr, FILE_ATTRIBUTE_NORMAL);
			Bool succ = ::DeleteFileW(wptr) != 0;
			Text::StrDelNew(wptr);
			return succ;
		}
		else
		{
			Text::StrDelNew(wptr);
			return false;
		}
	}
	else
	{
		Text::StrDelNew(wptr);
		return true;
	}
}

/*Bool IO::FileUtil::DeleteFile(const WChar *file, Bool deleteRdonlyFile)
{
	IO::Path::PathType pt = IO::Path::GetPathType(file);
	if (pt == IO::Path::PathType::Directory)
	{
		UTF8Char sbuff[512];
		Text::StrWChar_UTF8(sbuff, file);
		return DeleteDir(sbuff, deleteRdonlyFile);
	}
	BOOL ret = ::DeleteFileW(file);
	if (ret == 0)
	{
		if (deleteRdonlyFile && GetLastError() == ERROR_ACCESS_DENIED)
		{
			::SetFileAttributesW(file, FILE_ATTRIBUTE_NORMAL);
			return ::DeleteFileW(file) != 0;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return true;
	}
}*/

Bool IO::FileUtil::RenameFile(const UTF8Char *srcFile, const UTF8Char *destFile)
{
	const WChar *srcFileW = Text::StrToWCharNew(srcFile);
	const WChar *destFileW = Text::StrToWCharNew(destFile);
	BOOL retV = MoveFileW(srcFileW, destFileW);
	Text::StrDelNew(srcFileW);
	Text::StrDelNew(destFileW);
	if (retV)
		return true;
	else
		return false;
}

#ifdef _WIN32_WCE
UTF8Char *IO::FileUtil::GetMountPoint(UTF8Char *buff, const UTF8Char *fileName)
{
	return 0;
}

Bool IO::FileUtil::IsSamePartition(const UTF8Char *file1, const UTF8Char *file2)
{
	ULARGE_INTEGER total1;
	ULARGE_INTEGER total2;
	ULARGE_INTEGER free1;
	ULARGE_INTEGER free2;
	ULARGE_INTEGER avail;

	const WChar *wptr = Text::StrToWCharNew(file1);
	if (GetDiskFreeSpaceEx(wptr, &avail, &total1, &free1) == 0)
	{
		Text::StrDelNew(wptr);
		return false;;
	}
	Text::StrDelNew(wptr);
	wptr = Text::StrToWCharNew(file2);
	if (GetDiskFreeSpaceEx(wptr, &avail, &total2, &free2) == 0)
	{
		Text::StrDelNew(wptr);
		return false;
	}
	Text::StrDelNew(wptr);
	return total1.QuadPart == total2.QuadPart && free1.QuadPart == free2.QuadPart;
}

#elif (_WIN32_WINNT >= 0x0500)
UTF8Char *IO::FileUtil::GetMountPoint(UTF8Char *buff, const UTF8Char *fileName)
{
	WChar sbuff[512];
	const WChar *wptr = Text::StrToWCharNew(fileName);
	if (GetVolumePathNameW(wptr, sbuff, 512))
	{
		Text::StrDelNew(wptr);
		return Text::StrWChar_UTF8(buff, sbuff);
	}
	else
	{
		Text::StrDelNew(wptr);
		return 0;
	}
}

Bool IO::FileUtil::IsSamePartition(const UTF8Char *file1, const UTF8Char *file2)
{
	WChar buff1[256];
	WChar buff2[256];
	const WChar *wptr = Text::StrToWCharNew(file1);
	if (GetVolumePathNameW(wptr, buff1, 256) == 0)
	{
		Text::StrDelNew(wptr);
		return false;
	}
	Text::StrDelNew(wptr);
	wptr = Text::StrToWCharNew(file2);
	if (GetVolumePathNameW(wptr, buff2, 256) == 0)
	{
		Text::StrDelNew(wptr);
		return false;
	}
	Text::StrDelNew(wptr);
	return Text::StrCompareICase(buff1, buff2) == 0;
}
#else
UTF8Char *IO::Path::GetMountPoint(UTF8Char *buff, UTF8Char *fileName)
{
	return 0;
}

Bool IO::FileUtil::IsSamePartition(const UTF8Char *file1, const UTF8Char *file2)
{
	return true;
}

#endif

typedef struct
{
	IO::FileStream *destStm;
	UInt64 writeSize;
	IO::IProgressHandler *progHdlr;
	UInt64 fileSize;
} CopySess;

/*Bool FileUtil_CopyFile(const WChar *file1, const WChar *file2, IO::FileUtil::FileExistAction fea, IO::IProgressHandler *progHdlr, IO::ActiveStreamReader::BottleNeckType *bnt)
{
	IO::FileStream *fs1;
	IO::FileStream *fs2;
	IO::ActiveStreamReader *asr;
	if (fea == IO::FileUtil::FEA_FAIL)
	{
		if (IO::Path::GetPathType(file2) != IO::Path::PathType::Unknown)
			return false;
	}
	NEW_CLASS(fs1, IO::FileStream(file1, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	if (fs1->IsError())
	{
		DEL_CLASS(fs1);
		return false;
	}
	if (fea == IO::FileUtil::FEA_CONTINUE)
	{
		NEW_CLASS(fs2, IO::FileStream(file2, IO::FileMode::Append, IO::FileShare::DenyNone, IO::FileStream::BufferType::NoWriteBuffer));
	}
	else
	{
		NEW_CLASS(fs2, IO::FileStream(file2, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::NoWriteBuffer));
	}
	if (fs2->IsError())
	{
		DEL_CLASS(fs2);
		DEL_CLASS(fs1);
		return false;
	}
	Int64 fileSize = fs1->GetLength();
	Int64 ramSize = 104857600;//MemGetRAMSize();
	Int64 writeSize = 0;
	Int64 writenSize;
	Bool samePart = FileUtil_IsSamePartition(file1, file2);
	UInt8 *buff;
	if (fea == IO::FileUtil::FEA_CONTINUE)
	{
		Int64 destPos = fs2->GetPosition();
		if (destPos > fileSize)
		{
			DEL_CLASS(fs2);
			DEL_CLASS(fs1);
			return false;
		}
		else if (destPos == fileSize)
		{
			DEL_CLASS(fs2);
			DEL_CLASS(fs1);
			return true;
		}
		else if (destPos > 0)
		{
			fileSize -= destPos;
			fs1->SeekFromBeginning(destPos);
		}
	}

	if (progHdlr)
	{
		const UTF8Char *u8ptr = Text::StrToUTF8New(file1);
		progHdlr->ProgressStart(u8ptr, fileSize);
		Text::StrDelNew(u8ptr);
	}
	if (fileSize < 1048576)
	{
		Data::DateTime dt1;
		Data::DateTime dt2;
		Data::DateTime dt3;
		buff = MemAlloc(UInt8, (OSInt)1048576);
		writeSize = fs1->Read(buff, (OSInt)1048576);
		writeSize = fs2->Write(buff, (OSInt)writeSize);
		MemFree(buff);
		fs1->GetFileTimes(&dt1, &dt2, &dt3);
		fs2->SetFileTimes(&dt1, &dt2, &dt3);
		if (progHdlr)
		{
			progHdlr->ProgressUpdate(writeSize, fileSize);
		}
	}
	else if (samePart)
	{
		Data::DateTime dt1;
		Data::DateTime dt2;
		Data::DateTime dt3;
		OSInt readSize;
		OSInt thisSize;
		if (fileSize < ramSize)
		{
			buff = MemAllocA(UInt8, readSize = (OSInt)fileSize);
		}
		else
		{
			buff = MemAllocA(UInt8, readSize = (OSInt)ramSize);
		}
		while (writeSize < fileSize)
		{
			if ((thisSize = fs1->Read(buff, readSize)) <= 0)
			{
				break;
			}
			writenSize = fs2->Write(buff, thisSize);
			writeSize += writenSize;
			if (progHdlr)
			{
				progHdlr->ProgressUpdate(writeSize, fileSize);
			}
			if (writenSize != thisSize)
			{
				break;
			}
		}
		MemFreeA(buff);
		fs1->GetFileTimes(&dt1, &dt2, &dt3);
		fs2->SetFileTimes(&dt1, &dt2, &dt3);
	}
	else
	{
		Data::DateTime dt1;
		Data::DateTime dt2;
		Data::DateTime dt3;
		CopySess csess;
		csess.destStm = fs2;
		csess.writeSize = 0;
		csess.progHdlr = progHdlr;
		csess.fileSize = fileSize;

		NEW_CLASS(asr, IO::ActiveStreamReader(IO::FileUtil::CopyHdlr, &csess, fs1, 1048576));
		asr->ReadStream(bnt);
		DEL_CLASS(asr);
		writeSize = csess.writeSize;
		fs1->GetFileTimes(&dt1, &dt2, &dt3);
		fs2->SetFileTimes(&dt1, &dt2, &dt3);
	}
	DEL_CLASS(fs2);
	DEL_CLASS(fs1);
	SetFileAttributesW(file2, GetFileAttributesW(file1));
	return writeSize == fileSize;
}*/

Bool IO::FileUtil::CopyFile(const UTF8Char *file1, const UTF8Char *file2, FileExistAction fea, IO::IProgressHandler *progHdlr, IO::ActiveStreamReader::BottleNeckType *bnt)
{
	IO::FileStream *fs1;
	IO::FileStream *fs2;
	IO::ActiveStreamReader *asr;
	if (fea == IO::FileUtil::FileExistAction::Fail)
	{
		if (IO::Path::GetPathType(file2) != IO::Path::PathType::Unknown)
			return false;
	}
	NEW_CLASS(fs1, IO::FileStream(file1, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	if (fs1->IsError())
	{
		DEL_CLASS(fs1);
		return false;
	}
	if (fea == IO::FileUtil::FileExistAction::Continue)
	{
		NEW_CLASS(fs2, IO::FileStream(file2, IO::FileMode::Append, IO::FileShare::DenyNone, IO::FileStream::BufferType::NoWriteBuffer));
	}
	else
	{
		NEW_CLASS(fs2, IO::FileStream(file2, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::NoWriteBuffer));
	}
	if (fs2->IsError())
	{
		DEL_CLASS(fs2);
		DEL_CLASS(fs1);
		return false;
	}
	UInt64 fileSize = fs1->GetLength();
	UInt64 ramSize = 104857600;//MemGetRAMSize();
	UInt64 writeSize = 0;
	UInt64 writenSize;
	Bool samePart = IO::FileUtil::IsSamePartition(file1, file2);
	UInt8 *buff;
	if (fea == IO::FileUtil::FileExistAction::Continue)
	{
		UInt64 destPos = fs2->GetPosition();
		if (destPos > fileSize)
		{
			DEL_CLASS(fs2);
			DEL_CLASS(fs1);
			return false;
		}
		else if (destPos == fileSize)
		{
			DEL_CLASS(fs2);
			DEL_CLASS(fs1);
			return true;
		}
		else if (destPos > 0)
		{
			fileSize -= destPos;
			fs1->SeekFromBeginning(destPos);
		}
	}

	if (progHdlr)
	{
		progHdlr->ProgressStart(file1, fileSize);
	}
	if (fileSize < 1048576)
	{
		Data::DateTime dt1;
		Data::DateTime dt2;
		Data::DateTime dt3;
		buff = MemAlloc(UInt8, (UOSInt)1048576);
		writeSize = fs1->Read(buff, (UOSInt)1048576);
		writeSize = fs2->Write(buff, (UOSInt)writeSize);
		MemFree(buff);
		fs1->GetFileTimes(&dt1, &dt2, &dt3);
		fs2->SetFileTimes(&dt1, &dt2, &dt3);
		if (progHdlr)
		{
			progHdlr->ProgressUpdate(writeSize, fileSize);
		}
	}
	else if (samePart)
	{
		Data::DateTime dt1;
		Data::DateTime dt2;
		Data::DateTime dt3;
		UOSInt readSize;
		UOSInt thisSize;
		if (fileSize < ramSize)
		{
			buff = MemAllocA(UInt8, readSize = (UOSInt)fileSize);
		}
		else
		{
			buff = MemAllocA(UInt8, readSize = (UOSInt)ramSize);
		}
		while (writeSize < fileSize)
		{
			if ((thisSize = fs1->Read(buff, readSize)) <= 0)
			{
				break;
			}
			writenSize = fs2->Write(buff, thisSize);
			writeSize += writenSize;
			if (progHdlr)
			{
				progHdlr->ProgressUpdate(writeSize, fileSize);
			}
			if (writenSize != thisSize)
			{
				break;
			}
		}
		MemFreeA(buff);
		fs1->GetFileTimes(&dt1, &dt2, &dt3);
		fs2->SetFileTimes(&dt1, &dt2, &dt3);
	}
	else
	{
		Data::DateTime dt1;
		Data::DateTime dt2;
		Data::DateTime dt3;
		CopySess csess;
		csess.destStm = fs2;
		csess.writeSize = 0;
		csess.progHdlr = progHdlr;
		csess.fileSize = fileSize;

		NEW_CLASS(asr, IO::ActiveStreamReader(IO::FileUtil::CopyHdlr, &csess, fs1, 1048576));
		asr->ReadStream(bnt);
		DEL_CLASS(asr);
		writeSize = csess.writeSize;
		fs1->GetFileTimes(&dt1, &dt2, &dt3);
		fs2->SetFileTimes(&dt1, &dt2, &dt3);
	}
	DEL_CLASS(fs2);
	DEL_CLASS(fs1);
	const WChar *wptr = Text::StrToWCharNew(file1);
	UInt32 attr = GetFileAttributesW(wptr);
	Text::StrDelNew(wptr);
	wptr = Text::StrToWCharNew(file2);
	SetFileAttributesW(wptr, attr);
	Text::StrDelNew(wptr);
	return writeSize == fileSize;
}

/*Bool FileUtil_CopyDir(const WChar *srcDir, const WChar *destDir, FileExistAction fea, IO::IProgressHandler *progHdlr, IO::ActiveStreamReader::BottleNeckType *bnt)
{
	WChar sbuff[512];
	WChar dbuff[512];
	WChar *sptr;
	WChar *dptr;
	IO::Path::FindFileSession *sess;
	UInt32 attr = GetFileAttributesW(srcDir);
	sptr = Text::StrConcat(sbuff, srcDir);
	dptr = Text::StrConcat(dbuff, destDir);
	IO::Path::CreateDirectory(destDir);
	if (attr != 0xffffffff)
		SetFileAttributesW(destDir, attr);
	if (sptr[-1] != '\\')
	{
		*sptr++ = '\\';
	}
	if (dptr[-1] != '\\')
	{
		*dptr++ = '\\';
	}
	Text::StrConcatC(sptr, IO::Path::ALL_FiLES, IO::Path::ALL_FILES_LEN);
	sess = IO::Path::FindFile(sbuff);
	if (sess)
	{
		IO::Path::PathType pt;
		Bool succ = true;
		while (IO::Path::FindNextFile(sptr, sess, 0, &pt))
		{
			if (pt == IO::Path::PathType::File)
			{
				Text::StrConcat(dptr, sptr);
				if (!CopyFile(sbuff, dbuff, fea, progHdlr, bnt))
				{
					succ = false;
					break;
				}
			}
			else if (pt == IO::Path::PathType::Directory)
			{
				if (sptr[0] == '.' && sptr[1] == 0)
				{
				}
				else if (sptr[0] == '.' && sptr[1] == '.' && sptr[2] == 0)
				{
				}
				else
				{
					Text::StrConcat(dptr, sptr);
					if (!CopyDir(sbuff, dbuff, fea, progHdlr, bnt))
					{
						succ = false;
						break;
					}
				}
			}
		}
		IO::Path::FindFileClose(sess);

		return succ;
	}
	else
	{
		return false;
	}
}*/

Bool IO::FileUtil::CopyDir(const UTF8Char *srcDir, const UTF8Char *destDir, FileExistAction fea, IO::IProgressHandler *progHdlr, IO::ActiveStreamReader::BottleNeckType *bnt)
{
	UTF8Char sbuff[512];
	UTF8Char dbuff[512];
	UTF8Char *sptr;
	UTF8Char *dptr;
	IO::Path::FindFileSession *sess;
	const WChar *wptr;
	wptr = Text::StrToWCharNew(srcDir);
	UInt32 attr = GetFileAttributesW(wptr);
	Text::StrDelNew(wptr);
	sptr = Text::StrConcat(sbuff, srcDir);
	dptr = Text::StrConcat(dbuff, destDir);
	IO::Path::CreateDirectory(destDir);
	if (attr != 0xffffffff)
	{
		wptr = Text::StrToWCharNew(destDir);
		SetFileAttributesW(wptr, attr);
		Text::StrDelNew(wptr);
	}
	if (sptr[-1] != IO::Path::PATH_SEPERATOR)
	{
		*sptr++ = IO::Path::PATH_SEPERATOR;
	}
	if (dptr[-1] != IO::Path::PATH_SEPERATOR)
	{
		*dptr++ = IO::Path::PATH_SEPERATOR;
	}
	Text::StrConcatC(sptr, IO::Path::ALL_FILES, IO::Path::ALL_FILES_LEN);
	sess = IO::Path::FindFile(sbuff);
	if (sess)
	{
		IO::Path::PathType pt;
		Bool succ = true;
		while (IO::Path::FindNextFile(sptr, sess, 0, &pt, 0))
		{
			if (pt == IO::Path::PathType::File)
			{
				Text::StrConcat(dptr, sptr);
				if (!CopyFile(sbuff, dbuff, fea, progHdlr, bnt))
				{
					succ = false;
					break;
				}
			}
			else if (pt == IO::Path::PathType::Directory)
			{
				if (sptr[0] == '.' && sptr[1] == 0)
				{
				}
				else if (sptr[0] == '.' && sptr[1] == '.' && sptr[2] == 0)
				{
				}
				else
				{
					Text::StrConcat(dptr, sptr);
					if (!CopyDir(sbuff, dbuff, fea, progHdlr, bnt))
					{
						succ = false;
						break;
					}
				}
			}
		}
		IO::Path::FindFileClose(sess);

		return succ;
	}
	else
	{
		return false;
	}
}

/*Bool FileUtil_MoveFile(const WChar *srcFile, const WChar *destFile, FileExistAction fea, IO::IProgressHandler *progHdlr, IO::ActiveStreamReader::BottleNeckType *bnt)
{
	Bool samePart = IsSamePartition(srcFile, destFile);
	if (samePart)
	{
		BOOL retV = MoveFileW(srcFile, destFile);
		if (retV)
			return true;
		else
		{
//			UInt32 err = GetLastError();
			return false;
		}
	}

	samePart = IO::FileUtil::CopyFile(srcFile, destFile, fea, progHdlr, bnt);
	if (samePart)
	{
		if (DeleteFile(srcFile, true))
			return true;
		else
		{
			DeleteFile(destFile, true);
			return false;
		}
	}
	else
	{
		return false;
	}
}*/

Bool IO::FileUtil::MoveFile(const UTF8Char *srcFile, const UTF8Char *destFile, FileExistAction fea, IO::IProgressHandler *progHdlr, IO::ActiveStreamReader::BottleNeckType *bnt)
{
	Bool samePart = IsSamePartition(srcFile, destFile);
	if (samePart)
	{
		const WChar *sFile = Text::StrToWCharNew(srcFile);
		const WChar *dFile = Text::StrToWCharNew(destFile);
		BOOL retV = MoveFileW(sFile, dFile);
		Text::StrDelNew(sFile);
		Text::StrDelNew(dFile);
		if (retV)
			return true;
		else
		{
//			UInt32 err = GetLastError();
			return false;
		}
	}

	samePart = IO::FileUtil::CopyFile(srcFile, destFile, fea, progHdlr, bnt);
	if (samePart)
	{
		if (DeleteFile(srcFile, true))
			return true;
		else
		{
			DeleteFile(destFile, true);
			return false;
		}
	}
	else
	{
		return false;
	}
}

/*Bool FileUtil_MoveDir(const WChar *srcDir, const WChar *destDir, FileExistAction fea, IO::IProgressHandler *progHdlr, IO::ActiveStreamReader::BottleNeckType *bnt)
{
	WChar sbuff[512];
	WChar dbuff[512];
	WChar *sptr;
	WChar *dptr;
	IO::Path::FindFileSession *sess;
	Bool succ;

	Bool samePart = IsSamePartition(srcDir, destDir);
	if (samePart)
	{
		BOOL retV = MoveFileW(srcDir, destDir);
		if (retV)
			return true;
		else
			return false;
	}
	IO::Path::CreateDirectory(destDir);
	sptr = Text::StrConcat(sbuff, srcDir);
	dptr = Text::StrConcat(dbuff, destDir);
	if (sptr != sbuff && sptr[-1] != '\\')
	{
		*sptr++ = '\\';
	}
	if (dptr != dbuff && dptr[-1] != '\\')
	{
		*dptr++ = '\\';
	}
	Text::StrConcat(sptr, L"*.*");
	sess = IO::Path::FindFile(sbuff);
	succ = true;
	if (sess)
	{
		IO::Path::PathType pt;
		while (IO::Path::FindNextFile(sptr, sess, 0, &pt))
		{
			if (sptr[0] == '.' && sptr[1] == 0)
			{
			}
			else if (sptr[0] == '.' && sptr[1] == '.' && sptr[2] == 0)
			{
			}
			else
			{
				if (pt == IO::Path::PathType::File)
				{
					Text::StrConcat(dptr, sptr);
					succ = IO::FileUtil::MoveFile(sbuff, dbuff, fea, progHdlr, bnt);
					if (!succ)
					{
						break;
					}
				}
				else if (pt == IO::Path::PathType::Directory)
				{
					Text::StrConcat(dptr, sptr);
					succ = IO::FileUtil::MoveDir(sbuff, dbuff, fea, progHdlr, bnt);
					if (!succ)
					{
						break;
					}
				}
			}
		}
		IO::Path::FindFileClose(sess);
		if (succ)
		{
			IO::Path::RemoveDirectory(srcDir);
		}
	}
	else
	{
		succ = false;
	}
	return succ;
}*/

Bool IO::FileUtil::MoveDir(const UTF8Char *srcDir, const UTF8Char *destDir, FileExistAction fea, IO::IProgressHandler *progHdlr, IO::ActiveStreamReader::BottleNeckType *bnt)
{
	UTF8Char sbuff[512];
	UTF8Char dbuff[512];
	UTF8Char *sptr;
	UTF8Char *dptr;
	IO::Path::FindFileSession *sess;
	Bool succ;

	Bool samePart = IsSamePartition(srcDir, destDir);
	if (samePart)
	{
		const WChar *sDir = Text::StrToWCharNew(srcDir);
		const WChar *dDir = Text::StrToWCharNew(destDir);
		BOOL retV = MoveFileW(sDir, dDir);
		Text::StrDelNew(sDir);
		Text::StrDelNew(dDir);
		if (retV)
			return true;
		else
			return false;
	}
	IO::Path::CreateDirectory(destDir);
	sptr = Text::StrConcat(sbuff, srcDir);
	dptr = Text::StrConcat(dbuff, destDir);
	if (sptr != sbuff && sptr[-1] != IO::Path::PATH_SEPERATOR)
	{
		*sptr++ = IO::Path::PATH_SEPERATOR;
	}
	if (dptr != dbuff && dptr[-1] != IO::Path::PATH_SEPERATOR)
	{
		*dptr++ = IO::Path::PATH_SEPERATOR;
	}
	Text::StrConcatC(sptr, IO::Path::ALL_FILES, IO::Path::ALL_FILES_LEN);
	sess = IO::Path::FindFile(sbuff);
	succ = true;
	if (sess)
	{
		IO::Path::PathType pt;
		while (IO::Path::FindNextFile(sptr, sess, 0, &pt, 0))
		{
			if (sptr[0] == '.' && sptr[1] == 0)
			{
			}
			else if (sptr[0] == '.' && sptr[1] == '.' && sptr[2] == 0)
			{
			}
			else
			{
				if (pt == IO::Path::PathType::File)
				{
					Text::StrConcat(dptr, sptr);
					succ = IO::FileUtil::MoveFile(sbuff, dbuff, fea, progHdlr, bnt);
					if (!succ)
					{
						break;
					}
				}
				else if (pt == IO::Path::PathType::Directory)
				{
					Text::StrConcat(dptr, sptr);
					succ = IO::FileUtil::MoveDir(sbuff, dbuff, fea, progHdlr, bnt);
					if (!succ)
					{
						break;
					}
				}
			}
		}
		IO::Path::FindFileClose(sess);
		if (succ)
		{
			IO::Path::RemoveDirectory(srcDir);
		}
	}
	else
	{
		succ = false;
	}
	return succ;
}

void __stdcall IO::FileUtil::CopyHdlr(const UInt8 *buff, UOSInt buffSize, void *userData)
{
	CopySess *csess = (CopySess*)userData;
	UOSInt writenSize;
	writenSize = csess->destStm->Write(buff, buffSize);
	csess->writeSize += writenSize;
	if (csess->progHdlr)
	{
		csess->progHdlr->ProgressUpdate(csess->writeSize, csess->fileSize);
	}
}

Bool IO::FileUtil::DeleteDir(UTF8Char *dir, Bool deleteRdonlyFile)
{
	UTF8Char *sptr = &dir[Text::StrCharCnt(dir)];
	if (sptr[-1] != IO::Path::PATH_SEPERATOR)
	{
		*sptr++ = IO::Path::PATH_SEPERATOR;
	}
	Text::StrConcatC(sptr, IO::Path::ALL_FILES, IO::Path::ALL_FILES_LEN);
	Bool succ = true;
	IO::Path::PathType pt;
	IO::Path::FindFileSession *sess = IO::Path::FindFile(dir);
	if (sess == 0)
		return false;
	while (succ && IO::Path::FindNextFile(sptr, sess, 0, &pt, 0))
	{
		if (pt == IO::Path::PathType::File)
		{
			const WChar *wdir = Text::StrToWCharNew(dir);
			BOOL ret = ::DeleteFileW(wdir);
			if (ret == 0)
			{
				if (deleteRdonlyFile && GetLastError() == ERROR_ACCESS_DENIED)
				{
					::SetFileAttributesW(wdir, FILE_ATTRIBUTE_NORMAL);
					if (::DeleteFileW(wdir) == 0)
						succ = false;
				}
				else
				{
					succ = false;
				}
			}
			Text::StrDelNew(wdir);
		}
		else if (pt == IO::Path::PathType::Directory)
		{
			if (sptr[0] == '.' && sptr[1] == 0)
			{
			}
			else if (sptr[0] == '.' && sptr[1] == '.' && sptr[2] == 0)
			{
			}
			else
			{
				DeleteDir(dir, deleteRdonlyFile);
			}
		}
		else
		{
			succ = false;
		}
	}
	if (succ)
	{
		sptr[-1] = 0;
		succ = succ && IO::Path::RemoveDirectory(dir);
	}
	IO::Path::FindFileClose(sess);
	return succ;
}

