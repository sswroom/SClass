#include "Stdafx.h"
#include "MyMemory.h"
#include "Crypto/Hash/HashCreator.h"
#include "Crypto/Hash/HashStream.h"
#include "Data/ByteBuffer.h"
#include "Data/ByteTool.h"
#include "Data/ICaseStringMap.h"
#include "IO/DirectoryPackage.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "IO/StreamData.h"
#include "IO/VirtualPackageFile.h"
#include "IO/StmData/FileData.h"
#include "Sync/Event.h"
#include "Sync/Interlocked.h"
#include "Sync/Mutex.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"

void IO::VirtualPackageFile::ReusePackFileItem(IO::PackFileItem *item)
{
	SDEL_CLASS(item->fd);
	SDEL_CLASS(item->pobj);
	if (item->compInfo)
	{
		if (item->compInfo->compExtras)
		{
			MemFree(item->compInfo->compExtras);
		}
		MemFree(item->compInfo);
	}
}

IO::VirtualPackageFile::VirtualPackageFile(const VirtualPackageFile *pkg) : IO::PackageFile(pkg->GetSourceNameObj()), pkgFiles(&pkg->pkgFiles)
{
	this->parent = pkg->parent;
	this->items = pkg->items->Clone();
	this->namedItems = pkg->namedItems->Clone();
	IO::PackFileItem *item;
	UOSInt i;
	UOSInt j;
	i = 0;
	j = this->items->GetCount();
	while (i < j)
	{
		item = this->items->GetItem(i);
		Sync::Interlocked::IncrementI32(item->useCnt);
		i++;
	}
}

IO::VirtualPackageFile::VirtualPackageFile(NotNullPtr<Text::String> fileName) : IO::PackageFile(fileName)
{
	this->parent = 0;
	NEW_CLASSNN(this->items, Data::ArrayList<PackFileItem*>());
	if (IO::Path::PATH_SEPERATOR == '\\')
	{
		NEW_CLASSNN(this->namedItems, Data::ICaseStringMap<PackFileItem*>());
	}
	else
	{
		NEW_CLASSNN(this->namedItems, Data::StringMap<PackFileItem*>());
	}
}

IO::VirtualPackageFile::VirtualPackageFile(Text::CStringNN fileName) : IO::PackageFile(fileName)
{
	this->parent = 0;
	NEW_CLASSNN(this->items, Data::ArrayList<PackFileItem*>());
	if (IO::Path::PATH_SEPERATOR == '\\')
	{
		NEW_CLASSNN(this->namedItems, Data::ICaseStringMap<PackFileItem*>());
	}
	else
	{
		NEW_CLASSNN(this->namedItems, Data::StringMap<PackFileItem*>());
	}
}

IO::VirtualPackageFile::~VirtualPackageFile()
{
	PackFileItem *item;
	UOSInt i = this->items->GetCount();
	while (i-- > 0)
	{
		item = this->items->GetItem(i);
		if (Sync::Interlocked::DecrementI32(item->useCnt) == 0)
		{
			item->name->Release();
			SDEL_CLASS(item->fd);
			SDEL_CLASS(item->pobj);
			if (item->compInfo)
			{
				if (item->compInfo->compExtras)
				{
					MemFree(item->compInfo->compExtras);
				}
				MemFree(item->compInfo);
			}
			MemFree(item);
		}
	}
	this->items.Delete();
	this->namedItems.Delete();
	i = this->infoMap.GetCount();
	while (i-- > 0)
	{
		Text::StrDelNew(this->infoMap.GetItem(i));
	}
}

Bool IO::VirtualPackageFile::AddData(NotNullPtr<IO::StreamData> fd, UInt64 ofst, UInt64 length, Text::CStringNN name, const Data::Timestamp &modTime, const Data::Timestamp &accTime, const Data::Timestamp &createTime, UInt32 unixAttr)
{
	PackFileItem *item;
	item = MemAlloc(PackFileItem, 1);
	item->itemType = IO::PackFileItem::PackItemType::Uncompressed;
	item->fd = fd->GetPartialData(ofst, length).Ptr();
	item->name = Text::String::New(name);
	item->pobj = 0;
	item->compInfo = 0;
	item->modTime = modTime;
	item->accTime = accTime;
	item->createTime = createTime;
	item->unixAttr = unixAttr;
	item->useCnt = 1;
	this->items->Add(item);
	this->namedItems->PutNN(item->name, item);
	return true;
}

Bool IO::VirtualPackageFile::AddObject(IO::ParsedObject *pobj, Text::CStringNN name, const Data::Timestamp &modTime, const Data::Timestamp &accTime, const Data::Timestamp &createTime, UInt32 unixAttr)
{
	PackFileItem *item;
	item = MemAlloc(PackFileItem, 1);
	item->itemType = IO::PackFileItem::PackItemType::ParsedObject;
	item->fd = 0;
	if (name.leng > 0)
	{
		item->name = Text::String::New(name);
	}
	else
	{
		item->name = pobj->GetSourceNameObj()->Clone();
	}
	item->pobj = pobj;
	item->compInfo = 0;
	item->modTime = modTime;
	item->accTime = accTime;
	item->createTime = createTime;
	item->unixAttr = unixAttr;
	item->useCnt = 1;
	this->items->Add(item);
	this->namedItems->PutNN(item->name, item);
	return true;
}

Bool IO::VirtualPackageFile::AddCompData(NotNullPtr<IO::StreamData> fd, UInt64 ofst, UInt64 length, IO::PackFileItem::CompressInfo *compInfo, Text::CStringNN name, const Data::Timestamp &modTime, const Data::Timestamp &accTime, const Data::Timestamp &createTime, UInt32 unixAttr)
{
	PackFileItem *item;
	item = MemAlloc(PackFileItem, 1);
	item->itemType = IO::PackFileItem::PackItemType::Compressed;
	item->fd = fd->GetPartialData(ofst, length).Ptr();
	item->name = Text::String::New(name);
	item->pobj = 0;
	item->compInfo = MemAlloc(PackFileItem::CompressInfo, 1);
	MemCopyNO(item->compInfo, compInfo, sizeof(PackFileItem::CompressInfo));
	if (compInfo->compExtras)
	{
		item->compInfo->compExtras = MemAlloc(UInt8, compInfo->compExtraSize);
		MemCopyNO(item->compInfo->compExtras, compInfo->compExtras, compInfo->compExtraSize);
	}
	item->modTime = modTime;
	item->accTime = accTime;
	item->createTime = createTime;
	item->unixAttr = unixAttr;
	item->useCnt = 1;
	this->items->Add(item);
	this->namedItems->PutNN(item->name, item);
	return true;
}

Bool IO::VirtualPackageFile::AddPack(NotNullPtr<IO::PackageFile> pkg, Text::CStringNN name, const Data::Timestamp &modTime, const Data::Timestamp &accTime, const Data::Timestamp &createTime, UInt32 unixAttr)
{
	PackFileItem *item;
	item = MemAlloc(PackFileItem, 1);
	item->itemType = IO::PackFileItem::PackItemType::ParsedObject;
	item->fd = 0;
	item->name = Text::String::New(name);
	item->pobj = pkg.Ptr();
	item->compInfo = 0;
	item->modTime = modTime;
	item->accTime = accTime;
	item->createTime = createTime;
	item->unixAttr = unixAttr;
	item->useCnt = 1;
	pkg->SetParent(this);
	this->items->Add(item);
	this->pkgFiles.PutNN(item->name, item);
	this->namedItems->PutNN(item->name, item);
	return true;
}

Bool IO::VirtualPackageFile::AddOrReplaceData(NotNullPtr<StreamData> fd, UInt64 ofst, UInt64 length, Text::CStringNN name, const Data::Timestamp &modTime, const Data::Timestamp &accTime, const Data::Timestamp &createTime, UInt32 unixAttr)
{
	PackFileItem *item;
	UOSInt i = GetItemIndex(name);
	if (i != INVALID_INDEX)
	{
		item = this->items->GetItem(i);
		if (item && (item->itemType == IO::PackFileItem::PackItemType::Uncompressed || item->itemType == IO::PackFileItem::PackItemType::Compressed))
		{
			ReusePackFileItem(item);
			item->itemType = IO::PackFileItem::PackItemType::Uncompressed;
			item->fd = fd->GetPartialData(ofst, length).Ptr();
			item->pobj = 0;
			item->compInfo = 0;
			item->modTime = modTime;
			item->accTime = accTime;
			item->createTime = createTime;
			item->unixAttr = unixAttr;
			return true;
		}
		return false;
	}
	return this->AddData(fd, ofst, length, name, modTime, accTime, createTime, unixAttr);
}

Bool IO::VirtualPackageFile::AddOrReplaceObject(IO::ParsedObject *pobj, Text::CStringNN name, const Data::Timestamp &modTime, const Data::Timestamp &accTime, const Data::Timestamp &createTime, UInt32 unixAttr)
{
	PackFileItem *item;
	UOSInt i = GetItemIndex(name);
	if (i != INVALID_INDEX)
	{
		item = this->items->GetItem(i);
		if (item && (item->itemType == IO::PackFileItem::PackItemType::Uncompressed || item->itemType == IO::PackFileItem::PackItemType::Compressed))
		{
			ReusePackFileItem(item);
			item->itemType = IO::PackFileItem::PackItemType::ParsedObject;
			item->fd = 0;
			item->pobj = pobj;
			item->compInfo = 0;
			item->modTime = modTime;
			item->accTime = accTime;
			item->createTime = createTime;
			item->unixAttr = unixAttr;
			return true;
		}
		DEL_CLASS(pobj);
		return false;
	}
	return this->AddObject(pobj, name, modTime, accTime, createTime, unixAttr);
}

Bool IO::VirtualPackageFile::AddOrReplaceCompData(NotNullPtr<StreamData> fd, UInt64 ofst, UInt64 length, PackFileItem::CompressInfo *compInfo, Text::CStringNN name, const Data::Timestamp &modTime, const Data::Timestamp &accTime, const Data::Timestamp &createTime, UInt32 unixAttr)
{
	PackFileItem *item;
	UOSInt i = GetItemIndex(name);
	if (i != INVALID_INDEX)
	{
		item = this->items->GetItem(i);
		if (item && (item->itemType == IO::PackFileItem::PackItemType::Uncompressed || item->itemType == IO::PackFileItem::PackItemType::Compressed))
		{
			ReusePackFileItem(item);
			item->itemType = IO::PackFileItem::PackItemType::Compressed;
			item->fd = fd->GetPartialData(ofst, length).Ptr();
			item->pobj = 0;
			item->compInfo = MemAlloc(PackFileItem::CompressInfo, 1);
			MemCopyNO(item->compInfo, compInfo, sizeof(PackFileItem::CompressInfo));
			if (compInfo->compExtras)
			{
				item->compInfo->compExtras = MemAlloc(UInt8, compInfo->compExtraSize);
				MemCopyNO(item->compInfo->compExtras, compInfo->compExtras, compInfo->compExtraSize);
			}
			item->modTime = modTime;
			item->accTime = accTime;
			item->createTime = createTime;
			item->unixAttr = unixAttr;
			return true;
		}
		return false;
	}
	return this->AddCompData(fd, ofst, length, compInfo, name, modTime, accTime, createTime, unixAttr);
}

Bool IO::VirtualPackageFile::AddOrReplacePack(NotNullPtr<IO::PackageFile> pkg, Text::CStringNN name, const Data::Timestamp &modTime, const Data::Timestamp &accTime, const Data::Timestamp &createTime, UInt32 unixAttr)
{
	PackFileItem *item;
	UOSInt i = GetItemIndex(name);
	if (i != INVALID_INDEX)
	{
		item = this->items->GetItem(i);
		if (item && item->itemType == IO::PackFileItem::PackItemType::ParsedObject && item->pobj->GetParserType() == IO::ParserType::PackageFile)
		{
			IO::PackageFile *myPkg = (IO::PackageFile*)item->pobj;
			if (myPkg->GetFileType() == IO::PackageFileType::Virtual)
			{
				item->modTime = modTime;
				item->accTime = accTime;
				item->createTime = createTime;
				item->unixAttr = unixAttr;
				Bool succ = ((IO::VirtualPackageFile*)myPkg)->MergePackage(pkg);
				pkg.Delete();
				return succ;
			}
		}
		pkg.Delete();
		return false;
	}
	return this->AddPack(pkg, name, modTime, accTime, createTime, unixAttr);
}

IO::PackageFile *IO::VirtualPackageFile::GetPackFile(Text::CStringNN name) const
{
	IO::PackFileItem *item = this->pkgFiles.GetC(name);
	if (item)
		return (IO::PackageFile*)item->pobj;
	return 0;
}

Bool IO::VirtualPackageFile::UpdateCompInfo(const UTF8Char *name, NotNullPtr<IO::StreamData> fd, UInt64 ofst, Int32 crc, UOSInt compSize, UInt32 decSize)
{
	UOSInt i;
	IO::PackFileItem *item;
	UOSInt nameLen = Text::StrCharCnt(name);
	i = this->items->GetCount();
	while (i-- > 0)
	{
		item = this->items->GetItem(i);
		if (item->name->Equals(name, nameLen))
		{
			if (item->itemType == IO::PackFileItem::PackItemType::Compressed)
			{
				item->compInfo->decSize = decSize;
				DEL_CLASS(item->fd);
				item->fd = fd->GetPartialData(ofst, compSize).Ptr();
				WriteMInt32(item->compInfo->checkBytes, crc);
				return true;
			}
			break;
		}
	}
	return false;
}

Bool IO::VirtualPackageFile::MergePackage(NotNullPtr<IO::PackageFile> pkg)
{
	if (pkg->GetFileType() == IO::PackageFileType::Virtual)
	{
		NotNullPtr<IO::VirtualPackageFile> vpkg = NotNullPtr<IO::VirtualPackageFile>::ConvertFrom(pkg);
		const IO::PackFileItem *item;
		NotNullPtr<IO::StreamData> fd;
		UOSInt i = 0;
		UOSInt j = vpkg->GetCount();
		while (i < j)
		{
			item = vpkg->GetPackFileItem(i);
			switch (item->itemType)
			{
			case IO::PackFileItem::PackItemType::Uncompressed:
				if (!fd.Set(item->fd))
					return false;
				if (!this->AddOrReplaceData(fd, 0, fd->GetDataSize(), item->name->ToCString(), item->modTime, item->accTime, item->createTime, item->unixAttr))
					return false;
				break;
			case IO::PackFileItem::PackItemType::Compressed:
				if (!fd.Set(item->fd))
					return false;
				if (!this->AddOrReplaceCompData(fd, 0, fd->GetDataSize(), item->compInfo, item->name->ToCString(), item->modTime, item->accTime, item->createTime, item->unixAttr))
					return false;
				break;
			case IO::PackFileItem::PackItemType::ParsedObject:
				if (item->pobj == 0)
					return false;
				if (item->pobj->GetParserType() != IO::ParserType::PackageFile)
					return false;
				if (!this->AddOrReplacePack(((IO::PackageFile*)item->pobj)->Clone(), item->name->ToCString(), item->modTime, item->accTime, item->createTime, item->unixAttr))
					return false;
				break;
			default:
				return false;
			}
			i++;
		}

		return true;
	}
	else
	{
		UTF8Char sbuff[512];
		UTF8Char *sptr;
		NotNullPtr<IO::PackageFile> ipkg;
		Bool needRelease;
		NotNullPtr<IO::StreamData> fd;
		UOSInt i = 0;
		UOSInt j = pkg->GetCount();
		while (i < j)
		{
			sptr = pkg->GetItemName(sbuff, i);
			if (sptr == 0)
				return false;
			switch (pkg->GetItemType(i))
			{
			case IO::PackageFile::PackObjectType::PackageFileType:
				if (!ipkg.Set(pkg->GetItemPack(i, needRelease)))
					return false;
				if (!needRelease)
					ipkg = ipkg->Clone();
				if (!this->AddOrReplacePack(ipkg, CSTRP(sbuff, sptr), pkg->GetItemModTime(i), pkg->GetItemAccTime(i), pkg->GetItemCreateTime(i), pkg->GetItemUnixAttr(i)))
					return false;
				break;
			case IO::PackageFile::PackObjectType::StreamData:
				if (!fd.Set(pkg->GetItemStmDataNew(i)))
					return false;
				if (!this->AddOrReplaceData(fd, 0, fd->GetDataSize(), CSTRP(sbuff, sptr), pkg->GetItemModTime(i), pkg->GetItemAccTime(i), pkg->GetItemCreateTime(i), pkg->GetItemUnixAttr(i)))
					return false;
				break;
			case IO::PackageFile::PackObjectType::ParsedObject:
			case IO::PackageFile::PackObjectType::Unknown:
			default:
				return false;
			}
			i++;
		}

		return true;
	}
}

const IO::PackFileItem *IO::VirtualPackageFile::GetPackFileItem(const UTF8Char *name) const
{
	UTF8Char sbuff[256];
	const UTF8Char *sptr;
	UOSInt nameLen;
	UTF8Char c;
	sptr = name;
	while (true)
	{
		c = *sptr++;
		if (c == 0)
		{
			nameLen = (UOSInt)(sptr - name - 1);
			if (nameLen <= 0)
				return 0;
			return this->namedItems->Get({name, nameLen});
		}
		else if (c == '/' || c == '\\')
		{
			nameLen = (UOSInt)(sptr - name - 1);
			Text::StrConcatC(sbuff, name, nameLen);
			IO::PackFileItem *item = this->namedItems->Get({sbuff, nameLen});
			if (item == 0)
			{
				return 0;
			}
			if (sptr[0] == 0)
				return item;
			if (item->itemType != IO::PackFileItem::PackItemType::ParsedObject)
			{
				return 0;
			}
			if (item->pobj->GetParserType() != IO::ParserType::PackageFile)
			{
				return 0;
			}
			IO::PackageFile *pf = (IO::PackageFile*)item->pobj;
			if (pf->GetFileType() != PackageFileType::Virtual)
			{
				return 0;
			}
			return ((IO::VirtualPackageFile*)pf)->GetPackFileItem(sptr);
		}
	}
	return 0;
}

const IO::PackFileItem *IO::VirtualPackageFile::GetPackFileItem(UOSInt index) const
{
	return this->items->GetItem(index);
}

IO::PackageFile::PackObjectType IO::VirtualPackageFile::GetPItemType(const PackFileItem *itemObj) const
{
	if (itemObj == 0)
	{
		return IO::PackageFile::PackObjectType::Unknown;
	}
	else if (itemObj->itemType == IO::PackFileItem::PackItemType::Compressed || itemObj->itemType == IO::PackFileItem::PackItemType::Uncompressed)
	{
		return IO::PackageFile::PackObjectType::StreamData;
	}
	else if (itemObj->itemType == IO::PackFileItem::PackItemType::ParsedObject)
	{
		if (itemObj->pobj->GetParserType() == IO::ParserType::PackageFile)
		{
			return IO::PackageFile::PackObjectType::PackageFileType;
		}
		else
		{
			return IO::PackageFile::PackObjectType::ParsedObject;
		}
	}
	else
	{
		return IO::PackageFile::PackObjectType::Unknown;
	}
}

IO::StreamData *IO::VirtualPackageFile::GetPItemStmDataNew(const PackFileItem *item) const
{
	if (item != 0)
	{
		if (item->itemType == IO::PackFileItem::PackItemType::Uncompressed)
		{
			NotNullPtr<IO::StreamData> data = item->fd->GetPartialData(0, item->fd->GetDataSize());
			Text::StringBuilderUTF8 sb;
			sb.Append(this->sourceName);
			sb.AppendC(UTF8STRC("\\"));
			sb.Append(item->name);
			data->SetFullName(sb.ToCString());
			return data.Ptr();
		}
		else if (item->itemType == IO::PackFileItem::PackItemType::Compressed)
		{
			Data::Compress::Decompressor *decomp = Data::Compress::Decompressor::CreateDecompressor(item->compInfo->compMethod);
			Crypto::Hash::IHash *hash;
			NotNullPtr<IO::StreamData> fd;
			if (decomp == 0 || !fd.Set(item->fd))
				return 0;

			hash = Crypto::Hash::HashCreator::CreateHash(item->compInfo->checkMethod);
			if (hash == 0)
			{
				DEL_CLASS(decomp);
				return 0;
			}
			UTF8Char sbuff[512];
			UTF8Char *sptr;
			UInt8 chkResult[32];
			UOSInt resSize;
			UOSInt i;
			Bool diff = false;
			sptr = IO::Path::GetProcessFileName(sbuff);
			sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("temp"));
			IO::Path::CreateDirectory(CSTRP(sbuff, sptr));
			*sptr++ = IO::Path::PATH_SEPERATOR;
			sptr = Text::StrHexVal64(sptr, (UInt64)Data::DateTimeUtil::GetCurrTimeMillis());
			*sptr++ = '_';
			sptr = item->name->ConcatTo(sptr);
			{
				IO::FileStream fs(CSTRP(sbuff, sptr), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
				Crypto::Hash::HashStream hashStm(&fs, hash);

				hash->Clear();
				decomp->Decompress(&hashStm, fd);
				resSize = hash->GetResultSize();
				hash->GetValue(chkResult);

				i = 0;
				while (i < resSize)
				{
					if (chkResult[i] != item->compInfo->checkBytes[i])
					{
						diff = true;
						break;
					}
					i++;
				}
			}

			DEL_CLASS(hash);
			DEL_CLASS(decomp);
			if (diff)
			{
				IO::Path::DeleteFile(sbuff);
				return 0;
			}
			else
			{
				IO::StmData::FileData *fd;
				Text::StringBuilderUTF8 sb;
				NEW_CLASS(fd, IO::StmData::FileData(CSTRP(sbuff, sptr), true));
				sb.Append(this->sourceName);
				sb.AppendC(UTF8STRC("\\"));
				sb.Append(item->name);
				fd->SetFullName(sb.ToCString());
				return fd;
			}
		}
		else
		{
			return 0;
		}
	}
	else
	{
		return 0;
	}
}

IO::PackageFile *IO::VirtualPackageFile::GetPItemPack(const PackFileItem *item, OutParam<Bool> needRelease) const
{
	if (item != 0)
	{
		if (item->itemType == PackFileItem::PackItemType::ParsedObject && item->pobj->GetParserType() == IO::ParserType::PackageFile)
		{
			needRelease.Set(false);
			return ((IO::PackageFile*)item->pobj);
		}
		return 0;
	}
	else
	{
		return 0;
	}
}

UOSInt IO::VirtualPackageFile::GetCount() const
{
	return this->items->GetCount();
}

IO::PackageFile::PackObjectType IO::VirtualPackageFile::GetItemType(UOSInt index) const
{
	IO::PackFileItem *item = this->items->GetItem(index);
	return GetPItemType(item);
}

UTF8Char *IO::VirtualPackageFile::GetItemName(UTF8Char *sbuff, UOSInt index) const
{
	IO::PackFileItem *item = this->items->GetItem(index);
	if (item == 0)
	{
		*sbuff = 0;
		return 0;
	}
	return item->name->ConcatTo(sbuff);
}

IO::StreamData *IO::VirtualPackageFile::GetItemStmDataNew(UOSInt index) const
{
	IO::PackFileItem *item = this->items->GetItem(index);
	return GetPItemStmDataNew(item);
}

IO::StreamData *IO::VirtualPackageFile::GetItemStmDataNew(const UTF8Char* name, UOSInt nameLen) const
{
	UOSInt index = GetItemIndex({name, nameLen});
	if (index == INVALID_INDEX)
	{
		return 0;
	}
	return this->GetItemStmDataNew(index);
}

IO::PackageFile *IO::VirtualPackageFile::GetItemPack(UOSInt index, OutParam<Bool> needRelease) const
{
	IO::PackFileItem *item = this->items->GetItem(index);
	return GetPItemPack(item, needRelease);
}

IO::ParsedObject *IO::VirtualPackageFile::GetItemPObj(UOSInt index, OutParam<Bool> needRelease) const
{
	IO::PackFileItem *item = this->items->GetItem(index);
	needRelease.Set(false);
	if (item != 0)
	{
		if (item->itemType == IO::PackFileItem::PackItemType::ParsedObject)
		{
			return item->pobj;
		}
		else
		{
			return 0;
		}
	}
	else
	{
		return 0;
	}
}

Data::Timestamp IO::VirtualPackageFile::GetItemModTime(UOSInt index) const
{
	IO::PackFileItem *item = this->items->GetItem(index);
	if (item != 0)
	{
		return item->modTime;
	}
	return Data::Timestamp(0);
}

Data::Timestamp IO::VirtualPackageFile::GetItemAccTime(UOSInt index) const
{
	IO::PackFileItem *item = this->items->GetItem(index);
	if (item != 0)
	{
		return item->accTime;
	}
	return Data::Timestamp(0);
}

Data::Timestamp IO::VirtualPackageFile::GetItemCreateTime(UOSInt index) const
{
	IO::PackFileItem *item = this->items->GetItem(index);
	if (item != 0)
	{
		return item->createTime;
	}
	return Data::Timestamp(0);
}

UInt32 IO::VirtualPackageFile::GetItemUnixAttr(UOSInt index) const
{
	IO::PackFileItem *item = this->items->GetItem(index);
	if (item != 0)
	{
		return item->unixAttr;
	}
	return 0;
}

UInt64 IO::VirtualPackageFile::GetItemStoreSize(UOSInt index) const
{
	IO::PackFileItem *item = this->items->GetItem(index);
	if (item != 0)
	{
		return item->fd->GetDataSize();
	}
	return 0;
}

UInt64 IO::VirtualPackageFile::GetItemSize(UOSInt index) const
{
	IO::PackFileItem *item = this->items->GetItem(index);
	if (item != 0)
	{
		if (item->itemType == IO::PackFileItem::PackItemType::Uncompressed)
		{
			return item->fd->GetDataSize();
		}
		else if (item->itemType == IO::PackFileItem::PackItemType::Compressed)
		{
			return item->compInfo->decSize;
		}
		else
		{
			return 0;
		}
	}
	return 0;
}

UOSInt IO::VirtualPackageFile::GetItemIndex(Text::CStringNN name) const
{
	UOSInt i;
	IO::PackFileItem *item;
	i = this->items->GetCount();
	while (i-- > 0)
	{
		item = this->items->GetItem(i);
		if (item)
		{
			if (item->name->EqualsICase(name))
				return i;
			if (item->itemType == IO::PackFileItem::PackItemType::Compressed || item->itemType == IO::PackFileItem::PackItemType::Uncompressed)
			{
				Text::CString shName = item->fd->GetShortName();
				if (shName.EqualsICase(name))
					return i;
			}
			else if (item->itemType == IO::PackFileItem::PackItemType::ParsedObject)
			{
				if (item->pobj->GetSourceNameObj()->EqualsICase(name))
					return i;
			}
		}
	}
	return INVALID_INDEX;
}

Bool IO::VirtualPackageFile::IsCompressed(UOSInt index) const
{
	IO::PackFileItem *item = this->items->GetItem(index);
	if (item != 0 && item->itemType == IO::PackFileItem::PackItemType::Compressed)
	{
		return true;
	}
	return false;
}

Data::Compress::Decompressor::CompressMethod IO::VirtualPackageFile::GetItemComp(UOSInt index) const
{
	IO::PackFileItem *item = this->items->GetItem(index);
	if (item != 0)
	{
		if (item->itemType == IO::PackFileItem::PackItemType::Compressed)
		{
			return item->compInfo->compMethod;
		}
		else if (item->itemType == IO::PackFileItem::PackItemType::Uncompressed)
		{
			return Data::Compress::Decompressor::CM_UNCOMPRESSED;
		}
		else
		{
			return Data::Compress::Decompressor::CM_UNKNOWN;
		}
	}
	return Data::Compress::Decompressor::CM_UNKNOWN;
}

NotNullPtr<IO::PackageFile> IO::VirtualPackageFile::Clone() const
{
	NotNullPtr<IO::PackageFile> pkg;
	NEW_CLASSNN(pkg, VirtualPackageFile(this));
	return pkg;
}

IO::PackageFileType IO::VirtualPackageFile::GetFileType() const
{
	return PackageFileType::Virtual;
}

Bool IO::VirtualPackageFile::CopyFrom(Text::CStringNN fileName, IO::ProgressHandler *progHdlr, OptOut<IO::ActiveStreamReader::BottleNeckType> bnt)
{
	IO::Path::PathType pt = IO::Path::GetPathType(fileName);
	if (pt == IO::Path::PathType::File)
	{
		UOSInt i = fileName.LastIndexOf(IO::Path::PATH_SEPERATOR);
		Text::CStringNN fName = fileName.Substring(i + 1);
		IO::StmData::FileData fd(fileName, false);
		if (fd.IsError())
		{
			return false;
		}
		Data::Timestamp modTime = 0;
		Data::Timestamp createTime = 0;
		Data::Timestamp accTime = 0;
		fd.GetFileStream()->GetFileTimes(createTime, accTime, modTime);
		UInt32 unixAttr = IO::Path::GetFileUnixAttr(fileName);
		return this->AddOrReplaceData(fd, 0, fd.GetDataSize(), fName, modTime, accTime, createTime, unixAttr);
	}
	else if (pt == IO::Path::PathType::Directory)
	{
		UOSInt i = fileName.LastIndexOf(IO::Path::PATH_SEPERATOR);
		Text::CStringNN fName = fileName.Substring(i + 1);
		NotNullPtr<IO::DirectoryPackage> dpkg;
		NEW_CLASSNN(dpkg, IO::DirectoryPackage(fileName));
		Data::Timestamp modTime = 0;
		Data::Timestamp createTime = 0;
		Data::Timestamp accTime = 0;
		IO::Path::GetFileTime(fileName, modTime, createTime, accTime);
		UInt32 unixAttr = IO::Path::GetFileUnixAttr(fileName);
		return this->AddOrReplacePack(dpkg, fName, modTime, accTime, createTime, unixAttr);
	}
	return false;
}

Bool IO::VirtualPackageFile::MoveFrom(Text::CStringNN fileName, IO::ProgressHandler *progHdlr, OptOut<IO::ActiveStreamReader::BottleNeckType> bnt)
{
	return false;
}

Bool IO::VirtualPackageFile::RetryCopyFrom(Text::CStringNN fileName, IO::ProgressHandler *progHdlr, OptOut<IO::ActiveStreamReader::BottleNeckType> bnt)
{
	return this->CopyFrom(fileName, progHdlr, bnt);
}

Bool IO::VirtualPackageFile::RetryMoveFrom(Text::CStringNN fileName, IO::ProgressHandler *progHdlr, OptOut<IO::ActiveStreamReader::BottleNeckType> bnt)
{
	return this->MoveFrom(fileName, progHdlr, bnt);
}

Bool IO::VirtualPackageFile::CopyTo(UOSInt index, Text::CString destPath, Bool fullFileName)
{
	IO::PackFileItem *item = this->items->GetItem(index);
	if (item != 0)
	{
		Text::StringBuilderUTF8 sb;
		Bool succ = true;
		sb.Append(destPath);
		if (sb.EndsWith(IO::Path::PATH_SEPERATOR))
		{
			sb.Append(item->name);
		}
		else if (fullFileName && (item->itemType == IO::PackFileItem::PackItemType::Compressed || item->itemType == IO::PackFileItem::PackItemType::Uncompressed))
		{
		}
		else
		{
			sb.AppendChar(IO::Path::PATH_SEPERATOR, 1);
			sb.Append(item->name);
		}
		if (item->itemType == IO::PackFileItem::PackItemType::Compressed)
		{
			if (IO::Path::GetPathType(sb.ToCString()) != IO::Path::PathType::Unknown)
				return false;

			Data::Compress::Decompressor *decomp = Data::Compress::Decompressor::CreateDecompressor(item->compInfo->compMethod);
			Crypto::Hash::IHash *hash;
			NotNullPtr<IO::StreamData> fd;
			if (decomp == 0 || !fd.Set(item->fd))
				return false;

			hash = Crypto::Hash::HashCreator::CreateHash(item->compInfo->checkMethod);
			if (hash == 0)
			{
				DEL_CLASS(decomp);
				return false;
			}
			UInt8 chkResult[32];
			UOSInt resSize;
			UOSInt i;
			Bool diff = false;
			{
				IO::FileStream fs(sb.ToCString(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::NoWriteBuffer);
				Crypto::Hash::HashStream hashStm(&fs, hash);

				hash->Clear();
				decomp->Decompress(&hashStm, fd);
				resSize = hash->GetResultSize();
				hash->GetValue(chkResult);

				i = 0;
				while (i < resSize)
				{
					if (chkResult[i] != item->compInfo->checkBytes[i])
					{
						diff = true;
						break;
					}
					i++;
				}
			}

			DEL_CLASS(hash);
			DEL_CLASS(decomp);
			if (diff)
			{
				IO::Path::DeleteFile(sb.ToString());
				succ = false;
			}

			return succ;
		}
		else if (item->itemType == IO::PackFileItem::PackItemType::Uncompressed)
		{
			UInt64 fileSize = item->fd->GetDataSize();
			UOSInt readSize;
			if (IO::Path::GetPathType(sb.ToCString()) != IO::Path::PathType::Unknown)
				return false;

			if (fileSize < 1048576)
			{
				Data::ByteBuffer tmpBuff((UOSInt)fileSize);
				succ = (item->fd->GetRealData(0, (UOSInt)fileSize, tmpBuff) == fileSize);
				if (succ)
				{
					IO::FileStream fs(sb.ToCString(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::NoWriteBuffer);
					succ = (fs.Write(tmpBuff.Ptr(), (UOSInt)fileSize) == fileSize);
				}
			}
			else
			{
				Data::ByteBuffer tmpBuff(1048576);
				UInt64 currOfst = 0;
				{
					IO::FileStream fs(sb.ToCString(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::NoWriteBuffer);
					while (currOfst < fileSize)
					{
						if ((fileSize - currOfst) >= 1048576)
						{
							readSize = 1048576;
						}
						else
						{
							readSize = (UOSInt)(fileSize - currOfst);
						}
						if (item->fd->GetRealData(currOfst, readSize, tmpBuff) != readSize)
						{
							succ = false;
							break;
						}
						if (fs.Write(tmpBuff.Ptr(), readSize) != readSize)
						{
							succ = false;
							break;
						}
						currOfst += readSize;
					}
				}

				if (!succ)
				{
					IO::Path::DeleteFile(sb.ToString());
				}
			}

			return succ;
		}
		else if (item->itemType == IO::PackFileItem::PackItemType::ParsedObject && item->pobj->GetParserType() == IO::ParserType::PackageFile)
		{
			IO::PackageFile *pf = (IO::PackageFile*)item->pobj;
			IO::Path::CreateDirectory(sb.ToCString());
			UOSInt i = 0;
			UOSInt j = pf->GetCount();
			while (i < j)
			{
				succ = pf->CopyTo(i, sb.ToCString(), false);
				if (!succ)
					break;
				i++;
			}
			return succ;
		}
		else
		{
			return false;
		}
	}
	return false;
}

IO::StreamData *IO::VirtualPackageFile::OpenStreamData(Text::CString fileName) const
{
	if (fileName.IndexOf(':') != INVALID_INDEX)
	{
		return 0;
	}

	IO::StreamData *retFD = 0;
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	UOSInt i;
	UOSInt j;
	Text::StringBuilderUTF8 sb;
	sb.Append(fileName);
	sb.Replace('\\', '/');
	const IO::PackageFile *pf;
	IO::PackageFile *pf2;
	Bool needRel = false;
	Bool needRel2 = false;
	Bool found;

	pf = this;
	j = sb.IndexOf('/');
	while (j != INVALID_INDEX)
	{
		sb.v[j] = 0;
		found = false;
		i = pf->GetCount();
		while (i-- > 0)
		{
			if (pf->GetItemType(i) == IO::PackageFile::PackObjectType::PackageFileType)
			{
				sbuff[0] = 0;
				sptr = pf->GetItemName(sbuff, i);
				if (Text::StrEqualsC(sb.ToString(), j, sbuff, (UOSInt)(sptr - sbuff)))
				{
					pf2 = pf->GetItemPack(i, needRel2);
					if (pf2)
					{
						found = true;
						if (needRel)
						{
							DEL_CLASS(pf);
						}
						sb.SetSubstr((UOSInt)j + 1);
						pf = pf2;
						needRel = needRel2;
						break;
					}
				}
			}
		}
		if (!found)
		{
			if (needRel)
			{
				DEL_CLASS(pf);
			}
			return 0;
		}
		j = sb.IndexOf('/');
	}

	i = pf->GetCount();
	while (i-- > 0)
	{
		if (pf->GetItemType(i) == IO::PackageFile::PackObjectType::StreamData)
		{
			sbuff[0] = 0;
			sptr = pf->GetItemName(sbuff, i);
			if (sb.Equals(sbuff, (UOSInt)(sptr - sbuff)))
			{
				retFD = pf->GetItemStmDataNew(i);
				break;
			}
		}
	}

	if (needRel)
	{
		DEL_CLASS(pf);
	}
	return retFD;
}

Bool IO::VirtualPackageFile::HasParent() const
{
	return this->parent != 0;
}

IO::PackageFile *IO::VirtualPackageFile::GetParent(OutParam<Bool> needRelease) const
{
	needRelease.Set(false);
	return this->parent;
}

Bool IO::VirtualPackageFile::DeleteItem(UOSInt index)
{
	IO::PackFileItem *item = this->items->RemoveAt(index);
	if (item)
	{
		if (item->itemType == IO::PackFileItem::PackItemType::ParsedObject && item->pobj->GetParserType() == IO::ParserType::PackageFile)
		{
			this->pkgFiles.RemoveNN(item->name);
		}
		this->namedItems->RemoveNN(item->name);
		ReusePackFileItem(item);
		MemFree(item);
		return true;
	}
	return false;
}

void IO::VirtualPackageFile::SetParent(IO::PackageFile *pkg)
{
	this->parent = pkg;
	PackFileItem *item;
	UOSInt i = 0;
	UOSInt j = this->items->GetCount();
	while (i < j)
	{
		item = this->items->GetItem(i);
		if (item->itemType == IO::PackFileItem::PackItemType::ParsedObject && item->pobj->GetParserType() == IO::ParserType::PackageFile)
		{
			((IO::PackageFile*)item->pobj)->SetParent(this);
		}
		i++;
	}
}

void IO::VirtualPackageFile::SetInfo(InfoType infoType, const UTF8Char *val)
{
	const UTF8Char *csptr = this->infoMap.Put(infoType, Text::StrCopyNew(val).Ptr());
	if (csptr)
	{
		Text::StrDelNew(csptr);
	}
}

void IO::VirtualPackageFile::GetInfoText(NotNullPtr<Text::StringBuilderUTF8> sb) const
{
	UOSInt i;
	UOSInt j;
	i = 0;
	j = this->infoMap.GetCount();
	while (i < j)
	{
		if (i > 0)
		{
			sb->AppendC(UTF8STRC("\r\n"));
		}
		sb->Append(GetInfoTypeName((InfoType)this->infoMap.GetKey(i)));
		sb->AppendC(UTF8STRC(": "));
		sb->AppendSlow(this->infoMap.GetItem(i));
		i++;
	}
}

Text::CString IO::VirtualPackageFile::GetInfoTypeName(InfoType infoType)
{
	switch(infoType)
	{
	case IT_SYSTEM_ID:
		return CSTR("System");
	case IT_VOLUME_ID:
		return CSTR("Volume");
	case IT_VOLUME_SET_ID:
		return CSTR("Volume Set");
	case IT_PUBLISHER_ID:
		return CSTR("Publisher");
	case IT_DATA_PREPARER_ID:
		return CSTR("Data Preparer");
	case IT_APPLICATION_ID:
		return CSTR("Application ID");
	case IT_COPYRIGHT_FILE_ID:
		return CSTR("Copyright File");
	case IT_ABSTRACT_FILE_ID:
		return CSTR("Abstract File");
	case IT_BIBLIOGRAHPICAL_FILE_ID:
		return CSTR("Bibliographical File");
	default:
		return CSTR("Unknown");
	}
}