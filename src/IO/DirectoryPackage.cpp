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
			this->files->Add(Text::StrCopyNew(fileName));
			this->fileSizes->Add(fileSize);
			this->fileTimes->Add(dt.ToTicks());
		}
		IO::Path::FindFileClose(sess);
	}
}

IO::DirectoryPackage::DirectoryPackage(const UTF8Char *dirName) : IO::PackageFile(dirName)
{
	Text::StringBuilderUTF8 *sb;
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	IO::Path::PathType pt;
	UInt64 fileSize;
	Data::DateTime *dt;
	IO::Path::FindFileSession *sess;
	this->dirName = Text::StrCopyNew(dirName);
	NEW_CLASS(this->files, Data::ArrayList<const UTF8Char*>());
	NEW_CLASS(this->fileSizes, Data::ArrayListUInt64());
	NEW_CLASS(this->fileTimes, Data::ArrayListInt64());
	NEW_CLASS(sb, Text::StringBuilderUTF8());
	NEW_CLASS(dt, Data::DateTime());
	sb->Append(dirName);
	if (sb->GetLength() > 0)
	{
		if (sb->GetEndPtr()[-1] != IO::Path::PATH_SEPERATOR)
		{
			sb->AppendChar(IO::Path::PATH_SEPERATOR, 1);
		}
		sptr = sb->GetEndPtr();
		sb->Append(IO::Path::ALL_FILES);
		sess = IO::Path::FindFile(sb->ToString());
		if (sess)
		{
			while (IO::Path::FindNextFile(sbuff, sess, dt, &pt, &fileSize))
			{
				if (sbuff[0] == '.' && sbuff[1] == 0)
				{
				}
				else if (sbuff[0] == '.' && sbuff[1] == '.' && sbuff[2] == 0)
				{
				}
				else
				{
					sb->SetEndPtr(sptr);
					sb->Append(sbuff);
					this->files->Add(Text::StrCopyNew(sb->ToString()));
					this->fileSizes->Add(fileSize);
					this->fileTimes->Add(dt->ToTicks());
				}
			}
			IO::Path::FindFileClose(sess);
		}
	}
	DEL_CLASS(dt);
	DEL_CLASS(sb);
}

IO::DirectoryPackage::~DirectoryPackage()
{
	UOSInt i;
	i = this->files->GetCount();
	while (i-- > 0)
	{
		Text::StrDelNew(this->files->GetItem(i));
	}
	DEL_CLASS(this->files);
	DEL_CLASS(this->fileSizes);
	DEL_CLASS(this->fileTimes);
	Text::StrDelNew(this->dirName);
}

UOSInt IO::DirectoryPackage::GetCount()
{
	return this->files->GetCount();
}

IO::PackageFile::PackObjectType IO::DirectoryPackage::GetItemType(UOSInt index)
{
	const UTF8Char *fileName = this->files->GetItem(index);
	if (fileName == 0)
		return IO::PackageFile::POT_UNKNOWN;
	IO::Path::PathType pt = IO::Path::GetPathType(fileName);
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
	const UTF8Char *fileName = this->files->GetItem(index);
	UOSInt i;
	if (fileName == 0)
		return 0;
	i = Text::StrLastIndexOf(fileName, IO::Path::PATH_SEPERATOR);
	return Text::StrConcat(sbuff, &fileName[i + 1]);
}

IO::IStreamData *IO::DirectoryPackage::GetItemStmData(UOSInt index)
{
	const UTF8Char *fileName = this->files->GetItem(index);
	if (fileName == 0)
		return 0;
	IO::Path::PathType pt = IO::Path::GetPathType(fileName);
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
	const UTF8Char *fileName = this->files->GetItem(index);
	if (fileName == 0)
		return 0;
	IO::Path::PathType pt = IO::Path::GetPathType(fileName);
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

UOSInt IO::DirectoryPackage::GetItemIndex(const UTF8Char *name)
{
	UOSInt j = this->files->GetCount();
	UOSInt i;
	while (j-- > 0)
	{
		const UTF8Char *fileName = this->files->GetItem(j);
		if (fileName)
		{
			i = Text::StrLastIndexOf(fileName, IO::Path::PATH_SEPERATOR);
			if (Text::StrEquals(&fileName[i + 1], name))
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
		UTF8Char *sptr = Text::StrConcat(sbuff, this->dirName);
		if (sptr[-1] != IO::Path::PATH_SEPERATOR)
		{
			*sptr++ = IO::Path::PATH_SEPERATOR;
		}
		i = Text::StrLastIndexOf(fileName, IO::Path::PATH_SEPERATOR);
		Text::StrConcat(sptr, &fileName[i + 1]);
		ret = IO::FileUtil::CopyFile(fileName, sbuff, IO::FileUtil::FEA_FAIL, progHdlr, bnt);
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
		UTF8Char *sptr = Text::StrConcat(sbuff, this->dirName);
		if (sptr[-1] != IO::Path::PATH_SEPERATOR)
		{
			*sptr++ = IO::Path::PATH_SEPERATOR;
		}
		i = Text::StrLastIndexOf(fileName, IO::Path::PATH_SEPERATOR);
		Text::StrConcat(sptr, &fileName[i + 1]);
		ret = IO::FileUtil::CopyDir(fileName, sbuff, IO::FileUtil::FEA_FAIL, progHdlr, bnt);
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
		UTF8Char *sptr = Text::StrConcat(sbuff, this->dirName);
		if (sptr[-1] != IO::Path::PATH_SEPERATOR)
		{
			*sptr++ = IO::Path::PATH_SEPERATOR;
		}
		i = Text::StrLastIndexOf(fileName, IO::Path::PATH_SEPERATOR);
		Text::StrConcat(sptr, &fileName[i + 1]);
		ret = IO::FileUtil::MoveFile(fileName, sbuff, IO::FileUtil::FEA_FAIL, progHdlr, bnt);
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
		UTF8Char *sptr = Text::StrConcat(sbuff, this->dirName);
		if (sptr[-1] != IO::Path::PATH_SEPERATOR)
		{
			*sptr++ = IO::Path::PATH_SEPERATOR;
		}
		i = Text::StrLastIndexOf(fileName, IO::Path::PATH_SEPERATOR);
		Text::StrConcat(sptr, &fileName[i + 1]);
		ret = IO::FileUtil::MoveDir(fileName, sbuff, IO::FileUtil::FEA_FAIL, progHdlr, bnt);
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
		UTF8Char *sptr = Text::StrConcat(sbuff, this->dirName);
		if (sptr[-1] != IO::Path::PATH_SEPERATOR)
		{
			*sptr++ = IO::Path::PATH_SEPERATOR;
		}
		i = Text::StrLastIndexOf(fileName, IO::Path::PATH_SEPERATOR);
		Text::StrConcat(sptr, &fileName[i + 1]);
		ret = IO::FileUtil::CopyFile(fileName, sbuff, IO::FileUtil::FEA_CONTINUE, progHdlr, bnt);
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
		UTF8Char *sptr = Text::StrConcat(sbuff, this->dirName);
		if (sptr[-1] != IO::Path::PATH_SEPERATOR)
		{
			*sptr++ = IO::Path::PATH_SEPERATOR;
		}
		i = Text::StrLastIndexOf(fileName, IO::Path::PATH_SEPERATOR);
		Text::StrConcat(sptr, &fileName[i + 1]);
		ret = IO::FileUtil::CopyDir(fileName, sbuff, IO::FileUtil::FEA_CONTINUE, progHdlr, bnt);
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
		UTF8Char *sptr = Text::StrConcat(sbuff, this->dirName);
		if (sptr[-1] != IO::Path::PATH_SEPERATOR)
		{
			*sptr++ = IO::Path::PATH_SEPERATOR;
		}
		i = Text::StrLastIndexOf(fileName, IO::Path::PATH_SEPERATOR);
		Text::StrConcat(sptr, &fileName[i + 1]);
		ret = IO::FileUtil::MoveFile(fileName, sbuff, IO::FileUtil::FEA_CONTINUE, progHdlr, bnt);
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
		UTF8Char *sptr = Text::StrConcat(sbuff, this->dirName);
		if (sptr[-1] != IO::Path::PATH_SEPERATOR)
		{
			*sptr++ = IO::Path::PATH_SEPERATOR;
		}
		i = Text::StrLastIndexOf(fileName, IO::Path::PATH_SEPERATOR);
		Text::StrConcat(sptr, &fileName[i + 1]);
		ret = IO::FileUtil::MoveDir(fileName, sbuff, IO::FileUtil::FEA_CONTINUE, progHdlr, bnt);
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
	const UTF8Char *name;
	UInt64 fileSize;
	Int64 fileTime;
} DirFile;

OSInt __stdcall DirectoryPackage_Compare(void *obj1, void *obj2)
{
	DirFile *df1 = (DirFile*)obj1;
	DirFile *df2 = (DirFile*)obj2;
	return Text::StrCompare(df1->name, df2->name);
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
