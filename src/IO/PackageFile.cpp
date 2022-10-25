#include "Stdafx.h"
#include "MyMemory.h"
#include "Crypto/Hash/HashCreator.h"
#include "Crypto/Hash/HashStream.h"
#include "Data/ByteTool.h"
#include "Data/ICaseStringMap.h"
#include "IO/FileStream.h"
#include "IO/IStreamData.h"
#include "IO/PackageFile.h"
#include "IO/Path.h"
#include "IO/StmData/FileData.h"
#include "Sync/Event.h"
#include "Sync/Interlocked.h"
#include "Sync/Mutex.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"

IO::PackageFile::PackageFile(const PackageFile *pkg) : IO::ParsedObject(pkg->GetSourceNameObj()), pkgFiles(pkg->pkgFiles)
{
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
		Sync::Interlocked::Increment(&item->useCnt);
		i++;
	}
}

IO::PackageFile::PackageFile(Text::String *fileName) : IO::ParsedObject(fileName)
{
	NEW_CLASS(this->items, Data::ArrayList<PackFileItem*>());
	if (IO::Path::PATH_SEPERATOR == '\\')
	{
		NEW_CLASS(this->namedItems, Data::ICaseStringMap<PackFileItem*>());
	}
	else
	{
		NEW_CLASS(this->namedItems, Data::StringMap<PackFileItem*>());
	}
}

IO::PackageFile::PackageFile(Text::CString fileName) : IO::ParsedObject(fileName)
{
	NEW_CLASS(this->items, Data::ArrayList<PackFileItem*>());
	if (IO::Path::PATH_SEPERATOR == '\\')
	{
		NEW_CLASS(this->namedItems, Data::ICaseStringMap<PackFileItem*>());
	}
	else
	{
		NEW_CLASS(this->namedItems, Data::StringMap<PackFileItem*>());
	}
}

IO::PackageFile::~PackageFile()
{
	PackFileItem *item;
	UOSInt i = this->items->GetCount();
	while (i-- > 0)
	{
		item = this->items->GetItem(i);
		if (Sync::Interlocked::Decrement(&item->useCnt) == 0)
		{
			SDEL_STRING(item->name);
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
	DEL_CLASS(this->items);
	DEL_CLASS(this->namedItems);
	i = this->infoMap.GetCount();
	while (i-- > 0)
	{
		Text::StrDelNew(this->infoMap.GetItem(i));
	}
}

IO::ParserType IO::PackageFile::GetParserType() const
{
	return IO::ParserType::PackageFile;
}

void IO::PackageFile::AddData(IO::IStreamData *fd, UInt64 ofst, UInt64 length, Text::CString name, Data::Timestamp modTime)
{
	PackFileItem *item;
	item = MemAlloc(PackFileItem, 1);
	item->itemType = IO::PackFileItem::PIT_UNCOMPRESSED;
	item->fd = fd->GetPartialData(ofst, length);
	item->name = Text::String::New(name);
	item->pobj = 0;
	item->compInfo = 0;
	item->modTime = modTime;
	item->useCnt = 1;
	this->items->Add(item);
	this->namedItems->Put(item->name, item);
}

void IO::PackageFile::AddObject(IO::ParsedObject *pobj, Text::CString name, Data::Timestamp modTime)
{
	PackFileItem *item;
	item = MemAlloc(PackFileItem, 1);
	item->itemType = IO::PackFileItem::PIT_PARSEDOBJECT;
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
	item->useCnt = 1;
	this->items->Add(item);
	this->namedItems->Put(item->name, item);
}

void IO::PackageFile::AddCompData(IO::IStreamData *fd, UInt64 ofst, UInt64 length, IO::PackFileItem::CompressInfo *compInfo, Text::CString name, Data::Timestamp modTime)
{
	PackFileItem *item;
	item = MemAlloc(PackFileItem, 1);
	item->itemType = IO::PackFileItem::PIT_COMPRESSED;
	item->fd = fd->GetPartialData(ofst, length);
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
	item->useCnt = 1;
	this->items->Add(item);
	this->namedItems->Put(item->name, item);
}

void IO::PackageFile::AddPack(IO::PackageFile *pkg, Text::CString name, Data::Timestamp modTime)
{
	PackFileItem *item;
	item = MemAlloc(PackFileItem, 1);
	item->itemType = IO::PackFileItem::PIT_PARSEDOBJECT;
	item->fd = 0;
	item->name = Text::String::New(name);
	item->pobj = pkg;
	item->compInfo = 0;
	item->modTime = modTime;
	item->useCnt = 1;
	this->items->Add(item);
	this->pkgFiles.Put(item->name, item);
	this->namedItems->Put(item->name, item);
}

IO::PackageFile *IO::PackageFile::GetPackFile(Text::CString name) const
{
	IO::PackFileItem *item = this->pkgFiles.GetC(name);
	if (item)
		return (IO::PackageFile*)item->pobj;
	return 0;
}

Bool IO::PackageFile::UpdateCompInfo(const UTF8Char *name, IO::IStreamData *fd, UInt64 ofst, Int32 crc, UOSInt compSize, UInt32 decSize)
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
			if (item->itemType == IO::PackFileItem::PIT_COMPRESSED)
			{
				item->compInfo->decSize = decSize;
				DEL_CLASS(item->fd);
				item->fd = fd->GetPartialData(ofst, compSize);
				WriteMInt32(item->compInfo->checkBytes, crc);
				return true;
			}
			break;
		}
	}
	return false;
}

const IO::PackFileItem *IO::PackageFile::GetPackFileItem(const UTF8Char *name) const
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
			if (item->itemType != IO::PackFileItem::PIT_PARSEDOBJECT)
				return 0;
			if (item->pobj->GetParserType() != IO::ParserType::PackageFile)
				return 0;
			IO::PackageFile *pf = (IO::PackageFile*)item->pobj;
			return pf->GetPackFileItem(sptr);
		}
	}
	return 0;
}

IO::PackageFile::PackObjectType IO::PackageFile::GetPItemType(const PackFileItem *itemObj) const
{
	if (itemObj == 0)
	{
		return IO::PackageFile::PackObjectType::Unknown;
	}
	else if (itemObj->itemType == IO::PackFileItem::PIT_COMPRESSED || itemObj->itemType == IO::PackFileItem::PIT_UNCOMPRESSED)
	{
		return IO::PackageFile::PackObjectType::StreamData;
	}
	else if (itemObj->itemType == IO::PackFileItem::PIT_PARSEDOBJECT)
	{
		if (itemObj->pobj->GetParserType() == IO::ParserType::PackageFile)
		{
			return IO::PackageFile::PackObjectType::PackageFile;
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

IO::IStreamData *IO::PackageFile::GetPItemStmData(const PackFileItem *item) const
{
	if (item != 0)
	{
		if (item->itemType == IO::PackFileItem::PIT_UNCOMPRESSED)
		{
			IO::IStreamData *data = item->fd->GetPartialData(0, item->fd->GetDataSize());
			Text::StringBuilderUTF8 sb;
			sb.Append(this->sourceName);
			sb.AppendC(UTF8STRC("\\"));
			sb.Append(item->name);
			data->SetFullName(sb.ToCString());
			return data;
		}
		else if (item->itemType == IO::PackFileItem::PIT_COMPRESSED)
		{
			Data::Compress::Decompressor *decomp = Data::Compress::Decompressor::CreateDecompressor(item->compInfo->compMethod);
			Crypto::Hash::IHash *hash;
			if (decomp == 0)
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
			if (item->name)
			{
				sptr = item->name->ConcatTo(sptr);
			}
			else
			{
				sptr = item->fd->GetShortName().ConcatTo(sptr);
			}
			{
				IO::FileStream fs(CSTRP(sbuff, sptr), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
				Crypto::Hash::HashStream hashStm(&fs, hash);

				hash->Clear();
				decomp->Decompress(&hashStm, item->fd);
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
				if (item->name)
				{
					sb.Append(item->name);
				}
				else
				{
					sb.Append(item->fd->GetShortName());
				}
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

IO::PackageFile *IO::PackageFile::GetPItemPack(const PackFileItem *item) const
{
	if (item != 0)
	{
		if (item->itemType == item->PIT_PARSEDOBJECT && item->pobj->GetParserType() == IO::ParserType::PackageFile)
		{
			return ((IO::PackageFile*)item->pobj)->Clone();
		}
		return 0;
	}
	else
	{
		return 0;
	}
}

UOSInt IO::PackageFile::GetCount() const
{
	return this->items->GetCount();
}

IO::PackageFile::PackObjectType IO::PackageFile::GetItemType(UOSInt index) const
{
	IO::PackFileItem *item = this->items->GetItem(index);
	return GetPItemType(item);
}

UTF8Char *IO::PackageFile::GetItemName(UTF8Char *sbuff, UOSInt index) const
{
	IO::PackFileItem *item = this->items->GetItem(index);
	if (item == 0)
	{
		*sbuff = 0;
		return 0;
	}
	if (item->name)
	{
		return item->name->ConcatTo(sbuff);
	}
	if (item->itemType == IO::PackFileItem::PIT_COMPRESSED || item->itemType == IO::PackFileItem::PIT_UNCOMPRESSED)
	{
		return item->fd->GetShortName().ConcatTo(sbuff);
	}
	else if (item->itemType == IO::PackFileItem::PIT_PARSEDOBJECT)
	{
		return item->pobj->GetSourceName(sbuff);
	}
	else
	{
		*sbuff = 0;
		return 0;
	}
}

IO::IStreamData *IO::PackageFile::GetItemStmData(UOSInt index) const
{
	IO::PackFileItem *item = this->items->GetItem(index);
	return GetPItemStmData(item);
}

IO::IStreamData *IO::PackageFile::GetItemStmData(const UTF8Char* name, UOSInt nameLen) const
{
	UOSInt index = GetItemIndex({name, nameLen});
	if (index == INVALID_INDEX)
	{
		return 0;
	}
	return this->GetItemStmData(index);
}

IO::PackageFile *IO::PackageFile::GetItemPack(UOSInt index) const
{
	IO::PackFileItem *item = this->items->GetItem(index);
	return GetPItemPack(item);
}

IO::ParsedObject *IO::PackageFile::GetItemPObj(UOSInt index) const
{
	IO::PackFileItem *item = this->items->GetItem(index);
	if (item != 0)
	{
		if (item->itemType == IO::PackFileItem::PIT_PARSEDOBJECT)
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

Data::Timestamp IO::PackageFile::GetItemModTime(UOSInt index) const
{
	IO::PackFileItem *item = this->items->GetItem(index);
	if (item != 0)
	{
		return item->modTime;
	}
	return Data::Timestamp(0, 0);
}

UInt64 IO::PackageFile::GetItemStoreSize(UOSInt index) const
{
	IO::PackFileItem *item = this->items->GetItem(index);
	if (item != 0)
	{
		return item->fd->GetDataSize();
	}
	return 0;
}

UInt64 IO::PackageFile::GetItemSize(UOSInt index) const
{
	IO::PackFileItem *item = this->items->GetItem(index);
	if (item != 0)
	{
		if (item->itemType == IO::PackFileItem::PIT_UNCOMPRESSED)
		{
			return item->fd->GetDataSize();
		}
		else if (item->itemType == IO::PackFileItem::PIT_COMPRESSED)
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

UOSInt IO::PackageFile::GetItemIndex(Text::CString name) const
{
	UOSInt i;
	IO::PackFileItem *item;
	i = this->items->GetCount();
	while (i-- > 0)
	{
		item = this->items->GetItem(i);
		if (item)
		{
			if (item->name)
			{
				if (item->name->EqualsICase(name.v, name.leng))
					return i;
			}
			if (item->itemType == IO::PackFileItem::PIT_COMPRESSED || item->itemType == IO::PackFileItem::PIT_UNCOMPRESSED)
			{
				Text::CString shName = item->fd->GetShortName();
				if (shName.EqualsICase(name.v, name.leng))
					return i;
			}
			else if (item->itemType == IO::PackFileItem::PIT_PARSEDOBJECT)
			{
				if (item->pobj->GetSourceNameObj()->EqualsICase(name.v, name.leng))
					return i;
			}
		}
	}
	return INVALID_INDEX;
}

Bool IO::PackageFile::IsCompressed(UOSInt index) const
{
	IO::PackFileItem *item = this->items->GetItem(index);
	if (item != 0 && item->itemType == IO::PackFileItem::PIT_COMPRESSED)
	{
		return true;
	}
	return false;
}

Data::Compress::Decompressor::CompressMethod IO::PackageFile::GetItemComp(UOSInt index) const
{
	IO::PackFileItem *item = this->items->GetItem(index);
	if (item != 0)
	{
		if (item->itemType == IO::PackFileItem::PIT_COMPRESSED)
		{
			return item->compInfo->compMethod;
		}
		else if (item->itemType == IO::PackFileItem::PIT_UNCOMPRESSED)
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

IO::PackageFile *IO::PackageFile::Clone() const
{
	return NEW_CLASS_D(PackageFile(this));
}

Bool IO::PackageFile::AllowWrite() const
{
	return false;
}

Bool IO::PackageFile::CopyFrom(Text::CString fileName, IO::IProgressHandler *progHdlr, IO::ActiveStreamReader::BottleNeckType *bnt)
{
	return false;
}

Bool IO::PackageFile::MoveFrom(Text::CString fileName, IO::IProgressHandler *progHdlr, IO::ActiveStreamReader::BottleNeckType *bnt)
{
	return false;
}

Bool IO::PackageFile::RetryCopyFrom(Text::CString fileName, IO::IProgressHandler *progHdlr, IO::ActiveStreamReader::BottleNeckType *bnt)
{
	return this->CopyFrom(fileName, progHdlr, bnt);
}

Bool IO::PackageFile::RetryMoveFrom(Text::CString fileName, IO::IProgressHandler *progHdlr, IO::ActiveStreamReader::BottleNeckType *bnt)
{
	return this->MoveFrom(fileName, progHdlr, bnt);
}

Bool IO::PackageFile::CopyTo(UOSInt index, Text::CString destPath, Bool fullFileName)
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
		else if (fullFileName && (item->itemType == IO::PackFileItem::PIT_COMPRESSED || item->itemType == IO::PackFileItem::PIT_UNCOMPRESSED))
		{
		}
		else
		{
			sb.AppendChar(IO::Path::PATH_SEPERATOR, 1);
			sb.Append(item->name);
		}
		if (item->itemType == IO::PackFileItem::PIT_COMPRESSED)
		{
			if (IO::Path::GetPathType(sb.ToCString()) != IO::Path::PathType::Unknown)
				return false;

			Data::Compress::Decompressor *decomp = Data::Compress::Decompressor::CreateDecompressor(item->compInfo->compMethod);
			Crypto::Hash::IHash *hash;
			if (decomp == 0)
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
				decomp->Decompress(&hashStm, item->fd);
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
		else if (item->itemType == IO::PackFileItem::PIT_UNCOMPRESSED)
		{
			UInt64 fileSize = item->fd->GetDataSize();
			UOSInt readSize;
			if (IO::Path::GetPathType(sb.ToCString()) != IO::Path::PathType::Unknown)
				return false;

			if (fileSize < 1048576)
			{
				UInt8 *tmpBuff = MemAlloc(UInt8, (UOSInt)fileSize);
				succ = (item->fd->GetRealData(0, (UOSInt)fileSize, tmpBuff) == fileSize);
				if (succ)
				{
					IO::FileStream fs(sb.ToCString(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::NoWriteBuffer);
					succ = (fs.Write(tmpBuff, (UOSInt)fileSize) == fileSize);
				}
				MemFree(tmpBuff);
			}
			else
			{
				UInt8 *tmpBuff = MemAlloc(UInt8, 1048576);
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
						if (fs.Write(tmpBuff, readSize) != readSize)
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
				MemFree(tmpBuff);
			}

			return succ;
		}
		else if (item->itemType == IO::PackFileItem::PIT_PARSEDOBJECT && item->pobj->GetParserType() == IO::ParserType::PackageFile)
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

IO::IStreamData *IO::PackageFile::OpenStreamData(Text::CString fileName) const
{
	if (fileName.IndexOf(':') != INVALID_INDEX)
	{
		return 0;
	}

	IO::IStreamData *retFD = 0;
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
			if (pf->GetItemType(i) == IO::PackageFile::PackObjectType::PackageFile)
			{
				sbuff[0] = 0;
				sptr = pf->GetItemName(sbuff, i);
				if (Text::StrEqualsC(sb.ToString(), j, sbuff, (UOSInt)(sptr - sbuff)))
				{
					pf2 = pf->GetItemPack(i);
					if (pf2)
					{
						found = true;
						if (needRel)
						{
							DEL_CLASS(pf);
						}
						sb.SetSubstr((UOSInt)j + 1);
						pf = pf2;
						needRel = true;
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
				retFD = pf->GetItemStmData(i);
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

void IO::PackageFile::SetInfo(InfoType infoType, const UTF8Char *val)
{
	const UTF8Char *csptr = this->infoMap.Put(infoType, Text::StrCopyNew(val));
	if (csptr)
	{
		Text::StrDelNew(csptr);
	}
}

void IO::PackageFile::GetInfoText(Text::StringBuilderUTF8 *sb) const
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

Text::CString IO::PackageFile::GetInfoTypeName(InfoType infoType)
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
