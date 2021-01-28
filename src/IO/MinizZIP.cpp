#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/MinizZIP.h"
#include "IO/Path.h"
#include "Text/MyString.h"
#define _fseeki64 fseek
#define _ftelli64 ftell
#include "miniz.h"

Bool IO::MinizZIP::AddDir(UTF8Char *zipPath, UTF8Char *dirPath)
{
	mz_zip_archive *zip = (mz_zip_archive *)this->hand;
	UTF8Char *sptr;
	void *sess;
	IO::Path::PathType pt;
	Bool succ;
	Int64 fileSize;
	sptr = &zipPath[Text::StrCharCnt(zipPath)];
	if (sptr[-1] != IO::Path::PATH_SEPERATOR)
	{
		*sptr++ = IO::Path::PATH_SEPERATOR;
	}
	Text::StrConcat(sptr, IO::Path::ALL_FILES);
	sess = IO::Path::FindFile(dirPath);
	if (sess == 0)
		return false;
	succ = true;
	while (succ && IO::Path::FindNextFile(sptr, sess, 0, &pt, &fileSize))
	{
		if (pt == IO::Path::PT_FILE)
		{
			Char sbuff2[512];
			Char sbuff[512];
			if (fileSize >= 0x80000000LL)
			{
				succ = false;
				break;
			}

			this->enc->UTF8ToBytes((UInt8*)sbuff, dirPath, -1);
			this->enc->UTF8ToBytes((UInt8*)sbuff2, zipPath, -1);
			Text::StrReplace(sbuff2, '\\', '/');
			if (mz_zip_writer_add_file(zip, sbuff2, sbuff, 0, 0, MZ_BEST_COMPRESSION) == MZ_FALSE)
			{
				succ = false;
			}
		}
		else if (pt == IO::Path::PT_DIRECTORY)
		{
			if (sptr[0] == '.')
			{
			}
			else
			{
				succ = succ & AddDir(zipPath, dirPath);
			}
		}
	}
	IO::Path::FindFileClose(sess);
	return succ;
}


IO::MinizZIP::MinizZIP(const UTF8Char *zipFile)
{
	Char sbuff[512];
	mz_zip_archive *zip;
	NEW_CLASS(this->enc, Text::Encoding());
	zip = MemAlloc(mz_zip_archive, 1);
	MemClear(zip, sizeof(mz_zip_archive));
	this->enc->UTF8ToBytes((UInt8*)sbuff, zipFile, -1);
	if (mz_zip_writer_init_file(zip, sbuff, 0))
	{
		this->hand = zip;
	}
	else
	{
		MemFree(zip);
	}
}

IO::MinizZIP::~MinizZIP()
{
	if (this->hand)
	{
		mz_zip_writer_finalize_archive((mz_zip_archive *)this->hand);
		mz_zip_writer_end((mz_zip_archive *)this->hand);
		MemFree(this->hand);
		this->hand = 0;
	}
}

Bool IO::MinizZIP::AddFile(const UTF8Char *sourceFile)
{
	mz_zip_archive *zip = (mz_zip_archive *)this->hand;
	IO::Path::PathType pt;
	if (zip == 0)
		return false;

	pt = IO::Path::GetPathType(sourceFile);
	if (pt == IO::Path::PT_FILE)
	{
		Char sbuff[512];
		OSInt i;
		this->enc->UTF8ToBytes((UInt8*)sbuff, sourceFile, -1);
		i = Text::StrLastIndexOf(sbuff, IO::Path::PATH_SEPERATOR);
		return mz_zip_writer_add_file(zip, &sbuff[i + 1], sbuff, 0, 0, MZ_BEST_COMPRESSION) != MZ_FALSE;
	}
	else if (pt == IO::Path::PT_DIRECTORY)
	{
		UTF8Char sbuff[512];
		Text::StrConcat(sbuff, sourceFile);
		OSInt i = Text::StrLastIndexOf(sbuff, IO::Path::PATH_SEPERATOR);
		return this->AddDir(&sbuff[i + 1], sbuff);
	}
	return false;
}

Bool IO::MinizZIP::AddFiles(Data::ArrayList<const UTF8Char *> *files)
{
	OSInt i;
	OSInt j;
	Bool succ = true;
	i = 0;
	j = files->GetCount();
	while (succ && i < j)
	{
		succ = succ & AddFile(files->GetItem(i));
		i++;
	}
	return succ;
}

Bool IO::MinizZIP::AddContent(const UInt8 *content, UOSInt contLeng, const UTF8Char *fileName)
{
	Char sbuff2[512];
	mz_zip_archive *zip = (mz_zip_archive *)this->hand;
	if (contLeng >= 0x80000000LL)
	{
		return false;
	}

	this->enc->UTF8ToBytes((UInt8*)sbuff2, fileName, -1);
	Text::StrReplace(sbuff2, '\\', '/');
	if (mz_zip_writer_add_mem(zip, sbuff2, content, contLeng, MZ_BEST_COMPRESSION) == MZ_FALSE)
	{
		return false;
	}
	return true;
}
