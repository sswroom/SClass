#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "IO/FileAnalyse/FrameDetailHandler.h"
#include "Net/MACInfo.h"
#include "Net/NetBIOSUtil.h"
#include "Net/SocketUtil.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"

void IO::FileAnalyse::FrameDetailHandler::AddBool(UOSInt frameOfst, const Char *name, UInt8 v)
{
	if (v == 0)
	{
		this->AddField(frameOfst, 1, (const UTF8Char*)name, (const UTF8Char*)"false");
	}
	else if (v == 1)
	{
		this->AddField(frameOfst, 1, (const UTF8Char*)name, (const UTF8Char*)"true");
	}
	else
	{
		UTF8Char sbuff[16];
		Text::StrHexByte(Text::StrConcatC(sbuff, UTF8STRC("0x")), v);
		this->AddField(frameOfst, 1, (const UTF8Char*)name, sbuff);
	}
}

void IO::FileAnalyse::FrameDetailHandler::AddInt64(UOSInt frameOfst, const Char *name, Int64 v)
{
	UTF8Char sbuff[24];
	Text::StrInt64(sbuff, v);
	this->AddField(frameOfst, 8, (const UTF8Char*)name, sbuff);
}

void IO::FileAnalyse::FrameDetailHandler::AddInt64V(UOSInt frameOfst, UOSInt size, const Char *name, Int64 v)
{
	UTF8Char sbuff[24];
	Text::StrInt64(sbuff, v);
	this->AddField(frameOfst, size, (const UTF8Char*)name, sbuff);
}

void IO::FileAnalyse::FrameDetailHandler::AddUInt64(UOSInt frameOfst, const Char *name, UInt64 v)
{
	UTF8Char sbuff[24];
	Text::StrUInt64(sbuff, v);
	this->AddField(frameOfst, 8, (const UTF8Char*)name, sbuff);
}

void IO::FileAnalyse::FrameDetailHandler::AddUInt64V(UOSInt frameOfst, UOSInt size, const Char *name, UInt64 v)
{
	UTF8Char sbuff[24];
	Text::StrUInt64(sbuff, v);
	this->AddField(frameOfst, size, (const UTF8Char*)name, sbuff);
}

void IO::FileAnalyse::FrameDetailHandler::AddInt(UOSInt frameOfst, UOSInt size, const Char *name, OSInt v)
{
	UTF8Char sbuff[16];
	Text::StrOSInt(sbuff, v);
	this->AddField(frameOfst, size, (const UTF8Char*)name, sbuff);
}

void IO::FileAnalyse::FrameDetailHandler::AddUInt(UOSInt frameOfst, UOSInt size, const Char *name, UOSInt v)
{
	UTF8Char sbuff[16];
	Text::StrUOSInt(sbuff, v);
	this->AddField(frameOfst, size, (const UTF8Char*)name, sbuff);
}

void IO::FileAnalyse::FrameDetailHandler::AddUIntName(UOSInt frameOfst, UOSInt size, const Char *name, UOSInt v, const UTF8Char *vName)
{
	Text::StringBuilderUTF8 sb;
	sb.AppendUOSInt(v);
	if (vName)
	{
		sb.AppendC(UTF8STRC(" ("));
		sb.Append(vName);
		sb.AppendChar(')', 1);
	}
	else
	{
		sb.AppendC(UTF8STRC(" (Unkonwn)"));
	}
	this->AddField(frameOfst, size, (const UTF8Char*)name, sb.ToString());
}

void IO::FileAnalyse::FrameDetailHandler::AddUInt64Name(UOSInt frameOfst, UOSInt size, const Char *name, UInt64 v, const UTF8Char *vName)
{
	Text::StringBuilderUTF8 sb;
	sb.AppendU64(v);
	if (vName)
	{
		sb.AppendC(UTF8STRC(" ("));
		sb.Append(vName);
		sb.AppendChar(')', 1);
	}
	else
	{
		sb.AppendC(UTF8STRC(" (Unkonwn)"));
	}
	this->AddField(frameOfst, size, (const UTF8Char*)name, sb.ToString());
}

void IO::FileAnalyse::FrameDetailHandler::AddFloat(UOSInt frameOfst, UOSInt size, const Char *name, Double v)
{
	UTF8Char sbuff[64];
	Text::StrDouble(sbuff, v);
	this->AddField(frameOfst, size, (const UTF8Char*)name, sbuff);
}

void IO::FileAnalyse::FrameDetailHandler::AddHex8(UOSInt frameOfst, const Char *name, UInt8 v)
{
	UTF8Char sbuff[16];
	Text::StrHexByte(Text::StrConcatC(sbuff, UTF8STRC("0x")), v);
	this->AddField(frameOfst, 1, (const UTF8Char*)name, sbuff);
}

void IO::FileAnalyse::FrameDetailHandler::AddHex16(UOSInt frameOfst, const Char *name, UInt16 v)
{
	UTF8Char sbuff[16];
	Text::StrHexVal16(Text::StrConcatC(sbuff, UTF8STRC("0x")), v);
	this->AddField(frameOfst, 2, (const UTF8Char*)name, sbuff);
}

void IO::FileAnalyse::FrameDetailHandler::AddHex24(UOSInt frameOfst, const Char *name, UInt32 v)
{
	UTF8Char sbuff[16];
	Text::StrHexVal24(Text::StrConcatC(sbuff, UTF8STRC("0x")), v);
	this->AddField(frameOfst, 3, (const UTF8Char*)name, sbuff);
}

void IO::FileAnalyse::FrameDetailHandler::AddHex32(UOSInt frameOfst, const Char *name, UInt32 v)
{
	UTF8Char sbuff[16];
	Text::StrHexVal32(Text::StrConcatC(sbuff, UTF8STRC("0x")), v);
	this->AddField(frameOfst, 4, (const UTF8Char*)name, sbuff);
}

void IO::FileAnalyse::FrameDetailHandler::AddHex64(UOSInt frameOfst, const Char *name, UInt64 v)
{
	UTF8Char sbuff[19];
	Text::StrHexVal64(Text::StrConcatC(sbuff, UTF8STRC("0x")), v);
	this->AddField(frameOfst, 8, (const UTF8Char*)name, sbuff);
}

void IO::FileAnalyse::FrameDetailHandler::AddHex64V(UOSInt frameOfst, UOSInt size, const Char *name, UInt64 v)
{
	UTF8Char sbuff[19];
	Text::StrHexVal64V(Text::StrConcatC(sbuff, UTF8STRC("0x")), v);
	this->AddField(frameOfst, size, (const UTF8Char*)name, sbuff);
}

void IO::FileAnalyse::FrameDetailHandler::AddHex8Name(UOSInt frameOfst, const Char *name, UInt8 v, const UTF8Char *vName)
{
	Text::StringBuilderUTF8 sb;
	sb.AppendC(UTF8STRC("0x"));
	sb.AppendHex8(v);
	if (vName)
	{
		sb.AppendC(UTF8STRC(" ("));
		sb.Append(vName);
		sb.AppendChar(')', 1);
	}
	else
	{
		sb.AppendC(UTF8STRC(" (Unkonwn)"));
	}
	this->AddField(frameOfst, 1, (const UTF8Char*)name, sb.ToString());
}

void IO::FileAnalyse::FrameDetailHandler::AddHex16Name(UOSInt frameOfst, const Char *name, UInt16 v, const UTF8Char *vName)
{
	Text::StringBuilderUTF8 sb;
	sb.AppendC(UTF8STRC("0x"));
	sb.AppendHex16(v);
	if (vName)
	{
		sb.AppendC(UTF8STRC(" ("));
		sb.Append(vName);
		sb.AppendChar(')', 1);
	}
	else
	{
		sb.AppendC(UTF8STRC(" (Unkonwn)"));
	}
	this->AddField(frameOfst, 2, (const UTF8Char*)name, sb.ToString());
}

void IO::FileAnalyse::FrameDetailHandler::AddStrC(UOSInt frameOfst, UOSInt size, const Char *name, const UTF8Char *vBuff)
{
	const UTF8Char *csptr = Text::StrCopyNewC(vBuff, size);
	this->AddField(frameOfst, size, (const UTF8Char*)name, csptr);
	Text::StrDelNew(csptr);
}

void IO::FileAnalyse::FrameDetailHandler::AddStrS(UOSInt frameOfst, UOSInt size, const Char *name, const UTF8Char *vBuff)
{
	UTF8Char *sbuff = MemAlloc(UTF8Char, size + 1);
	Text::StrConcatS(sbuff, vBuff, size + 1);
	this->AddField(frameOfst, size, (const UTF8Char*)name, sbuff);
	MemFree(sbuff);
}

void IO::FileAnalyse::FrameDetailHandler::AddHexBuff(UOSInt frameOfst, UOSInt size, const Char *name, const UInt8 *vBuff, Bool multiLine)
{
	this->AddHexBuff(frameOfst, size, name, vBuff, ' ', multiLine);
}

void IO::FileAnalyse::FrameDetailHandler::AddHexBuff(UOSInt frameOfst, UOSInt size, const Char *name, const UTF8Char *vBuff, UTF32Char seperator, Bool multiLine)
{
	Text::StringBuilderUTF8 sb;
	sb.AppendHexBuff(vBuff, size, seperator, multiLine?Text::LineBreakType::CRLF:Text::LineBreakType::None);
	this->AddField(frameOfst, size, (const UTF8Char*)name, sb.ToString());
}

void IO::FileAnalyse::FrameDetailHandler::AddIPv4(UOSInt frameOfst, const Char *name, const UInt8 *vBuff)
{
	UTF8Char sbuff[32];
	Net::SocketUtil::GetIPv4Name(sbuff, ReadNUInt32(vBuff));
	this->AddField(frameOfst, 4, (const UTF8Char*)name, sbuff);
}

void IO::FileAnalyse::FrameDetailHandler::AddIPv6(UOSInt frameOfst, const Char *name, const UInt8 *vBuff)
{
	UTF8Char sbuff[64];
	Net::SocketUtil::AddressInfo addr;
	Net::SocketUtil::SetAddrInfoV6(&addr, vBuff, 0);
	Net::SocketUtil::GetAddrName(sbuff, &addr);
	this->AddField(frameOfst, 16, (const UTF8Char*)name, sbuff);
}

void IO::FileAnalyse::FrameDetailHandler::AddMACAddr(UOSInt frameOfst, const Char *name, const UInt8 *macBuff, Bool showVendor)
{
	Text::StringBuilderUTF8 sb;
	sb.AppendHexBuff(macBuff, 6, ':', Text::LineBreakType::None);
	if (showVendor)
	{
		const Net::MACInfo::MACEntry *entry;
		sb.AppendC(UTF8STRC(" ("));
		entry = Net::MACInfo::GetMACInfoBuff(macBuff);
		if (entry && entry->name[0])
		{
			sb.Append((const UTF8Char*)entry->name);
		}
		else
		{
			sb.AppendC(UTF8STRC("Unknown"));
		}
		sb.AppendChar(')', 1);
	}
	this->AddField(frameOfst, 6, (const UTF8Char*)name, sb.ToString());
}

void IO::FileAnalyse::FrameDetailHandler::AddNetBIOSName(UOSInt frameOfst, UOSInt size, const Char *name, const UTF8Char *nbName)
{
	UTF8Char sbuff2[17];
	UTF8Char *sptr;
	Text::StringBuilderUTF8 sb;
	sb.Append(nbName);
	if ((sptr = Net::NetBIOSUtil::GetName(sbuff2, nbName)) != 0)
	{
		sptr[-1] = 0;
		Text::StrRTrim(sbuff2);
		sb.AppendC(UTF8STRC(" ("));
		sb.Append(sbuff2);
		sb.AppendC(UTF8STRC(")"));
	}
	this->AddField(frameOfst, size, (const UTF8Char*)name, sb.ToString());
}

void IO::FileAnalyse::FrameDetailHandler::AddTextHexBuff(UOSInt frameOfst, UOSInt size, const UInt8 *vBuff, Bool multiLine)
{
	Text::StringBuilderUTF8 sb;
	sb.AppendHexBuff(vBuff, size, ' ', multiLine?Text::LineBreakType::CRLF:Text::LineBreakType::None);
	this->AddText(frameOfst, sb.ToString());
}
