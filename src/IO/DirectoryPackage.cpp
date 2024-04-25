#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/Sort/ArtificialQuickSortFunc.h"
#include "IO/DirectoryPackage.h"
#include "IO/FileUtil.h"
#include "IO/Path.h"
#include "IO/StmData/FileData.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"

void IO::DirectoryPackage::AddFile(Text::CStringNN fileName)
{
	UTF8Char sbuff[512];
	IO::Path::FindFileSession *sess = IO::Path::FindFile(fileName);
	if (sess)
	{
		Data::Timestamp ts;
		UInt64 fileSize;
		IO::Path::PathType pt;
		FileItem item;
		if (IO::Path::FindNextFile(sbuff, sess, &ts, &pt, &fileSize))
		{
			item.fileName = Text::String::New(fileName);
			item.fileSize = fileSize;
			item.modTime = ts;
			item.accTime = 0;
			item.createTime = 0;
			IO::Path::GetFileTime(fileName, item.modTime, item.accTime, item.createTime);
			item.unixAttr = IO::Path::GetFileUnixAttr(fileName);
			this->files.Add(item);
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
	FileItem item;
	IO::Path::FindFileSession *sess;
	this->parent = 0;
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
					item.fileName = Text::String::New(sbuff, (UOSInt)(sptr2 - sbuff));
					item.fileSize = fileSize;
					item.modTime = ts;
					item.accTime = 0;
					item.createTime = 0;
					IO::Path::GetFileTime(CSTRP(sbuff, sptr2), item.modTime, item.accTime, item.createTime);
					item.unixAttr = IO::Path::GetFileUnixAttr(CSTRP(sbuff, sptr2));
					this->files.Add(item);
				}
			}
			IO::Path::FindFileClose(sess);
		}
	}
}

IO::DirectoryPackage::DirectoryPackage(NN<Text::String> dirName) : IO::PackageFile(dirName)
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

IO::DirectoryPackage::DirectoryPackage(Text::CStringNN dirName) : IO::PackageFile(dirName)
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
		this->files.GetItem(i).fileName->Release();
	}
	this->dirName->Release();
}

UOSInt IO::DirectoryPackage::GetCount() const
{
	return this->files.GetCount();
}

IO::PackageFile::PackObjectType IO::DirectoryPackage::GetItemType(UOSInt index) const
{
	if (this->files.GetCount() <= index)
		return IO::PackageFile::PackObjectType::Unknown;
	NN<Text::String> fileName = this->files.GetItem(index).fileName;
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
	UOSInt i;
	if (this->files.GetCount() <= index)
		return 0;
	NN<Text::String> fileName = this->files.GetItem(index).fileName;
	i = fileName->LastIndexOf(IO::Path::PATH_SEPERATOR);
	return Text::StrConcat(sbuff, &fileName->v[i + 1]);
}

Optional<IO::StreamData> IO::DirectoryPackage::GetItemStmDataNew(UOSInt index) const
{
	if (this->files.GetCount() <= index)
		return 0;
	NN<Text::String> fileName = this->files.GetItem(index).fileName;
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

Optional<IO::PackageFile> IO::DirectoryPackage::GetItemPack(UOSInt index, OutParam<Bool> needDelete) const
{
	if (this->files.GetCount() <= index)
		return 0;
	NN<Text::String> fileName = this->files.GetItem(index).fileName;
	IO::Path::PathType pt = IO::Path::GetPathType(fileName->ToCString());
	if (pt == IO::Path::PathType::Directory)
	{
		IO::DirectoryPackage *pkg;
		NEW_CLASS(pkg, IO::DirectoryPackage(fileName));
		pkg->SetParent(this->parent);
		needDelete.Set(true);
		return pkg;
	}
	else
	{
		return 0;
	}
}

Optional<IO::ParsedObject> IO::DirectoryPackage::GetItemPObj(UOSInt index, OutParam<Bool> needRelease) const
{
	return GetItemPack(index, needRelease);
}

Data::Timestamp IO::DirectoryPackage::GetItemModTime(UOSInt index) const
{
	return this->files.GetItem(index).modTime;
}

Data::Timestamp IO::DirectoryPackage::GetItemAccTime(UOSInt index) const
{
	return this->files.GetItem(index).accTime;
}

Data::Timestamp IO::DirectoryPackage::GetItemCreateTime(UOSInt index) const
{
	return this->files.GetItem(index).createTime;
}

UInt32 IO::DirectoryPackage::GetItemUnixAttr(UOSInt index) const
{
	return this->files.GetItem(index).unixAttr;
}

UInt64 IO::DirectoryPackage::GetItemStoreSize(UOSInt index) const
{
	return this->files.GetItem(index).fileSize;
}

UInt64 IO::DirectoryPackage::GetItemSize(UOSInt index) const
{
	return this->files.GetItem(index).fileSize;
}

UOSInt IO::DirectoryPackage::GetItemIndex(Text::CStringNN name) const
{
	UOSInt j = this->files.GetCount();
	UOSInt i;
	while (j-- > 0)
	{
		NN<Text::String> fileName = this->files.GetItem(j).fileName;
		i = fileName->LastIndexOf(IO::Path::PATH_SEPERATOR);
		if (name.Equals(&fileName->v[i + 1], fileName->leng - i - 1))
		{
			return j;
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

NN<IO::PackageFile> IO::DirectoryPackage::Clone() const
{
	NN<IO::DirectoryPackage> dpkg;
	NEW_CLASSNN(dpkg, IO::DirectoryPackage(this->dirName));
	return dpkg;
}

IO::PackageFileType IO::DirectoryPackage::GetFileType() const
{
	return IO::PackageFileType::Directory;
}

Bool IO::DirectoryPackage::CopyFrom(Text::CStringNN fileName, Optional<IO::ProgressHandler> progHdlr, OptOut<IO::ActiveStreamReader::BottleNeckType> bnt)
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

Bool IO::DirectoryPackage::MoveFrom(Text::CStringNN fileName, Optional<IO::ProgressHandler> progHdlr, OptOut<IO::ActiveStreamReader::BottleNeckType> bnt)
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

Bool IO::DirectoryPackage::RetryCopyFrom(Text::CStringNN fileName, Optional<IO::ProgressHandler> progHdlr, OptOut<IO::ActiveStreamReader::BottleNeckType> bnt)
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

Bool IO::DirectoryPackage::RetryMoveFrom(Text::CStringNN fileName, Optional<IO::ProgressHandler> progHdlr, OptOut<IO::ActiveStreamReader::BottleNeckType> bnt)
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

Bool IO::DirectoryPackage::CopyTo(UOSInt index, Text::CString destPath, Bool fullFileName)
{
	return false;
}

Optional<IO::StreamData> IO::DirectoryPackage::OpenStreamData(Text::CString fileName) const
{
	return 0;
}

Bool IO::DirectoryPackage::HasParent() const
{
	if (!this->parent.IsNull())
		return true;
	if (IO::Path::PATH_SEPERATOR == '/')
	{
	}
	else
	{
		if (this->dirName->StartsWith(UTF8STRC("\\\\")))
		{

		}
	}
	return false;
}

Optional<IO::PackageFile> IO::DirectoryPackage::GetParent(OutParam<Bool> needRelease) const
{
	return this->parent;
}

OSInt __stdcall DirectoryPackage_Compare(IO::DirectoryPackage::FileItem obj1, IO::DirectoryPackage::FileItem obj2)
{
	return obj1.fileName->CompareTo(obj2.fileName);
}

Bool IO::DirectoryPackage::DeleteItem(UOSInt index)
{
	if (index >= this->files.GetCount())
		return false;
	FileItem file = this->files.GetItem(index);
	IO::Path::PathType pt = IO::Path::GetPathType(file.fileName->ToCString());
	if (pt == IO::Path::PathType::File)
	{
		if (IO::Path::DeleteFile(file.fileName->v))
		{
			file.fileName->Release();
			this->files.RemoveAt(index);
			return true;
		}
	}
	return false;
}

void IO::DirectoryPackage::SetParent(Optional<IO::PackageFile> pkg)
{
	this->parent = pkg;
}

Bool IO::DirectoryPackage::Sort()
{
	UOSInt i;
	UOSInt j;
	FileItem df;
	FileItem *arr;
	i = 0;
	j = this->files.GetCount();
	if (j <= 0)
		return true;
	arr = MemAlloc(FileItem, j);
	while (i < j)
	{
		arr[i] = this->files.GetItem(i);
		i++;
	}
	Data::Sort::ArtificialQuickSortFunc<FileItem>::Sort(arr, DirectoryPackage_Compare, 0, (OSInt)j - 1);
	i = 0;
	while (i < j)
	{
		df = arr[i];
		this->files.SetItem(i, df);
		i++;
	}
	MemFree(arr);
	return true;
}
