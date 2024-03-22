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

void IO::VirtualPackageFile::ReusePackFileItem(NotNullPtr<IO::PackFileItem> item)
{
	SDEL_CLASS(item->fullFd);
	item->pobj.Delete();
	if (item->compInfo)
	{
		if (item->compInfo->compExtras)
		{
			MemFree(item->compInfo->compExtras);
		}
		MemFree(item->compInfo);
	}
}

IO::VirtualPackageFile::VirtualPackageFile(NotNullPtr<Text::String> fileName) : IO::PackageFile(fileName)
{
	this->parent = 0;
}

IO::VirtualPackageFile::VirtualPackageFile(Text::CStringNN fileName) : IO::PackageFile(fileName)
{
	this->parent = 0;
}

IO::VirtualPackageFile::~VirtualPackageFile()
{
	NotNullPtr<PackFileItem> item;
	Data::ArrayIterator<NotNullPtr<PackFileItem>> it = this->items.Iterator();
	while (it.HasNext())
	{
		item = it.Next();
		if (Sync::Interlocked::DecrementI32(item->useCnt) == 0)
		{
			item->name->Release();
			SDEL_CLASS(item->fullFd);
			item->pobj.Delete();
			if (item->compInfo)
			{
				if (item->compInfo->compExtras)
				{
					MemFree(item->compInfo->compExtras);
				}
				MemFree(item->compInfo);
			}
			MemFreeNN(item);
		}
	}
	UOSInt i = this->infoMap.GetCount();
	while (i-- > 0)
	{
		Text::StrDelNew(this->infoMap.GetItem(i));
	}
}

Bool IO::VirtualPackageFile::AddData(NotNullPtr<IO::StreamData> fd, UInt64 ofst, UInt64 dataLength, PackFileItem::HeaderType headerType, Text::CStringNN name, const Data::Timestamp &modTime, const Data::Timestamp &accTime, const Data::Timestamp &createTime, UInt32 unixAttr)
{
	NotNullPtr<PackFileItem> item;
	item = MemAllocNN(PackFileItem, 1);
	item->itemType = IO::PackFileItem::PackItemType::Uncompressed;
	item->fileOfst = ofst;
	item->dataLength = dataLength;
	item->headerType = headerType;
	item->fullFd = fd->GetPartialData(0, fd->GetDataSize()).Ptr();
	item->name = Text::String::New(name);
	item->pobj = 0;
	item->compInfo = 0;
	item->modTime = modTime;
	item->accTime = accTime;
	item->createTime = createTime;
	item->unixAttr = unixAttr;
	item->useCnt = 1;
	this->items.Add(item);
	this->PutItem(item->name, item);
	return true;
}

Bool IO::VirtualPackageFile::AddObject(IO::ParsedObject *pobj, Text::CStringNN name, const Data::Timestamp &modTime, const Data::Timestamp &accTime, const Data::Timestamp &createTime, UInt32 unixAttr)
{
	NotNullPtr<PackFileItem> item;
	item = MemAllocNN(PackFileItem, 1);
	item->itemType = IO::PackFileItem::PackItemType::ParsedObject;
	item->dataLength = 0;
	item->fileOfst = 0;
	item->headerType = PackFileItem::HeaderType::No;
	item->fullFd = 0;
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
	this->items.Add(item);
	this->PutItem(item->name, item);
	return true;
}

Bool IO::VirtualPackageFile::AddCompData(NotNullPtr<IO::StreamData> fd, UInt64 ofst, UInt64 dataLength, PackFileItem::HeaderType headerType, IO::PackFileItem::CompressInfo *compInfo, Text::CStringNN name, const Data::Timestamp &modTime, const Data::Timestamp &accTime, const Data::Timestamp &createTime, UInt32 unixAttr)
{
	NotNullPtr<PackFileItem> item;
	item = MemAllocNN(PackFileItem, 1);
	item->itemType = IO::PackFileItem::PackItemType::Compressed;
	item->headerType = headerType;
	item->fileOfst = ofst;
	item->dataLength = dataLength;
	item->fullFd = fd->GetPartialData(0, fd->GetDataSize()).Ptr();
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
	this->items.Add(item);
	this->PutItem(item->name, item);
	return true;
}

Bool IO::VirtualPackageFile::AddPack(NotNullPtr<IO::PackageFile> pkg, Text::CStringNN name, const Data::Timestamp &modTime, const Data::Timestamp &accTime, const Data::Timestamp &createTime, UInt32 unixAttr)
{
	NotNullPtr<PackFileItem> item;
	item = MemAllocNN(PackFileItem, 1);
	item->itemType = IO::PackFileItem::PackItemType::ParsedObject;
	item->fileOfst = 0;
	item->dataLength = 0;
	item->headerType = PackFileItem::HeaderType::No;
	item->fullFd = 0;
	item->name = Text::String::New(name);
	item->pobj = pkg.Ptr();
	item->compInfo = 0;
	item->modTime = modTime;
	item->accTime = accTime;
	item->createTime = createTime;
	item->unixAttr = unixAttr;
	item->useCnt = 1;
	pkg->SetParent(this);
	this->items.Add(item);
	this->pkgFiles.PutNN(item->name, item.Ptr());
	this->PutItem(item->name, item);
	return true;
}

Bool IO::VirtualPackageFile::AddOrReplaceData(NotNullPtr<StreamData> fd, UInt64 ofst, UInt64 dataLength, PackFileItem::HeaderType headerType, Text::CStringNN name, const Data::Timestamp &modTime, const Data::Timestamp &accTime, const Data::Timestamp &createTime, UInt32 unixAttr)
{
	NotNullPtr<PackFileItem> item;
	UOSInt i = GetItemIndex(name);
	if (i != INVALID_INDEX)
	{
		if (this->items.GetItem(i).SetTo(item) && (item->itemType == IO::PackFileItem::PackItemType::Uncompressed || item->itemType == IO::PackFileItem::PackItemType::Compressed))
		{
			ReusePackFileItem(item);
			item->itemType = IO::PackFileItem::PackItemType::Uncompressed;
			item->fileOfst = ofst;
			item->dataLength = dataLength;
			item->headerType = headerType;
			item->fullFd = fd->GetPartialData(0, fd->GetDataSize()).Ptr();
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
	return this->AddData(fd, ofst, dataLength, headerType, name, modTime, accTime, createTime, unixAttr);
}

Bool IO::VirtualPackageFile::AddOrReplaceObject(IO::ParsedObject *pobj, Text::CStringNN name, const Data::Timestamp &modTime, const Data::Timestamp &accTime, const Data::Timestamp &createTime, UInt32 unixAttr)
{
	NotNullPtr<PackFileItem> item;
	UOSInt i = GetItemIndex(name);
	if (i != INVALID_INDEX)
	{
		if (this->items.GetItem(i).SetTo(item) && (item->itemType == IO::PackFileItem::PackItemType::Uncompressed || item->itemType == IO::PackFileItem::PackItemType::Compressed))
		{
			ReusePackFileItem(item);
			item->itemType = IO::PackFileItem::PackItemType::ParsedObject;
			item->fileOfst = 0;
			item->dataLength = 0;
			item->headerType = PackFileItem::HeaderType::No;
			item->fullFd = 0;
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

Bool IO::VirtualPackageFile::AddOrReplaceCompData(NotNullPtr<StreamData> fd, UInt64 ofst, UInt64 dataLength, PackFileItem::HeaderType headerType, PackFileItem::CompressInfo *compInfo, Text::CStringNN name, const Data::Timestamp &modTime, const Data::Timestamp &accTime, const Data::Timestamp &createTime, UInt32 unixAttr)
{
	NotNullPtr<PackFileItem> item;
	UOSInt i = GetItemIndex(name);
	if (i != INVALID_INDEX)
	{
		if (this->items.GetItem(i).SetTo(item) && (item->itemType == IO::PackFileItem::PackItemType::Uncompressed || item->itemType == IO::PackFileItem::PackItemType::Compressed))
		{
			ReusePackFileItem(item);
			item->itemType = IO::PackFileItem::PackItemType::Compressed;
			item->fileOfst = ofst;
			item->dataLength = dataLength;
			item->headerType = headerType;
			item->fullFd = fd->GetPartialData(0, fd->GetDataSize()).Ptr();
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
	return this->AddCompData(fd, ofst, dataLength, headerType, compInfo, name, modTime, accTime, createTime, unixAttr);
}

Bool IO::VirtualPackageFile::AddOrReplacePack(NotNullPtr<IO::PackageFile> pkg, Text::CStringNN name, const Data::Timestamp &modTime, const Data::Timestamp &accTime, const Data::Timestamp &createTime, UInt32 unixAttr)
{
	NotNullPtr<PackFileItem> item;
	NotNullPtr<IO::ParsedObject> pobj;
	UOSInt i = GetItemIndex(name);
	if (i != INVALID_INDEX)
	{
		if (this->items.GetItem(i).SetTo(item) && item->itemType == IO::PackFileItem::PackItemType::ParsedObject && item->pobj.SetTo(pobj) && pobj->GetParserType() == IO::ParserType::PackageFile)
		{
			NotNullPtr<IO::PackageFile> myPkg = NotNullPtr<IO::PackageFile>::ConvertFrom(pobj);
			if (myPkg->GetFileType() == IO::PackageFileType::Virtual)
			{
				item->modTime = modTime;
				item->accTime = accTime;
				item->createTime = createTime;
				item->unixAttr = unixAttr;
				Bool succ = NotNullPtr<IO::VirtualPackageFile>::ConvertFrom(myPkg)->MergePackage(pkg);
				pkg.Delete();
				return succ;
			}
		}
		pkg.Delete();
		return false;
	}
	return this->AddPack(pkg, name, modTime, accTime, createTime, unixAttr);
}

Optional<IO::PackageFile> IO::VirtualPackageFile::GetPackFile(Text::CStringNN name) const
{
	IO::PackFileItem *item = this->pkgFiles.GetC(name);
	if (item)
		return Optional<IO::PackageFile>::ConvertFrom(item->pobj);
	return 0;
}

Bool IO::VirtualPackageFile::MergePackage(NotNullPtr<IO::PackageFile> pkg)
{
	if (pkg->GetFileType() == IO::PackageFileType::Virtual)
	{
		NotNullPtr<IO::VirtualPackageFile> vpkg = NotNullPtr<IO::VirtualPackageFile>::ConvertFrom(pkg);
		NotNullPtr<const IO::PackFileItem> item;
		NotNullPtr<IO::StreamData> fd;
		NotNullPtr<IO::ParsedObject> pobj;
		Data::ArrayIterator<NotNullPtr<PackFileItem>> it = vpkg->PackFileIterator();
		while (it.HasNext())
		{
			item = it.Next();
			switch (item->itemType)
			{
			case IO::PackFileItem::PackItemType::Uncompressed:
				if (!fd.Set(item->fullFd))
					return false;
				if (!this->AddOrReplaceData(fd, item->fileOfst, item->dataLength, item->headerType, item->name->ToCString(), item->modTime, item->accTime, item->createTime, item->unixAttr))
					return false;
				break;
			case IO::PackFileItem::PackItemType::Compressed:
				if (!fd.Set(item->fullFd))
					return false;
				if (!this->AddOrReplaceCompData(fd, item->fileOfst, item->dataLength, item->headerType, item->compInfo, item->name->ToCString(), item->modTime, item->accTime, item->createTime, item->unixAttr))
					return false;
				break;
			case IO::PackFileItem::PackItemType::ParsedObject:
				if (!item->pobj.SetTo(pobj))
					return false;
				if (pobj->GetParserType() != IO::ParserType::PackageFile)
					return false;
				if (!this->AddOrReplacePack(NotNullPtr<IO::PackageFile>::ConvertFrom(pobj)->Clone(), item->name->ToCString(), item->modTime, item->accTime, item->createTime, item->unixAttr))
					return false;
				break;
			default:
				return false;
			}
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
				if (!pkg->GetItemPack(i, needRelease).SetTo(ipkg))
					return false;
				if (!needRelease)
					ipkg = ipkg->Clone();
				if (!this->AddOrReplacePack(ipkg, CSTRP(sbuff, sptr), pkg->GetItemModTime(i), pkg->GetItemAccTime(i), pkg->GetItemCreateTime(i), pkg->GetItemUnixAttr(i)))
					return false;
				break;
			case IO::PackageFile::PackObjectType::StreamData:
				if (!pkg->GetItemStmDataNew(i).SetTo(fd))
					return false;
				if (!this->AddOrReplaceData(fd, 0, fd->GetDataSize(), PackFileItem::HeaderType::No, CSTRP(sbuff, sptr), pkg->GetItemModTime(i), pkg->GetItemAccTime(i), pkg->GetItemCreateTime(i), pkg->GetItemUnixAttr(i)))
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

Optional<const IO::PackFileItem> IO::VirtualPackageFile::GetPackFileItem(const UTF8Char *name) const
{
	UTF8Char sbuff[256];
	const UTF8Char *sptr;
	UOSInt nameLen;
	NotNullPtr<IO::ParsedObject> pobj;
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
			return this->GetItemByName({name, nameLen});
		}
		else if (c == '/' || c == '\\')
		{
			nameLen = (UOSInt)(sptr - name - 1);
			Text::StrConcatC(sbuff, name, nameLen);
			NotNullPtr<const IO::PackFileItem> item;
			if (!this->GetItemByName({sbuff, nameLen}).SetTo(item))
			{
				return 0;
			}
			if (sptr[0] == 0)
				return item;
			if (item->itemType != IO::PackFileItem::PackItemType::ParsedObject || !item->pobj.SetTo(pobj))
			{
				return 0;
			}
			if (pobj->GetParserType() != IO::ParserType::PackageFile)
			{
				return 0;
			}
			NotNullPtr<IO::PackageFile> pf = NotNullPtr<IO::PackageFile>::ConvertFrom(pobj);
			if (pf->GetFileType() != PackageFileType::Virtual)
			{
				return 0;
			}
			return NotNullPtr<IO::VirtualPackageFile>::ConvertFrom(pf)->GetPackFileItem(sptr);
		}
	}
	return 0;
}

Optional<const IO::PackFileItem> IO::VirtualPackageFile::GetPackFileItem(UOSInt index) const
{
	return this->items.GetItem(index);
}

IO::PackageFile::PackObjectType IO::VirtualPackageFile::GetPItemType(NotNullPtr<const PackFileItem> itemObj) const
{
	NotNullPtr<IO::ParsedObject> pobj;
	if (itemObj->itemType == IO::PackFileItem::PackItemType::Compressed || itemObj->itemType == IO::PackFileItem::PackItemType::Uncompressed)
	{
		return IO::PackageFile::PackObjectType::StreamData;
	}
	else if (itemObj->itemType == IO::PackFileItem::PackItemType::ParsedObject && itemObj->pobj.SetTo(pobj))
	{
		if (pobj->GetParserType() == IO::ParserType::PackageFile)
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

UInt64 IO::VirtualPackageFile::GetPItemDataOfst(NotNullPtr<const PackFileItem> itemObj) const
{
	UInt8 hdrBuff[30];
	if (itemObj->itemType == PackFileItem::PackItemType::Uncompressed || itemObj->itemType == PackFileItem::PackItemType::Compressed)
	{
		if (itemObj->headerType == PackFileItem::HeaderType::Zip)
		{
			if (itemObj->fullFd->GetRealData(itemObj->fileOfst, 30, BYTEARR(hdrBuff)) != 30)
				return itemObj->fileOfst;
			UInt16 fnameLen = ReadUInt16(&hdrBuff[26]);
			UInt16 extraLen = ReadUInt16(&hdrBuff[28]);
			return itemObj->fileOfst + 30 + fnameLen + extraLen;
		}
		return itemObj->fileOfst;
	}
	return itemObj->fileOfst;
}

Optional<IO::StreamData> IO::VirtualPackageFile::GetPItemStmDataNew(NotNullPtr<const PackFileItem> item) const
{
	if (item->itemType == IO::PackFileItem::PackItemType::Uncompressed)
	{
		NotNullPtr<IO::StreamData> data;
		data = item->fullFd->GetPartialData(GetPItemDataOfst(item), item->dataLength);
		Text::StringBuilderUTF8 sb;
		sb.Append(this->sourceName);
		sb.AppendC(UTF8STRC("\\"));
		sb.Append(item->name);
		data->SetFullName(sb.ToCString());
		return data;
	}
	else if (item->itemType == IO::PackFileItem::PackItemType::Compressed)
	{
		Data::Compress::Decompressor *decomp = Data::Compress::Decompressor::CreateDecompressor(item->compInfo->compMethod);
		Crypto::Hash::IHash *hash;
		NotNullPtr<IO::StreamData> fd;
		if (decomp == 0)
			return 0;
		hash = Crypto::Hash::HashCreator::CreateHash(item->compInfo->checkMethod);
		if (hash == 0)
		{
			DEL_CLASS(decomp);
			return 0;
		}
		fd = item->fullFd->GetPartialData(GetPItemDataOfst(item), item->dataLength);
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
			Crypto::Hash::HashStream hashStm(fs, hash);

			hash->Clear();
			decomp->Decompress(hashStm, fd);
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
		fd.Delete();
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

Optional<IO::PackageFile> IO::VirtualPackageFile::GetPItemPack(NotNullPtr<const PackFileItem> item, OutParam<Bool> needRelease) const
{
	NotNullPtr<IO::ParsedObject> pobj;
	if (item->itemType == PackFileItem::PackItemType::ParsedObject && item->pobj.SetTo(pobj) && pobj->GetParserType() == IO::ParserType::PackageFile)
	{
		needRelease.Set(false);
		return Optional<IO::PackageFile>::ConvertFrom(item->pobj);
	}
	return 0;
}

Data::ArrayIterator<NotNullPtr<IO::PackFileItem>> IO::VirtualPackageFile::PackFileIterator() const
{
	return this->items.Iterator();
}

UOSInt IO::VirtualPackageFile::GetCount() const
{
	return this->items.GetCount();
}

IO::PackageFile::PackObjectType IO::VirtualPackageFile::GetItemType(UOSInt index) const
{
	NotNullPtr<IO::PackFileItem> item;
	if (this->items.GetItem(index).SetTo(item))
		return GetPItemType(item);
	return PackObjectType::Unknown;
}

UTF8Char *IO::VirtualPackageFile::GetItemName(UTF8Char *sbuff, UOSInt index) const
{
	NotNullPtr<IO::PackFileItem> item;
	if (!this->items.GetItem(index).SetTo(item))
	{
		*sbuff = 0;
		return 0;
	}
	return item->name->ConcatTo(sbuff);
}

Optional<IO::StreamData> IO::VirtualPackageFile::GetItemStmDataNew(UOSInt index) const
{
	NotNullPtr<IO::PackFileItem> item;
	if (this->items.GetItem(index).SetTo(item))
		return GetPItemStmDataNew(item);
	return 0;
}

Optional<IO::StreamData> IO::VirtualPackageFile::GetItemStmDataNew(Text::CStringNN name) const
{
	UOSInt index = GetItemIndex(name);
	if (index == INVALID_INDEX)
	{
		return 0;
	}
	return this->GetItemStmDataNew(index);
}

Optional<IO::PackageFile> IO::VirtualPackageFile::GetItemPack(UOSInt index, OutParam<Bool> needRelease) const
{
	NotNullPtr<IO::PackFileItem> item;
	if (this->items.GetItem(index).SetTo(item))
		return GetPItemPack(item, needRelease);
	return 0;
}

Optional<IO::ParsedObject> IO::VirtualPackageFile::GetItemPObj(UOSInt index, OutParam<Bool> needRelease) const
{
	NotNullPtr<IO::PackFileItem> item;
	needRelease.Set(false);
	if (this->items.GetItem(index).SetTo(item))
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
	NotNullPtr<IO::PackFileItem> item;
	if (this->items.GetItem(index).SetTo(item))
	{
		return item->modTime;
	}
	return Data::Timestamp(0);
}

Data::Timestamp IO::VirtualPackageFile::GetItemAccTime(UOSInt index) const
{
	NotNullPtr<IO::PackFileItem> item;
	if (this->items.GetItem(index).SetTo(item))
	{
		return item->accTime;
	}
	return Data::Timestamp(0);
}

Data::Timestamp IO::VirtualPackageFile::GetItemCreateTime(UOSInt index) const
{
	NotNullPtr<IO::PackFileItem> item;
	if (this->items.GetItem(index).SetTo(item))
	{
		return item->createTime;
	}
	return Data::Timestamp(0);
}

UInt32 IO::VirtualPackageFile::GetItemUnixAttr(UOSInt index) const
{
	NotNullPtr<IO::PackFileItem> item;
	if (this->items.GetItem(index).SetTo(item))
	{
		return item->unixAttr;
	}
	return 0;
}

UInt64 IO::VirtualPackageFile::GetItemStoreSize(UOSInt index) const
{
	NotNullPtr<IO::PackFileItem> item;
	if (this->items.GetItem(index).SetTo(item))
	{
		return item->dataLength;
	}
	return 0;
}

UInt64 IO::VirtualPackageFile::GetItemSize(UOSInt index) const
{
	NotNullPtr<IO::PackFileItem> item;
	if (this->items.GetItem(index).SetTo(item))
	{
		if (item->itemType == IO::PackFileItem::PackItemType::Uncompressed)
		{
			return item->dataLength;
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
	NotNullPtr<IO::PackFileItem> item;
	NotNullPtr<IO::ParsedObject> pobj;
	Data::ArrayIterator<NotNullPtr<PackFileItem>> it = this->items.Iterator();
	i = 0;
	while (it.HasNext())
	{
		item = it.Next();
		if (item->name->EqualsICase(name))
			return i;
		if (item->itemType == IO::PackFileItem::PackItemType::Compressed || item->itemType == IO::PackFileItem::PackItemType::Uncompressed)
		{
			Text::CString shName = item->fullFd->GetShortName();
			if (shName.EqualsICase(name))
				return i;
		}
		else if (item->itemType == IO::PackFileItem::PackItemType::ParsedObject)
		{
			if (item->pobj.SetTo(pobj) && pobj->GetSourceNameObj()->EqualsICase(name))
				return i;
		}
		i++;
	}
	return INVALID_INDEX;
}

Bool IO::VirtualPackageFile::IsCompressed(UOSInt index) const
{
	NotNullPtr<IO::PackFileItem> item;
	if (this->items.GetItem(index).SetTo(item) && item->itemType == IO::PackFileItem::PackItemType::Compressed)
	{
		return true;
	}
	return false;
}

Data::Compress::Decompressor::CompressMethod IO::VirtualPackageFile::GetItemComp(UOSInt index) const
{
	NotNullPtr<IO::PackFileItem> item;
	if (this->items.GetItem(index).SetTo(item))
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

IO::PackageFileType IO::VirtualPackageFile::GetFileType() const
{
	return PackageFileType::Virtual;
}

Bool IO::VirtualPackageFile::CopyFrom(Text::CStringNN fileName, Optional<IO::ProgressHandler> progHdlr, OptOut<IO::ActiveStreamReader::BottleNeckType> bnt)
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
		return this->AddOrReplaceData(fd, 0, fd.GetDataSize(), PackFileItem::HeaderType::No, fName, modTime, accTime, createTime, unixAttr);
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

Bool IO::VirtualPackageFile::MoveFrom(Text::CStringNN fileName, Optional<IO::ProgressHandler> progHdlr, OptOut<IO::ActiveStreamReader::BottleNeckType> bnt)
{
	return false;
}

Bool IO::VirtualPackageFile::RetryCopyFrom(Text::CStringNN fileName, Optional<IO::ProgressHandler> progHdlr, OptOut<IO::ActiveStreamReader::BottleNeckType> bnt)
{
	return this->CopyFrom(fileName, progHdlr, bnt);
}

Bool IO::VirtualPackageFile::RetryMoveFrom(Text::CStringNN fileName, Optional<IO::ProgressHandler> progHdlr, OptOut<IO::ActiveStreamReader::BottleNeckType> bnt)
{
	return this->MoveFrom(fileName, progHdlr, bnt);
}

Bool IO::VirtualPackageFile::CopyTo(UOSInt index, Text::CString destPath, Bool fullFileName)
{
	NotNullPtr<IO::PackFileItem> item;
	NotNullPtr<IO::ParsedObject> pobj;
	if (this->items.GetItem(index).SetTo(item))
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
			if (decomp == 0)
				return false;

			hash = Crypto::Hash::HashCreator::CreateHash(item->compInfo->checkMethod);
			if (hash == 0)
			{
				DEL_CLASS(decomp);
				return false;
			}
			fd = item->fullFd->GetPartialData(GetPItemDataOfst(item), item->dataLength);

			UInt8 chkResult[32];
			UOSInt resSize;
			UOSInt i;
			Bool diff = false;
			{
				IO::FileStream fs(sb.ToCString(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::NoWriteBuffer);
				Crypto::Hash::HashStream hashStm(fs, hash);

				hash->Clear();
				decomp->Decompress(hashStm, fd);
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

			fd.Delete();
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
			UInt64 fileSize = item->dataLength;
			UOSInt readSize;
			if (IO::Path::GetPathType(sb.ToCString()) != IO::Path::PathType::Unknown)
				return false;

			UInt64 currOfst = GetPItemDataOfst(item);
			if (fileSize < 1048576)
			{
				Data::ByteBuffer tmpBuff((UOSInt)fileSize);
				succ = (item->fullFd->GetRealData(currOfst, (UOSInt)fileSize, tmpBuff) == fileSize);
				if (succ)
				{
					IO::FileStream fs(sb.ToCString(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::NoWriteBuffer);
					succ = (fs.Write(tmpBuff.Ptr(), (UOSInt)fileSize) == fileSize);
				}
			}
			else
			{
				Data::ByteBuffer tmpBuff(1048576);
				{
					IO::FileStream fs(sb.ToCString(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::NoWriteBuffer);
					UInt64 endOfst = fileSize + currOfst;
					while (currOfst < endOfst)
					{
						if ((endOfst - currOfst) >= 1048576)
						{
							readSize = 1048576;
						}
						else
						{
							readSize = (UOSInt)(endOfst - currOfst);
						}
						if (item->fullFd->GetRealData(currOfst, readSize, tmpBuff) != readSize)
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
		else if (item->itemType == IO::PackFileItem::PackItemType::ParsedObject && item->pobj.SetTo(pobj) && pobj->GetParserType() == IO::ParserType::PackageFile)
		{
			NotNullPtr<IO::PackageFile> pf = NotNullPtr<IO::PackageFile>::ConvertFrom(pobj);
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

Optional<IO::StreamData> IO::VirtualPackageFile::OpenStreamData(Text::CString fileName) const
{
	if (fileName.IndexOf(':') != INVALID_INDEX)
	{
		return 0;
	}

	Optional<IO::StreamData> retFD = 0;
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	UOSInt i;
	UOSInt j;
	Text::StringBuilderUTF8 sb;
	sb.Append(fileName);
	sb.Replace('\\', '/');
	const IO::PackageFile *pf;
	NotNullPtr<IO::PackageFile> pf2;
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
					if (pf->GetItemPack(i, needRel2).SetTo(pf2))
					{
						found = true;
						if (needRel)
						{
							DEL_CLASS(pf);
						}
						sb.SetSubstr((UOSInt)j + 1);
						pf = pf2.Ptr();
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
	return !this->parent.IsNull();
}

Optional<IO::PackageFile> IO::VirtualPackageFile::GetParent(OutParam<Bool> needRelease) const
{
	needRelease.Set(false);
	return this->parent;
}

Bool IO::VirtualPackageFile::DeleteItem(UOSInt index)
{
	NotNullPtr<IO::PackFileItem> item;
	NotNullPtr<IO::ParsedObject> pobj;
	if (this->items.RemoveAt(index).SetTo(item))
	{
		if (item->itemType == IO::PackFileItem::PackItemType::ParsedObject && item->pobj.SetTo(pobj) && pobj->GetParserType() == IO::ParserType::PackageFile)
		{
			this->pkgFiles.RemoveNN(item->name);
		}
		this->RemoveItem(item->name);
		ReusePackFileItem(item);
		MemFreeNN(item);
		return true;
	}
	return false;
}

void IO::VirtualPackageFile::SetParent(Optional<IO::PackageFile> pkg)
{
	this->parent = pkg;
	NotNullPtr<PackFileItem> item;
	NotNullPtr<IO::ParsedObject> pobj;
	Data::ArrayIterator<NotNullPtr<PackFileItem>> it = this->items.Iterator();
	while (it.HasNext())
	{
		item = it.Next();
		if (item->itemType == IO::PackFileItem::PackItemType::ParsedObject && item->pobj.SetTo(pobj) && pobj->GetParserType() == IO::ParserType::PackageFile)
		{
			NotNullPtr<IO::PackageFile>::ConvertFrom(pobj)->SetParent(this);
		}
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
