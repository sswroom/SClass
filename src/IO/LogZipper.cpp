#include "Stdafx.h"
#include "Data/DateTime.h"
#include "IO/FileStream.h"
#include "IO/FileUtil.h"
#include "IO/LogZipper.h"
#include "IO/Path.h"

Bool IO::LogZipper::ZipDir(NN<IO::ZIPMTBuilder> zip, UnsafeArray<UTF8Char> filePath, UnsafeArray<UTF8Char> filePathEnd, UnsafeArray<UTF8Char> refPathStart)
{
	UnsafeArray<UTF8Char> sptr;
	NN<IO::Path::FindFileSession> sess;
	IO::Path::PathType pt;
	Bool succ = true;
	sptr = Text::StrConcatC(filePathEnd, IO::Path::ALL_FILES, IO::Path::ALL_FILES_LEN);
	if (IO::Path::FindFile(CSTRP(filePath, sptr)).SetTo(sess))
	{
		while (IO::Path::FindNextFile(filePathEnd, sess, 0, pt, 0).SetTo(sptr))
		{
			if (pt == IO::Path::PathType::Directory && filePathEnd[0] != '.')
			{
				Data::Timestamp lastModTime = nullptr;
				Data::Timestamp createTime = nullptr;
				Data::Timestamp accessTime = nullptr;
				if (!IO::Path::GetFileTime(CSTRP(filePath, sptr), lastModTime, createTime, accessTime))
				{
					succ = false;
					break;
				}
				zip->AddDir(CSTRP(refPathStart, sptr), lastModTime, accessTime, createTime, 0);
				sptr[0] = IO::Path::PATH_SEPERATOR;
				if (!ZipDir(zip, filePath, sptr + 1, refPathStart))
				{
					succ = false;
					break;
				}
			}
			else if (pt == IO::Path::PathType::File)
			{
				NN<IO::FileStream> rfs;
				NEW_CLASSNN(rfs, IO::FileStream(CSTRP(filePath, sptr), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
				UInt32 unixAttr = IO::Path::GetFileUnixAttr(CSTRP(filePath, filePathEnd));
				Data::Timestamp lastModTime = nullptr;
				Data::Timestamp createTime = nullptr;
				Data::Timestamp accessTime = nullptr;
				rfs->GetFileTimes(createTime, accessTime, lastModTime);

				if (!zip->AddFileOTF(CSTRP(refPathStart, sptr), rfs, lastModTime, accessTime, createTime, Data::Compress::Deflater::CompLevel::BestCompression, unixAttr))
				{
					succ = false;
					break;
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

Bool IO::LogZipper::ZipLogs(Text::CStringNN logDir)
{
	UTF8Char sbuff[512];
	UTF8Char sbuff2[64];
	UIntOS nameSize;
	Int32 logTime;
	UnsafeArray<UTF8Char> filePath;
	UnsafeArray<UTF8Char> filePathEnd;
	UnsafeArray<UTF8Char> sptr;
	NN<IO::Path::FindFileSession> sess;
	Data::DateTime currTime;
	Bool succ;
	IO::Path::PathType pt;
	currTime.SetCurrTimeUTC();
	filePath = logDir.ConcatTo(sbuff);
	if (filePath == sbuff)
	{
		return false;
	}
	if (IO::Path::GetPathType(CSTRP(sbuff, filePath)) != IO::Path::PathType::Directory)
	{
		return false;
	}
	if (filePath[-1] != IO::Path::PATH_SEPERATOR)
	{
		*filePath++ = IO::Path::PATH_SEPERATOR;
	}
	filePathEnd = Text::StrConcatC(filePath, IO::Path::ALL_FILES, IO::Path::ALL_FILES_LEN);
	if (IO::Path::FindFile(CSTRP(sbuff, filePathEnd)).SetTo(sess))
	{
		while (IO::Path::FindNextFile(filePath, sess, 0, pt, 0).SetTo(filePathEnd))
		{
			nameSize = (UIntOS)(Text::StrConcatS(sbuff2, filePath, 63) - sbuff2);
			if (nameSize >= 6 && pt == IO::Path::PathType::Directory)
			{
				sptr = Text::StrConcatC(filePathEnd, UTF8STRC(".zip"));
				if (IO::Path::GetPathType(CSTRP(sbuff, sptr)) == IO::Path::PathType::Unknown)
				{
					logTime = Text::StrToInt32(&sbuff2[nameSize - 6]);
					if (logTime > 200000 && logTime < currTime.GetYear() * 100 + currTime.GetMonth())
					{
						succ = true;
						{
							IO::FileStream fs(CSTRP(sbuff, sptr), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
							IO::ZIPMTBuilder zip(fs, IO::ZIPOS::UNIX);
							filePathEnd[0] = IO::Path::PATH_SEPERATOR;
							if (!ZipDir(NN<IO::ZIPMTBuilder>(zip), sbuff, filePathEnd + 1, filePathEnd + 1))
							{
								succ = false;
							}
						}
						if (succ)
						{
							filePathEnd[0] = 0;
							IO::FileUtil::DeleteFile(CSTRP(sbuff, filePathEnd), true);
						}
					}
				}
			}
		}
		IO::Path::FindFileClose(sess);
		return true;
	}
	else
	{
		return false;
	}
}
