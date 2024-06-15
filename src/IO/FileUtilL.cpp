#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/DateTime.h"
#include "IO/ActiveStreamReader.h"
#include "IO/FileStream.h"
#include "IO/FileUtil.h"
#include "IO/Path.h"
#include "Text/MyString.h"

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

Bool IO::FileUtil::DeleteFile(Text::CStringNN file, Bool deleteRdonlyFile)
{
	IO::Path::PathType pt = IO::Path::GetPathType(file);
	if (pt == IO::Path::PathType::Directory)
	{
		UTF8Char sbuff[512];
		UnsafeArray<UTF8Char> sptr;
		sptr = file.ConcatTo(sbuff);
		return DeleteDir(sbuff, sptr, deleteRdonlyFile);
	}

	Int32 ret = unlink((const Char*)file.v.Ptr());
	if (ret == 0)
	{
		return true;
	}
	else
	{
		return false;
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

	UOSInt len = Text::StrWChar_UTF8Cnt(file);
	Char *tmpBuff = MemAlloc(Char, len + 1);
	Text::StrWChar_UTF8((UTF8Char*)tmpBuff, file);
	Int32 ret = unlink(tmpBuff);
	MemFree(tmpBuff);
	if (ret == 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}*/

Bool IO::FileUtil::RenameFile(UnsafeArray<const UTF8Char> srcFile, UnsafeArray<const UTF8Char> destFile)
{
	struct stat s;
	int status = lstat((const Char*)destFile.Ptr(), &s);
	if (status == 0)
		return false;
	Int32 retV = rename((const Char*)srcFile.Ptr(), (const Char*)destFile.Ptr());
	if (retV == 0)
		return true;
	else
		return false;
}

/*Bool IO::FileUtil::RenameFile(const WChar *srcFile, const WChar *destFile)
{
	UOSInt len = Text::StrWChar_UTF8Cnt(srcFile) + Text::StrWChar_UTF8Cnt(destFile);
	Char *tmpBuff = MemAlloc(Char, len + 3);
	Char *tmpBuff2 = (Char*)Text::StrWChar_UTF8((UTF8Char*)tmpBuff, srcFile) + 1;
	Text::StrWChar_UTF8((UTF8Char*)tmpBuff2, destFile);
	Int32 retV = rename(tmpBuff, tmpBuff2);
	MemFree(tmpBuff);
	if (retV == 0)
		return true;
	else
		return false;
}*/

Bool FileUtil_Stat(UnsafeArray<const UTF8Char> fileName, struct stat *outStat)
{
	if (stat((const Char*)fileName.Ptr(), outStat) == 0)
	{
		return true;
	}
	Text::StringBuilderUTF8 sb;
	UOSInt i;
	sb.AppendSlow(fileName);
	while (true)
	{
		i = sb.LastIndexOf('/');
		if (i == INVALID_INDEX || i == 0)
		{
			break;
		}
		sb.RemoveChars(sb.GetLength() - (UOSInt)i);
		if (stat((const Char*)sb.ToPtr(), outStat) == 0)
		{
			return true;
		}
	}
	return false;
}

Bool IO::FileUtil::IsSamePartition(UnsafeArray<const UTF8Char> file1, UnsafeArray<const UTF8Char> file2)
{
	Bool ret;
	struct stat s1;
	struct stat s2;

	if (!FileUtil_Stat(file1, &s1))
	{
		ret = false;
	}
	else if (!FileUtil_Stat(file2, &s2))
	{
		ret = false;
	}
	else
	{
		ret = (s1.st_dev == s2.st_dev);
	}
	return ret;
}

/*Bool IO::FileUtil::IsSamePartition(const WChar *file1, const WChar *file2)
{
	Bool ret;
	UOSInt len = Text::StrWChar_UTF8Cnt(file1) + Text::StrWChar_UTF8Cnt(file2);
	Char *tmpBuff = MemAlloc(Char, len + 3);
	Char *tmpBuff2 = (Char*)Text::StrWChar_UTF8((UTF8Char*)tmpBuff, file1) + 1;
	Text::StrWChar_UTF8((UTF8Char*)tmpBuff2, file2);

	struct stat s1;
	struct stat s2;

	if (stat(tmpBuff, &s1) != 0)
	{
		ret = false;
	}
	else if (stat(tmpBuff2, &s2) != 0)
	{
		ret = false;
	}
	else
	{
		ret = (s1.st_dev == s2.st_dev);
	}
	MemFree(tmpBuff);
	return ret;
}*/

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
	if (fea == FileExistAction::Fail)
	{
		if (IO::Path::GetPathType(file2) != IO::Path::PathType::Unknown)
			return false;
	}
	IO::FileStream fs1(file1, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
	if (fs1.IsError())
	{
		return false;
	}
	if (fea == FileExistAction::Continue)
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
	UInt64 fileSize = fs1.GetLength();
	UInt64 ramSize = 104857600;//MemGetRAMSize();
	UInt64 writeSize = 0;
	UInt64 writenSize;
	Bool samePart = IsSamePartition(file1.v, file2.v);
	UInt8 *buff;
	if (fea == FileExistAction::Continue)
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

	NN<IO::ProgressHandler> nnprogHdlr;
	if (progHdlr.SetTo(nnprogHdlr))
	{
		nnprogHdlr->ProgressStart(file1, fileSize);
	}
	if (fileSize < 1048576)
	{
		Data::Timestamp ts1;
		Data::Timestamp ts2;
		Data::Timestamp ts3;
		buff = MemAlloc(UInt8, 1048576);
		writeSize = fs1.Read(Data::ByteArray(buff, (UOSInt)1048576));
		writeSize = fs2->Write(Data::ByteArrayR(buff, (UOSInt)writeSize));
		MemFree(buff);
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
			writenSize = fs2->Write(Data::ByteArrayR(buff, thisSize));
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

		NEW_CLASS(asr, IO::ActiveStreamReader(CopyHdlr, &csess, 1048576));
		asr->ReadStream(fs1, bnt);
		DEL_CLASS(asr);
		writeSize = csess.writeSize;
		fs1.GetFileTimes(ts1, ts2, ts3);
		fs2->SetFileTimes(ts1, ts2, ts3);
	}
	DEL_CLASS(fs2);
//	SetFileAttributesW(file2, GetFileAttributesW(file1));
	return writeSize == fileSize;
}

Bool IO::FileUtil::CopyDir(Text::CStringNN srcDir, Text::CStringNN destDir, FileExistAction fea, Optional<IO::ProgressHandler> progHdlr, OptOut<IO::ActiveStreamReader::BottleNeckType> bnt)
{
	UTF8Char sbuff[512];
	UTF8Char dbuff[512];
	UnsafeArray<UTF8Char> sptr;
	UnsafeArray<UTF8Char> sptr2;
	UnsafeArray<UTF8Char> dptr;
	UnsafeArray<UTF8Char> dptr2;
	IO::Path::FindFileSession *sess;
//	UInt32 attr = GetFileAttributesW(srcDir);
	sptr = srcDir.ConcatTo(sbuff);
	dptr = destDir.ConcatTo(dbuff);
	IO::Path::CreateDirectory(destDir);
//	if (attr != 0xffffffff)
//		SetFileAttributesW(destDir, attr);
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
		while (IO::Path::FindNextFile(sptr, sess, 0, &pt, 0).SetTo(sptr2))
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
	Bool samePart = IO::FileUtil::IsSamePartition(srcFile.v, destFile.v);
	if (samePart)
	{
		Bool retV = IO::FileUtil::RenameFile(srcFile.v, destFile.v);
		if (retV)
			return true;
		else
		{
			return false;
		}
	}

	samePart = IO::FileUtil::CopyFile(srcFile, destFile, fea, progHdlr, bnt);
	if (samePart)
	{
		if (IO::FileUtil::DeleteFile(srcFile, true))
			return true;
		else
		{
			IO::FileUtil::DeleteFile(destFile, true);
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
	UnsafeArray<UTF8Char> sptr;
	UnsafeArray<UTF8Char> sptr2;
	UnsafeArray<UTF8Char> dptr;
	UnsafeArray<UTF8Char> dptr2;
	IO::Path::FindFileSession *sess;
	Bool succ;

	Bool samePart = IsSamePartition(srcDir.v, destDir.v);
	if (samePart)
	{
		Bool retV = IO::FileUtil::RenameFile(srcDir.v, destDir.v);
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
		while (IO::Path::FindNextFile(sptr, sess, 0, &pt, 0).SetTo(sptr2))
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

/*Bool IO::FileUtil::MoveDir(const WChar *srcDir, const WChar *destDir, FileExistAction fea, IO::ProgressHandler *progHdlr, IO::ActiveStreamReader::BottleNeckType *bnt)
{
	UTF8Char sbuff[512];
	UTF8Char dbuff[512];
	UnsafeArray<UTF8Char> sptr;
	UnsafeArray<UTF8Char> dptr;
	IO::Path::FindFileSession *sess;
	Bool succ;

	Bool samePart = IsSamePartition(srcDir, destDir);
	if (samePart)
	{
		Bool retV = IO::FileUtil::RenameFile(srcDir, destDir);
		if (retV)
			return true;
		else
			return false;
	}
	IO::Path::CreateDirectory(destDir);
	sptr = Text::StrWChar_UTF8(sbuff, srcDir);
	dptr = Text::StrWChar_UTF8(dbuff, destDir);
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
}*/

void __stdcall IO::FileUtil::CopyHdlr(Data::ByteArrayR buff, AnyType userData)
{
	NN<CopySess> csess = userData.GetNN<CopySess>();
	UOSInt writenSize;
	writenSize = csess->destStm->Write(buff);
	csess->writeSize += writenSize;
	NN<IO::ProgressHandler> progHdlr;
	if (csess->progHdlr.SetTo(progHdlr))
	{
		progHdlr->ProgressUpdate(csess->writeSize, csess->fileSize);
	}
}

Bool IO::FileUtil::DeleteDir(UnsafeArray<UTF8Char> dir, UnsafeArray<UTF8Char> dirEnd, Bool deleteRdonlyFile)
{
	UnsafeArray<UTF8Char> sptr2;
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
	while (succ && IO::Path::FindNextFile(dirEnd, sess, 0, &pt, 0).SetTo(sptr2))
	{
		if (pt == IO::Path::PathType::File)
		{
			if (!IO::FileUtil::DeleteFile(CSTRP(dir, sptr2), deleteRdonlyFile))
			{
				succ = false;
			}
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

