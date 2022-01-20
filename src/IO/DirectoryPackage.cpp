#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/Sort/ArtificialQuickSort.h"
#include "IO/DirectoryPackage.h"
#include "IO/FileUtil.h"
#include "IO/Path.h"
#include "IO/StmData/FileData.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"

void IO::DirectoryPackage::AddFile(const UTF8Char *fileName)
{
	UTF8Char sbuff[512];
	IO::Path::FindFileSession *sess = IO::Path::FindFile(fileName);
	if (sess)
	{
		Data::DateTime dt;
		UInt64 fileSize;
		IO::Path::PathType pt;
		if (IO::Path::FindNextFile(sbuff, sess, &dt, &pt, &fileSize))
		{
			this->files->Add(Text::String::NewNotNull(fileName));
			this->fileSizes->Add(fileSize);
			this->fileTimes->Add(dt.ToTicks());
		}
		IO::Path::FindFileClose(sess);
	}
}

void IO::DirectoryPackage::Init()
{
	Data::DateTime dt;
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	UTF8Char *sptr2;
	IO::Path::PathType pt;
	UInt64 fileSize;
	IO::Path::FindFileSession *sess;
	NEW_CLASS(this->files, Data::ArrayList<Text::String*>());
	NEW_CLASS(this->fileSizes, Data::ArrayListUInt64());
	NEW_CLASS(this->fileTimes, Data::ArrayListInt64());
	if (this->dirName->leng > 0)
	{
		sptr = this->dirName->ConcatTo(sbuff);
		if (sptr[-1] != IO::Path::PATH_SEPERATOR)
		{
			*sptr++ = IO::Path::PATH_SEPERATOR;
		}
		Text::StrConcatC(sptr, IO::Path::ALL_FILES, IO::Path::ALL_FILES_LEN);
		sess = IO::Path::FindFile(sbuff);
		if (sess)
		{
			while ((sptr2 = IO::Path::FindNextFile(sptr, sess, &dt, &pt, &fileSize)) != 0)
			{
				if (sptr[0] == '.' && sptr[1] == 0)
				{
				}
				else if (sptr[0] == '.' && sptr[1] == '.' && sptr[2] == 0)
				{
				}
				else
				{
					this->files->Add(Text::String::New(sbuff, (UOSInt)(sptr2 - sbuff)));
					this->fileSizes->Add(fileSize);
					this->fileTimes->Add(dt.ToTicks());
				}
			}
			IO::Path::FindFileClose(sess);
		}
	}
}

IO::DirectoryPackage::DirectoryPackage(Text::String *dirName) : IO::PackageFile(dirName->v)
{
	UTF8Char sbuff[512];
	if (dirName->StartsWith(UTF8STRC("~/")))
	{
		Text::StrConcatC(IO::Path::GetUserHome(sbuff), dirName->v + 1, dirName->leng - 1);
		this->dirName = Text::String::NewNotNull(sbuff);
	}
	else
	{
		this->dirName = dirName->Clone();
	}
	this->Init();
}

IO::DirectoryPackage::DirectoryPackage(const UTF8Char *dirName) : IO::PackageFile(dirName)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	UOSInt nameLen = Text::StrCharCnt(dirName);
	if (Text::StrStartsWithC(dirName, nameLen, UTF8STRC("~/")))
	{
		sptr = Text::StrConcatC(IO::Path::GetUserHome(sbuff), dirName + 1, nameLen - 1);
		this->dirName = Text::String::New(sbuff, (UOSInt)(sptr - sbuff));
	}
	else
	{
		this->dirName = Text::String::New(dirName, nameLen);
	}
	this->Init();
}

IO::DirectoryPackage::~DirectoryPackage()
{
	UOSInt i;
	i = this->files->GetCount();
	while (i-- > 0)
	{
		this->files->GetItem(i)->Release();
	}
	DEL_CLASS(this->files);
	DEL_CLASS(this->fileSizes);
	DEL_CLASS(this->fileTimes);
	this->dirName->Release();
}

UOSInt IO::DirectoryPackage::GetCount()
{
	return this->files->GetCount();
}

IO::PackageFile::PackObjectType IO::DirectoryPackage::GetItemType(UOSInt index)
{
	Text::String *fileName = this->files->GetItem(index);
	if (fileName == 0)
		return IO::PackageFile::POT_UNKNOWN;
	IO::Path::PathType pt = IO::Path::GetPathType(fileName->v);
	if (pt == IO::Path::PathType::File)
	{
		return IO::PackageFile::POT_STREAMDATA;
	}
	else if (pt == IO::Path::PathType::Directory)
	{
		return IO::PackageFile::POT_PACKAGEFILE;
	}
	else
	{
		return IO::PackageFile::POT_UNKNOWN;
	}
}

UTF8Char *IO::DirectoryPackage::GetItemName(UTF8Char *sbuff, UOSInt index)
{
	Text::String *fileName = this->files->GetItem(index);
	UOSInt i;
	if (fileName == 0)
		return 0;
	i = fileName->LastIndexOf(IO::Path::PATH_SEPERATOR);
	return Text::StrConcat(sbuff, &fileName->v[i + 1]);
}

IO::IStreamData *IO::DirectoryPackage::GetItemStmData(UOSInt index)
{
	Text::String *fileName = this->files->GetItem(index);
	if (fileName == 0)
		return 0;
	IO::Path::PathType pt = IO::Path::GetPathType(fileName->v);
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

IO::PackageFile *IO::DirectoryPackage::GetItemPack(UOSInt index)
{
	Text::String *fileName = this->files->GetItem(index);
	if (fileName == 0)
		return 0;
	IO::Path::PathType pt = IO::Path::GetPathType(fileName->v);
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

IO::ParsedObject *IO::DirectoryPackage::GetItemPObj(UOSInt index)
{
	return GetItemPack(index);
}

Int64 IO::DirectoryPackage::GetItemModTimeTick(UOSInt index)
{
	return this->fileTimes->GetItem(index);
}

UInt64 IO::DirectoryPackage::GetItemSize(UOSInt index)
{
	return this->fileSizes->GetItem(index);
}

UOSInt IO::DirectoryPackage::GetItemIndex(const UTF8Char *name, UOSInt nameLen)
{
	UOSInt j = this->files->GetCount();
	UOSInt i;
	while (j-- > 0)
	{
		Text::String *fileName = this->files->GetItem(j);
		if (fileName)
		{
			i = fileName->LastIndexOf(IO::Path::PATH_SEPERATOR);
			if (Text::StrEqualsC(&fileName->v[i + 1], fileName->leng - i - 1, name, nameLen))
			{
				return j;
			}
		}
	}
	return INVALID_INDEX;
}

Bool IO::DirectoryPackage::IsCompressed(UOSInt index)
{
	return false;
}

Data::Compress::Decompressor::CompressMethod IO::DirectoryPackage::GetItemComp(UOSInt index)
{
	return Data::Compress::Decompressor::CM_UNCOMPRESSED;
}

IO::PackageFile *IO::DirectoryPackage::Clone()
{
	IO::DirectoryPackage *dpkg;
	NEW_CLASS(dpkg, IO::DirectoryPackage(this->dirName));
	return dpkg;
}

Bool IO::DirectoryPackage::AllowWrite()
{
	return true;
}

Bool IO::DirectoryPackage::CopyFrom(const UTF8Char *fileName, IO::IProgressHandler *progHdlr, IO::ActiveStreamReader::BottleNeckType *bnt)
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
		i = Text::StrLastIndexOfChar(fileName, IO::Path::PATH_SEPERATOR);
		Text::StrConcat(sptr, &fileName[i + 1]);
		ret = IO::FileUtil::CopyFile(fileName, sbuff, IO::FileUtil::FileExistAction::Fail, progHdlr, bnt);
		if (ret)
		{
			this->AddFile(sbuff);
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
		i = Text::StrLastIndexOfChar(fileName, IO::Path::PATH_SEPERATOR);
		Text::StrConcat(sptr, &fileName[i + 1]);
		ret = IO::FileUtil::CopyDir(fileName, sbuff, IO::FileUtil::FileExistAction::Fail, progHdlr, bnt);
		if (ret)
		{
			this->AddFile(sbuff);
		}
		return ret;
	}
	return false;
}

Bool IO::DirectoryPackage::MoveFrom(const UTF8Char *fileName, IO::IProgressHandler *progHdlr, IO::ActiveStreamReader::BottleNeckType *bnt)
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
		i = Text::StrLastIndexOfChar(fileName, IO::Path::PATH_SEPERATOR);
		Text::StrConcat(sptr, &fileName[i + 1]);
		ret = IO::FileUtil::MoveFile(fileName, sbuff, IO::FileUtil::FileExistAction::Fail, progHdlr, bnt);
		if (ret)
		{
			this->AddFile(sbuff);
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
		i = Text::StrLastIndexOfChar(fileName, IO::Path::PATH_SEPERATOR);
		Text::StrConcat(sptr, &fileName[i + 1]);
		ret = IO::FileUtil::MoveDir(fileName, sbuff, IO::FileUtil::FileExistAction::Fail, progHdlr, bnt);
		if (ret)
		{
			this->AddFile(sbuff);
		}
		return ret;
	}
	return false;
}

Bool IO::DirectoryPackage::RetryCopyFrom(const UTF8Char *fileName, IO::IProgressHandler *progHdlr, IO::ActiveStreamReader::BottleNeckType *bnt)
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
		i = Text::StrLastIndexOfChar(fileName, IO::Path::PATH_SEPERATOR);
		Text::StrConcat(sptr, &fileName[i + 1]);
		ret = IO::FileUtil::CopyFile(fileName, sbuff, IO::FileUtil::FileExistAction::Continue, progHdlr, bnt);
		if (ret)
		{
			this->AddFile(sbuff);
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
		i = Text::StrLastIndexOfChar(fileName, IO::Path::PATH_SEPERATOR);
		Text::StrConcat(sptr, &fileName[i + 1]);
		ret = IO::FileUtil::CopyDir(fileName, sbuff, IO::FileUtil::FileExistAction::Continue, progHdlr, bnt);
		if (ret)
		{
			this->AddFile(sbuff);
		}
		return ret;
	}
	return false;
}

Bool IO::DirectoryPackage::RetryMoveFrom(const UTF8Char *fileName, IO::IProgressHandler *progHdlr, IO::ActiveStreamReader::BottleNeckType *bnt)
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
		i = Text::StrLastIndexOfChar(fileName, IO::Path::PATH_SEPERATOR);
		Text::StrConcat(sptr, &fileName[i + 1]);
		ret = IO::FileUtil::MoveFile(fileName, sbuff, IO::FileUtil::FileExistAction::Continue, progHdlr, bnt);
		if (ret)
		{
			this->AddFile(sbuff);
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
		i = Text::StrLastIndexOfChar(fileName, IO::Path::PATH_SEPERATOR);
		Text::StrConcat(sptr, &fileName[i + 1]);
		ret = IO::FileUtil::MoveDir(fileName, sbuff, IO::FileUtil::FileExistAction::Continue, progHdlr, bnt);
		if (ret)
		{
			this->AddFile(sbuff);
		}
		return ret;
	}
	return false;
}

typedef struct
{
	Text::String *name;
	UInt64 fileSize;
	Int64 fileTime;
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
	j = this->files->GetCount();
	if (j <= 0)
		return true;
	arr = MemAlloc(void*, j);
	while (i < j)
	{
		df = MemAlloc(DirFile, 1);
		df->name = this->files->GetItem(i);
		df->fileSize = this->fileSizes->GetItem(i);
		df->fileTime = this->fileTimes->GetItem(i);
		arr[i] = df;
		i++;
	}
	ArtificialQuickSort_SortCmp(arr, DirectoryPackage_Compare, 0, (OSInt)j - 1);
	i = 0;
	while (i < j)
	{
		df = (DirFile*)arr[i];
		this->files->SetItem(i, df->name);
		this->fileSizes->SetItem(i, df->fileSize);
		this->fileTimes->SetItem(i, df->fileTime);
		MemFree(df);
		i++;
	}
	MemFree(arr);
	return true;
}
