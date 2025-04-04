#include "Stdafx.h"
#include "Data/ByteBuffer.h"
#include "Data/ByteOrderLSB.h"
#include "Data/ByteOrderMSB.h"
#include "Data/ByteTool.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "IO/FileAnalyse/TIFFFileAnalyse.h"
#include "Manage/Process.h"
#include "Media/EXIFData.h"
#include "Net/SocketFactory.h"

void __stdcall IO::FileAnalyse::TIFFFileAnalyse::ParseThread(NN<Sync::Thread> thread)
{
	NN<IO::FileAnalyse::TIFFFileAnalyse> me = thread->GetUserObj().GetNN<IO::FileAnalyse::TIFFFileAnalyse>();
	UInt8 buff[256];
	NN<PackInfo> pack;

	me->fd->GetRealData(0, 256, BYTEARR(buff));
	UInt16 fmt = me->bo->GetUInt16(&buff[2]);
	if (fmt == 42)
	{
		UOSInt nextOfst = me->bo->GetUInt32(&buff[4]);
		pack = MemAllocNN(PackInfo);
		pack->fileOfst = 0;
		pack->packSize = 8;
		pack->packType = PT_HEADER;
		me->packs.Add(pack);
		if (nextOfst > 8)
		{
			pack = MemAllocNN(PackInfo);
			pack->fileOfst = 8;
			pack->packSize = nextOfst - 8;
			pack->packType = PT_RESERVED;
			me->packs.Add(pack);
		}
		UInt64 thisOfst;
		while (nextOfst != 0)
		{
			UInt64 nTags;
			me->fd->GetRealData(nextOfst, 2, BYTEARR(buff));
			nTags = me->bo->GetUInt16(&buff[0]);
			pack = MemAllocNN(PackInfo);
			pack->fileOfst = nextOfst;
			pack->packSize = nTags * 12 + 6;
			pack->packType = PT_IFD;
			me->packs.Add(pack);
			me->fd->GetRealData(nextOfst + nTags * 12 + 2, 4, BYTEARR(buff));
			thisOfst = nextOfst + nTags * 12 + 2;
			nextOfst = me->bo->GetUInt32(&buff[0]);
			if (nextOfst < thisOfst)
				break;
		}
	}
	else if (fmt == 43 && me->bo->GetUInt16(&buff[4]) == 8 && me->bo->GetUInt16(&buff[6]) == 0) //BigTIFF
	{
		UInt64 nextOfst = me->bo->GetUInt64(&buff[8]);
		pack = MemAllocNN(PackInfo);
		pack->fileOfst = 0;
		pack->packSize = 16;
		pack->packType = PT_HEADER;
		me->packs.Add(pack);
		if (nextOfst > 16)
		{
			pack = MemAllocNN(PackInfo);
			pack->fileOfst = 16;
			pack->packSize = nextOfst - 16;
			pack->packType = PT_RESERVED;
			me->packs.Add(pack);
		}
		UInt64 thisOfst;
		while (nextOfst != 0)
		{
			UInt64 nTags;
			me->fd->GetRealData(nextOfst, 8, BYTEARR(buff));
			nTags = me->bo->GetUInt64(&buff[0]);
			pack = MemAllocNN(PackInfo);
			pack->fileOfst = nextOfst;
			pack->packSize = nTags * 20 + 16;
			pack->packType = PT_IFD8;
			me->packs.Add(pack);
			me->fd->GetRealData(nextOfst + nTags * 20 + 8, 8, BYTEARR(buff));
			thisOfst = nextOfst + nTags * 20 + 16;
			nextOfst = me->bo->GetUInt64(&buff[0]);
			if (nextOfst < thisOfst)
				break;
		}
	}
	else
	{
	}
}

void __stdcall IO::FileAnalyse::TIFFFileAnalyse::FreePackInfo(NN<PackInfo> pack)
{
	MemFreeNN(pack);
}

IO::FileAnalyse::TIFFFileAnalyse::TIFFFileAnalyse(NN<IO::StreamData> fd) : thread(ParseThread, this, CSTR("TIFFFileAnalyse"))
{
	UInt8 buff[256];
	this->fd = 0;
	this->bo = 0;
	this->pauseParsing = false;

	fd->GetRealData(0, 256, BYTEARR(buff));
	if (*(Int16*)&buff[0] == *(Int16*)"MM")
	{
		NEW_CLASS(this->bo, Data::ByteOrderMSB());
	}
	else if (*(Int16*)&buff[0] == *(Int16*)"II")
	{
		NEW_CLASS(this->bo, Data::ByteOrderLSB());
	}
	else
	{
		return;
	}
	UInt16 fmt = bo->GetUInt16(&buff[2]);
	if (fmt == 42)
	{
	}
	else if (fmt == 43 && bo->GetUInt16(&buff[4]) == 8 && bo->GetUInt16(&buff[6]) == 0) //BigTIFF
	{
	}
	else
	{
		return;
	}
	this->fd = fd->GetPartialData(0, fd->GetDataSize()).Ptr();
	this->thread.Start();
}

IO::FileAnalyse::TIFFFileAnalyse::~TIFFFileAnalyse()
{
	this->thread.Stop();
	SDEL_CLASS(this->fd);
	SDEL_CLASS(this->bo);
	this->packs.FreeAll(FreePackInfo);
}

Text::CStringNN IO::FileAnalyse::TIFFFileAnalyse::GetFormatName()
{
	return CSTR("TIFF");
}

UOSInt IO::FileAnalyse::TIFFFileAnalyse::GetFrameCount()
{
	return this->packs.GetCount();
}

Bool IO::FileAnalyse::TIFFFileAnalyse::GetFrameName(UOSInt index, NN<Text::StringBuilderUTF8> sb)
{
	NN<PackInfo> pack;
	if (!this->packs.GetItem(index).SetTo(pack))
		return false;
	sb->AppendU64(pack->fileOfst);
	sb->AppendC(UTF8STRC(": Type="));
	switch (pack->packType)
	{
	case PT_HEADER:
		sb->AppendC(UTF8STRC("File header"));
		break;
	case PT_RESERVED:
		sb->AppendC(UTF8STRC("Reserved"));
		break;
	case PT_IFD:
		sb->AppendC(UTF8STRC("IFD"));
		break;
	case PT_IFD8:
		sb->AppendC(UTF8STRC("IFD8"));
		break;
	}
	sb->AppendC(UTF8STRC(", size="));
	sb->AppendI32((Int32)pack->packSize);
	return true;
}

UOSInt IO::FileAnalyse::TIFFFileAnalyse::GetFrameIndex(UInt64 ofst)
{
	OSInt i = 0;
	OSInt j = (OSInt)this->packs.GetCount() - 1;
	OSInt k;
	NN<PackInfo> pack;
	while (i <= j)
	{
		k = (i + j) >> 1;
		pack = this->packs.GetItemNoCheck((UOSInt)k);
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

Optional<IO::FileAnalyse::FrameDetail> IO::FileAnalyse::TIFFFileAnalyse::GetFrameDetail(UOSInt index)
{
	NN<IO::FileAnalyse::FrameDetail> frame;
	NN<PackInfo> pack;
	UTF8Char sbuff[32];
	UnsafeArray<UTF8Char> sptr;
	if (!this->packs.GetItem(index).SetTo(pack))
		return 0;

	NEW_CLASSNN(frame, IO::FileAnalyse::FrameDetail(pack->fileOfst, pack->packSize));
	switch (pack->packType)
	{
	case PT_HEADER:
		frame->AddText(0, CSTR("Type=File header"));
		break;
	case PT_RESERVED:
		frame->AddText(0, CSTR("Type=Reserved"));
		break;
	case PT_IFD:
		frame->AddText(0, CSTR("Type=IFD"));
		break;
	case PT_IFD8:
		frame->AddText(0, CSTR("Type=IFD8"));
		break;
	}
	sptr = Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("Size=")), pack->packSize);
	frame->AddText(0, CSTRP(sbuff, sptr));

	if (pack->packType == PT_HEADER)
	{
		Data::ByteBuffer packBuff(pack->packSize);
		this->fd->GetRealData(pack->fileOfst, pack->packSize, packBuff);

		UOSInt verNum = this->bo->GetUInt16(&packBuff[2]);
		frame->AddStrC(0, 2, CSTR("Byte Order"), &packBuff[0]);
		frame->AddUInt(2, 2, CSTR("Version Number"), verNum);
		if (verNum == 42)
		{
			frame->AddUInt(4, 4, CSTR("Offset to first IFD"), this->bo->GetUInt32(&packBuff[4]));
		}
		else
		{
			frame->AddUInt(4, 2, CSTR("Bytesize of offsets"), this->bo->GetUInt16(&packBuff[4]));
			frame->AddUInt(6, 2, CSTR("Reserved"), this->bo->GetUInt16(&packBuff[6]));
			frame->AddUInt64(8, CSTR("Offset to first IFD"), this->bo->GetUInt64(&packBuff[8]));
		}
	}
	else if (pack->packType == PT_RESERVED)
	{
		Data::ByteBuffer packBuff(pack->packSize);
		this->fd->GetRealData(pack->fileOfst, pack->packSize, packBuff);
		frame->AddStrS(0, pack->packSize, CSTR("Reserved"), packBuff.Arr());
	}
	else if (pack->packType == PT_IFD8)
	{
		Data::ByteBuffer packBuff(pack->packSize);
		this->fd->GetRealData(pack->fileOfst, pack->packSize, packBuff);
		UInt64 tagCnt = this->bo->GetUInt64(packBuff.Arr());
		frame->AddUInt64(0, CSTR("Number of tags in IFD"), tagCnt);
		UOSInt i = 0;
		UOSInt ofst = 8;
		while (i < tagCnt)
		{
			UInt16 dataType = this->bo->GetUInt16(&packBuff[ofst + 2]);
			UInt16 tag = this->bo->GetUInt16(&packBuff[ofst]);
			frame->AddUIntName(ofst, 2, CSTR("Tag Id"), tag, Media::EXIFData::GetEXIFName(Media::EXIFData::EM_STANDARD, tag));
			frame->AddUIntName(ofst + 2, 2, CSTR("Data Type"), dataType, Media::EXIFData::GetFieldTypeName(dataType));
			frame->AddUInt64(ofst + 4, CSTR("Number of values"), this->bo->GetUInt64(&packBuff[ofst + 4]));
			frame->AddUInt64(ofst + 12, CSTR("Offset to tag data"), this->bo->GetUInt64(&packBuff[ofst + 12]));
			i++;
			ofst += 20;
		}
		frame->AddUInt64(ofst, CSTR("Offset to next IFD"), this->bo->GetUInt64(&packBuff[ofst]));
	}
	else if (pack->packType == PT_IFD)
	{
		Data::ByteBuffer packBuff(pack->packSize);
		this->fd->GetRealData(pack->fileOfst, pack->packSize, packBuff);
		UInt32 tagCnt = this->bo->GetUInt16(packBuff.Arr());
		frame->AddUInt(0, 2, CSTR("Number of tags in IFD"), tagCnt);
		UOSInt i = 0;
		UOSInt ofst = 2;
		while (i < tagCnt)
		{
			UInt16 dataType = this->bo->GetUInt16(&packBuff[ofst + 2]);
			UInt16 tag = this->bo->GetUInt16(&packBuff[ofst]);
			frame->AddUIntName(ofst, 2, CSTR("Tag Id"), tag, Media::EXIFData::GetEXIFName(Media::EXIFData::EM_STANDARD, tag));
			frame->AddUIntName(ofst + 2, 2, CSTR("Data Type"), dataType, Media::EXIFData::GetFieldTypeName(dataType));
			frame->AddUInt(ofst + 4, 4, CSTR("Number of values"), this->bo->GetUInt32(&packBuff[ofst + 4]));
			frame->AddUInt(ofst + 8, 4, CSTR("Offset to tag data"), this->bo->GetUInt32(&packBuff[ofst + 8]));
			i++;
			ofst += 12;
		}
		frame->AddUInt(ofst, 4, CSTR("Offset to next IFD"), this->bo->GetUInt32(&packBuff[ofst]));
	}
	return frame;
}

Bool IO::FileAnalyse::TIFFFileAnalyse::IsError()
{
	return this->fd == 0;
}

Bool IO::FileAnalyse::TIFFFileAnalyse::IsParsing()
{
	return this->thread.IsRunning();
}

Bool IO::FileAnalyse::TIFFFileAnalyse::TrimPadding(Text::CStringNN outputFile)
{
	return false;
}
