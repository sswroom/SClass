#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "Data/ArrayListInt32.h"
#include "IO/FileAnalyse/QTFileAnalyse.h"
#include "Sync/Thread.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"

void IO::FileAnalyse::QTFileAnalyse::ParseRange(UOSInt lev, UInt64 ofst, UInt64 size)
{
	UInt8 buff[16];
	UInt64 endOfst = ofst + size;
	UInt64 sz;
	IO::FileAnalyse::QTFileAnalyse::PackInfo *pack;
	Data::ArrayListInt32 contList;
	contList.SortedInsert(*(Int32*)"dinf");
	contList.SortedInsert(*(Int32*)"mdia");
	contList.SortedInsert(*(Int32*)"minf");
	contList.SortedInsert(*(Int32*)"moov");
	contList.SortedInsert(*(Int32*)"stbl");
	contList.SortedInsert(*(Int32*)"trak");

	while (ofst <= (endOfst - 8) && !this->threadToStop)
	{
		if (this->pauseParsing)
		{
			Sync::Thread::Sleep(100);
		}
		else
		{
			this->fd->GetRealData(ofst, 16, buff);
			sz = ReadMUInt32(&buff[0]);
			if (sz == 1)
			{
				sz = ReadMUInt64(&buff[8]);
			}
			if (sz < 8 || ofst + sz > endOfst)
			{
				return;
			}
			pack = MemAlloc(IO::FileAnalyse::QTFileAnalyse::PackInfo, 1);
			pack->lev = lev;
			pack->fileOfst = ofst;
			pack->packSize = sz;
			pack->packType = *(Int32*)&buff[4];
			this->packs->Add(pack);
			
			if (this->maxLev < lev)
			{
				this->maxLev = lev;
			}
			if (contList.SortedIndexOf(pack->packType) >= 0)
			{
				this->ParseRange(lev + 1, pack->fileOfst + 8, pack->packSize - 8);
			}

			ofst += sz;
		}
	}
}

UInt32 __stdcall IO::FileAnalyse::QTFileAnalyse::ParseThread(void *userObj)
{
	IO::FileAnalyse::QTFileAnalyse *me = (IO::FileAnalyse::QTFileAnalyse*)userObj;
	me->threadRunning = true;
	me->threadStarted = true;
	me->ParseRange(0, 0, me->fd->GetDataSize());
	me->threadRunning = false;
	return 0;
}

UOSInt IO::FileAnalyse::QTFileAnalyse::GetFrameIndex(UOSInt lev, UInt64 ofst)
{
	OSInt i = 0;
	OSInt j = (OSInt)this->packs->GetCount() - 1;
	OSInt k;
	PackInfo *pack;
	while (i <= j)
	{
		k = (i + j) >> 1;
		pack = this->packs->GetItem((UOSInt)k);
		if (ofst < pack->fileOfst)
		{
			j = k - 1;
		}
		else if (ofst >= pack->fileOfst + pack->packSize)
		{
			i = k + 1;
		}
		else if (pack->lev < lev)
		{
			i = k + 1;
		}
		else
		{
			return (UOSInt)k;
		}
	}
	return INVALID_INDEX;
}

IO::FileAnalyse::QTFileAnalyse::QTFileAnalyse(IO::IStreamData *fd)
{
	UInt8 buff[8];
	this->fd = 0;
	this->threadRunning = false;
	this->pauseParsing = false;
	this->threadToStop = false;
	this->threadStarted = false;
	this->maxLev = 0;
	NEW_CLASS(this->packs, Data::SyncArrayList<IO::FileAnalyse::QTFileAnalyse::PackInfo*>());
	fd->GetRealData(0, 8, buff);
	if (ReadInt32(&buff[4]) != *(Int32*)"ftyp" && ReadInt32(&buff[4]) != *(Int32*)"moov")
	{
		return;
	}
	UInt32 size = ReadMUInt32(&buff[0]);
	if (size > 32)
	{
		return;
	}
	this->fd = fd->GetPartialData(0, fd->GetDataSize());
	Sync::Thread::Create(ParseThread, this);
	while (!this->threadStarted)
	{
		Sync::Thread::Sleep(10);
	}
}

IO::FileAnalyse::QTFileAnalyse::~QTFileAnalyse()
{
	if (this->threadRunning)
	{
		this->threadToStop = true;
		while (this->threadRunning)
		{
			Sync::Thread::Sleep(10);
		}
	}
	SDEL_CLASS(this->fd);
	LIST_FREE_FUNC(this->packs, MemFree);
	DEL_CLASS(this->packs);
}

Text::CString IO::FileAnalyse::QTFileAnalyse::GetFormatName()
{
	return {UTF8STRC("Quicktime")};
}

UOSInt IO::FileAnalyse::QTFileAnalyse::GetFrameCount()
{
	return this->packs->GetCount();
}

Bool IO::FileAnalyse::QTFileAnalyse::GetFrameName(UOSInt index, Text::StringBuilderUTF8 *sb)
{
	IO::FileAnalyse::QTFileAnalyse::PackInfo *pack;
	UInt8 buff[5];
	pack = this->packs->GetItem(index);
	if (pack == 0)
		return false;
	sb->AppendU64(pack->fileOfst);
	sb->AppendC(UTF8STRC(": Type="));
	*(Int32*)buff = pack->packType;
	buff[4] = 0;
	sb->AppendSlow((UTF8Char*)buff);
	sb->AppendC(UTF8STRC(", size="));
	sb->AppendU64(pack->packSize);
	return true;
}

Bool IO::FileAnalyse::QTFileAnalyse::GetFrameDetail(UOSInt index, Text::StringBuilderUTF8 *sb)
{
	IO::FileAnalyse::QTFileAnalyse::PackInfo *pack;
	UInt8 *packBuff;
	UInt8 buff[5];
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UOSInt l;
	pack = this->packs->GetItem(index);
	if (pack == 0)
		return false;

	sb->AppendU64(pack->fileOfst);
	sb->AppendC(UTF8STRC(": Type="));
	*(Int32*)buff = pack->packType;
	buff[4] = 0;
	sb->AppendSlow((UTF8Char*)buff);
	sb->AppendC(UTF8STRC(", size="));
	sb->AppendU64(pack->packSize);
	sb->AppendC(UTF8STRC("\r\nLev="));
	sb->AppendUOSInt(pack->lev);
	sb->AppendC(UTF8STRC("\r\n"));

	if (pack->packType == *(Int32*)"ftyp")
	{
		packBuff = MemAlloc(UInt8, (UOSInt)(pack->packSize - 8));
		this->fd->GetRealData(pack->fileOfst + 8, (UOSInt)pack->packSize - 8, packBuff);

		sb->AppendC(UTF8STRC("\r\nMajor_Brand = "));
		*(Int32*)buff = *(Int32*)&packBuff[0];
		sb->AppendSlow((UTF8Char*)buff);
		sb->AppendC(UTF8STRC("\r\nMinor_Version = "));
		sb->AppendHex32(ReadMUInt32(&packBuff[4]));
		i = 8;
		j = (UOSInt)(pack->packSize - 8);
		while (i < j)
		{
			sb->AppendC(UTF8STRC("\r\nCompatible_Brands["));
			sb->AppendU32((UInt32)i);
			sb->AppendC(UTF8STRC("] = "));
			*(Int32*)buff = *(Int32*)&packBuff[i];
			sb->AppendSlow((UTF8Char*)buff);

			i += 4;
		}

		MemFree(packBuff);
	}
	else if (pack->packType == *(Int32*)"free")
	{
		if (pack->packSize > 8)
		{
			packBuff = MemAlloc(UInt8, (UOSInt)(pack->packSize - 8));
			this->fd->GetRealData(pack->fileOfst + 8, (UOSInt)(pack->packSize - 8), packBuff);

			sb->AppendC(UTF8STRC("\r\nData:\r\n"));
			sb->AppendHexBuff(packBuff, (UOSInt)(pack->packSize - 8), ' ', Text::LineBreakType::CRLF);

			MemFree(packBuff);
		}
	}
	else if (pack->packType == *(Int32*)"pnot")
	{
		if (pack->packSize >= 20)
		{
			packBuff = MemAlloc(UInt8, (UOSInt)(pack->packSize - 8));
			this->fd->GetRealData(pack->fileOfst + 8, (UOSInt)(pack->packSize - 8), packBuff);

			sb->AppendC(UTF8STRC("\r\nModification date = "));
			sb->AppendI32(ReadMInt32(&packBuff[0]));
			sb->AppendC(UTF8STRC("\r\nVersion number = "));
			sb->AppendU16(ReadMUInt16(&packBuff[4]));
			sb->AppendC(UTF8STRC("\r\nAtom type = "));
			*(Int32*)buff = *(Int32*)&packBuff[6];
			sb->AppendSlow((UTF8Char*)buff);
			sb->AppendC(UTF8STRC("\r\nAtom Index = "));
			sb->AppendU16(ReadMUInt16(&packBuff[10]));
			MemFree(packBuff);
		}
	}
	else if (pack->packType == *(Int32*)"mvhd")
	{
		if (pack->packSize >= 108)
		{
			Data::DateTime dt;
			packBuff = MemAlloc(UInt8, (UOSInt)(pack->packSize - 8));
			this->fd->GetRealData(pack->fileOfst + 8, (UOSInt)(pack->packSize - 8), packBuff);

			sb->AppendC(UTF8STRC("\r\nVersion = "));
			sb->AppendU16(packBuff[0]);
			sb->AppendC(UTF8STRC("\r\nFlags = "));
			sb->AppendHex24(ReadMUInt32(&packBuff[0]));
			sb->AppendC(UTF8STRC("\r\nCreation time = "));
			dt.SetUnixTimestamp(ReadMUInt32(&packBuff[4]));
			sb->AppendDate(&dt);
			sb->AppendC(UTF8STRC("\r\nModification time = "));
			dt.SetUnixTimestamp(ReadMUInt32(&packBuff[8]));
			sb->AppendDate(&dt);
			sb->AppendC(UTF8STRC("\r\nTime Scale = "));
			sb->AppendI32(ReadMInt32(&packBuff[12]));
			sb->AppendC(UTF8STRC("\r\nDuration = "));
			sb->AppendI32(ReadMInt32(&packBuff[16]));
			sb->AppendC(UTF8STRC("\r\nPreferred rate = "));
			Text::SBAppendF64(sb, ReadMInt32(&packBuff[20]) / 65536.0);
			sb->AppendC(UTF8STRC("\r\nPreferred volume = "));
			Text::SBAppendF64(sb, ReadMInt16(&packBuff[24]) / 256.0);
			sb->AppendC(UTF8STRC("\r\nReserved = "));
			sb->AppendHexBuff(&packBuff[26], 10, ' ', Text::LineBreakType::None);
			sb->AppendC(UTF8STRC("\r\nMatrix:"));
			sb->AppendC(UTF8STRC("\r\na b u   "));
			Text::SBAppendF64(sb, ReadMInt32(&packBuff[36]) / 65536.0);
			sb->AppendC(UTF8STRC(" "));
			Text::SBAppendF64(sb, ReadMInt32(&packBuff[40]) / 65536.0);
			sb->AppendC(UTF8STRC(" "));
			Text::SBAppendF64(sb, ReadMInt32(&packBuff[44]) / 65536.0);
			sb->AppendC(UTF8STRC("\r\nc d v   "));
			Text::SBAppendF64(sb, ReadMInt32(&packBuff[48]) / 65536.0);
			sb->AppendC(UTF8STRC(" "));
			Text::SBAppendF64(sb, ReadMInt32(&packBuff[52]) / 65536.0);
			sb->AppendC(UTF8STRC(" "));
			Text::SBAppendF64(sb, ReadMInt32(&packBuff[56]) / 65536.0);
			sb->AppendC(UTF8STRC("\r\nx y w   "));
			Text::SBAppendF64(sb, ReadMInt32(&packBuff[60]) / 65536.0);
			sb->AppendC(UTF8STRC(" "));
			Text::SBAppendF64(sb, ReadMInt32(&packBuff[64]) / 65536.0);
			sb->AppendC(UTF8STRC(" "));
			Text::SBAppendF64(sb, ReadMInt32(&packBuff[68]) / 65536.0);
			sb->AppendC(UTF8STRC("\r\nPreview time = "));
			sb->AppendI32(ReadMInt32(&packBuff[72]));
			sb->AppendC(UTF8STRC("\r\nPreview duration = "));
			sb->AppendI32(ReadMInt32(&packBuff[76]));
			sb->AppendC(UTF8STRC("\r\nPoster time = "));
			sb->AppendI32(ReadMInt32(&packBuff[80]));
			sb->AppendC(UTF8STRC("\r\nSelection time = "));
			sb->AppendI32(ReadMInt32(&packBuff[84]));
			sb->AppendC(UTF8STRC("\r\nSelection duration = "));
			sb->AppendI32(ReadMInt32(&packBuff[88]));
			sb->AppendC(UTF8STRC("\r\nCurrent time = "));
			sb->AppendI32(ReadMInt32(&packBuff[92]));
			sb->AppendC(UTF8STRC("\r\nNext track ID = "));
			sb->AppendI32(ReadMInt32(&packBuff[96]));

			MemFree(packBuff);
		}
	}
	else if (pack->packType == *(Int32*)"tkhd")
	{
		if (pack->packSize >= 92)
		{
			Data::DateTime dt;
			packBuff = MemAlloc(UInt8, (UOSInt)(pack->packSize - 8));
			this->fd->GetRealData(pack->fileOfst + 8, (UOSInt)(pack->packSize - 8), packBuff);

			sb->AppendC(UTF8STRC("\r\nVersion = "));
			sb->AppendU16(packBuff[0]);
			sb->AppendC(UTF8STRC("\r\nFlags = "));
			sb->AppendHex24(ReadMUInt32(&packBuff[0]));
			sb->AppendC(UTF8STRC("\r\nCreation time = "));
			dt.SetUnixTimestamp(ReadMUInt32(&packBuff[4]));
			sb->AppendDate(&dt);
			sb->AppendC(UTF8STRC("\r\nModification time = "));
			dt.SetUnixTimestamp(ReadMUInt32(&packBuff[8]));
			sb->AppendDate(&dt);
			sb->AppendC(UTF8STRC("\r\nTrack ID = "));
			sb->AppendI32(ReadMInt32(&packBuff[12]));
			sb->AppendC(UTF8STRC("\r\nReserved = "));
			sb->AppendI32(ReadMInt32(&packBuff[16]));
			sb->AppendC(UTF8STRC("\r\nDuration = "));
			sb->AppendI32(ReadMInt32(&packBuff[20]));
			sb->AppendC(UTF8STRC("\r\nReserved = "));
			sb->AppendI64(ReadMInt64(&packBuff[24]));
			sb->AppendC(UTF8STRC("\r\nLayer = "));
			sb->AppendI16(ReadMInt16(&packBuff[32]));
			sb->AppendC(UTF8STRC("\r\nAlternate group = "));
			sb->AppendI16(ReadMInt16(&packBuff[34]));
			sb->AppendC(UTF8STRC("\r\nVolume = "));
			sb->AppendI16(ReadMInt16(&packBuff[36]));
			sb->AppendC(UTF8STRC("\r\nReserved = "));
			sb->AppendI16(ReadMInt16(&packBuff[38]));
			sb->AppendC(UTF8STRC("\r\nMatrix:"));
			sb->AppendC(UTF8STRC("\r\na b u   "));
			Text::SBAppendF64(sb, ReadMInt32(&packBuff[40]) / 65536.0);
			sb->AppendC(UTF8STRC(" "));
			Text::SBAppendF64(sb, ReadMInt32(&packBuff[44]) / 65536.0);
			sb->AppendC(UTF8STRC(" "));
			Text::SBAppendF64(sb, ReadMInt32(&packBuff[48]) / 65536.0);
			sb->AppendC(UTF8STRC("\r\nc d v   "));
			Text::SBAppendF64(sb, ReadMInt32(&packBuff[52]) / 65536.0);
			sb->AppendC(UTF8STRC(" "));
			Text::SBAppendF64(sb, ReadMInt32(&packBuff[56]) / 65536.0);
			sb->AppendC(UTF8STRC(" "));
			Text::SBAppendF64(sb, ReadMInt32(&packBuff[60]) / 65536.0);
			sb->AppendC(UTF8STRC("\r\nx y w   "));
			Text::SBAppendF64(sb, ReadMInt32(&packBuff[64]) / 65536.0);
			sb->AppendC(UTF8STRC(" "));
			Text::SBAppendF64(sb, ReadMInt32(&packBuff[68]) / 65536.0);
			sb->AppendC(UTF8STRC(" "));
			Text::SBAppendF64(sb, ReadMInt32(&packBuff[72]) / 65536.0);
			sb->AppendC(UTF8STRC("\r\nTrack Width = "));
			Text::SBAppendF64(sb, ReadMInt32(&packBuff[76]) / 65536.0);
			sb->AppendC(UTF8STRC("\r\nTrack Height = "));
			Text::SBAppendF64(sb, ReadMInt32(&packBuff[80]) / 65536.0);

			MemFree(packBuff);
		}
	}
	else if (pack->packType == *(Int32*)"mdhd")
	{
		if (pack->packSize >= 32)
		{
			Data::DateTime dt;
			packBuff = MemAlloc(UInt8, (UOSInt)(pack->packSize - 8));
			this->fd->GetRealData(pack->fileOfst + 8, (UOSInt)(pack->packSize - 8), packBuff);

			sb->AppendC(UTF8STRC("\r\nVersion = "));
			sb->AppendU16(packBuff[0]);
			sb->AppendC(UTF8STRC("\r\nFlags = "));
			sb->AppendHex24(ReadMUInt32(&packBuff[0]));
			sb->AppendC(UTF8STRC("\r\nCreation time = "));
			dt.SetUnixTimestamp(ReadMUInt32(&packBuff[4]));
			sb->AppendDate(&dt);
			sb->AppendC(UTF8STRC("\r\nModification time = "));
			dt.SetUnixTimestamp(ReadMUInt32(&packBuff[8]));
			sb->AppendDate(&dt);
			sb->AppendC(UTF8STRC("\r\nTime scale = "));
			sb->AppendI32(ReadMInt32(&packBuff[12]));
			sb->AppendC(UTF8STRC("\r\nDuration = "));
			sb->AppendI32(ReadMInt32(&packBuff[16]));
			sb->AppendC(UTF8STRC("\r\nLanguage = "));
			sb->AppendI16(ReadMInt16(&packBuff[20]));
			sb->AppendC(UTF8STRC("\r\nQuality = "));
			sb->AppendI16(ReadMInt16(&packBuff[22]));

			MemFree(packBuff);
		}
	}
	else if (pack->packType == *(Int32*)"hdlr")
	{
		if (pack->packSize >= 32)
		{
			packBuff = MemAlloc(UInt8, (UOSInt)(pack->packSize - 8));
			this->fd->GetRealData(pack->fileOfst + 8, (UOSInt)(pack->packSize - 8), packBuff);

			sb->AppendC(UTF8STRC("\r\nVersion = "));
			sb->AppendU16(packBuff[0]);
			sb->AppendC(UTF8STRC("\r\nFlags = "));
			sb->AppendHex24(ReadMUInt32(&packBuff[0]));
			sb->AppendC(UTF8STRC("\r\nComponent type = "));
			*(Int32*)buff = *(Int32*)&packBuff[4];
			sb->AppendSlow((UTF8Char*)buff);
			sb->AppendC(UTF8STRC("\r\nComponent subtype = "));
			*(Int32*)buff = *(Int32*)&packBuff[8];
			sb->AppendSlow((UTF8Char*)buff);
			sb->AppendC(UTF8STRC("\r\nComponent manufacturer = "));
			sb->AppendI32(ReadMInt32(&packBuff[12]));
			sb->AppendC(UTF8STRC("\r\nComponent flags = "));
			sb->AppendI32(ReadMInt32(&packBuff[16]));
			sb->AppendC(UTF8STRC("\r\nComponent flags mask = "));
			sb->AppendI32(ReadMInt32(&packBuff[20]));
			sb->AppendC(UTF8STRC("\r\nComponent name = "));
			sb->AppendC((UTF8Char*)&packBuff[24], (UOSInt)(pack->packSize - 32));

			MemFree(packBuff);
		}
	}
	else if (pack->packType == *(Int32*)"vmhd")
	{
		if (pack->packSize >= 20)
		{
			packBuff = MemAlloc(UInt8, (UOSInt)(pack->packSize - 8));
			this->fd->GetRealData(pack->fileOfst + 8, (UOSInt)(pack->packSize - 8), packBuff);

			sb->AppendC(UTF8STRC("\r\nVersion = "));
			sb->AppendU16(packBuff[0]);
			sb->AppendC(UTF8STRC("\r\nFlags = "));
			sb->AppendHex24(ReadMUInt32(&packBuff[0]));
			sb->AppendC(UTF8STRC("\r\nGraphic mode = "));
			sb->AppendU16(ReadMUInt16(&packBuff[4]));
			sb->AppendC(UTF8STRC("\r\nOpcolor Red = "));
			sb->AppendU16(ReadMUInt16(&packBuff[6]));
			sb->AppendC(UTF8STRC("\r\nOpcolor Green = "));
			sb->AppendU16(ReadMUInt16(&packBuff[8]));
			sb->AppendC(UTF8STRC("\r\nOpcolor Blue = "));
			sb->AppendU16(ReadMUInt16(&packBuff[10]));

			MemFree(packBuff);
		}
	}
	else if (pack->packType == *(Int32*)"smhd")
	{
		if (pack->packSize >= 16)
		{
			packBuff = MemAlloc(UInt8, (UOSInt)(pack->packSize - 8));
			this->fd->GetRealData(pack->fileOfst + 8, (UOSInt)(pack->packSize - 8), packBuff);

			sb->AppendC(UTF8STRC("\r\nVersion = "));
			sb->AppendU16(packBuff[0]);
			sb->AppendC(UTF8STRC("\r\nFlags = "));
			sb->AppendHex24(ReadMUInt32(&packBuff[0]));
			sb->AppendC(UTF8STRC("\r\nBalance = "));
			sb->AppendU16(ReadMUInt16(&packBuff[4]));
			sb->AppendC(UTF8STRC("\r\nReserved = "));
			sb->AppendU16(ReadMUInt16(&packBuff[6]));

			MemFree(packBuff);
		}
	}
	else if (pack->packType == *(Int32*)"dref")
	{
		if (pack->packSize >= 16)
		{
			packBuff = MemAlloc(UInt8, (UOSInt)(pack->packSize - 8));
			this->fd->GetRealData(pack->fileOfst + 8, (UOSInt)(pack->packSize - 8), packBuff);

			sb->AppendC(UTF8STRC("\r\nVersion = "));
			sb->AppendU16(packBuff[0]);
			sb->AppendC(UTF8STRC("\r\nFlags = "));
			sb->AppendHex24(ReadMUInt32(&packBuff[0]));
			sb->AppendC(UTF8STRC("\r\nNumber of entries = "));
			sb->AppendU32((UInt32)(j = ReadMUInt32(&packBuff[4])));
			k = 8;
			i = 0;
			while (i < j)
			{
				l = ReadMUInt32(&packBuff[k]);
				if (l < 12 || (UInt32)(l + k) > pack->packSize - 8)
				{
					break;
				}
				sb->AppendC(UTF8STRC("\r\n-Type = "));
				*(Int32*)buff = *(Int32*)&packBuff[k + 4];
				sb->AppendSlow((UTF8Char*)buff);
				sb->AppendC(UTF8STRC("\r\n-Version = "));
				sb->AppendU16(packBuff[k + 8]);
				sb->AppendC(UTF8STRC("\r\n-Flags = "));
				sb->AppendHex24(ReadMUInt32(&packBuff[k + 8]));
				sb->AppendC(UTF8STRC("\r\nData:\r\n"));
				sb->AppendHexBuff(&packBuff[k + 12], l - 12, ' ', Text::LineBreakType::CRLF);
				k += l;
				i++;
			}

			MemFree(packBuff);
		}
	}
	else if (pack->packType == *(Int32*)"stsd")
	{
		if (pack->packSize >= 16)
		{
			packBuff = MemAlloc(UInt8, (UOSInt)(pack->packSize - 8));
			this->fd->GetRealData(pack->fileOfst + 8, (UOSInt)(pack->packSize - 8), packBuff);

			sb->AppendC(UTF8STRC("\r\nVersion = "));
			sb->AppendU16(packBuff[0]);
			sb->AppendC(UTF8STRC("\r\nFlags = "));
			sb->AppendHex24(ReadMUInt32(&packBuff[0]));
			sb->AppendC(UTF8STRC("\r\nNumber of entries = "));
			sb->AppendU32((UInt32)(j = ReadMUInt32(&packBuff[4])));
			k = 8;
			i = 0;
			while (i < j)
			{
				Int32 dataType = 0;
				l = ReadMUInt32(&packBuff[k]);
				if (l < 12 || (l + k) > pack->packSize - 8)
				{
					break;
				}
				sb->AppendC(UTF8STRC("\r\n-Data format = "));
				WriteNInt32(buff, ReadNInt32(&packBuff[k + 4]));
				sb->AppendSlow((UTF8Char*)buff);
				if (ReadNInt32(&packBuff[k + 4]) == *(Int32*)"mp4a")
				{
					dataType = 2;
				}
				sb->AppendC(UTF8STRC("\r\n-Reserved = "));
				sb->AppendHexBuff(&packBuff[k + 8], 6, ' ', Text::LineBreakType::None);
				sb->AppendC(UTF8STRC("\r\n-Data reference index = "));
				sb->AppendHex16(ReadMUInt16(&packBuff[k + 14]));
				if (dataType == 2)
				{
					UInt16 version = ReadMUInt16(&packBuff[k + 16]);
					UOSInt endOfst;
					sb->AppendC(UTF8STRC("\r\nVersion = "));
					sb->AppendU16(version);
					sb->AppendC(UTF8STRC("\r\nRevision level = "));
					sb->AppendU16(ReadMUInt16(&packBuff[k + 18]));
					sb->AppendC(UTF8STRC("\r\nVendor = "));
					sb->AppendU32(ReadMUInt32(&packBuff[k + 20]));
					if (version == 0)
					{
						sb->AppendC(UTF8STRC("\r\nNumber of channels = "));
						sb->AppendU16(ReadMUInt16(&packBuff[k + 24]));
						sb->AppendC(UTF8STRC("\r\nSample size (bits) = "));
						sb->AppendU16(ReadMUInt16(&packBuff[k + 26]));
						sb->AppendC(UTF8STRC("\r\nCompression ID = "));
						sb->AppendI16(ReadMInt16(&packBuff[k + 28]));
						sb->AppendC(UTF8STRC("\r\nPacket size = "));
						sb->AppendI16(ReadMInt16(&packBuff[k + 30]));
						sb->AppendC(UTF8STRC("\r\nSample rate = "));
						Text::SBAppendF64(sb, ReadMUInt32(&packBuff[k + 32]) / 65536.0);
						endOfst = 36;
					}
					else if (version == 1)
					{
						sb->AppendC(UTF8STRC("\r\nNumber of channels = "));
						sb->AppendU16(ReadMUInt16(&packBuff[k + 24]));
						sb->AppendC(UTF8STRC("\r\nSample size (bits) = "));
						sb->AppendU16(ReadMUInt16(&packBuff[k + 26]));
						sb->AppendC(UTF8STRC("\r\nCompression ID = "));
						sb->AppendI16(ReadMInt16(&packBuff[k + 28]));
						sb->AppendC(UTF8STRC("\r\nPacket size = "));
						sb->AppendI16(ReadMInt16(&packBuff[k + 30]));
						sb->AppendC(UTF8STRC("\r\nSample rate = "));
						Text::SBAppendF64(sb, ReadMUInt32(&packBuff[k + 32]) / 65536.0);
						sb->AppendC(UTF8STRC("\r\nSamples per packet = "));
						sb->AppendU32(ReadMUInt32(&packBuff[k + 36]));
						sb->AppendC(UTF8STRC("\r\nBytes per packet = "));
						sb->AppendU32(ReadMUInt32(&packBuff[k + 40]));
						sb->AppendC(UTF8STRC("\r\nBytes per frame = "));
						sb->AppendU32(ReadMUInt32(&packBuff[k + 44]));
						sb->AppendC(UTF8STRC("\r\nBytes per sample = "));
						sb->AppendU32(ReadMUInt32(&packBuff[k + 48]));
						endOfst = 52;
					}
					else if (version == 2)
					{
						sb->AppendC(UTF8STRC("\r\nalways3 = "));
						sb->AppendU16(ReadMUInt16(&packBuff[k + 24]));
						sb->AppendC(UTF8STRC("\r\nalways16 = "));
						sb->AppendU16(ReadMUInt16(&packBuff[k + 26]));
						sb->AppendC(UTF8STRC("\r\nalwaysMinus2 = "));
						sb->AppendI16(ReadMInt16(&packBuff[k + 28]));
						sb->AppendC(UTF8STRC("\r\nalways0 = "));
						sb->AppendI16(ReadMInt16(&packBuff[k + 30]));
						sb->AppendC(UTF8STRC("\r\nalways65536 = "));
						sb->AppendU32(ReadMUInt32(&packBuff[k + 32]));
						sb->AppendC(UTF8STRC("\r\nsizeOfStructOnly = "));
						sb->AppendU32(ReadMUInt32(&packBuff[k + 36]));
						sb->AppendC(UTF8STRC("\r\naudioSampleRate = "));
						Text::SBAppendF64(sb, ReadMDouble(&packBuff[k + 40]));
						sb->AppendC(UTF8STRC("\r\nnumAudioChannels = "));
						sb->AppendU32(ReadMUInt32(&packBuff[k + 48]));
						sb->AppendC(UTF8STRC("\r\nalways7F000000 = "));
						sb->AppendU32(ReadMUInt32(&packBuff[k + 52]));
						sb->AppendC(UTF8STRC("\r\nconstBitsPerChannel = "));
						sb->AppendU32(ReadMUInt32(&packBuff[k + 56]));
						sb->AppendC(UTF8STRC("\r\nformatSpecificFlags = 0x"));
						sb->AppendHex32(ReadMUInt32(&packBuff[k + 60]));
						sb->AppendC(UTF8STRC("\r\nconstBytesPerAudioPacket = "));
						sb->AppendU32(ReadMUInt32(&packBuff[k + 64]));
						sb->AppendC(UTF8STRC("\r\nconstLPCMFramesPerAudioPacket = "));
						sb->AppendU32(ReadMUInt32(&packBuff[k + 68]));
						endOfst = 72;
					}
					else
					{
						endOfst = 16;
					}
					if (endOfst < l)
					{
						sb->AppendC(UTF8STRC("\r\nExtra Data:\r\n"));
						sb->AppendHexBuff(&packBuff[k + endOfst], l - endOfst, ' ', Text::LineBreakType::CRLF);
					}
				}
				else
				{
					sb->AppendC(UTF8STRC("\r\nData:\r\n"));
					sb->AppendHexBuff(&packBuff[k + 16], l - 16, ' ', Text::LineBreakType::CRLF);
				}
				k += l;
				i++;
			}

			MemFree(packBuff);
		}
	}
	else if (pack->packType == *(Int32*)"stts")
	{
		if (pack->packSize >= 16)
		{
			packBuff = MemAlloc(UInt8, (UOSInt)(pack->packSize - 8));
			this->fd->GetRealData(pack->fileOfst + 8, (UOSInt)(pack->packSize - 8), packBuff);

			sb->AppendC(UTF8STRC("\r\nVersion = "));
			sb->AppendU16(packBuff[0]);
			sb->AppendC(UTF8STRC("\r\nFlags = "));
			sb->AppendHex24(ReadMUInt32(&packBuff[0]));
			sb->AppendC(UTF8STRC("\r\nNumber of entries = "));
			sb->AppendU32((UInt32)(j = ReadMUInt32(&packBuff[4])));
			k = 8;
			i = 0;
			while (i < j)
			{
				if ((UInt32)(k + 8) > pack->packSize - 8)
				{
					break;
				}
				sb->AppendC(UTF8STRC("\r\n-Sample Count = "));
				sb->AppendI32(ReadMInt32(&packBuff[k + 0]));
				sb->AppendC(UTF8STRC(", Sample duration = "));
				sb->AppendI32(ReadMInt32(&packBuff[k + 4]));
				k += 8;
				i++;
			}

			MemFree(packBuff);
		}
	}
	else if (pack->packType == *(Int32*)"ctts")
	{
		if (pack->packSize >= 16)
		{
			packBuff = MemAlloc(UInt8, (UOSInt)(pack->packSize - 8));
			this->fd->GetRealData(pack->fileOfst + 8, (UOSInt)(pack->packSize - 8), packBuff);

			sb->AppendC(UTF8STRC("\r\nVersion = "));
			sb->AppendU16(packBuff[0]);
			sb->AppendC(UTF8STRC("\r\nFlags = "));
			sb->AppendHex24(ReadMUInt32(&packBuff[0]));
			sb->AppendC(UTF8STRC("\r\nEntry count = "));
			sb->AppendU32((UInt32)(j = ReadMUInt32(&packBuff[4])));
			if (j > 3000)
			{
				j = 3000;
			}
			k = 8;
			i = 0;
			while (i < j)
			{
				if ((UInt32)(k + 8) > pack->packSize - 8)
				{
					break;
				}
				sb->AppendC(UTF8STRC("\r\n-Sample Count = "));
				sb->AppendI32(ReadMInt32(&packBuff[k + 0]));
				sb->AppendC(UTF8STRC(", compositionOffset = "));
				sb->AppendI32(ReadMInt32(&packBuff[k + 4]));
				k += 8;
				i++;
			}

			MemFree(packBuff);
		}
	}
	else if (pack->packType == *(Int32*)"stss")
	{
		if (pack->packSize >= 16)
		{
			packBuff = MemAlloc(UInt8, (UOSInt)(pack->packSize - 8));
			this->fd->GetRealData(pack->fileOfst + 8, (UOSInt)(pack->packSize - 8), packBuff);

			sb->AppendC(UTF8STRC("\r\nVersion = "));
			sb->AppendU16(packBuff[0]);
			sb->AppendC(UTF8STRC("\r\nFlags = "));
			sb->AppendHex24(ReadMUInt32(&packBuff[0]));
			sb->AppendC(UTF8STRC("\r\nNumber of entries = "));
			sb->AppendU32((UInt32)(j = ReadMUInt32(&packBuff[4])));
			if (j > 3000)
			{
				j = 3000;
			}
			k = 8;
			i = 0;
			while (i < j)
			{
				if ((UInt32)(k + 4) > pack->packSize - 8)
				{
					break;
				}
				sb->AppendC(UTF8STRC("\r\n-Key "));
				sb->AppendI32((Int32)i);
				sb->AppendC(UTF8STRC(" = "));
				sb->AppendI32(ReadMInt32(&packBuff[k + 0]));
				k += 4;
				i++;
			}

			MemFree(packBuff);
		}
	}
	else if (pack->packType == *(Int32*)"stsc")
	{
		if (pack->packSize >= 16)
		{
			packBuff = MemAlloc(UInt8, (UOSInt)(pack->packSize - 8));
			this->fd->GetRealData(pack->fileOfst + 8, (UOSInt)(pack->packSize - 8), packBuff);

			sb->AppendC(UTF8STRC("\r\nVersion = "));
			sb->AppendU16(packBuff[0]);
			sb->AppendC(UTF8STRC("\r\nFlags = "));
			sb->AppendHex24(ReadMUInt32(&packBuff[0]));
			sb->AppendC(UTF8STRC("\r\nNumber of entries = "));
			sb->AppendU32((UInt32)(j = ReadMUInt32(&packBuff[4])));
			if (j > 3000)
			{
				j = 3000;
			}
			k = 8;
			i = 0;
			while (i < j)
			{
				if ((UInt32)(k + 12) > pack->packSize - 8)
				{
					break;
				}
				sb->AppendC(UTF8STRC("\r\n-First chunk = "));
				sb->AppendI32(ReadMInt32(&packBuff[k + 0]));
				sb->AppendC(UTF8STRC(", Samples per chunk = "));
				sb->AppendI32(ReadMInt32(&packBuff[k + 4]));
				sb->AppendC(UTF8STRC(", Sample description ID = "));
				sb->AppendI32(ReadMInt32(&packBuff[k + 8]));
				k += 12;
				i++;
			}

			MemFree(packBuff);
		}
	}
	else if (pack->packType == *(Int32*)"stsz")
	{
		if (pack->packSize >= 20)
		{
			packBuff = MemAlloc(UInt8, (UOSInt)(pack->packSize - 8));
			this->fd->GetRealData(pack->fileOfst + 8, (UOSInt)(pack->packSize - 8), packBuff);

			sb->AppendC(UTF8STRC("\r\nVersion = "));
			sb->AppendU16(packBuff[0]);
			sb->AppendC(UTF8STRC("\r\nFlags = "));
			sb->AppendHex24(ReadMUInt32(&packBuff[0]));
			sb->AppendC(UTF8STRC("\r\nSample size = "));
			sb->AppendI32(ReadMInt32(&packBuff[4]));
			sb->AppendC(UTF8STRC("\r\nNumber of entries = "));
			sb->AppendU32((UInt32)(j = ReadMUInt32(&packBuff[8])));
			if (j > 3000)
			{
				j = 3000;
			}
			k = 12;
			i = 0;
			while (i < j)
			{
				if ((UInt32)(k + 4) > pack->packSize - 8)
				{
					break;
				}
				sb->AppendC(UTF8STRC("\r\n-Sample size "));
				sb->AppendI32((Int32)i);
				sb->AppendC(UTF8STRC(" = "));
				sb->AppendI32(ReadMInt32(&packBuff[k + 0]));
				k += 4;
				i++;
			}

			MemFree(packBuff);
		}
	}
	else if (pack->packType == *(Int32*)"stco")
	{
		if (pack->packSize >= 20)
		{
			packBuff = MemAlloc(UInt8, (UOSInt)(pack->packSize - 8));
			this->fd->GetRealData(pack->fileOfst + 8, (UOSInt)(pack->packSize - 8), packBuff);

			sb->AppendC(UTF8STRC("\r\nVersion = "));
			sb->AppendU16(packBuff[0]);
			sb->AppendC(UTF8STRC("\r\nFlags = "));
			sb->AppendHex24(ReadMUInt32(&packBuff[0]));
			sb->AppendC(UTF8STRC("\r\nNumber of entries = "));
			sb->AppendU32((UInt32)(j = ReadMUInt32(&packBuff[4])));
			if (j > 3000)
			{
				j = 3000;
			}
			k = 8;
			i = 0;
			while (i < j)
			{
				if ((UInt32)(k + 4) > pack->packSize - 8)
				{
					break;
				}
				sb->AppendC(UTF8STRC("\r\n-Offset "));
				sb->AppendI32((Int32)i);
				sb->AppendC(UTF8STRC(" = "));
				sb->AppendI32(ReadMInt32(&packBuff[k + 0]));
				k += 4;
				i++;
			}

			MemFree(packBuff);
		}
	}
	else if (pack->packType == *(Int32*)"co64")
	{
		if (pack->packSize >= 20)
		{
			packBuff = MemAlloc(UInt8, (UOSInt)(pack->packSize - 8));
			this->fd->GetRealData(pack->fileOfst + 8, (UOSInt)(pack->packSize - 8), packBuff);

			sb->AppendC(UTF8STRC("\r\nVersion = "));
			sb->AppendU16(packBuff[0]);
			sb->AppendC(UTF8STRC("\r\nFlags = "));
			sb->AppendHex24(ReadMUInt32(&packBuff[0]));
			sb->AppendC(UTF8STRC("\r\nNumber of entries = "));
			sb->AppendU32((UInt32)(j = ReadMUInt32(&packBuff[4])));
			if (j > 3000)
			{
				j = 3000;
			}
			k = 8;
			i = 0;
			while (i < j)
			{
				if ((UInt32)(k + 8) > pack->packSize - 8)
				{
					break;
				}
				sb->AppendC(UTF8STRC("\r\n-Offset "));
				sb->AppendI32((Int32)i);
				sb->AppendC(UTF8STRC(" = "));
				sb->AppendI64(ReadMInt64(&packBuff[k + 0]));
				k += 8;
				i++;
			}

			MemFree(packBuff);
		}
	}
	return true;
}

UOSInt IO::FileAnalyse::QTFileAnalyse::GetFrameIndex(UInt64 ofst)
{
	UOSInt ret;
	UOSInt i = this->maxLev;
	while (true)
	{
		ret = this->GetFrameIndex(i, ofst);
		if (ret != INVALID_INDEX)
		{
			return ret;
		}
		if (i == 0)
		{
			return INVALID_INDEX;
		}
		i--;
	}
}

IO::FileAnalyse::FrameDetail *IO::FileAnalyse::QTFileAnalyse::GetFrameDetail(UOSInt index)
{
	IO::FileAnalyse::FrameDetail *frame;
	IO::FileAnalyse::QTFileAnalyse::PackInfo *pack;
	UInt8 *packBuff;
	UTF8Char sbuff[64];
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UOSInt l;
	pack = this->packs->GetItem(index);
	if (pack == 0)
		return 0;

	NEW_CLASS(frame, IO::FileAnalyse::FrameDetail(pack->fileOfst, (UInt32)pack->packSize));
	frame->AddStrS(0, 4, "Type", (const UTF8Char*)&pack->packType);
	if (pack->packSize >= 0x100000000)
	{
		frame->AddUInt64V(4, 4, "Size", 1);
		frame->AddUInt64(8, "Size64", pack->packSize);
	}
	else
	{
		frame->AddUInt64V(4, 4, "Size", pack->packSize);
	}

	if (pack->packType == *(Int32*)"dinf" ||
		pack->packType == *(Int32*)"mdia" ||
		pack->packType == *(Int32*)"minf" ||
		pack->packType == *(Int32*)"moov" ||
		pack->packType == *(Int32*)"stbl" ||
		pack->packType == *(Int32*)"trak")
	{
		if (pack->packSize >= 0x100000000)
		{
			frame->AddSubframe(16, pack->packSize - 16);
		}
		else
		{
			frame->AddSubframe(8, pack->packSize - 8);
		}
	}
	else if (pack->packType == *(Int32*)"ftyp")
	{
		packBuff = MemAlloc(UInt8, (UOSInt)(pack->packSize - 8));
		this->fd->GetRealData(pack->fileOfst + 8, (UOSInt)pack->packSize - 8, packBuff);
		frame->AddStrS(8, 4, "Major_Brand", &packBuff[0]);
		frame->AddHex32(12, "Minor_Version", ReadMUInt32(&packBuff[4]));
		i = 8;
		j = (UOSInt)(pack->packSize - 8);
		while (i < j)
		{
			Text::StrConcat(Text::StrUOSInt(Text::StrConcat(sbuff, (const UTF8Char*)"Compatible_Brands["), i), (const UTF8Char*)"]");
			frame->AddStrS(i + 8, 4, (const Char*)sbuff, &packBuff[i]);
			i += 4;
		}
		MemFree(packBuff);
	}
	else if (pack->packType == *(Int32*)"free")
	{
		if (pack->packSize > 8)
		{
			packBuff = MemAlloc(UInt8, (UOSInt)(pack->packSize - 8));
			this->fd->GetRealData(pack->fileOfst + 8, (UOSInt)(pack->packSize - 8), packBuff);
			frame->AddHexBuff(8, (UOSInt)(pack->packSize - 8), "Data", packBuff, true);
			MemFree(packBuff);
		}
	}
	else if (pack->packType == *(Int32*)"pnot")
	{
		if (pack->packSize >= 20)
		{
			packBuff = MemAlloc(UInt8, (UOSInt)(pack->packSize - 8));
			this->fd->GetRealData(pack->fileOfst + 8, (UOSInt)(pack->packSize - 8), packBuff);
			frame->AddInt(8, 4, "Modification date", ReadMInt32(&packBuff[0]));
			frame->AddUInt(12, 2, "Version number", ReadMUInt16(&packBuff[4]));
			frame->AddStrS(14, 4, "Atom type", &packBuff[6]);
			frame->AddInt(18, 2, "Atom Index", ReadMUInt16(&packBuff[10]));
			MemFree(packBuff);
		}
	}
	else if (pack->packType == *(Int32*)"mvhd")
	{
		if (pack->packSize >= 108)
		{
			Data::DateTime dt;
			packBuff = MemAlloc(UInt8, (UOSInt)(pack->packSize - 8));
			this->fd->GetRealData(pack->fileOfst + 8, (UOSInt)(pack->packSize - 8), packBuff);
			frame->AddUInt(8, 1, "Version", packBuff[0]);
			frame->AddHex24(9, "Flags", ReadMUInt24(&packBuff[1]));
			dt.SetUnixTimestamp(ReadMUInt32(&packBuff[4]));
			dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss");
			frame->AddField(12, 4, (const UTF8Char*)"Creation time", sbuff);
			dt.SetUnixTimestamp(ReadMUInt32(&packBuff[8]));
			dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss");
			frame->AddField(16, 4, (const UTF8Char*)"Modification time", sbuff);
			frame->AddUInt(20, 4, "Time Scale", ReadMUInt32(&packBuff[12]));
			frame->AddUInt(24, 4, "Duration", ReadMUInt32(&packBuff[16]));
			frame->AddFloat(28, 4, "Preferred rate", ReadMInt32(&packBuff[20]) / 65536.0);
			frame->AddFloat(32, 2, "Preferred volume", ReadMInt16(&packBuff[24]) / 256.0);
			frame->AddHexBuff(34, 10, "Reserved", &packBuff[26], false);
			Text::StringBuilderUTF8 sb;
			sb.AppendC(UTF8STRC("a b u   "));
			Text::SBAppendF64(&sb, ReadMInt32(&packBuff[36]) / 65536.0);
			sb.AppendC(UTF8STRC(" "));
			Text::SBAppendF64(&sb, ReadMInt32(&packBuff[40]) / 65536.0);
			sb.AppendC(UTF8STRC(" "));
			Text::SBAppendF64(&sb, ReadMInt32(&packBuff[44]) / 65536.0);
			sb.AppendC(UTF8STRC("\r\nc d v   "));
			Text::SBAppendF64(&sb, ReadMInt32(&packBuff[48]) / 65536.0);
			sb.AppendC(UTF8STRC(" "));
			Text::SBAppendF64(&sb, ReadMInt32(&packBuff[52]) / 65536.0);
			sb.AppendC(UTF8STRC(" "));
			Text::SBAppendF64(&sb, ReadMInt32(&packBuff[56]) / 65536.0);
			sb.AppendC(UTF8STRC("\r\nx y w   "));
			Text::SBAppendF64(&sb, ReadMInt32(&packBuff[60]) / 65536.0);
			sb.AppendC(UTF8STRC(" "));
			Text::SBAppendF64(&sb, ReadMInt32(&packBuff[64]) / 65536.0);
			sb.AppendC(UTF8STRC(" "));
			Text::SBAppendF64(&sb, ReadMInt32(&packBuff[68]) / 65536.0);
			frame->AddField(44, 36, (const UTF8Char*)"Matrix", sb.ToString());
			frame->AddUInt(80, 4, "Preview time", ReadMUInt32(&packBuff[72]));
			frame->AddUInt(84, 4, "Preview duration", ReadMUInt32(&packBuff[76]));
			frame->AddUInt(88, 4, "Poster time", ReadMUInt32(&packBuff[80]));
			frame->AddUInt(92, 4, "Selection time", ReadMUInt32(&packBuff[84]));
			frame->AddUInt(96, 4, "Selection duration", ReadMUInt32(&packBuff[88]));
			frame->AddUInt(100, 4, "Current time", ReadMUInt32(&packBuff[92]));
			frame->AddUInt(104, 4, "Next track ID", ReadMUInt32(&packBuff[96]));
			MemFree(packBuff);
		}
	}
	else if (pack->packType == *(Int32*)"tkhd")
	{
		if (pack->packSize >= 92)
		{
			Data::DateTime dt;
			packBuff = MemAlloc(UInt8, (UOSInt)(pack->packSize - 8));
			this->fd->GetRealData(pack->fileOfst + 8, (UOSInt)(pack->packSize - 8), packBuff);
			frame->AddUInt(8, 1, "Version", packBuff[0]);
			frame->AddHex24(9, "Flags", ReadMUInt24(&packBuff[1]));
			dt.SetUnixTimestamp(ReadMUInt32(&packBuff[4]));
			dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss");
			frame->AddField(12, 4, (const UTF8Char*)"Creation time", sbuff);
			dt.SetUnixTimestamp(ReadMUInt32(&packBuff[8]));
			dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss");
			frame->AddField(16, 4, (const UTF8Char*)"Modification time", sbuff);
			frame->AddUInt(20, 4, "Track ID", ReadMUInt32(&packBuff[12]));
			frame->AddUInt(24, 4, "Reserved", ReadMUInt32(&packBuff[16]));
			frame->AddUInt(28, 4, "Duration", ReadMUInt32(&packBuff[20]));
			frame->AddUInt64(32, "Reserved", ReadMUInt64(&packBuff[24]));
			frame->AddUInt(40, 2, "Layer", ReadMUInt16(&packBuff[32]));
			frame->AddUInt(42, 2, "Alternate group", ReadMUInt16(&packBuff[34]));
			frame->AddUInt(44, 2, "Volume", ReadMUInt16(&packBuff[36]));
			frame->AddUInt(46, 2, "Reserved", ReadMUInt16(&packBuff[38]));
			Text::StringBuilderUTF8 sb;
			sb.AppendC(UTF8STRC("a b u   "));
			Text::SBAppendF64(&sb, ReadMInt32(&packBuff[40]) / 65536.0);
			sb.AppendC(UTF8STRC(" "));
			Text::SBAppendF64(&sb, ReadMInt32(&packBuff[44]) / 65536.0);
			sb.AppendC(UTF8STRC(" "));
			Text::SBAppendF64(&sb, ReadMInt32(&packBuff[48]) / 65536.0);
			sb.AppendC(UTF8STRC("\r\nc d v   "));
			Text::SBAppendF64(&sb, ReadMInt32(&packBuff[52]) / 65536.0);
			sb.AppendC(UTF8STRC(" "));
			Text::SBAppendF64(&sb, ReadMInt32(&packBuff[56]) / 65536.0);
			sb.AppendC(UTF8STRC(" "));
			Text::SBAppendF64(&sb, ReadMInt32(&packBuff[60]) / 65536.0);
			sb.AppendC(UTF8STRC("\r\nx y w   "));
			Text::SBAppendF64(&sb, ReadMInt32(&packBuff[64]) / 65536.0);
			sb.AppendC(UTF8STRC(" "));
			Text::SBAppendF64(&sb, ReadMInt32(&packBuff[68]) / 65536.0);
			sb.AppendC(UTF8STRC(" "));
			Text::SBAppendF64(&sb, ReadMInt32(&packBuff[72]) / 65536.0);
			frame->AddField(48, 36, (const UTF8Char*)"Matrix", sb.ToString());
			frame->AddFloat(84, 4, "Track Width", ReadMInt32(&packBuff[76]) / 65536.0);
			frame->AddFloat(88, 4, "Track Height", ReadMInt32(&packBuff[80]) / 65536.0);
			MemFree(packBuff);
		}
	}
	else if (pack->packType == *(Int32*)"mdhd")
	{
		if (pack->packSize >= 32)
		{
			Data::DateTime dt;
			packBuff = MemAlloc(UInt8, (UOSInt)(pack->packSize - 8));
			this->fd->GetRealData(pack->fileOfst + 8, (UOSInt)(pack->packSize - 8), packBuff);
			frame->AddUInt(8, 1, "Version", packBuff[0]);
			frame->AddHex24(9, "Flags", ReadMUInt24(&packBuff[1]));
			dt.SetUnixTimestamp(ReadMUInt32(&packBuff[4]));
			dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss");
			frame->AddField(12, 4, (const UTF8Char*)"Creation time", sbuff);
			dt.SetUnixTimestamp(ReadMUInt32(&packBuff[8]));
			dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss");
			frame->AddField(16, 4, (const UTF8Char*)"Modification time", sbuff);
			frame->AddUInt(20, 4, "Time scale", ReadMUInt32(&packBuff[12]));
			frame->AddUInt(24, 4, "Duration", ReadMUInt32(&packBuff[16]));
			frame->AddUInt(28, 2, "Language", ReadMUInt16(&packBuff[20]));
			frame->AddUInt(30, 2, "Quality", ReadMUInt16(&packBuff[22]));
			MemFree(packBuff);
		}
	}
	else if (pack->packType == *(Int32*)"hdlr")
	{
		if (pack->packSize >= 32)
		{
			packBuff = MemAlloc(UInt8, (UOSInt)(pack->packSize - 8));
			this->fd->GetRealData(pack->fileOfst + 8, (UOSInt)(pack->packSize - 8), packBuff);
			frame->AddUInt(8, 1, "Version", packBuff[0]);
			frame->AddHex24(9, "Flags", ReadMUInt24(&packBuff[1]));
			frame->AddStrS(12, 4, "Component type", &packBuff[4]);
			frame->AddStrS(16, 4, "Component subtype", &packBuff[8]);
			frame->AddUInt(20, 4, "Component manufacturer", ReadMUInt32(&packBuff[12]));
			frame->AddUInt(24, 4, "Component flags", ReadMUInt32(&packBuff[16]));
			frame->AddUInt(28, 4, "Component flags mask", ReadMUInt32(&packBuff[20]));
			frame->AddStrC(32, pack->packSize - 32, "Component name",&packBuff[24]);
			MemFree(packBuff);
		}
	}
	else if (pack->packType == *(Int32*)"vmhd")
	{
		if (pack->packSize >= 20)
		{
			packBuff = MemAlloc(UInt8, (UOSInt)(pack->packSize - 8));
			this->fd->GetRealData(pack->fileOfst + 8, (UOSInt)(pack->packSize - 8), packBuff);
			frame->AddUInt(8, 1, "Version", packBuff[0]);
			frame->AddHex24(9, "Flags", ReadMUInt24(&packBuff[1]));
			frame->AddUInt(12, 2, "Graphic mode", ReadMUInt16(&packBuff[4]));
			frame->AddUInt(14, 2, "Opcolor Red", ReadMUInt16(&packBuff[6]));
			frame->AddUInt(16, 2, "Opcolor Green", ReadMUInt16(&packBuff[8]));
			frame->AddUInt(18, 2, "Opcolor Blue", ReadMUInt16(&packBuff[10]));
			MemFree(packBuff);
		}
	}
	else if (pack->packType == *(Int32*)"smhd")
	{
		if (pack->packSize >= 16)
		{
			packBuff = MemAlloc(UInt8, (UOSInt)(pack->packSize - 8));
			this->fd->GetRealData(pack->fileOfst + 8, (UOSInt)(pack->packSize - 8), packBuff);
			frame->AddUInt(8, 1, "Version", packBuff[0]);
			frame->AddHex24(9, "Flags", ReadMUInt24(&packBuff[1]));
			frame->AddUInt(12, 2, "Balance", ReadMUInt16(&packBuff[4]));
			frame->AddUInt(14, 2, "Reserved", ReadMUInt16(&packBuff[6]));
			MemFree(packBuff);
		}
	}
	else if (pack->packType == *(Int32*)"dref")
	{
		if (pack->packSize >= 16)
		{
			packBuff = MemAlloc(UInt8, (UOSInt)(pack->packSize - 8));
			this->fd->GetRealData(pack->fileOfst + 8, (UOSInt)(pack->packSize - 8), packBuff);
			frame->AddUInt(8, 1, "Version", packBuff[0]);
			frame->AddHex24(9, "Flags", ReadMUInt24(&packBuff[1]));
			frame->AddUInt(12, 4, "Number of entries", j = ReadMUInt32(&packBuff[4]));
			k = 8;
			i = 0;
			while (i < j)
			{
				l = ReadMUInt32(&packBuff[k]);
				if (l < 12 || (UInt32)(l + k) > pack->packSize - 8)
				{
					break;
				}
				frame->AddUInt(k + 8, 4, "Entry Size", l);
				frame->AddStrS(k + 12, 4, "Type", &packBuff[k + 4]);
				frame->AddUInt(k + 16, 1, "Version", packBuff[k + 8]);
				frame->AddHex24(k + 17, "Flags", ReadMUInt24(&packBuff[k + 9]));
				frame->AddHexBuff(k + 20, l - 12, "Entry Data", &packBuff[k + 12], true);
				k += l;
				i++;
			}

			MemFree(packBuff);
		}
	}
	else if (pack->packType == *(Int32*)"stsd")
	{
		if (pack->packSize >= 16)
		{
			packBuff = MemAlloc(UInt8, (UOSInt)(pack->packSize - 8));
			this->fd->GetRealData(pack->fileOfst + 8, (UOSInt)(pack->packSize - 8), packBuff);
			frame->AddUInt(8, 1, "Version", packBuff[0]);
			frame->AddHex24(9, "Flags", ReadMUInt24(&packBuff[1]));
			frame->AddUInt(12, 4, "Number of entries", j = ReadMUInt32(&packBuff[4]));
			k = 8;
			i = 0;
			while (i < j)
			{
				Int32 dataType = 0;
				l = ReadMUInt32(&packBuff[k]);
				if (l < 12 || (l + k) > pack->packSize - 8)
				{
					break;
				}
				frame->AddUInt(k + 8, 4, "Entry Size", l);
				frame->AddStrS(k + 12, 4, "Data format", &packBuff[k + 4]);
				if (ReadNInt32(&packBuff[k + 4]) == *(Int32*)"mp4a")
				{
					dataType = 2;
				}
				frame->AddHexBuff(k + 16, 6, "Reserved", &packBuff[k + 8], false);
				frame->AddHex16(k + 22, "Data reference index", ReadMUInt16(&packBuff[k + 14]));
				if (dataType == 2)
				{
					UInt16 version = ReadMUInt16(&packBuff[k + 16]);
					UOSInt endOfst;
					frame->AddUInt(k + 24, 2, "Version", version);
					frame->AddUInt(k + 26, 2, "Revision level", ReadMUInt16(&packBuff[k + 18]));
					frame->AddUInt(k + 28, 4, "Vendor", ReadMUInt32(&packBuff[k + 20]));
					if (version == 0)
					{
						frame->AddUInt(k + 32, 2, "Number of channels", ReadMUInt16(&packBuff[k + 24]));
						frame->AddUInt(k + 34, 2, "Sample size (bits)", ReadMUInt16(&packBuff[k + 26]));
						frame->AddUInt(k + 36, 2, "Compression ID", ReadMUInt16(&packBuff[k + 28]));
						frame->AddUInt(k + 38, 2, "Packet size", ReadMUInt16(&packBuff[k + 30]));
						frame->AddFloat(k + 40, 4, "Sample rate", ReadMUInt32(&packBuff[k + 32]) / 65536.0);
						endOfst = 36;
					}
					else if (version == 1)
					{
						frame->AddUInt(k + 32, 2, "Number of channels", ReadMUInt16(&packBuff[k + 24]));
						frame->AddUInt(k + 34, 2, "Sample size (bits)", ReadMUInt16(&packBuff[k + 26]));
						frame->AddUInt(k + 36, 2, "Compression ID", ReadMUInt16(&packBuff[k + 28]));
						frame->AddUInt(k + 38, 2, "Packet size", ReadMUInt16(&packBuff[k + 30]));
						frame->AddFloat(k + 40, 4, "Sample rate", ReadMUInt32(&packBuff[k + 32]) / 65536.0);
						frame->AddUInt(k + 44, 4, "Samples per packet", ReadMUInt32(&packBuff[k + 36]));
						frame->AddUInt(k + 48, 4, "Bytes per packet", ReadMUInt32(&packBuff[k + 40]));
						frame->AddUInt(k + 52, 4, "Bytes per frame", ReadMUInt32(&packBuff[k + 44]));
						frame->AddUInt(k + 56, 4, "Bytes per sample", ReadMUInt32(&packBuff[k + 48]));
						endOfst = 52;
					}
					else if (version == 2)
					{
						frame->AddUInt(k + 32, 2, "always3", ReadMUInt16(&packBuff[k + 24]));
						frame->AddUInt(k + 34, 2, "always16", ReadMUInt16(&packBuff[k + 26]));
						frame->AddInt(k + 36, 2, "alwaysMinus2", ReadMInt16(&packBuff[k + 28]));
						frame->AddInt(k + 38, 2, "always0", ReadMInt16(&packBuff[k + 30]));
						frame->AddUInt(k + 40, 4, "always65536", ReadMUInt32(&packBuff[k + 32]));
						frame->AddUInt(k + 44, 4, "sizeOfStructOnly", ReadMUInt32(&packBuff[k + 36]));
						frame->AddFloat(k + 48, 4, "audioSampleRate", ReadMDouble(&packBuff[k + 40]));
						frame->AddUInt(k + 56, 4, "numAudioChannels", ReadMUInt32(&packBuff[k + 48]));
						frame->AddHex32(k + 60, "always7F000000", ReadMUInt32(&packBuff[k + 52]));
						frame->AddUInt(k + 64, 4, "constBitsPerChannel", ReadMUInt32(&packBuff[k + 56]));
						frame->AddHex32(k + 68, "formatSpecificFlags", ReadMUInt32(&packBuff[k + 60]));
						frame->AddUInt(k + 72, 4, "constBytesPerAudioPacket", ReadMUInt32(&packBuff[k + 64]));
						frame->AddUInt(k + 76, 4, "constLPCMFramesPerAudioPacket", ReadMUInt32(&packBuff[k + 68]));
						endOfst = 72;
					}
					else
					{
						endOfst = 16;
					}
					if (endOfst < l)
					{
						frame->AddHexBuff(k + endOfst + 8, l - endOfst, "Extra Data", &packBuff[k + endOfst], true);
					}
				}
				else
				{
					frame->AddHexBuff(k + 16 + 8, l - 16, "Data", &packBuff[k + 16], true);
				}
				k += l;
				i++;
			}

			MemFree(packBuff);
		}
	}
	else if (pack->packType == *(Int32*)"stts")
	{
		if (pack->packSize >= 16)
		{
			packBuff = MemAlloc(UInt8, (UOSInt)(pack->packSize - 8));
			this->fd->GetRealData(pack->fileOfst + 8, (UOSInt)(pack->packSize - 8), packBuff);
			frame->AddUInt(8, 1, "Version", packBuff[0]);
			frame->AddHex24(9, "Flags", ReadMUInt24(&packBuff[1]));
			frame->AddUInt(12, 4, "Number of entries", j = ReadMUInt32(&packBuff[4]));
			k = 8;
			i = 0;
			while (i < j)
			{
				if ((UInt32)(k + 8) > pack->packSize - 8)
				{
					break;
				}
				frame->AddInt(k + 8, 4, "Sample Count", ReadMInt32(&packBuff[k + 0]));
				frame->AddInt(k + 12, 4, "Sample duration", ReadMInt32(&packBuff[k + 4]));
				k += 8;
				i++;
			}

			MemFree(packBuff);
		}
	}
	else if (pack->packType == *(Int32*)"ctts")
	{
		if (pack->packSize >= 16)
		{
			packBuff = MemAlloc(UInt8, (UOSInt)(pack->packSize - 8));
			this->fd->GetRealData(pack->fileOfst + 8, (UOSInt)(pack->packSize - 8), packBuff);
			frame->AddUInt(8, 1, "Version", packBuff[0]);
			frame->AddHex24(9, "Flags", ReadMUInt24(&packBuff[1]));
			frame->AddUInt(12, 4, "Entry count", j = ReadMUInt32(&packBuff[4]));
			if (j > 3000)
			{
				j = 3000;
			}
			k = 8;
			i = 0;
			while (i < j)
			{
				if ((UInt32)(k + 8) > pack->packSize - 8)
				{
					break;
				}
				frame->AddInt(k + 8, 4, "Sample Count", ReadMInt32(&packBuff[k + 0]));
				frame->AddInt(k + 12, 4, "compositionOffset", ReadMInt32(&packBuff[k + 4]));
				k += 8;
				i++;
			}

			MemFree(packBuff);
		}
	}
	else if (pack->packType == *(Int32*)"stss")
	{
		if (pack->packSize >= 16)
		{
			packBuff = MemAlloc(UInt8, (UOSInt)(pack->packSize - 8));
			this->fd->GetRealData(pack->fileOfst + 8, (UOSInt)(pack->packSize - 8), packBuff);
			frame->AddUInt(8, 1, "Version", packBuff[0]);
			frame->AddHex24(9, "Flags", ReadMUInt24(&packBuff[1]));
			frame->AddUInt(12, 4, "Number of entries", j = ReadMUInt32(&packBuff[4]));
			if (j > 3000)
			{
				j = 3000;
			}
			k = 8;
			i = 0;
			while (i < j)
			{
				if ((UInt32)(k + 4) > pack->packSize - 8)
				{
					break;
				}
				Text::StrUOSInt(Text::StrConcat(sbuff, (const UTF8Char*)"Key "), i);
				frame->AddUInt(k + 8, 4, (const Char*)sbuff, ReadMUInt32(&packBuff[k + 0]));
				k += 4;
				i++;
			}

			MemFree(packBuff);
		}
	}
	else if (pack->packType == *(Int32*)"stsc")
	{
		if (pack->packSize >= 16)
		{
			packBuff = MemAlloc(UInt8, (UOSInt)(pack->packSize - 8));
			this->fd->GetRealData(pack->fileOfst + 8, (UOSInt)(pack->packSize - 8), packBuff);
			frame->AddUInt(8, 1, "Version", packBuff[0]);
			frame->AddHex24(9, "Flags", ReadMUInt24(&packBuff[1]));
			frame->AddUInt(12, 4, "Number of entries", j = ReadMUInt32(&packBuff[4]));
			if (j > 3000)
			{
				j = 3000;
			}
			k = 8;
			i = 0;
			while (i < j)
			{
				if ((UInt32)(k + 12) > pack->packSize - 8)
				{
					break;
				}
				frame->AddUInt(k + 8, 4, "First chunk", ReadMUInt32(&packBuff[k + 0]));
				frame->AddUInt(k + 12, 4, "Samples per chunk", ReadMUInt32(&packBuff[k + 4]));
				frame->AddUInt(k + 16, 4, "Sample description ID", ReadMUInt32(&packBuff[k + 8]));
				k += 12;
				i++;
			}

			MemFree(packBuff);
		}
	}
	else if (pack->packType == *(Int32*)"stsz")
	{
		if (pack->packSize >= 20)
		{
			packBuff = MemAlloc(UInt8, (UOSInt)(pack->packSize - 8));
			this->fd->GetRealData(pack->fileOfst + 8, (UOSInt)(pack->packSize - 8), packBuff);
			frame->AddUInt(8, 1, "Version", packBuff[0]);
			frame->AddHex24(9, "Flags", ReadMUInt24(&packBuff[1]));
			frame->AddUInt(12, 4, "Sample size", ReadMUInt32(&packBuff[4]));
			frame->AddUInt(16, 4, "Number of entries", j = ReadMUInt32(&packBuff[8]));
			if (j > 3000)
			{
				j = 3000;
			}
			k = 12;
			i = 0;
			while (i < j)
			{
				if ((UInt32)(k + 4) > pack->packSize - 8)
				{
					break;
				}
				Text::StrUOSInt(Text::StrConcat(sbuff, (const UTF8Char*)"Sample size "), i);
				frame->AddUInt(k + 8, 4, (const Char*)sbuff, ReadMUInt32(&packBuff[k + 0]));
				k += 4;
				i++;
			}

			MemFree(packBuff);
		}
	}
	else if (pack->packType == *(Int32*)"stco")
	{
		if (pack->packSize >= 20)
		{
			packBuff = MemAlloc(UInt8, (UOSInt)(pack->packSize - 8));
			this->fd->GetRealData(pack->fileOfst + 8, (UOSInt)(pack->packSize - 8), packBuff);
			frame->AddUInt(8, 1, "Version", packBuff[0]);
			frame->AddHex24(9, "Flags", ReadMUInt24(&packBuff[1]));
			frame->AddUInt(12, 4, "Number of entries", j = ReadMUInt32(&packBuff[4]));
			if (j > 3000)
			{
				j = 3000;
			}
			k = 8;
			i = 0;
			while (i < j)
			{
				if ((UInt32)(k + 4) > pack->packSize - 8)
				{
					break;
				}
				Text::StrUOSInt(Text::StrConcat(sbuff, (const UTF8Char*)"Offset "), i);
				frame->AddUInt(k + 8, 4, (const Char*)sbuff, ReadMUInt32(&packBuff[k + 0]));
				k += 4;
				i++;
			}

			MemFree(packBuff);
		}
	}
	else if (pack->packType == *(Int32*)"co64")
	{
		if (pack->packSize >= 20)
		{
			packBuff = MemAlloc(UInt8, (UOSInt)(pack->packSize - 8));
			this->fd->GetRealData(pack->fileOfst + 8, (UOSInt)(pack->packSize - 8), packBuff);
			frame->AddUInt(8, 1, "Version", packBuff[0]);
			frame->AddHex24(9, "Flags", ReadMUInt24(&packBuff[1]));
			frame->AddUInt(12, 4, "Number of entries", j = ReadMUInt32(&packBuff[4]));
			if (j > 3000)
			{
				j = 3000;
			}
			k = 8;
			i = 0;
			while (i < j)
			{
				if ((UInt32)(k + 8) > pack->packSize - 8)
				{
					break;
				}
				Text::StrUOSInt(Text::StrConcat(sbuff, (const UTF8Char*)"Offset "), i);
				frame->AddUInt64(k + 8, (const Char*)sbuff, ReadMUInt64(&packBuff[k + 0]));
				k += 8;
				i++;
			}

			MemFree(packBuff);
		}
	}
	return frame;
}

Bool IO::FileAnalyse::QTFileAnalyse::IsError()
{
	return this->fd == 0;
}

Bool IO::FileAnalyse::QTFileAnalyse::IsParsing()
{
	return this->threadRunning;
}

Bool IO::FileAnalyse::QTFileAnalyse::TrimPadding(const UTF8Char *outputFile)
{
/*	UInt8 *readBuff;
	OSInt readSize;
	OSInt buffSize;
	OSInt j;
	OSInt frameSize;
	Int64 readOfst;
	Bool valid = true;
	IO::FileStream *dfs;
	NEW_CLASS(dfs, IO::FileStream(outputFile, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	if (dfs->IsError())
	{
		DEL_CLASS(dfs);
		return false;
	}
	readBuff = MemAlloc(UInt8, 1048576);
	buffSize = 0;
	readOfst = 0;
	while (true)
	{
		if (buffSize < 256)
		{
			readSize = this->fd->GetRealData(readOfst, 256, &readBuff[buffSize]);
			readOfst += readSize;
			buffSize += readSize;
		}
		if (buffSize < 4)
			break;
		j = 0;
		while (readBuff[j] != 0 || readBuff[j + 1] != 0 || readBuff[j + 2] != 1)
		{
			j++;
			if (j >= buffSize - 4)
			{
				break;
			}
		}
		if (j >= buffSize - 4 && buffSize > 4)
		{
			MemCopy(readBuff, &readBuff[j], buffSize - j);
			buffSize -= j;
			continue;
		}
		if (readBuff[j] != 0 || readBuff[j + 1] != 0 || readBuff[j + 2] != 1)
		{
			valid = false;
			break;
		}
		if (readBuff[j + 3] == 0xB9) //End Of File
		{
			dfs->Write(&readBuff[j], 4);
			break;
		}
		if (readBuff[j + 3] == 0xba) 
		{
			if ((readBuff[j + 4] & 0xc0) == 0x40)
			{
				frameSize = 14 + (readBuff[j + 13] & 7);
			}
			else if ((readBuff[j + 4] & 0xf0) == 0x20)
			{
				frameSize = 12;
			}
			else
			{
				valid = false;
				break;
			}
		}
		else
		{
			frameSize = 6 + ReadMUInt16(&readBuff[j + 4]);
		}
		if (j + frameSize <= buffSize)
		{
			dfs->Write(&readBuff[j], frameSize);
			if (j + frameSize < buffSize)
			{
				MemCopy(readBuff, &readBuff[j + frameSize], buffSize - j - frameSize);
				buffSize -= j + frameSize;
			}
			else
			{
				buffSize = 0;
			}
		}
		else
		{
			readSize = this->fd->GetRealData(readOfst, j + frameSize - buffSize, &readBuff[buffSize]);
			readOfst += readSize;
			if (readSize == j + frameSize - buffSize)
			{
				dfs->Write(&readBuff[j], frameSize);
				buffSize = 0;
			}
			else
			{
				valid = false;
				break;
			}
		}
	}

	MemFree(readBuff);
	DEL_CLASS(dfs);
	return valid;*/
	return false;
}
