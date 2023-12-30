#include "Stdafx.h"
#include "Data/ByteBuffer.h"
#include "Data/ByteTool.h"
#include "IO/FileAnalyse/SSLFileAnalyse.h"
#include "Manage/Process.h"
#include "Net/ASN1Names.h"
#include "Net/ASN1Util.h"
#include "Net/SSLUtil.h"

void __stdcall IO::FileAnalyse::SSLFileAnalyse::ParseThread(NotNullPtr<Sync::Thread> thread)
{
	IO::FileAnalyse::SSLFileAnalyse *me = (IO::FileAnalyse::SSLFileAnalyse *)thread->GetUserObj();
	PackInfo *pack;
	UInt8 buff[16];
	UInt64 ofst = 0;
	UInt64 leng = me->fd->GetDataSize();
	UOSInt packLen;
	while (ofst < leng)
	{
		if (me->fd->GetRealData(ofst, 16, BYTEARR(buff)) < 5)
		{
			break;
		}
		packLen = ReadMUInt16(&buff[3]);
		if (ofst + packLen + 5 <= leng)
		{
			pack = MemAlloc(PackInfo, 1);
			pack->fileOfst = ofst;
			pack->packSize = packLen + 5;
			pack->packType = buff[0];
			me->packs.Add(pack);
			ofst += packLen + 5;
		}
		else
		{
			break;
		}
	}
}

void IO::FileAnalyse::SSLFileAnalyse::FreePackInfo(PackInfo *pack)
{
	MemFree(pack);
}

UOSInt IO::FileAnalyse::SSLFileAnalyse::AppendExtension(NotNullPtr<IO::FileAnalyse::FrameDetail> frame, Data::ByteArrayR buff, UOSInt ofst, UOSInt totalLeng)
{
	UInt16 extType = buff.ReadMU16(ofst);
	UOSInt extLen = buff.ReadMU16(ofst + 2);
	UOSInt i;
	UOSInt len;
	frame->AddUIntName(ofst, 2, CSTR("Extension Type"), extType, Net::SSLUtil::ExtensionTypeGetName(extType));
	frame->AddUInt(ofst + 2, 2, CSTR("Length"), extLen);
	if (extLen > 0) frame->AddArea(ofst + 4, extLen, Net::SSLUtil::ExtensionTypeGetName(extType));
	switch (extType)
	{
	case 0x0b: //ec_point_formats
		frame->AddUInt(ofst + 4, 1, CSTR("ec_point_formats Length"), len = buff[ofst + 4]);
		i = ofst + 5;
		len += ofst + 5;
		while (i < len)
		{
			frame->AddUIntName(i, 1, CSTR("EC point format"), buff[i], Net::SSLUtil::ECPointFormatGetName(buff[i]));
			i++;
		}
		break;
	case 0xff01: // renegotiation_info
		frame->AddUInt(ofst + 4, 1, CSTR("renegotiation_info Length"), buff[ofst + 4]);
		break;
	}
	return ofst + 4 + extLen;
}

IO::FileAnalyse::SSLFileAnalyse::SSLFileAnalyse(NotNullPtr<IO::StreamData> fd) : thread(ParseThread, this, CSTR("SSLFileAnalyse"))
{
	this->fd = 0;
	this->pauseParsing = false;

	this->fd = fd->GetPartialData(0, fd->GetDataSize()).Ptr();
	this->thread.Start();
}

IO::FileAnalyse::SSLFileAnalyse::~SSLFileAnalyse()
{
	this->thread.Stop();
	SDEL_CLASS(this->fd);
	LIST_FREE_FUNC(&this->packs, FreePackInfo);
}

Text::CStringNN IO::FileAnalyse::SSLFileAnalyse::GetFormatName()
{
	return CSTR("SSL");
}

UOSInt IO::FileAnalyse::SSLFileAnalyse::GetFrameCount()
{
	return this->packs.GetCount();
}

Bool IO::FileAnalyse::SSLFileAnalyse::GetFrameName(UOSInt index, NotNullPtr<Text::StringBuilderUTF8> sb)
{
	PackInfo *pack;
	pack = this->packs.GetItem(index);
	if (pack == 0)
		return false;
	sb->AppendU64(pack->fileOfst);
	sb->AppendC(UTF8STRC(": Type="));
	sb->Append(Net::SSLUtil::RecordTypeGetName(pack->packType));
	sb->AppendC(UTF8STRC(", size="));
	sb->AppendI32((Int32)pack->packSize);
	return true;
}

UOSInt IO::FileAnalyse::SSLFileAnalyse::GetFrameIndex(UInt64 ofst)
{
	OSInt i = 0;
	OSInt j = (OSInt)this->packs.GetCount() - 1;
	OSInt k;
	PackInfo *pack;
	while (i <= j)
	{
		k = (i + j) >> 1;
		pack = this->packs.GetItem((UOSInt)k);
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

Optional<IO::FileAnalyse::FrameDetail> IO::FileAnalyse::SSLFileAnalyse::GetFrameDetail(UOSInt index)
{
	NotNullPtr<IO::FileAnalyse::FrameDetail> frame;
	PackInfo *pack;
	pack = this->packs.GetItem(index);
	if (pack == 0)
		return 0;

	UOSInt i;
	UOSInt j;
	UOSInt k;
	UOSInt l;
	Data::ByteBuffer packBuff(pack->packSize);
	this->fd->GetRealData(pack->fileOfst, pack->packSize, packBuff);
	NEW_CLASSNN(frame, IO::FileAnalyse::FrameDetail(pack->fileOfst, pack->packSize));
	frame->AddUIntName(0, 1, CSTR("SSL record Type"), packBuff[0], Net::SSLUtil::RecordTypeGetName(packBuff[0]));
	frame->AddHex16Name(1, CSTR("SSL version"), packBuff.ReadMU16(1), Net::SSLVerGetName(packBuff.ReadMU16(1)));
	frame->AddUInt(3, 2, CSTR("Record Length"), packBuff.ReadMU16(3));
	switch (packBuff[0])
	{
	case 21:
		frame->AddArea(5, pack->packSize - 5, CSTR("Alert"));
		if (pack->packSize == 7)
		{
			frame->AddUIntName(5, 1, CSTR("Level"), packBuff[5], Net::SSLUtil::AlertLevelGetName(packBuff[5]));
			frame->AddUIntName(6, 1, CSTR("Description"), packBuff[6], Net::SSLUtil::AlertDescGetName(packBuff[6]));
		}
		break;
	case 22:
		frame->AddArea(5, pack->packSize - 5, CSTR("Handshake"));
		i = 5;
		while (i < pack->packSize - 3)
		{
			frame->AddUIntName(i, 1, CSTR("Type"), packBuff[i], Net::SSLUtil::HandshakeTypeGetName(packBuff[i]));
			UInt32 leng = packBuff.ReadMU24(i + 1);
			frame->AddUInt(i + 1, 3, CSTR("Length"), leng);
			if (i + leng + 4 <= pack->packSize)
			{
				frame->AddArea(i + 4, leng, Net::SSLUtil::HandshakeTypeGetName(packBuff[i]));
				switch (packBuff[i])
				{
				case 1: //Client Hello
					frame->AddHex16Name(i + 4, CSTR("Version"), packBuff.ReadMU16(i + 4), Net::SSLVerGetName(packBuff.ReadMU16(i + 4)));
					frame->AddHexBuff(i + 6, CSTR("Random"), packBuff.SubArray(i + 6, 32), true);
					frame->AddUInt(i + 38, 1, CSTR("Session ID Length"), j = packBuff[i + 38]);
					if (j > 0)
					{
						frame->AddHexBuff(i + 39, CSTR("Session ID"), packBuff.SubArray(i + 39, j), false);
					}
					j += i + 39;
					k = packBuff.ReadMU16(j);
					frame->AddUInt(j, 2, CSTR("Cipher Suites Length"), k);
					j += 2;
					l = 0;
					while (l < k)
					{
						frame->AddHex16Name(j + l, CSTR("Cipher Suite"), packBuff.ReadMU16(j + l), Net::SSLUtil::CipherSuiteGetName(packBuff.ReadMU16(j + l)));
						l += 2;
					}
					j += k;
					break;
				case 2: //Server Hello
					frame->AddHex16Name(i + 4, CSTR("Version"), packBuff.ReadMU16(i + 4), Net::SSLVerGetName(packBuff.ReadMU16(i + 4)));
					frame->AddHexBuff(i + 6, CSTR("Random"), packBuff.SubArray(i + 6, 32), true);
					frame->AddUInt(i + 38, 1, CSTR("Session ID Length"), j = packBuff[i + 38]);
					if (j > 0)
					{
						frame->AddHexBuff(i + 39, CSTR("Session ID"), packBuff.SubArray(i + 39, j), false);
					}
					j += i + 39;
					frame->AddHex16Name(j, CSTR("Cipher Suite"), packBuff.ReadMU16(j), Net::SSLUtil::CipherSuiteGetName(packBuff.ReadMU16(j)));
					frame->AddUIntName(j + 2, 1, CSTR("Compression Method"), packBuff[j + 2], Net::SSLUtil::CompressionMethodGetName(packBuff[j + 2]));
					l = packBuff.ReadMU16(j + 3);
					frame->AddUInt(j + 3, 2, CSTR("Extensions Length"), l);
					j += 5;
					if (l > 0)
					{
						frame->AddArea(j, l, CSTR("Extensions"));
					}
					k = j;
					l += j;
					while (k < l)
					{
						k = AppendExtension(frame, packBuff, k, l);
					}
					break;
				case 11: //Certificate
					j = packBuff.ReadMU24(i + 4);
					frame->AddUInt(i + 4, 3, CSTR("Certificates Length"), j);
					frame->AddArea(i + 7, j, CSTR("Certificates"));
					k = 0;
					while (k < j)
					{
						l = packBuff.ReadMU24(i + 7 + k);
						frame->AddUInt(i + 7 + k, 3, CSTR("Certificate Length"), l);
						frame->AddArea(i + 10 + k, l, CSTR("Certificate"));
						Net::ASN1Util::PDUAnalyse(frame, packBuff, i + 10 + k, i + 10 + k + l, Net::ASN1Names().SetCertificate().Ptr());
						k += 3 + l;
					}
					break;
				case 14: //Server Hello Done
					break;
				}
			}
			i += leng + 4;
		}
		break;
	}
	return frame;
}

Bool IO::FileAnalyse::SSLFileAnalyse::IsError()
{
	return this->fd == 0;
}

Bool IO::FileAnalyse::SSLFileAnalyse::IsParsing()
{
	return this->thread.IsRunning();
}

Bool IO::FileAnalyse::SSLFileAnalyse::TrimPadding(Text::CStringNN outputFile)
{
	return false;
}
