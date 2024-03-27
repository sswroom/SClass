#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteBuffer.h"
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

Bool IO::FileUtil::DeleteFile(Text::CStringNN file, Bool deleteRdonlyFile)
{
	IO::Path::PathType pt = IO::Path::GetPathType(file);
	if (pt == IO::Path::PathType::Directory)
	{
		UTF8Char sbuff[512];
		UTF8Char *sptr;
		sptr = file.ConcatTo(sbuff);
		return DeleteDir(sbuff, sptr, deleteRdonlyFile);
	}
	const WChar *wptr = Text::StrToWCharNew(file.v);
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
	WChar wbuff[512];
	const WChar *wptr = Text::StrToWCharNew(fileName);
	if (GetVolumePathNameW(wptr, wbuff, 512))
	{
		Text::StrDelNew(wptr);
		return Text::StrWChar_UTF8(buff, wbuff);
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
	Optional<IO::ProgressHandler> progHdlr;
	UInt64 fileSize;
} CopySess;

Bool IO::FileUtil::CopyFile(Text::CStringNN file1, Text::CStringNN file2, FileExistAction fea, Optional<IO::ProgressHandler> progHdlr, OptOut<IO::ActiveStreamReader::BottleNeckType> bnt)
{
	IO::FileStream *fs2;
	IO::ActiveStreamReader *asr;
	WChar wfile2[MAX_PATH];
	Text::StrUTF8_WChar(wfile2, file2.v, 0);
	if (fea == IO::FileUtil::FileExistAction::Fail)
	{
		if (IO::Path::GetPathTypeW(wfile2) != IO::Path::PathType::Unknown)
			return false;
	}
	UInt64 writeSize = 0;
	UInt64 fileSize;
	{
		IO::FileStream fs1(file1, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		if (fs1.IsError())
		{
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
			return false;
		}
		fileSize = fs1.GetLength();
		UInt64 ramSize = 104857600;//MemGetRAMSize();
		UInt64 writenSize;
		Bool samePart = IO::FileUtil::IsSamePartition(file1.v, file2.v);
		if (fea == IO::FileUtil::FileExistAction::Continue)
		{
			UInt64 destPos = fs2->GetPosition();
			if (destPos > fileSize)
			{
				DEL_CLASS(fs2);
				return false;
			}
			else if (destPos == fileSize)
			{
				DEL_CLASS(fs2);
				return true;
			}
			else if (destPos > 0)
			{
				fileSize -= destPos;
				fs1.SeekFromBeginning(destPos);
			}
		}

		NotNullPtr<IO::ProgressHandler> nnprogHdlr;
		if (progHdlr.SetTo(nnprogHdlr))
		{
			nnprogHdlr->ProgressStart(file1, fileSize);
		}
		if (fileSize < 1048576)
		{
			Data::Timestamp ts1;
			Data::Timestamp ts2;
			Data::Timestamp ts3;
			{
				Data::ByteBuffer buff((UOSInt)1048576);
				writeSize = fs1.Read(buff);
				writeSize = fs2->Write(buff.Ptr(), (UOSInt)writeSize);
			}
			fs1.GetFileTimes(ts1, ts2, ts3);
			fs2->SetFileTimes(ts1, ts2, ts3);
			if (progHdlr.SetTo(nnprogHdlr))
			{
				nnprogHdlr->ProgressUpdate(writeSize, fileSize);
			}
		}
		else if (samePart)
		{
			Data::Timestamp ts1;
			Data::Timestamp ts2;
			Data::Timestamp ts3;
			UOSInt readSize;
			UOSInt thisSize;
			UInt8 *buff;
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
				if ((thisSize = fs1.Read(Data::ByteArray(buff, readSize))) <= 0)
				{
					break;
				}
				writenSize = fs2->Write(buff, thisSize);
				writeSize += writenSize;
				if (progHdlr.SetTo(nnprogHdlr))
				{
					nnprogHdlr->ProgressUpdate(writeSize, fileSize);
				}
				if (writenSize != thisSize)
				{
					break;
				}
			}
			MemFreeA(buff);
			fs1.GetFileTimes(ts1, ts2, ts3);
			fs2->SetFileTimes(ts1, ts2, ts3);
		}
		else
		{
			Data::Timestamp ts1;
			Data::Timestamp ts2;
			Data::Timestamp ts3;
			CopySess csess;
			csess.destStm = fs2;
			csess.writeSize = 0;
			csess.progHdlr = progHdlr;
			csess.fileSize = fileSize;

			NEW_CLASS(asr, IO::ActiveStreamReader(IO::FileUtil::CopyHdlr, &csess, 1048576));
			asr->ReadStream(fs1, bnt);
			DEL_CLASS(asr);
			writeSize = csess.writeSize;
			fs1.GetFileTimes(ts1, ts2, ts3);
			fs2->SetFileTimes(ts1, ts2, ts3);
		}
		DEL_CLASS(fs2);
	}
	const WChar *wptr = Text::StrToWCharNew(file1.v);
	UInt32 attr = GetFileAttributesW(wptr);
	Text::StrDelNew(wptr);
	SetFileAttributesW(wfile2, attr);
	return writeSize == fileSize;
}

Bool IO::FileUtil::CopyDir(Text::CStringNN srcDir, Text::CStringNN destDir, FileExistAction fea, Optional<IO::ProgressHandler> progHdlr, OptOut<IO::ActiveStreamReader::BottleNeckType> bnt)
{
	UTF8Char sbuff[512];
	UTF8Char dbuff[512];
	UTF8Char *sptr;
	UTF8Char *sptr2;
	UTF8Char *dptr;
	UTF8Char *dptr2;
	IO::Path::FindFileSession *sess;
	const WChar *wptr;
	wptr = Text::StrToWCharNew(srcDir.v);
	UInt32 attr = GetFileAttributesW(wptr);
	Text::StrDelNew(wptr);
	sptr = srcDir.ConcatTo(sbuff);
	dptr = destDir.ConcatTo(dbuff);
	IO::Path::CreateDirectory(destDir);
	if (attr != 0xffffffff)
	{
		wptr = Text::StrToWCharNew(destDir.v);
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
	sptr2 = Text::StrConcatC(sptr, IO::Path::ALL_FILES, IO::Path::ALL_FILES_LEN);
	sess = IO::Path::FindFile(CSTRP(sbuff, sptr2));
	if (sess)
	{
		IO::Path::PathType pt;
		Bool succ = true;
		while ((sptr2 = IO::Path::FindNextFile(sptr, sess, 0, &pt, 0)) != 0)
		{
			if (pt == IO::Path::PathType::File)
			{
				dptr2 = Text::StrConcatC(dptr, sptr, (UOSInt)(sptr2 - sptr));
				if (!CopyFile(CSTRP(sbuff, sptr2), CSTRP(dbuff, dptr2), fea, progHdlr, bnt))
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
					dptr2 = Text::StrConcatC(dptr, sptr, (UOSInt)(sptr2 - sptr));
					if (!CopyDir(CSTRP(sbuff, sptr2), CSTRP(dbuff, dptr2), fea, progHdlr, bnt))
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

Bool IO::FileUtil::MoveFile(Text::CStringNN srcFile, Text::CStringNN destFile, FileExistAction fea, Optional<IO::ProgressHandler> progHdlr, OptOut<IO::ActiveStreamReader::BottleNeckType> bnt)
{
	Bool samePart = IsSamePartition(srcFile.v, destFile.v);
	if (samePart)
	{
		const WChar *sFile = Text::StrToWCharNew(srcFile.v);
		const WChar *dFile = Text::StrToWCharNew(destFile.v);
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

Bool IO::FileUtil::MoveDir(Text::CStringNN srcDir, Text::CStringNN destDir, FileExistAction fea, Optional<IO::ProgressHandler> progHdlr, OptOut<IO::ActiveStreamReader::BottleNeckType> bnt)
{
	UTF8Char sbuff[512];
	UTF8Char dbuff[512];
	UTF8Char *sptr;
	UTF8Char *sptr2;
	UTF8Char *dptr;
	UTF8Char *dptr2;
	IO::Path::FindFileSession *sess;
	Bool succ;

	Bool samePart = IsSamePartition(srcDir.v, destDir.v);
	if (samePart)
	{
		const WChar *sDir = Text::StrToWCharNew(srcDir.v);
		const WChar *dDir = Text::StrToWCharNew(destDir.v);
		BOOL retV = MoveFileW(sDir, dDir);
		Text::StrDelNew(sDir);
		Text::StrDelNew(dDir);
		if (retV)
			return true;
		else
			return false;
	}
	IO::Path::CreateDirectory(destDir);
	sptr = srcDir.ConcatTo(sbuff);
	dptr = destDir.ConcatTo(dbuff);
	if (sptr != sbuff && sptr[-1] != IO::Path::PATH_SEPERATOR)
	{
		*sptr++ = IO::Path::PATH_SEPERATOR;
	}
	if (dptr != dbuff && dptr[-1] != IO::Path::PATH_SEPERATOR)
	{
		*dptr++ = IO::Path::PATH_SEPERATOR;
	}
	sptr2 = Text::StrConcatC(sptr, IO::Path::ALL_FILES, IO::Path::ALL_FILES_LEN);
	sess = IO::Path::FindFile(CSTRP(sbuff, sptr2));
	succ = true;
	if (sess)
	{
		IO::Path::PathType pt;
		while ((sptr2 = IO::Path::FindNextFile(sptr, sess, 0, &pt, 0)) != 0)
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
					dptr2 = Text::StrConcatC(dptr, sptr, (UOSInt)(sptr2 - sptr));
					succ = IO::FileUtil::MoveFile(CSTRP(sbuff, sptr2), CSTRP(dbuff, dptr2), fea, progHdlr, bnt);
					if (!succ)
					{
						break;
					}
				}
				else if (pt == IO::Path::PathType::Directory)
				{
					dptr2 = Text::StrConcatC(dptr, sptr, (UOSInt)(sptr2 - sptr));
					succ = IO::FileUtil::MoveDir(CSTRP(sbuff, sptr2), CSTRP(dbuff, dptr2), fea, progHdlr, bnt);
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
			IO::Path::RemoveDirectory(srcDir.v);
		}
	}
	else
	{
		succ = false;
	}
	return succ;
}

void __stdcall IO::FileUtil::CopyHdlr(const UInt8 *buff, UOSInt buffSize, AnyType userData)
{
	NotNullPtr<CopySess> csess = userData.GetNN<CopySess>();
	UOSInt writenSize;
	writenSize = csess->destStm->Write(buff, buffSize);
	csess->writeSize += writenSize;
	NotNullPtr<IO::ProgressHandler> nnprogHdlr;
	if (csess->progHdlr.SetTo(nnprogHdlr))
	{
		nnprogHdlr->ProgressUpdate(csess->writeSize, csess->fileSize);
	}
}

Bool IO::FileUtil::DeleteDir(UTF8Char *dir, UTF8Char *dirEnd, Bool deleteRdonlyFile)
{
	UTF8Char *sptr2;
	if (dirEnd[-1] != IO::Path::PATH_SEPERATOR)
	{
		*dirEnd++ = IO::Path::PATH_SEPERATOR;
	}
	sptr2 = Text::StrConcatC(dirEnd, IO::Path::ALL_FILES, IO::Path::ALL_FILES_LEN);
	Bool succ = true;
	IO::Path::PathType pt;
	IO::Path::FindFileSession *sess = IO::Path::FindFile(CSTRP(dir, sptr2));
	if (sess == 0)
		return false;
	while (succ && (sptr2 = IO::Path::FindNextFile(dirEnd, sess, 0, &pt, 0)) != 0)
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
			if (dirEnd[0] == '.' && dirEnd[1] == 0)
			{
			}
			else if (dirEnd[0] == '.' && dirEnd[1] == '.' && dirEnd[2] == 0)
			{
			}
			else
			{
				DeleteDir(dir, sptr2, deleteRdonlyFile);
			}
		}
		else
		{
			succ = false;
		}
	}
	if (succ)
	{
		dirEnd[-1] = 0;
		succ = succ && IO::Path::RemoveDirectory(dir);
	}
	IO::Path::FindFileClose(sess);
	return succ;
}

