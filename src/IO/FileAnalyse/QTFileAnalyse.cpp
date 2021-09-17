#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "Data/ArrayListInt32.h"
#include "IO/FileAnalyse/QTFileAnalyse.h"
#include "Sync/Thread.h"
#include "Text/MyStringFloat.h"

void IO::FileAnalyse::QTFileAnalyse::ParseRange(UInt64 ofst, UInt64 size)
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
			pack->fileOfst = ofst;
			pack->packSize = sz;
			pack->packType = *(Int32*)&buff[4];
			this->packs->Add(pack);
			
			if (contList.SortedIndexOf(pack->packType) >= 0)
			{
				this->ParseRange(pack->fileOfst + 8, pack->packSize - 8);
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
	me->ParseRange(0, me->fd->GetDataSize());
	me->threadRunning = false;
	return 0;
}

IO::FileAnalyse::QTFileAnalyse::QTFileAnalyse(IO::IStreamData *fd)
{
	UInt8 buff[8];
	this->fd = 0;
	this->threadRunning = false;
	this->pauseParsing = false;
	this->threadToStop = false;
	this->threadStarted = false;
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

UOSInt IO::FileAnalyse::QTFileAnalyse::GetFrameCount()
{
	return this->packs->GetCount();
}

Bool IO::FileAnalyse::QTFileAnalyse::GetFrameName(UOSInt index, Text::StringBuilderUTF *sb)
{
	IO::FileAnalyse::QTFileAnalyse::PackInfo *pack;
	UInt8 buff[5];
	pack = this->packs->GetItem(index);
	if (pack == 0)
		return false;
	sb->AppendU64(pack->fileOfst);
	sb->Append((const UTF8Char*)": Type=");
	*(Int32*)buff = pack->packType;
	buff[4] = 0;
	sb->Append((UTF8Char*)buff);
	sb->Append((const UTF8Char*)", size=");
	sb->AppendU64(pack->packSize);
	return true;
}

Bool IO::FileAnalyse::QTFileAnalyse::GetFrameDetail(UOSInt index, Text::StringBuilderUTF *sb)
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
	sb->Append((const UTF8Char*)": Type=");
	*(Int32*)buff = pack->packType;
	buff[4] = 0;
	sb->Append((UTF8Char*)buff);
	sb->Append((const UTF8Char*)", size=");
	sb->AppendU64(pack->packSize);
	sb->Append((const UTF8Char*)"\r\n");

	if (pack->packType == *(Int32*)"ftyp")
	{
		packBuff = MemAlloc(UInt8, (UOSInt)(pack->packSize - 8));
		this->fd->GetRealData(pack->fileOfst + 8, (UOSInt)pack->packSize - 8, packBuff);

		sb->Append((const UTF8Char*)"\r\nMajor_Brand = ");
		*(Int32*)buff = *(Int32*)&packBuff[0];
		sb->Append((UTF8Char*)buff);
		sb->Append((const UTF8Char*)"\r\nMinor_Version = ");
		sb->AppendHex32(ReadMUInt32(&packBuff[4]));
		i = 8;
		j = (UOSInt)(pack->packSize - 8);
		while (i < j)
		{
			sb->Append((const UTF8Char*)"\r\nCompatible_Brands[");
			sb->AppendU32((UInt32)i);
			sb->Append((const UTF8Char*)"] = ");
			*(Int32*)buff = *(Int32*)&packBuff[i];
			sb->Append((UTF8Char*)buff);

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

			sb->Append((const UTF8Char*)"\r\nData:\r\n");
			sb->AppendHexBuff(packBuff, (UOSInt)(pack->packSize - 8), ' ', Text::LBT_CRLF);

			MemFree(packBuff);
		}
	}
	else if (pack->packType == *(Int32*)"pnot")
	{
		if (pack->packSize >= 20)
		{
			packBuff = MemAlloc(UInt8, (UOSInt)(pack->packSize - 8));
			this->fd->GetRealData(pack->fileOfst + 8, (UOSInt)(pack->packSize - 8), packBuff);

			sb->Append((const UTF8Char*)"\r\nModification date = ");
			sb->AppendI32(ReadMInt32(&packBuff[0]));
			sb->Append((const UTF8Char*)"\r\nVersion number = ");
			sb->AppendU16(ReadMUInt16(&packBuff[4]));
			sb->Append((const UTF8Char*)"\r\nAtom type = ");
			*(Int32*)buff = *(Int32*)&packBuff[6];
			sb->Append((UTF8Char*)buff);
			sb->Append((const UTF8Char*)"\r\nAtom Index = ");
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

			sb->Append((const UTF8Char*)"\r\nVersion = ");
			sb->AppendU16(packBuff[0]);
			sb->Append((const UTF8Char*)"\r\nFlags = ");
			sb->AppendHex24(ReadMUInt32(&packBuff[0]));
			sb->Append((const UTF8Char*)"\r\nCreation time = ");
			dt.SetUnixTimestamp(ReadMUInt32(&packBuff[4]));
			sb->AppendDate(&dt);
			sb->Append((const UTF8Char*)"\r\nModification time = ");
			dt.SetUnixTimestamp(ReadMUInt32(&packBuff[8]));
			sb->AppendDate(&dt);
			sb->Append((const UTF8Char*)"\r\nTime Scale = ");
			sb->AppendI32(ReadMInt32(&packBuff[12]));
			sb->Append((const UTF8Char*)"\r\nDuration = ");
			sb->AppendI32(ReadMInt32(&packBuff[16]));
			sb->Append((const UTF8Char*)"\r\nPreferred rate = ");
			Text::SBAppendF64(sb, ReadMInt32(&packBuff[20]) / 65536.0);
			sb->Append((const UTF8Char*)"\r\nPreferred volume = ");
			Text::SBAppendF64(sb, ReadMInt16(&packBuff[24]) / 256.0);
			sb->Append((const UTF8Char*)"\r\nReserved = ");
			sb->AppendHexBuff(&packBuff[26], 10, ' ', Text::LBT_NONE);
			sb->Append((const UTF8Char*)"\r\nMatrix:");
			sb->Append((const UTF8Char*)"\r\na b u   ");
			Text::SBAppendF64(sb, ReadMInt32(&packBuff[36]) / 65536.0);
			sb->Append((const UTF8Char*)" ");
			Text::SBAppendF64(sb, ReadMInt32(&packBuff[40]) / 65536.0);
			sb->Append((const UTF8Char*)" ");
			Text::SBAppendF64(sb, ReadMInt32(&packBuff[44]) / 65536.0);
			sb->Append((const UTF8Char*)"\r\nc d v   ");
			Text::SBAppendF64(sb, ReadMInt32(&packBuff[48]) / 65536.0);
			sb->Append((const UTF8Char*)" ");
			Text::SBAppendF64(sb, ReadMInt32(&packBuff[52]) / 65536.0);
			sb->Append((const UTF8Char*)" ");
			Text::SBAppendF64(sb, ReadMInt32(&packBuff[56]) / 65536.0);
			sb->Append((const UTF8Char*)"\r\nx y w   ");
			Text::SBAppendF64(sb, ReadMInt32(&packBuff[60]) / 65536.0);
			sb->Append((const UTF8Char*)" ");
			Text::SBAppendF64(sb, ReadMInt32(&packBuff[64]) / 65536.0);
			sb->Append((const UTF8Char*)" ");
			Text::SBAppendF64(sb, ReadMInt32(&packBuff[68]) / 65536.0);
			sb->Append((const UTF8Char*)"\r\nPreview time = ");
			sb->AppendI32(ReadMInt32(&packBuff[72]));
			sb->Append((const UTF8Char*)"\r\nPreview duration = ");
			sb->AppendI32(ReadMInt32(&packBuff[76]));
			sb->Append((const UTF8Char*)"\r\nPoster time = ");
			sb->AppendI32(ReadMInt32(&packBuff[80]));
			sb->Append((const UTF8Char*)"\r\nSelection time = ");
			sb->AppendI32(ReadMInt32(&packBuff[84]));
			sb->Append((const UTF8Char*)"\r\nSelection duration = ");
			sb->AppendI32(ReadMInt32(&packBuff[88]));
			sb->Append((const UTF8Char*)"\r\nCurrent time = ");
			sb->AppendI32(ReadMInt32(&packBuff[92]));
			sb->Append((const UTF8Char*)"\r\nNext track ID = ");
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

			sb->Append((const UTF8Char*)"\r\nVersion = ");
			sb->AppendU16(packBuff[0]);
			sb->Append((const UTF8Char*)"\r\nFlags = ");
			sb->AppendHex24(ReadMUInt32(&packBuff[0]));
			sb->Append((const UTF8Char*)"\r\nCreation time = ");
			dt.SetUnixTimestamp(ReadMUInt32(&packBuff[4]));
			sb->AppendDate(&dt);
			sb->Append((const UTF8Char*)"\r\nModification time = ");
			dt.SetUnixTimestamp(ReadMUInt32(&packBuff[8]));
			sb->AppendDate(&dt);
			sb->Append((const UTF8Char*)"\r\nTrack ID = ");
			sb->AppendI32(ReadMInt32(&packBuff[12]));
			sb->Append((const UTF8Char*)"\r\nReserved = ");
			sb->AppendI32(ReadMInt32(&packBuff[16]));
			sb->Append((const UTF8Char*)"\r\nDuration = ");
			sb->AppendI32(ReadMInt32(&packBuff[20]));
			sb->Append((const UTF8Char*)"\r\nReserved = ");
			sb->AppendI64(ReadMInt64(&packBuff[24]));
			sb->Append((const UTF8Char*)"\r\nLayer = ");
			sb->AppendI16(ReadMInt16(&packBuff[32]));
			sb->Append((const UTF8Char*)"\r\nAlternate group = ");
			sb->AppendI16(ReadMInt16(&packBuff[34]));
			sb->Append((const UTF8Char*)"\r\nVolume = ");
			sb->AppendI16(ReadMInt16(&packBuff[36]));
			sb->Append((const UTF8Char*)"\r\nReserved = ");
			sb->AppendI16(ReadMInt16(&packBuff[38]));
			sb->Append((const UTF8Char*)"\r\nMatrix:");
			sb->Append((const UTF8Char*)"\r\na b u   ");
			Text::SBAppendF64(sb, ReadMInt32(&packBuff[40]) / 65536.0);
			sb->Append((const UTF8Char*)" ");
			Text::SBAppendF64(sb, ReadMInt32(&packBuff[44]) / 65536.0);
			sb->Append((const UTF8Char*)" ");
			Text::SBAppendF64(sb, ReadMInt32(&packBuff[48]) / 65536.0);
			sb->Append((const UTF8Char*)"\r\nc d v   ");
			Text::SBAppendF64(sb, ReadMInt32(&packBuff[52]) / 65536.0);
			sb->Append((const UTF8Char*)" ");
			Text::SBAppendF64(sb, ReadMInt32(&packBuff[56]) / 65536.0);
			sb->Append((const UTF8Char*)" ");
			Text::SBAppendF64(sb, ReadMInt32(&packBuff[60]) / 65536.0);
			sb->Append((const UTF8Char*)"\r\nx y w   ");
			Text::SBAppendF64(sb, ReadMInt32(&packBuff[64]) / 65536.0);
			sb->Append((const UTF8Char*)" ");
			Text::SBAppendF64(sb, ReadMInt32(&packBuff[68]) / 65536.0);
			sb->Append((const UTF8Char*)" ");
			Text::SBAppendF64(sb, ReadMInt32(&packBuff[72]) / 65536.0);
			sb->Append((const UTF8Char*)"\r\nTrack Width = ");
			Text::SBAppendF64(sb, ReadMInt32(&packBuff[76]) / 65536.0);
			sb->Append((const UTF8Char*)"\r\nTrack Height = ");
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

			sb->Append((const UTF8Char*)"\r\nVersion = ");
			sb->AppendU16(packBuff[0]);
			sb->Append((const UTF8Char*)"\r\nFlags = ");
			sb->AppendHex24(ReadMUInt32(&packBuff[0]));
			sb->Append((const UTF8Char*)"\r\nCreation time = ");
			dt.SetUnixTimestamp(ReadMUInt32(&packBuff[4]));
			sb->AppendDate(&dt);
			sb->Append((const UTF8Char*)"\r\nModification time = ");
			dt.SetUnixTimestamp(ReadMUInt32(&packBuff[8]));
			sb->AppendDate(&dt);
			sb->Append((const UTF8Char*)"\r\nTime scale = ");
			sb->AppendI32(ReadMInt32(&packBuff[12]));
			sb->Append((const UTF8Char*)"\r\nDuration = ");
			sb->AppendI32(ReadMInt32(&packBuff[16]));
			sb->Append((const UTF8Char*)"\r\nLanguage = ");
			sb->AppendI16(ReadMInt16(&packBuff[20]));
			sb->Append((const UTF8Char*)"\r\nQuality = ");
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

			sb->Append((const UTF8Char*)"\r\nVersion = ");
			sb->AppendU16(packBuff[0]);
			sb->Append((const UTF8Char*)"\r\nFlags = ");
			sb->AppendHex24(ReadMUInt32(&packBuff[0]));
			sb->Append((const UTF8Char*)"\r\nComponent type = ");
			*(Int32*)buff = *(Int32*)&packBuff[4];
			sb->Append((UTF8Char*)buff);
			sb->Append((const UTF8Char*)"\r\nComponent subtype = ");
			*(Int32*)buff = *(Int32*)&packBuff[8];
			sb->Append((UTF8Char*)buff);
			sb->Append((const UTF8Char*)"\r\nComponent manufacturer = ");
			sb->AppendI32(ReadMInt32(&packBuff[12]));
			sb->Append((const UTF8Char*)"\r\nComponent flags = ");
			sb->AppendI32(ReadMInt32(&packBuff[16]));
			sb->Append((const UTF8Char*)"\r\nComponent flags mask = ");
			sb->AppendI32(ReadMInt32(&packBuff[20]));
			sb->Append((const UTF8Char*)"\r\nComponent name = ");
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

			sb->Append((const UTF8Char*)"\r\nVersion = ");
			sb->AppendU16(packBuff[0]);
			sb->Append((const UTF8Char*)"\r\nFlags = ");
			sb->AppendHex24(ReadMUInt32(&packBuff[0]));
			sb->Append((const UTF8Char*)"\r\nGraphic mode = ");
			sb->AppendU16(ReadMUInt16(&packBuff[4]));
			sb->Append((const UTF8Char*)"\r\nOpcolor Red = ");
			sb->AppendU16(ReadMUInt16(&packBuff[6]));
			sb->Append((const UTF8Char*)"\r\nOpcolor Green = ");
			sb->AppendU16(ReadMUInt16(&packBuff[8]));
			sb->Append((const UTF8Char*)"\r\nOpcolor Blue = ");
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

			sb->Append((const UTF8Char*)"\r\nVersion = ");
			sb->AppendU16(packBuff[0]);
			sb->Append((const UTF8Char*)"\r\nFlags = ");
			sb->AppendHex24(ReadMUInt32(&packBuff[0]));
			sb->Append((const UTF8Char*)"\r\nBalance = ");
			sb->AppendU16(ReadMUInt16(&packBuff[4]));
			sb->Append((const UTF8Char*)"\r\nReserved = ");
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

			sb->Append((const UTF8Char*)"\r\nVersion = ");
			sb->AppendU16(packBuff[0]);
			sb->Append((const UTF8Char*)"\r\nFlags = ");
			sb->AppendHex24(ReadMUInt32(&packBuff[0]));
			sb->Append((const UTF8Char*)"\r\nNumber of entries = ");
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
				sb->Append((const UTF8Char*)"\r\n-Type = ");
				*(Int32*)buff = *(Int32*)&packBuff[k + 4];
				sb->Append((UTF8Char*)buff);
				sb->Append((const UTF8Char*)"\r\n-Version = ");
				sb->AppendU16(packBuff[k + 8]);
				sb->Append((const UTF8Char*)"\r\n-Flags = ");
				sb->AppendHex24(ReadMUInt32(&packBuff[k + 8]));
				sb->Append((const UTF8Char*)"\r\nData:\r\n");
				sb->AppendHexBuff(&packBuff[k + 12], l - 12, ' ', Text::LBT_CRLF);
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

			sb->Append((const UTF8Char*)"\r\nVersion = ");
			sb->AppendU16(packBuff[0]);
			sb->Append((const UTF8Char*)"\r\nFlags = ");
			sb->AppendHex24(ReadMUInt32(&packBuff[0]));
			sb->Append((const UTF8Char*)"\r\nNumber of entries = ");
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
				sb->Append((const UTF8Char*)"\r\n-Data format = ");
				WriteNInt32(buff, ReadNInt32(&packBuff[k + 4]));
				sb->Append((UTF8Char*)buff);
				if (ReadNInt32(&packBuff[k + 4]) == *(Int32*)"mp4a")
				{
					dataType = 2;
				}
				sb->Append((const UTF8Char*)"\r\n-Reserved = ");
				sb->AppendHexBuff(&packBuff[k + 8], 6, ' ', Text::LBT_NONE);
				sb->Append((const UTF8Char*)"\r\n-Data reference index = ");
				sb->AppendHex16(ReadMUInt16(&packBuff[k + 14]));
				if (dataType == 2)
				{
					UInt16 version = ReadMUInt16(&packBuff[k + 16]);
					UOSInt endOfst;
					sb->Append((const UTF8Char*)"\r\nVersion = ");
					sb->AppendU16(version);
					sb->Append((const UTF8Char*)"\r\nRevision level = ");
					sb->AppendU16(ReadMUInt16(&packBuff[k + 18]));
					sb->Append((const UTF8Char*)"\r\nVendor = ");
					sb->AppendU32(ReadMUInt32(&packBuff[k + 20]));
					if (version == 0)
					{
						sb->Append((const UTF8Char*)"\r\nNumber of channels = ");
						sb->AppendU16(ReadMUInt16(&packBuff[k + 24]));
						sb->Append((const UTF8Char*)"\r\nSample size (bits) = ");
						sb->AppendU16(ReadMUInt16(&packBuff[k + 26]));
						sb->Append((const UTF8Char*)"\r\nCompression ID = ");
						sb->AppendI16(ReadMInt16(&packBuff[k + 28]));
						sb->Append((const UTF8Char*)"\r\nPacket size = ");
						sb->AppendI16(ReadMInt16(&packBuff[k + 30]));
						sb->Append((const UTF8Char*)"\r\nSample rate = ");
						Text::SBAppendF64(sb, ReadMUInt32(&packBuff[k + 32]) / 65536.0);
						endOfst = 36;
					}
					else if (version == 1)
					{
						sb->Append((const UTF8Char*)"\r\nNumber of channels = ");
						sb->AppendU16(ReadMUInt16(&packBuff[k + 24]));
						sb->Append((const UTF8Char*)"\r\nSample size (bits) = ");
						sb->AppendU16(ReadMUInt16(&packBuff[k + 26]));
						sb->Append((const UTF8Char*)"\r\nCompression ID = ");
						sb->AppendI16(ReadMInt16(&packBuff[k + 28]));
						sb->Append((const UTF8Char*)"\r\nPacket size = ");
						sb->AppendI16(ReadMInt16(&packBuff[k + 30]));
						sb->Append((const UTF8Char*)"\r\nSample rate = ");
						Text::SBAppendF64(sb, ReadMUInt32(&packBuff[k + 32]) / 65536.0);
						sb->Append((const UTF8Char*)"\r\nSamples per packet = ");
						sb->AppendU32(ReadMUInt32(&packBuff[k + 36]));
						sb->Append((const UTF8Char*)"\r\nBytes per packet = ");
						sb->AppendU32(ReadMUInt32(&packBuff[k + 40]));
						sb->Append((const UTF8Char*)"\r\nBytes per frame = ");
						sb->AppendU32(ReadMUInt32(&packBuff[k + 44]));
						sb->Append((const UTF8Char*)"\r\nBytes per sample = ");
						sb->AppendU32(ReadMUInt32(&packBuff[k + 48]));
						endOfst = 52;
					}
					else if (version == 2)
					{
						sb->Append((const UTF8Char*)"\r\nalways3 = ");
						sb->AppendU16(ReadMUInt16(&packBuff[k + 24]));
						sb->Append((const UTF8Char*)"\r\nalways16 = ");
						sb->AppendU16(ReadMUInt16(&packBuff[k + 26]));
						sb->Append((const UTF8Char*)"\r\nalwaysMinus2 = ");
						sb->AppendI16(ReadMInt16(&packBuff[k + 28]));
						sb->Append((const UTF8Char*)"\r\nalways0 = ");
						sb->AppendI16(ReadMInt16(&packBuff[k + 30]));
						sb->Append((const UTF8Char*)"\r\nalways65536 = ");
						sb->AppendU32(ReadMUInt32(&packBuff[k + 32]));
						sb->Append((const UTF8Char*)"\r\nsizeOfStructOnly = ");
						sb->AppendU32(ReadMUInt32(&packBuff[k + 36]));
						sb->Append((const UTF8Char*)"\r\naudioSampleRate = ");
						Text::SBAppendF64(sb, ReadMDouble(&packBuff[k + 40]));
						sb->Append((const UTF8Char*)"\r\nnumAudioChannels = ");
						sb->AppendU32(ReadMUInt32(&packBuff[k + 48]));
						sb->Append((const UTF8Char*)"\r\nalways7F000000 = ");
						sb->AppendU32(ReadMUInt32(&packBuff[k + 52]));
						sb->Append((const UTF8Char*)"\r\nconstBitsPerChannel = ");
						sb->AppendU32(ReadMUInt32(&packBuff[k + 56]));
						sb->Append((const UTF8Char*)"\r\nformatSpecificFlags = 0x");
						sb->AppendHex32(ReadMUInt32(&packBuff[k + 60]));
						sb->Append((const UTF8Char*)"\r\nconstBytesPerAudioPacket = ");
						sb->AppendU32(ReadMUInt32(&packBuff[k + 64]));
						sb->Append((const UTF8Char*)"\r\nconstLPCMFramesPerAudioPacket = ");
						sb->AppendU32(ReadMUInt32(&packBuff[k + 68]));
						endOfst = 72;
					}
					else
					{
						endOfst = 16;
					}
					if (endOfst < l)
					{
						sb->Append((const UTF8Char*)"\r\nExtra Data:\r\n");
						sb->AppendHexBuff(&packBuff[k + endOfst], l - endOfst, ' ', Text::LBT_CRLF);
					}
				}
				else
				{
					sb->Append((const UTF8Char*)"\r\nData:\r\n");
					sb->AppendHexBuff(&packBuff[k + 16], l - 16, ' ', Text::LBT_CRLF);
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

			sb->Append((const UTF8Char*)"\r\nVersion = ");
			sb->AppendU16(packBuff[0]);
			sb->Append((const UTF8Char*)"\r\nFlags = ");
			sb->AppendHex24(ReadMUInt32(&packBuff[0]));
			sb->Append((const UTF8Char*)"\r\nNumber of entries = ");
			sb->AppendU32((UInt32)(j = ReadMUInt32(&packBuff[4])));
			k = 8;
			i = 0;
			while (i < j)
			{
				if ((UInt32)(k + 8) > pack->packSize - 8)
				{
					break;
				}
				sb->Append((const UTF8Char*)"\r\n-Sample Count = ");
				sb->AppendI32(ReadMInt32(&packBuff[k + 0]));
				sb->Append((const UTF8Char*)", Sample duration = ");
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

			sb->Append((const UTF8Char*)"\r\nVersion = ");
			sb->AppendU16(packBuff[0]);
			sb->Append((const UTF8Char*)"\r\nFlags = ");
			sb->AppendHex24(ReadMUInt32(&packBuff[0]));
			sb->Append((const UTF8Char*)"\r\nEntry count = ");
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
				sb->Append((const UTF8Char*)"\r\n-Sample Count = ");
				sb->AppendI32(ReadMInt32(&packBuff[k + 0]));
				sb->Append((const UTF8Char*)", compositionOffset = ");
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

			sb->Append((const UTF8Char*)"\r\nVersion = ");
			sb->AppendU16(packBuff[0]);
			sb->Append((const UTF8Char*)"\r\nFlags = ");
			sb->AppendHex24(ReadMUInt32(&packBuff[0]));
			sb->Append((const UTF8Char*)"\r\nNumber of entries = ");
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
				sb->Append((const UTF8Char*)"\r\n-Key ");
				sb->AppendI32((Int32)i);
				sb->Append((const UTF8Char*)" = ");
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

			sb->Append((const UTF8Char*)"\r\nVersion = ");
			sb->AppendU16(packBuff[0]);
			sb->Append((const UTF8Char*)"\r\nFlags = ");
			sb->AppendHex24(ReadMUInt32(&packBuff[0]));
			sb->Append((const UTF8Char*)"\r\nNumber of entries = ");
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
				sb->Append((const UTF8Char*)"\r\n-First chunk = ");
				sb->AppendI32(ReadMInt32(&packBuff[k + 0]));
				sb->Append((const UTF8Char*)", Samples per chunk = ");
				sb->AppendI32(ReadMInt32(&packBuff[k + 4]));
				sb->Append((const UTF8Char*)", Sample description ID = ");
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

			sb->Append((const UTF8Char*)"\r\nVersion = ");
			sb->AppendU16(packBuff[0]);
			sb->Append((const UTF8Char*)"\r\nFlags = ");
			sb->AppendHex24(ReadMUInt32(&packBuff[0]));
			sb->Append((const UTF8Char*)"\r\nSample size = ");
			sb->AppendI32(ReadMInt32(&packBuff[4]));
			sb->Append((const UTF8Char*)"\r\nNumber of entries = ");
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
				sb->Append((const UTF8Char*)"\r\n-Sample size ");
				sb->AppendI32((Int32)i);
				sb->Append((const UTF8Char*)" = ");
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

			sb->Append((const UTF8Char*)"\r\nVersion = ");
			sb->AppendU16(packBuff[0]);
			sb->Append((const UTF8Char*)"\r\nFlags = ");
			sb->AppendHex24(ReadMUInt32(&packBuff[0]));
			sb->Append((const UTF8Char*)"\r\nNumber of entries = ");
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
				sb->Append((const UTF8Char*)"\r\n-Offset ");
				sb->AppendI32((Int32)i);
				sb->Append((const UTF8Char*)" = ");
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

			sb->Append((const UTF8Char*)"\r\nVersion = ");
			sb->AppendU16(packBuff[0]);
			sb->Append((const UTF8Char*)"\r\nFlags = ");
			sb->AppendHex24(ReadMUInt32(&packBuff[0]));
			sb->Append((const UTF8Char*)"\r\nNumber of entries = ");
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
				sb->Append((const UTF8Char*)"\r\n-Offset ");
				sb->AppendI32((Int32)i);
				sb->Append((const UTF8Char*)" = ");
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
	OSInt i = 0;
	OSInt j = (OSInt)this->packs->GetCount() - 1;
	OSInt k;
	PackInfo *pack;
	while (i <= j)
	{
		k = (i + j) >> 1;
		pack = this->packs->GetItem(k);
		if (ofst < pack->fileOfst)
		{
			j = k - 1;
		}
		else if (ofst >= pack->fileOfst + pack->packSize)
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
	NEW_CLASS(dfs, IO::FileStream(outputFile, IO::FileStream::FILE_MODE_CREATE, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));
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
