#include "Stdafx.h"
#include "MyMemory.h"
#include "Crypto/Hash/HashCreator.h"
#include "Crypto/Hash/HashStream.h"
#include "Data/ByteTool.h"
#include "Data/ICaseStringUTF8Map.h"
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

IO::PackageFile::PackageFile(const UTF8Char *fileName) : IO::ParsedObject(fileName)
{
	NEW_CLASS(this->items, Data::ArrayList<PackFileItem*>());
	NEW_CLASS(this->pkgFiles, Data::StringUTF8Map<PackFileItem*>());
	if (IO::Path::PATH_SEPERATOR == '\\')
	{
		NEW_CLASS(this->namedItems, Data::ICaseStringUTF8Map<PackFileItem*>());
	}
	else
	{
		NEW_CLASS(this->namedItems, Data::StringUTF8Map<PackFileItem*>());
	}
	NEW_CLASS(this->infoMap, Data::Int32Map<const UTF8Char *>());
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
			if (item->name)
			{
				Text::StrDelNew(item->name);
			}
			if (item->fd)
			{
				DEL_CLASS(item->fd);
			}
			if (item->pobj)
			{
				DEL_CLASS(item->pobj);
			}
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
	DEL_CLASS(this->pkgFiles);
	DEL_CLASS(this->namedItems);
	Data::ArrayList<const UTF8Char *> *infoList = this->infoMap->GetValues();
	i = infoList->GetCount();
	while (i-- > 0)
	{
		Text::StrDelNew(infoList->GetItem(i));
	}
	DEL_CLASS(this->infoMap);
}

IO::ParsedObject::ParserType IO::PackageFile::GetParserType()
{
	return IO::ParsedObject::PT_PACKAGE_PARSER;
}

void IO::PackageFile::AddData(IO::IStreamData *fd, UInt64 ofst, UInt64 length, const UTF8Char *name, Int64 modTimeTick)
{
	PackFileItem *item;
	item = MemAlloc(PackFileItem, 1);
	item->itemType = IO::PackFileItem::PIT_UNCOMPRESSED;
	item->fd = fd->GetPartialData(ofst, length);
	item->name = SCOPY_TEXT(name);
	item->pobj = 0;
	item->compInfo = 0;
	item->modTimeTick = modTimeTick;
	item->useCnt = 1;
	this->items->Add(item);
	this->namedItems->Put(item->name, item);
}

void IO::PackageFile::AddObject(IO::ParsedObject *pobj, const UTF8Char *name, Int64 modTimeTick)
{
	PackFileItem *item;
	item = MemAlloc(PackFileItem, 1);
	item->itemType = IO::PackFileItem::PIT_PARSEDOBJECT;
	item->fd = 0;
	if (name)
	{
		item->name = Text::StrCopyNew(name);
	}
	else
	{
		item->name = Text::StrCopyNew(pobj->GetSourceNameObj());
	}
	item->pobj = pobj;
	item->compInfo = 0;
	item->modTimeTick = modTimeTick;
	item->useCnt = 1;
	this->items->Add(item);
	this->namedItems->Put(item->name, item);
}

void IO::PackageFile::AddCompData(IO::IStreamData *fd, UInt64 ofst, UInt64 length, IO::PackFileItem::CompressInfo *compInfo, const UTF8Char *name, Int64 modTimeTick)
{
	PackFileItem *item;
	item = MemAlloc(PackFileItem, 1);
	item->itemType = IO::PackFileItem::PIT_COMPRESSED;
	item->fd = fd->GetPartialData(ofst, length);
	item->name = SCOPY_TEXT(name);
	item->pobj = 0;
	item->compInfo = MemAlloc(PackFileItem::CompressInfo, 1);
	MemCopyNO(item->compInfo, compInfo, sizeof(PackFileItem::CompressInfo));
	if (compInfo->compExtras)
	{
		item->compInfo->compExtras = MemAlloc(UInt8, compInfo->compExtraSize);
		MemCopyNO(item->compInfo->compExtras, compInfo->compExtras, compInfo->compExtraSize);
	}
	item->modTimeTick = modTimeTick;
	item->useCnt = 1;
	this->items->Add(item);
	this->namedItems->Put(item->name, item);
}

void IO::PackageFile::AddPack(IO::PackageFile *pkg, const UTF8Char *name, Int64 modTimeTick)
{
	PackFileItem *item;
	item = MemAlloc(PackFileItem, 1);
	item->itemType = IO::PackFileItem::PIT_PARSEDOBJECT;
	item->fd = 0;
	if (name)
	{
		item->name = Text::StrCopyNew(name);
	}
	else
	{
		item->name = 0;
	}
	item->pobj = pkg;
	item->compInfo = 0;
	item->modTimeTick = modTimeTick;
	item->useCnt = 1;
	this->items->Add(item);
	this->pkgFiles->Put(item->name, item);
	this->namedItems->Put(item->name, item);
}

IO::PackageFile *IO::PackageFile::GetPackFile(const UTF8Char *name)
{
	IO::PackFileItem *item = this->pkgFiles->Get(name);
	if (item)
		return (IO::PackageFile*)item->pobj;
	return 0;
}

Bool IO::PackageFile::UpdateCompInfo(const UTF8Char *name, IO::IStreamData *fd, UInt64 ofst, Int32 crc, UOSInt compSize, UInt32 decSize)
{
	UOSInt i;
	IO::PackFileItem *item;
	i = this->items->GetCount();
	while (i-- > 0)
	{
		item = this->items->GetItem(i);
		if (Text::StrEquals(item->name, name))
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

const IO::PackFileItem *IO::PackageFile::GetPackFileItem(const UTF8Char *name)
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
			return this->namedItems->Get(name);
		}
		else if (c == '/' || c == '\\')
		{
			nameLen = (UOSInt)(sptr - name - 1);
			Text::StrConcatC(sbuff, name, nameLen);
			IO::PackFileItem *item = this->namedItems->Get(sbuff);
			if (item == 0)
			{
				return 0;
			}
			if (sptr[0] == 0)
				return item;
			if (item->itemType != IO::PackFileItem::PIT_PARSEDOBJECT)
				return 0;
			if (item->pobj->GetParserType() != IO::ParsedObject::PT_PACKAGE_PARSER)
				return 0;
			IO::PackageFile *pf = (IO::PackageFile*)item->pobj;
			return pf->GetPackFileItem(sptr);
		}
	}
	return 0;
}

IO::PackageFile::PackObjectType IO::PackageFile::GetPItemType(const PackFileItem *itemObj)
{
	if (itemObj == 0)
	{
		return IO::PackageFile::POT_UNKNOWN;
	}
	else if (itemObj->itemType == IO::PackFileItem::PIT_COMPRESSED || itemObj->itemType == IO::PackFileItem::PIT_UNCOMPRESSED)
	{
		return IO::PackageFile::POT_STREAMDATA;
	}
	else if (itemObj->itemType == IO::PackFileItem::PIT_PARSEDOBJECT)
	{
		if (itemObj->pobj->GetParserType() == IO::ParsedObject::PT_PACKAGE_PARSER)
		{
			return IO::PackageFile::POT_PACKAGEFILE;
		}
		else
		{
			return IO::PackageFile::POT_PARSEDOBJECT;
		}
	}
	else
	{
		return IO::PackageFile::POT_UNKNOWN;
	}
}

IO::IStreamData *IO::PackageFile::GetPItemStmData(const PackFileItem *item)
{
	if (item != 0)
	{
		if (item->itemType == IO::PackFileItem::PIT_UNCOMPRESSED)
		{
			IO::IStreamData *data = item->fd->GetPartialData(0, item->fd->GetDataSize());
			Text::StringBuilderUTF8 sb;
			sb.Append(this->sourceName);
			sb.Append((const UTF8Char*)"\\");
			sb.Append(item->name);
			data->SetFullName(sb.ToString());
			return data;
		}
		else if (item->itemType == IO::PackFileItem::PIT_COMPRESSED)
		{
			Data::Compress::Decompressor *decomp = Data::Compress::Decompressor::CreateDecompressor(item->compInfo->compMethod);
			IO::FileStream *fs;
			Crypto::Hash::HashStream *hashStm;
			Crypto::Hash::IHash *hash;
			Data::DateTime *t;
			if (decomp == 0)
				return 0;

			hash = Crypto::Hash::HashCreator::CreateHash(item->compInfo->checkMethod);
			if (hash == 0)
			{
				DEL_CLASS(decomp);
				return 0;
			}
			NEW_CLASS(t, Data::DateTime());
			t->SetCurrTimeUTC();
			UTF8Char sbuff[512];
			UTF8Char *sptr;
			UInt8 chkResult[32];
			UOSInt resSize;
			UOSInt i;
			Bool diff = false;
			IO::Path::GetProcessFileName(sbuff);
			IO::Path::AppendPath(sbuff, (const UTF8Char*)"temp");
			IO::Path::CreateDirectory(sbuff);
			sptr = &sbuff[Text::StrCharCnt(sbuff)];
			*sptr++ = '\\';
			sptr = Text::StrHexVal64(sptr, (UInt64)t->ToTicks());
			*sptr++ = '_';
			if (item->name)
			{
				sptr = Text::StrConcat(sptr, item->name);
			}
			else
			{
				sptr = Text::StrConcat(sptr, item->fd->GetShortName());
			}
			NEW_CLASS(fs, IO::FileStream(sbuff, IO::FileStream::FILE_MODE_CREATE, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));
			NEW_CLASS(hashStm, Crypto::Hash::HashStream(fs, hash));

			hash->Clear();
			decomp->Decompress(hashStm, item->fd);
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
			DEL_CLASS(hashStm);
			DEL_CLASS(fs);

			DEL_CLASS(t);
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
				NEW_CLASS(fd, IO::StmData::FileData(sbuff, true));
				sb.Append(this->sourceName);
				sb.Append((const UTF8Char*)"\\");
				if (item->name)
				{
					sb.Append(item->name);
				}
				else
				{
					sb.Append(item->fd->GetShortName());
				}
				fd->SetFullName(sb.ToString());
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

IO::PackageFile *IO::PackageFile::GetPItemPack(const PackFileItem *item)
{
	if (item != 0)
	{
		if (item->itemType == item->PIT_PARSEDOBJECT && item->pobj->GetParserType() == IO::ParsedObject::PT_PACKAGE_PARSER)
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

UOSInt IO::PackageFile::GetCount()
{
	return this->items->GetCount();
}

IO::PackageFile::PackObjectType IO::PackageFile::GetItemType(UOSInt index)
{
	IO::PackFileItem *item = this->items->GetItem(index);
	return GetPItemType(item);
}

UTF8Char *IO::PackageFile::GetItemName(UTF8Char *sbuff, UOSInt index)
{
	IO::PackFileItem *item = this->items->GetItem(index);
	if (item == 0)
	{
		*sbuff = 0;
		return 0;
	}
	if (item->name)
	{
		return Text::StrConcat(sbuff, item->name);
	}
	if (item->itemType == IO::PackFileItem::PIT_COMPRESSED || item->itemType == IO::PackFileItem::PIT_UNCOMPRESSED)
	{
		return Text::StrConcat(sbuff, item->fd->GetShortName());
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

IO::IStreamData *IO::PackageFile::GetItemStmData(UOSInt index)
{
	IO::PackFileItem *item = this->items->GetItem(index);
	return GetPItemStmData(item);
}

IO::PackageFile *IO::PackageFile::GetItemPack(UOSInt index)
{
	IO::PackFileItem *item = this->items->GetItem(index);
	return GetPItemPack(item);
}

IO::ParsedObject *IO::PackageFile::GetItemPObj(UOSInt index)
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

Int64 IO::PackageFile::GetItemModTimeTick(UOSInt index)
{
	IO::PackFileItem *item = this->items->GetItem(index);
	if (item != 0)
	{
		return item->modTimeTick;
	}
	return 0;
}

UInt64 IO::PackageFile::GetItemSize(UOSInt index)
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

OSInt IO::PackageFile::GetItemIndex(const UTF8Char *name)
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
				if (Text::StrEqualsICase(item->name, name))
					return (OSInt)i;
			}
			if (item->itemType == IO::PackFileItem::PIT_COMPRESSED || item->itemType == IO::PackFileItem::PIT_UNCOMPRESSED)
			{
				if (Text::StrEqualsICase(item->fd->GetShortName(), name))
					return (OSInt)i;
			}
			else if (item->itemType == IO::PackFileItem::PIT_PARSEDOBJECT)
			{
				if (Text::StrEqualsICase(item->pobj->GetSourceNameObj(), name))
					return (OSInt)i;
			}
		}
	}
	return -1;
}

Bool IO::PackageFile::IsCompressed(UOSInt index)
{
	IO::PackFileItem *item = this->items->GetItem(index);
	if (item != 0 && item->itemType == IO::PackFileItem::PIT_COMPRESSED)
	{
		return true;
	}
	return false;
}

Data::Compress::Decompressor::CompressMethod IO::PackageFile::GetItemComp(UOSInt index)
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

IO::PackageFile *IO::PackageFile::Clone()
{
	IO::PackageFile *pkg;
	IO::PackFileItem *item;
	UOSInt i;
	UOSInt j;
	NEW_CLASS(pkg, IO::PackageFile(this->GetSourceNameObj()));
	i = 0;
	j = this->items->GetCount();
	while (i < j)
	{
		item = this->items->GetItem(i);
		Sync::Interlocked::Increment(&item->useCnt);
		pkg->items->Add(item);
		if (item->itemType == IO::PackFileItem::PIT_PARSEDOBJECT && item->pobj->GetParserType() == IO::ParsedObject::PT_PACKAGE_PARSER)
		{
			pkg->pkgFiles->Put(item->name, item);
		}
		pkg->namedItems->Put(item->name, item);
		i++;
	}
	return pkg;
}

Bool IO::PackageFile::AllowWrite()
{
	return false;
}

Bool IO::PackageFile::CopyFrom(const UTF8Char *fileName, IO::IProgressHandler *progHdlr, IO::ActiveStreamReader::BottleNeckType *bnt)
{
	return false;
}

Bool IO::PackageFile::MoveFrom(const UTF8Char *fileName, IO::IProgressHandler *progHdlr, IO::ActiveStreamReader::BottleNeckType *bnt)
{
	return false;
}

Bool IO::PackageFile::RetryCopyFrom(const UTF8Char *fileName, IO::IProgressHandler *progHdlr, IO::ActiveStreamReader::BottleNeckType *bnt)
{
	return this->CopyFrom(fileName, progHdlr, bnt);
}

Bool IO::PackageFile::RetryMoveFrom(const UTF8Char *fileName, IO::IProgressHandler *progHdlr, IO::ActiveStreamReader::BottleNeckType *bnt)
{
	return this->MoveFrom(fileName, progHdlr, bnt);
}

Bool IO::PackageFile::CopyTo(UOSInt index, const UTF8Char *destPath, Bool fullFileName)
{
	IO::PackFileItem *item = this->items->GetItem(index);
	if (item != 0)
	{
		Text::StringBuilderUTF8 sb;
		Bool succ = true;
		sb.Append(destPath);
		if (sb.EndsWith((Char)IO::Path::PATH_SEPERATOR))
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
			if (IO::Path::GetPathType(sb.ToString()) != IO::Path::PT_UNKNOWN)
				return false;

			Data::Compress::Decompressor *decomp = Data::Compress::Decompressor::CreateDecompressor(item->compInfo->compMethod);
			IO::FileStream *fs;
			Crypto::Hash::HashStream *hashStm;
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
			NEW_CLASS(fs, IO::FileStream(sb.ToString(), IO::FileStream::FILE_MODE_CREATE, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NO_WRITE_BUFFER));
			NEW_CLASS(hashStm, Crypto::Hash::HashStream(fs, hash));

			hash->Clear();
			decomp->Decompress(hashStm, item->fd);
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
			DEL_CLASS(hashStm);
			DEL_CLASS(fs);

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
			if (IO::Path::GetPathType(sb.ToString()) != IO::Path::PT_UNKNOWN)
				return false;

			if (fileSize < 1048576)
			{
				UInt8 *tmpBuff = MemAlloc(UInt8, (UOSInt)fileSize);
				succ = (item->fd->GetRealData(0, (UOSInt)fileSize, tmpBuff) == fileSize);
				if (succ)
				{
					IO::FileStream *fs;
					NEW_CLASS(fs, IO::FileStream(sb.ToString(), IO::FileStream::FILE_MODE_CREATE, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NO_WRITE_BUFFER));
					succ = (fs->Write(tmpBuff, (UOSInt)fileSize) == fileSize);
					DEL_CLASS(fs);
				}
				MemFree(tmpBuff);
			}
			else
			{
				UInt8 *tmpBuff = MemAlloc(UInt8, 1048576);
				UInt64 currOfst = 0;
				IO::FileStream *fs;
				NEW_CLASS(fs, IO::FileStream(sb.ToString(), IO::FileStream::FILE_MODE_CREATE, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NO_WRITE_BUFFER));
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
					if (fs->Write(tmpBuff, readSize) != readSize)
					{
						succ = false;
						break;
					}
					currOfst += readSize;
				}
				DEL_CLASS(fs);
				if (!succ)
				{
					IO::Path::DeleteFile(sb.ToString());
				}
				MemFree(tmpBuff);
			}

			return succ;
		}
		else if (item->itemType == IO::PackFileItem::PIT_PARSEDOBJECT && item->pobj->GetParserType() == IO::ParsedObject::PT_PACKAGE_PARSER)
		{
			IO::PackageFile *pf = (IO::PackageFile*)item->pobj;
			IO::Path::CreateDirectory(sb.ToString());
			UOSInt i = 0;
			UOSInt j = pf->GetCount();
			while (i < j)
			{
				succ = pf->CopyTo(i, sb.ToString(), false);
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

IO::IStreamData *IO::PackageFile::OpenStreamData(const UTF8Char *fileName)
{
	if (Text::StrIndexOf(fileName, ':') >= 0)
	{
		return 0;
	}

	IO::IStreamData *retFD = 0;
	UTF8Char sbuff[512];
	UOSInt i;
	OSInt j;
	Text::StringBuilderUTF8 sb;
	sb.Append(fileName);
	sb.Replace('\\', '/');
	IO::PackageFile *pf;
	IO::PackageFile *pf2;
	Bool needRel = false;
	Bool found;

	pf = this;
	j = sb.IndexOf('/');
	while (j >= 0)
	{
		sb.ToString()[j] = 0;
		found = false;
		i = pf->GetCount();
		while (i-- > 0)
		{
			if (pf->GetItemType(i) == IO::PackageFile::POT_PACKAGEFILE)
			{
				sbuff[0] = 0;
				pf->GetItemName(sbuff, i);
				if (sb.Equals(sbuff))
				{
					pf2 = pf->GetItemPack(i);
					if (pf2)
					{
						found = true;
						if (needRel)
						{
							DEL_CLASS(pf);
						}
						sb.SetSubstr(j + 1);
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
		if (pf->GetItemType(i) == IO::PackageFile::POT_STREAMDATA)
		{
			sbuff[0] = 0;
			pf->GetItemName(sbuff, i);
			if (sb.Equals(sbuff))
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
	const UTF8Char *csptr = this->infoMap->Put(infoType, Text::StrCopyNew(val));
	if (csptr)
	{
		Text::StrDelNew(csptr);
	}
}

void IO::PackageFile::GetInfoText(Text::StringBuilderUTF *sb)
{
	UOSInt i;
	UOSInt j;
	Data::ArrayList<Int32> *typeList = this->infoMap->GetKeys();
	Data::ArrayList<const UTF8Char *> *valList = this->infoMap->GetValues();
	i = 0;
	j = valList->GetCount();
	while (i < j)
	{
		if (i > 0)
		{
			sb->Append((const UTF8Char*)"\r\n");
		}
		sb->Append(GetInfoTypeName((InfoType)typeList->GetItem(i)));
		sb->Append((const UTF8Char*)": ");
		sb->Append(valList->GetItem(i));
		i++;
	}
}

const UTF8Char *IO::PackageFile::GetInfoTypeName(InfoType infoType)
{
	switch(infoType)
	{
	case IT_SYSTEM_ID:
		return (const UTF8Char*)"System";
	case IT_VOLUME_ID:
		return (const UTF8Char*)"Volume";
	case IT_VOLUME_SET_ID:
		return (const UTF8Char*)"Volume Set";
	case IT_PUBLISHER_ID:
		return (const UTF8Char*)"Publisher";
	case IT_DATA_PREPARER_ID:
		return (const UTF8Char*)"Data Preparer";
	case IT_APPLICATION_ID:
		return (const UTF8Char*)"Application ID";
	case IT_COPYRIGHT_FILE_ID:
		return (const UTF8Char*)"Copyright File";
	case IT_ABSTRACT_FILE_ID:
		return (const UTF8Char*)"Abstract File";
	case IT_BIBLIOGRAHPICAL_FILE_ID:
		return (const UTF8Char*)"Bibliographical File";
	default:
		return (const UTF8Char*)"Unknown";
	}
}
