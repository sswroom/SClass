#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/Sort/ArtificialQuickSort.h"
#include "IO/DirectoryPackage.h"
#include "IO/FileUtil.h"
#include "IO/Path.h"
#include "IO/StmData/FileData.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"

void IO::DirectoryPackage::AddFile(Text::CString fileName)
{
	UTF8Char sbuff[512];
	IO::Path::FindFileSession *sess = IO::Path::FindFile(fileName);
	if (sess)
	{
		Data::Timestamp ts;
		UInt64 fileSize;
		IO::Path::PathType pt;
		if (IO::Path::FindNextFile(sbuff, sess, &ts, &pt, &fileSize))
		{
			this->files.Add(Text::String::New(fileName.v, fileName.leng));
			this->fileSizes.Add(fileSize);
			this->fileTimes.Add(ts);
		}
		IO::Path::FindFileClose(sess);
	}
}

void IO::DirectoryPackage::Init()
{
	Data::Timestamp ts;
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	UTF8Char *sptr2;
	IO::Path::PathType pt;
	UInt64 fileSize;
	IO::Path::FindFileSession *sess;
	if (this->dirName->leng > 0)
	{
		sptr = this->dirName->ConcatTo(sbuff);
		if (sptr[-1] != IO::Path::PATH_SEPERATOR)
		{
			*sptr++ = IO::Path::PATH_SEPERATOR;
		}
		sptr2 = Text::StrConcatC(sptr, IO::Path::ALL_FILES, IO::Path::ALL_FILES_LEN);
		sess = IO::Path::FindFile(CSTRP(sbuff, sptr2));
		if (sess)
		{
			while ((sptr2 = IO::Path::FindNextFile(sptr, sess, &ts, &pt, &fileSize)) != 0)
			{
				if (sptr[0] == '.' && sptr[1] == 0)
				{
				}
				else if (sptr[0] == '.' && sptr[1] == '.' && sptr[2] == 0)
				{
				}
				else
				{
					this->files.Add(Text::String::New(sbuff, (UOSInt)(sptr2 - sbuff)));
					this->fileSizes.Add(fileSize);
					this->fileTimes.Add(ts);
				}
			}
			IO::Path::FindFileClose(sess);
		}
	}
}

IO::DirectoryPackage::DirectoryPackage(Text::String *dirName) : IO::PackageFile(dirName)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	if (dirName->StartsWith(UTF8STRC("~/")))
	{
		sptr = Text::StrConcatC(IO::Path::GetUserHome(sbuff), dirName->v + 1, dirName->leng - 1);
		this->dirName = Text::String::New(sbuff, (UOSInt)(sptr - sbuff));
	}
	else
	{
		this->dirName = dirName->Clone();
	}
	this->Init();
}

IO::DirectoryPackage::DirectoryPackage(Text::CString dirName) : IO::PackageFile(dirName)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	if (dirName.StartsWith(UTF8STRC("~/")))
	{
		sptr = Text::StrConcatC(IO::Path::GetUserHome(sbuff), dirName.v + 1, dirName.leng - 1);
		this->dirName = Text::String::New(sbuff, (UOSInt)(sptr - sbuff));
	}
	else
	{
		this->dirName = Text::String::New(dirName.v, dirName.leng);
	}
	this->Init();
}

IO::DirectoryPackage::~DirectoryPackage()
{
	UOSInt i;
	i = this->files.GetCount();
	while (i-- > 0)
	{
		this->files.GetItem(i)->Release();
	}
	this->dirName->Release();
}

UOSInt IO::DirectoryPackage::GetCount() const
{
	return this->files.GetCount();
}

IO::PackageFile::PackObjectType IO::DirectoryPackage::GetItemType(UOSInt index) const
{
	Text::String *fileName = this->files.GetItem(index);
	if (fileName == 0)
		return IO::PackageFile::PackObjectType::Unknown;
	IO::Path::PathType pt = IO::Path::GetPathType(fileName->ToCString());
	if (pt == IO::Path::PathType::File)
	{
		return IO::PackageFile::PackObjectType::StreamData;
	}
	else if (pt == IO::Path::PathType::Directory)
	{
		return IO::PackageFile::PackObjectType::PackageFileType;
	}
	else
	{
		return IO::PackageFile::PackObjectType::Unknown;
	}
}

UTF8Char *IO::DirectoryPackage::GetItemName(UTF8Char *sbuff, UOSInt index) const
{
	Text::String *fileName = this->files.GetItem(index);
	UOSInt i;
	if (fileName == 0)
		return 0;
	i = fileName->LastIndexOf(IO::Path::PATH_SEPERATOR);
	return Text::StrConcat(sbuff, &fileName->v[i + 1]);
}

IO::StreamData *IO::DirectoryPackage::GetItemStmDataNew(UOSInt index) const
{
	Text::String *fileName = this->files.GetItem(index);
	if (fileName == 0)
		return 0;
	IO::Path::PathType pt = IO::Path::GetPathType(fileName->ToCString());
	if (pt == IO::Path::PathType::File)
	{
		IO::StmData::FileData *fd;
		NEW_CLASS(fd, IO::StmData::FileData(fileName, false));
		return fd;
	}
	else
	{
		return 0;
	}
}

IO::PackageFile *IO::DirectoryPackage::GetItemPackNew(UOSInt index) const
{
	Text::String *fileName = this->files.GetItem(index);
	if (fileName == 0)
		return 0;
	IO::Path::PathType pt = IO::Path::GetPathType(fileName->ToCString());
	if (pt == IO::Path::PathType::Directory)
	{
		IO::DirectoryPackage *pkg;
		NEW_CLASS(pkg, IO::DirectoryPackage(fileName));
		return pkg;
	}
	else
	{
		return 0;
	}
}

IO::ParsedObject *IO::DirectoryPackage::GetItemPObj(UOSInt index, Bool *needRelease) const
{
	*needRelease = true;
	return GetItemPackNew(index);
}

Data::Timestamp IO::DirectoryPackage::GetItemModTime(UOSInt index) const
{
	return this->fileTimes.GetItem(index);
}

UInt64 IO::DirectoryPackage::GetItemStoreSize(UOSInt index) const
{
	return this->fileSizes.GetItem(index);
}

UInt64 IO::DirectoryPackage::GetItemSize(UOSInt index) const
{
	return this->fileSizes.GetItem(index);
}

UOSInt IO::DirectoryPackage::GetItemIndex(Text::CString name) const
{
	UOSInt j = this->files.GetCount();
	UOSInt i;
	while (j-- > 0)
	{
		Text::String *fileName = this->files.GetItem(j);
		if (fileName)
		{
			i = fileName->LastIndexOf(IO::Path::PATH_SEPERATOR);
			if (name.Equals(&fileName->v[i + 1], fileName->leng - i - 1))
			{
				return j;
			}
		}
	}
	return INVALID_INDEX;
}

Bool IO::DirectoryPackage::IsCompressed(UOSInt index) const
{
	return false;
}

Data::Compress::Decompressor::CompressMethod IO::DirectoryPackage::GetItemComp(UOSInt index) const
{
	return Data::Compress::Decompressor::CM_UNCOMPRESSED;
}

IO::PackageFile *IO::DirectoryPackage::Clone() const
{
	IO::DirectoryPackage *dpkg;
	NEW_CLASS(dpkg, IO::DirectoryPackage(this->dirName));
	return dpkg;
}

Bool IO::DirectoryPackage::AllowWrite() const
{
	return true;
}

Bool IO::DirectoryPackage::CopyFrom(Text::CString fileName, IO::ProgressHandler *progHdlr, IO::ActiveStreamReader::BottleNeckType *bnt)
{
	IO::Path::PathType pt;
	Bool ret;
	pt = IO::Path::GetPathType(fileName);
	if (pt == IO::Path::PathType::File)
	{
		UTF8Char sbuff[512];
		UOSInt i;
		UTF8Char *sptr = this->dirName->ConcatTo(sbuff);
		if (sptr[-1] != IO::Path::PATH_SEPERATOR)
		{
			*sptr++ = IO::Path::PATH_SEPERATOR;
		}
		i = fileName.LastIndexOf(IO::Path::PATH_SEPERATOR);
		sptr = Text::StrConcatC(sptr, &fileName.v[i + 1], fileName.leng - i - 1);
		ret = IO::FileUtil::CopyFile(fileName, CSTRP(sbuff, sptr), IO::FileUtil::FileExistAction::Fail, progHdlr, bnt);
		if (ret)
		{
			this->AddFile(CSTRP(sbuff, sptr));
		}
		return ret;
	}
	else if (pt == IO::Path::PathType::Directory)
	{
		UTF8Char sbuff[512];
		UOSInt i;
		UTF8Char *sptr = this->dirName->ConcatTo(sbuff);
		if (sptr[-1] != IO::Path::PATH_SEPERATOR)
		{
			*sptr++ = IO::Path::PATH_SEPERATOR;
		}
		i = fileName.LastIndexOf(IO::Path::PATH_SEPERATOR);
		sptr = Text::StrConcatC(sptr, &fileName.v[i + 1], fileName.leng - i - 1);
		ret = IO::FileUtil::CopyDir(fileName, CSTRP(sbuff, sptr), IO::FileUtil::FileExistAction::Fail, progHdlr, bnt);
		if (ret)
		{
			this->AddFile(CSTRP(sbuff, sptr));
		}
		return ret;
	}
	return false;
}

Bool IO::DirectoryPackage::MoveFrom(Text::CString fileName, IO::ProgressHandler *progHdlr, IO::ActiveStreamReader::BottleNeckType *bnt)
{
	IO::Path::PathType pt;
	Bool ret;
	pt = IO::Path::GetPathType(fileName);
	if (pt == IO::Path::PathType::File)
	{
		UTF8Char sbuff[512];
		UOSInt i;
		UTF8Char *sptr = this->dirName->ConcatTo(sbuff);
		if (sptr[-1] != IO::Path::PATH_SEPERATOR)
		{
			*sptr++ = IO::Path::PATH_SEPERATOR;
		}
		i = fileName.LastIndexOf(IO::Path::PATH_SEPERATOR);
		sptr = Text::StrConcatC(sptr, &fileName.v[i + 1], fileName.leng - i - 1);
		ret = IO::FileUtil::MoveFile(fileName, CSTRP(sbuff, sptr), IO::FileUtil::FileExistAction::Fail, progHdlr, bnt);
		if (ret)
		{
			this->AddFile(CSTRP(sbuff, sptr));
		}
		return ret;
	}
	else if (pt == IO::Path::PathType::Directory)
	{
		UTF8Char sbuff[512];
		UOSInt i;
		UTF8Char *sptr = this->dirName->ConcatTo(sbuff);
		if (sptr[-1] != IO::Path::PATH_SEPERATOR)
		{
			*sptr++ = IO::Path::PATH_SEPERATOR;
		}
		i = fileName.LastIndexOf(IO::Path::PATH_SEPERATOR);
		sptr = Text::StrConcatC(sptr, &fileName.v[i + 1], fileName.leng - i - 1);
		ret = IO::FileUtil::MoveDir(fileName, CSTRP(sbuff, sptr), IO::FileUtil::FileExistAction::Fail, progHdlr, bnt);
		if (ret)
		{
			this->AddFile(CSTRP(sbuff, sptr));
		}
		return ret;
	}
	return false;
}

Bool IO::DirectoryPackage::RetryCopyFrom(Text::CString fileName, IO::ProgressHandler *progHdlr, IO::ActiveStreamReader::BottleNeckType *bnt)
{
	IO::Path::PathType pt;
	Bool ret;
	pt = IO::Path::GetPathType(fileName);
	if (pt == IO::Path::PathType::File)
	{
		UTF8Char sbuff[512];
		UOSInt i;
		UTF8Char *sptr = this->dirName->ConcatTo(sbuff);
		if (sptr[-1] != IO::Path::PATH_SEPERATOR)
		{
			*sptr++ = IO::Path::PATH_SEPERATOR;
		}
		i = fileName.LastIndexOf(IO::Path::PATH_SEPERATOR);
		sptr = Text::StrConcatC(sptr, &fileName.v[i + 1], fileName.leng - i - 1);
		ret = IO::FileUtil::CopyFile(fileName, CSTRP(sbuff, sptr), IO::FileUtil::FileExistAction::Continue, progHdlr, bnt);
		if (ret)
		{
			this->AddFile(CSTRP(sbuff, sptr));
		}
		return ret;
	}
	else if (pt == IO::Path::PathType::Directory)
	{
		UTF8Char sbuff[512];
		UOSInt i;
		UTF8Char *sptr = this->dirName->ConcatTo(sbuff);
		if (sptr[-1] != IO::Path::PATH_SEPERATOR)
		{
			*sptr++ = IO::Path::PATH_SEPERATOR;
		}
		i = fileName.LastIndexOf(IO::Path::PATH_SEPERATOR);
		sptr = Text::StrConcatC(sptr, &fileName.v[i + 1], fileName.leng - i - 1);
		ret = IO::FileUtil::CopyDir(fileName, CSTRP(sbuff, sptr), IO::FileUtil::FileExistAction::Continue, progHdlr, bnt);
		if (ret)
		{
			this->AddFile(CSTRP(sbuff, sptr));
		}
		return ret;
	}
	return false;
}

Bool IO::DirectoryPackage::RetryMoveFrom(Text::CString fileName, IO::ProgressHandler *progHdlr, IO::ActiveStreamReader::BottleNeckType *bnt)
{
	IO::Path::PathType pt;
	Bool ret;
	pt = IO::Path::GetPathType(fileName);
	if (pt == IO::Path::PathType::File)
	{
		UTF8Char sbuff[512];
		UOSInt i;
		UTF8Char *sptr = this->dirName->ConcatTo(sbuff);
		if (sptr[-1] != IO::Path::PATH_SEPERATOR)
		{
			*sptr++ = IO::Path::PATH_SEPERATOR;
		}
		i = fileName.LastIndexOf(IO::Path::PATH_SEPERATOR);
		sptr = Text::StrConcatC(sptr, &fileName.v[i + 1], fileName.leng - i - 1);
		ret = IO::FileUtil::MoveFile(fileName, CSTRP(sbuff, sptr), IO::FileUtil::FileExistAction::Continue, progHdlr, bnt);
		if (ret)
		{
			this->AddFile(CSTRP(sbuff, sptr));
		}
		return ret;
	}
	else if (pt == IO::Path::PathType::Directory)
	{
		UTF8Char sbuff[512];
		UOSInt i;
		UTF8Char *sptr = this->dirName->ConcatTo(sbuff);
		if (sptr[-1] != IO::Path::PATH_SEPERATOR)
		{
			*sptr++ = IO::Path::PATH_SEPERATOR;
		}
		i = fileName.LastIndexOf(IO::Path::PATH_SEPERATOR);
		sptr = Text::StrConcatC(sptr, &fileName.v[i + 1], fileName.leng - i - 1);
		ret = IO::FileUtil::MoveDir(fileName, CSTRP(sbuff, sptr), IO::FileUtil::FileExistAction::Continue, progHdlr, bnt);
		if (ret)
		{
			this->AddFile(CSTRP(sbuff, sptr));
		}
		return ret;
	}
	return false;
}

typedef struct
{
	Text::String *name;
	UInt64 fileSize;
	Data::Timestamp fileTime;
} DirFile;

OSInt __stdcall DirectoryPackage_Compare(void *obj1, void *obj2)
{
	DirFile *df1 = (DirFile*)obj1;
	DirFile *df2 = (DirFile*)obj2;
	return df1->name->CompareTo(df2->name);
}

Bool IO::DirectoryPackage::Sort()
{
	UOSInt i;
	UOSInt j;
	DirFile *df;
	void **arr;
	i = 0;
	j = this->files.GetCount();
	if (j <= 0)
		return true;
	arr = MemAlloc(void*, j);
	while (i < j)
	{
		df = MemAlloc(DirFile, 1);
		df->name = this->files.GetItem(i);
		df->fileSize = this->fileSizes.GetItem(i);
		df->fileTime = this->fileTimes.GetItem(i);
		arr[i] = df;
		i++;
	}
	ArtificialQuickSort_SortCmp(arr, DirectoryPackage_Compare, 0, (OSInt)j - 1);
	i = 0;
	while (i < j)
	{
		df = (DirFile*)arr[i];
		this->files.SetItem(i, df->name);
		this->fileSizes.SetItem(i, df->fileSize);
		this->fileTimes.SetItem(i, df->fileTime);
		MemFree(df);
		i++;
	}
	MemFree(arr);
	return true;
}
