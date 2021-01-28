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

Bool IO::FileUtil::DeleteFile(const UTF8Char *file, Bool deleteRdonlyFile)
{
	IO::Path::PathType pt = IO::Path::GetPathType(file);
	if (pt == IO::Path::PT_DIRECTORY)
	{
		UTF8Char sbuff[512];
		Text::StrConcat(sbuff, file);
		return DeleteDir(sbuff, deleteRdonlyFile);
	}

	Int32 ret = unlink((const Char*)file);
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
	if (pt == IO::Path::PT_DIRECTORY)
	{
		UTF8Char sbuff[512];
		Text::StrWChar_UTF8(sbuff, file, -1);
		return DeleteDir(sbuff, deleteRdonlyFile);
	}

	OSInt len = Text::StrWChar_UTF8Cnt(file, -1);
	Char *tmpBuff = MemAlloc(Char, len + 1);
	Text::StrWChar_UTF8((UTF8Char*)tmpBuff, file, -1);
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

Bool IO::FileUtil::RenameFile(const UTF8Char *srcFile, const UTF8Char *destFile)
{
	Int32 retV = rename((const Char*)srcFile, (const Char*)destFile);
	if (retV == 0)
		return true;
	else
		return false;
}

/*Bool IO::FileUtil::RenameFile(const WChar *srcFile, const WChar *destFile)
{
	OSInt len = Text::StrWChar_UTF8Cnt(srcFile, -1) + Text::StrWChar_UTF8Cnt(destFile, -1);
	Char *tmpBuff = MemAlloc(Char, len + 3);
	Char *tmpBuff2 = (Char*)Text::StrWChar_UTF8((UTF8Char*)tmpBuff, srcFile, -1) + 1;
	Text::StrWChar_UTF8((UTF8Char*)tmpBuff2, destFile, -1);
	Int32 retV = rename(tmpBuff, tmpBuff2);
	MemFree(tmpBuff);
	if (retV == 0)
		return true;
	else
		return false;
}*/

Bool FileUtil_Stat(const UTF8Char *fileName, struct stat *outStat)
{
	if (stat((const Char*)fileName, outStat) == 0)
	{
		return true;
	}
	Text::StringBuilderUTF8 sb;
	OSInt i;
	sb.Append(fileName);
	while (true)
	{
		i = sb.LastIndexOf('/');
		if (i <= 0)
		{
			break;
		}
		sb.RemoveChars(sb.GetLength() - i);
		if (stat((const Char*)sb.ToString(), outStat) == 0)
		{
			return true;
		}
	}
	return false;
}

Bool IO::FileUtil::IsSamePartition(const UTF8Char *file1, const UTF8Char *file2)
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
	OSInt len = Text::StrWChar_UTF8Cnt(file1, -1) + Text::StrWChar_UTF8Cnt(file2, -1);
	Char *tmpBuff = MemAlloc(Char, len + 3);
	Char *tmpBuff2 = (Char*)Text::StrWChar_UTF8((UTF8Char*)tmpBuff, file1, -1) + 1;
	Text::StrWChar_UTF8((UTF8Char*)tmpBuff2, file2, -1);

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
	Int64 writeSize;
	IO::IProgressHandler *progHdlr;
	Int64 fileSize;
} CopySess;

Bool IO::FileUtil::CopyFile(const UTF8Char *file1, const UTF8Char *file2, FileExistAction fea, IO::IProgressHandler *progHdlr, IO::ActiveStreamReader::BottleNeckType *bnt)
{
	IO::FileStream *fs1;
	IO::FileStream *fs2;
	IO::ActiveStreamReader *asr;
	if (fea == FEA_FAIL)
	{
		if (IO::Path::GetPathType(file2) != IO::Path::PT_UNKNOWN)
			return false;
	}
	NEW_CLASS(fs1, IO::FileStream(file1, IO::FileStream::FILE_MODE_READONLY, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));
	if (fs1->IsError())
	{
		DEL_CLASS(fs1);
		return false;
	}
	if (fea == FEA_CONTINUE)
	{
		NEW_CLASS(fs2, IO::FileStream(file2, IO::FileStream::FILE_MODE_APPEND, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NO_WRITE_BUFFER));
	}
	else
	{
		NEW_CLASS(fs2, IO::FileStream(file2, IO::FileStream::FILE_MODE_CREATE, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NO_WRITE_BUFFER));
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
	Bool samePart = IsSamePartition(file1, file2);
	UInt8 *buff;
	if (fea == FEA_CONTINUE)
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
			fs1->Seek(IO::SeekableStream::ST_BEGIN, destPos);
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

		NEW_CLASS(asr, IO::ActiveStreamReader(CopyHdlr, &csess, fs1, 1048576));
		asr->ReadStream(bnt);
		DEL_CLASS(asr);
		writeSize = csess.writeSize;
		fs1->GetFileTimes(&dt1, &dt2, &dt3);
		fs2->SetFileTimes(&dt1, &dt2, &dt3);
	}
	DEL_CLASS(fs2);
	DEL_CLASS(fs1);
//	SetFileAttributesW(file2, GetFileAttributesW(file1));
	return writeSize == fileSize;
}

/*Bool IO::FileUtil::CopyFile(const WChar *file1, const WChar *file2, FileExistAction fea, IO::IProgressHandler *progHdlr, IO::ActiveStreamReader::BottleNeckType *bnt)
{
	IO::FileStream *fs1;
	IO::FileStream *fs2;
	IO::ActiveStreamReader *asr;
	if (fea == FEA_FAIL)
	{
		if (IO::Path::GetPathType(file2) != IO::Path::PT_UNKNOWN)
			return false;
	}
	NEW_CLASS(fs1, IO::FileStream(file1, IO::FileStream::FILE_MODE_READONLY, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));
	if (fs1->IsError())
	{
		DEL_CLASS(fs1);
		return false;
	}
	if (fea == FEA_CONTINUE)
	{
		NEW_CLASS(fs2, IO::FileStream(file2, IO::FileStream::FILE_MODE_APPEND, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NO_WRITE_BUFFER));
	}
	else
	{
		NEW_CLASS(fs2, IO::FileStream(file2, IO::FileStream::FILE_MODE_CREATE, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NO_WRITE_BUFFER));
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
	Bool samePart = IsSamePartition(file1, file2);
	UInt8 *buff;
	if (fea == FEA_CONTINUE)
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
			fs1->Seek(IO::SeekableStream::ST_BEGIN, destPos);
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

		NEW_CLASS(asr, IO::ActiveStreamReader(CopyHdlr, &csess, fs1, 1048576));
		asr->ReadStream(bnt);
		DEL_CLASS(asr);
		writeSize = csess.writeSize;
		fs1->GetFileTimes(&dt1, &dt2, &dt3);
		fs2->SetFileTimes(&dt1, &dt2, &dt3);
	}
	DEL_CLASS(fs2);
	DEL_CLASS(fs1);
//	SetFileAttributesW(file2, GetFileAttributesW(file1));
	return writeSize == fileSize;
}*/

Bool IO::FileUtil::CopyDir(const UTF8Char *srcDir, const UTF8Char *destDir, FileExistAction fea, IO::IProgressHandler *progHdlr, IO::ActiveStreamReader::BottleNeckType *bnt)
{
	UTF8Char sbuff[512];
	UTF8Char dbuff[512];
	UTF8Char *sptr;
	UTF8Char *dptr;
	void *sess;
//	UInt32 attr = GetFileAttributesW(srcDir);
	sptr = Text::StrConcat(sbuff, srcDir);
	dptr = Text::StrConcat(dbuff, destDir);
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
	Text::StrConcat(sptr, IO::Path::ALL_FILES);
	sess = IO::Path::FindFile(sbuff);
	if (sess)
	{
		IO::Path::PathType pt;
		Bool succ = true;
		while (IO::Path::FindNextFile(sptr, sess, 0, &pt, 0))
		{
			if (pt == IO::Path::PT_FILE)
			{
				Text::StrConcat(dptr, sptr);
				if (!CopyFile(sbuff, dbuff, fea, progHdlr, bnt))
				{
					succ = false;
					break;
				}
			}
			else if (pt == IO::Path::PT_DIRECTORY)
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

/*Bool IO::FileUtil::CopyDir(const WChar *srcDir, const WChar *destDir, FileExistAction fea, IO::IProgressHandler *progHdlr, IO::ActiveStreamReader::BottleNeckType *bnt)
{
	UTF8Char sbuff[512];
	UTF8Char dbuff[512];
	UTF8Char *sptr;
	UTF8Char *dptr;
	void *sess;
//	UInt32 attr = GetFileAttributesW(srcDir);
	sptr = Text::StrWChar_UTF8(sbuff, srcDir, -1);
	dptr = Text::StrWChar_UTF8(dbuff, destDir, -1);
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
	Text::StrConcat(sptr, IO::Path::ALL_FILES);
	sess = IO::Path::FindFile(sbuff);
	if (sess)
	{
		IO::Path::PathType pt;
		Bool succ = true;
		while (IO::Path::FindNextFile(sptr, sess, 0, &pt, 0))
		{
			if (pt == IO::Path::PT_FILE)
			{
				Text::StrConcat(dptr, sptr);
				if (!CopyFile(sbuff, dbuff, fea, progHdlr, bnt))
				{
					succ = false;
					break;
				}
			}
			else if (pt == IO::Path::PT_DIRECTORY)
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

Bool IO::FileUtil::MoveFile(const UTF8Char *srcFile, const UTF8Char *destFile, FileExistAction fea, IO::IProgressHandler *progHdlr, IO::ActiveStreamReader::BottleNeckType *bnt)
{
	Bool samePart = IO::FileUtil::IsSamePartition(srcFile, destFile);
	if (samePart)
	{
		Bool retV = IO::FileUtil::RenameFile(srcFile, destFile);
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

/*Bool IO::FileUtil::MoveFile(const WChar *srcFile, const WChar *destFile, FileExistAction fea, IO::IProgressHandler *progHdlr, IO::ActiveStreamReader::BottleNeckType *bnt)
{
	Bool samePart = IO::FileUtil::IsSamePartition(srcFile, destFile);
	if (samePart)
	{
		Bool retV = IO::FileUtil::RenameFile(srcFile, destFile);
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
}*/

Bool IO::FileUtil::MoveDir(const UTF8Char *srcDir, const UTF8Char *destDir, FileExistAction fea, IO::IProgressHandler *progHdlr, IO::ActiveStreamReader::BottleNeckType *bnt)
{
	UTF8Char sbuff[512];
	UTF8Char dbuff[512];
	UTF8Char *sptr;
	UTF8Char *dptr;
	void *sess;
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
	Text::StrConcat(sptr, IO::Path::ALL_FILES);
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
				if (pt == IO::Path::PT_FILE)
				{
					Text::StrConcat(dptr, sptr);
					succ = IO::FileUtil::MoveFile(sbuff, dbuff, fea, progHdlr, bnt);
					if (!succ)
					{
						break;
					}
				}
				else if (pt == IO::Path::PT_DIRECTORY)
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

/*Bool IO::FileUtil::MoveDir(const WChar *srcDir, const WChar *destDir, FileExistAction fea, IO::IProgressHandler *progHdlr, IO::ActiveStreamReader::BottleNeckType *bnt)
{
	UTF8Char sbuff[512];
	UTF8Char dbuff[512];
	UTF8Char *sptr;
	UTF8Char *dptr;
	void *sess;
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
	sptr = Text::StrWChar_UTF8(sbuff, srcDir, -1);
	dptr = Text::StrWChar_UTF8(dbuff, destDir, -1);
	if (sptr != sbuff && sptr[-1] != IO::Path::PATH_SEPERATOR)
	{
		*sptr++ = IO::Path::PATH_SEPERATOR;
	}
	if (dptr != dbuff && dptr[-1] != IO::Path::PATH_SEPERATOR)
	{
		*dptr++ = IO::Path::PATH_SEPERATOR;
	}
	Text::StrConcat(sptr, IO::Path::ALL_FILES);
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
				if (pt == IO::Path::PT_FILE)
				{
					Text::StrConcat(dptr, sptr);
					succ = IO::FileUtil::MoveFile(sbuff, dbuff, fea, progHdlr, bnt);
					if (!succ)
					{
						break;
					}
				}
				else if (pt == IO::Path::PT_DIRECTORY)
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

void __stdcall IO::FileUtil::CopyHdlr(const UInt8 *buff, OSInt buffSize, void *userData)
{
	CopySess *csess = (CopySess*)userData;
	OSInt writenSize;
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
	Text::StrConcat(sptr, IO::Path::ALL_FILES);
	Bool succ = true;
	IO::Path::PathType pt;
	void *sess = IO::Path::FindFile(dir);
	if (sess == 0)
		return false;
	while (succ && IO::Path::FindNextFile(sptr, sess, 0, &pt, 0))
	{
		if (pt == IO::Path::PT_FILE)
		{
			if (!IO::FileUtil::DeleteFile(dir, deleteRdonlyFile))
			{
				succ = false;
			}
		}
		else if (pt == IO::Path::PT_DIRECTORY)
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

