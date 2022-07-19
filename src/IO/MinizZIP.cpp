#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/MinizZIP.h"
#include "IO/Path.h"
#include "Text/MyString.h"
//#define _fseeki64 fseek
//#define _ftelli64 ftell
#include "miniz.h"

Bool IO::MinizZIP::AddDir(UTF8Char *zipPath, UTF8Char *dirPath)
{
	mz_zip_archive *zip = (mz_zip_archive *)this->hand;
	UTF8Char *sptr;
	UTF8Char *sptr2;
	IO::Path::FindFileSession *sess;
	IO::Path::PathType pt;
	Bool succ;
	UInt64 fileSize;
	sptr = &zipPath[Text::StrCharCnt(zipPath)];
	if (sptr[-1] != IO::Path::PATH_SEPERATOR)
	{
		*sptr++ = IO::Path::PATH_SEPERATOR;
	}
	sptr2 = Text::StrConcatC(sptr, IO::Path::ALL_FILES, IO::Path::ALL_FILES_LEN);
	sess = IO::Path::FindFile(CSTRP(dirPath, sptr2));
	if (sess == 0)
		return false;
	succ = true;
	while (succ && IO::Path::FindNextFile(sptr, sess, 0, &pt, &fileSize))
	{
		if (pt == IO::Path::PathType::File)
		{
			Char sbuff2[512];
			Char sbuff[512];
			if (fileSize >= 0x80000000LL)
			{
				succ = false;
				break;
			}

			this->enc->UTF8ToBytes((UInt8*)sbuff, dirPath);
			this->enc->UTF8ToBytes((UInt8*)sbuff2, zipPath);
			Text::StrReplace(sbuff2, '\\', '/');
			if (mz_zip_writer_add_file(zip, sbuff2, sbuff, 0, 0, MZ_BEST_COMPRESSION) == MZ_FALSE)
			{
				succ = false;
			}
		}
		else if (pt == IO::Path::PathType::Directory)
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
	this->enc->UTF8ToBytes((UInt8*)sbuff, zipFile);
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

Bool IO::MinizZIP::AddFile(Text::CString sourceFile)
{
	mz_zip_archive *zip = (mz_zip_archive *)this->hand;
	IO::Path::PathType pt;
	if (zip == 0)
		return false;

	pt = IO::Path::GetPathType(sourceFile);
	if (pt == IO::Path::PathType::File)
	{
		UTF8Char sbuff[512];
		UOSInt len;
		UOSInt i;
		len = this->enc->UTF8ToBytes((UInt8*)sbuff, sourceFile.v);
		i = Text::StrLastIndexOfCharC(sbuff, len, IO::Path::PATH_SEPERATOR);
		return mz_zip_writer_add_file(zip, (const Char*)&sbuff[i + 1], (const Char*)sbuff, 0, 0, MZ_BEST_COMPRESSION) != MZ_FALSE;
	}
	else if (pt == IO::Path::PathType::Directory)
	{
		UTF8Char sbuff[512];
		UTF8Char *sptr = sourceFile.ConcatTo(sbuff);
		UOSInt i = Text::StrLastIndexOfCharC(sbuff, (UOSInt)(sptr - sbuff), IO::Path::PATH_SEPERATOR);
		return this->AddDir(&sbuff[i + 1], sbuff);
	}
	return false;
}

Bool IO::MinizZIP::AddFiles(Data::ArrayList<Text::String *> *files)
{
	UOSInt i;
	UOSInt j;
	Bool succ = true;
	i = 0;
	j = files->GetCount();
	while (succ && i < j)
	{
		Text::String *s = files->GetItem(i);
		succ = succ & AddFile(s->ToCString());
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

	this->enc->UTF8ToBytes((UInt8*)sbuff2, fileName);
	Text::StrReplace(sbuff2, '\\', '/');
	if (mz_zip_writer_add_mem(zip, sbuff2, content, contLeng, MZ_BEST_COMPRESSION) == MZ_FALSE)
	{
		return false;
	}
	return true;
}
