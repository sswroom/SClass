#include "Stdafx.h"
#include "Core/ByteTool_C.h"
#include "IO/FileAnalyse/FrameDetailHandler.h"
#include "Net/MACInfo.h"
#include "Net/NetBIOSUtil.h"
#include "Net/SocketUtil.h"
#include "Text/MyStringFloat.h"
#include "Text/String.h"
#include "Text/StringBuilderUTF8.h"

void IO::FileAnalyse::FrameDetailHandler::AddBool(UOSInt frameOfst, Text::CStringNN name, UInt8 v)
{
	if (v == 0)
	{
		this->AddField(frameOfst, 1, name, CSTR("false"));
	}
	else if (v == 1)
	{
		this->AddField(frameOfst, 1, name, CSTR("true"));
	}
	else
	{
		UTF8Char sbuff[16];
		UnsafeArray<UTF8Char> sptr;
		sptr = Text::StrHexByte(Text::StrConcatC(sbuff, UTF8STRC("0x")), v);
		this->AddField(frameOfst, 1, name, CSTRP(sbuff, sptr));
	}
}

void IO::FileAnalyse::FrameDetailHandler::AddInt64(UOSInt frameOfst, Text::CStringNN name, Int64 v)
{
	UTF8Char sbuff[24];
	UnsafeArray<UTF8Char> sptr;
	sptr = Text::StrInt64(sbuff, v);
	this->AddField(frameOfst, 8, name, CSTRP(sbuff, sptr));
}

void IO::FileAnalyse::FrameDetailHandler::AddInt64V(UOSInt frameOfst, UOSInt size, Text::CStringNN name, Int64 v)
{
	UTF8Char sbuff[24];
	UnsafeArray<UTF8Char> sptr;
	sptr = Text::StrInt64(sbuff, v);
	this->AddField(frameOfst, size, name, CSTRP(sbuff, sptr));
}

void IO::FileAnalyse::FrameDetailHandler::AddUInt64(UOSInt frameOfst, Text::CStringNN name, UInt64 v)
{
	UTF8Char sbuff[24];
	UnsafeArray<UTF8Char> sptr;
	sptr = Text::StrUInt64(sbuff, v);
	this->AddField(frameOfst, 8, name, CSTRP(sbuff, sptr));
}

void IO::FileAnalyse::FrameDetailHandler::AddUInt64V(UOSInt frameOfst, UOSInt size, Text::CStringNN name, UInt64 v)
{
	UTF8Char sbuff[24];
	UnsafeArray<UTF8Char> sptr;
	sptr = Text::StrUInt64(sbuff, v);
	this->AddField(frameOfst, size, name, CSTRP(sbuff, sptr));
}

void IO::FileAnalyse::FrameDetailHandler::AddInt(UOSInt frameOfst, UOSInt size, Text::CStringNN name, OSInt v)
{
	UTF8Char sbuff[16];
	UnsafeArray<UTF8Char> sptr;
	sptr = Text::StrOSInt(sbuff, v);
	this->AddField(frameOfst, size, name, CSTRP(sbuff, sptr));
}

void IO::FileAnalyse::FrameDetailHandler::AddUInt(UOSInt frameOfst, UOSInt size, Text::CStringNN name, UOSInt v)
{
	UTF8Char sbuff[16];
	UnsafeArray<UTF8Char> sptr;
	sptr = Text::StrUOSInt(sbuff, v);
	this->AddField(frameOfst, size, name, CSTRP(sbuff, sptr));
}

void IO::FileAnalyse::FrameDetailHandler::AddBit(UOSInt frameOfst, Text::CStringNN name, UInt8 v, UOSInt bitNum)
{
	UTF8Char sbuff[16];
	UnsafeArray<UTF8Char> sptr;
	sptr = Text::StrUOSInt(sbuff, ((UOSInt)v >> bitNum) & 1);
	this->AddField(frameOfst, 1, name, CSTRP(sbuff, sptr));
}

void IO::FileAnalyse::FrameDetailHandler::AddUIntName(UOSInt frameOfst, UOSInt size, Text::CStringNN name, UOSInt v, Text::CString vName)
{
	Text::StringBuilderUTF8 sb;
	sb.AppendUOSInt(v);
	Text::CStringNN nns;
	if (vName.SetTo(nns))
	{
		sb.AppendC(UTF8STRC(" ("));
		sb.Append(nns);
		sb.AppendUTF8Char(')');
	}
	else
	{
		sb.AppendC(UTF8STRC(" (Unkonwn)"));
	}
	this->AddField(frameOfst, size, name, sb.ToCString());
}

void IO::FileAnalyse::FrameDetailHandler::AddUInt64Name(UOSInt frameOfst, UOSInt size, Text::CStringNN name, UInt64 v, Text::CString vName)
{
	Text::StringBuilderUTF8 sb;
	sb.AppendU64(v);
	Text::CStringNN nns;
	if (vName.SetTo(nns))
	{
		sb.AppendC(UTF8STRC(" ("));
		sb.Append(nns);
		sb.AppendUTF8Char(')');
	}
	else
	{
		sb.AppendC(UTF8STRC(" (Unkonwn)"));
	}
	this->AddField(frameOfst, size, name, sb.ToCString());
}

void IO::FileAnalyse::FrameDetailHandler::AddFloat(UOSInt frameOfst, UOSInt size, Text::CStringNN name, Double v)
{
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	sptr = Text::StrDouble(sbuff, v);
	this->AddField(frameOfst, size, name, CSTRP(sbuff, sptr));
}

void IO::FileAnalyse::FrameDetailHandler::AddHex8(UOSInt frameOfst, Text::CStringNN name, UInt8 v)
{
	UTF8Char sbuff[16];
	UnsafeArray<UTF8Char> sptr;
	sptr = Text::StrHexByte(Text::StrConcatC(sbuff, UTF8STRC("0x")), v);
	this->AddField(frameOfst, 1, name, CSTRP(sbuff, sptr));
}

void IO::FileAnalyse::FrameDetailHandler::AddHex16(UOSInt frameOfst, Text::CStringNN name, UInt16 v)
{
	UTF8Char sbuff[16];
	UnsafeArray<UTF8Char> sptr;
	sptr = Text::StrHexVal16(Text::StrConcatC(sbuff, UTF8STRC("0x")), v);
	this->AddField(frameOfst, 2, name, CSTRP(sbuff, sptr));
}

void IO::FileAnalyse::FrameDetailHandler::AddHex24(UOSInt frameOfst, Text::CStringNN name, UInt32 v)
{
	UTF8Char sbuff[16];
	UnsafeArray<UTF8Char> sptr;
	sptr = Text::StrHexVal24(Text::StrConcatC(sbuff, UTF8STRC("0x")), v);
	this->AddField(frameOfst, 3, name, CSTRP(sbuff, sptr));
}

void IO::FileAnalyse::FrameDetailHandler::AddHex32(UOSInt frameOfst, Text::CStringNN name, UInt32 v)
{
	UTF8Char sbuff[16];
	UnsafeArray<UTF8Char> sptr;
	sptr = Text::StrHexVal32(Text::StrConcatC(sbuff, UTF8STRC("0x")), v);
	this->AddField(frameOfst, 4, name, CSTRP(sbuff, sptr));
}

void IO::FileAnalyse::FrameDetailHandler::AddHex64(UOSInt frameOfst, Text::CStringNN name, UInt64 v)
{
	UTF8Char sbuff[19];
	UnsafeArray<UTF8Char> sptr;
	sptr = Text::StrHexVal64(Text::StrConcatC(sbuff, UTF8STRC("0x")), v);
	this->AddField(frameOfst, 8, name, CSTRP(sbuff, sptr));
}

void IO::FileAnalyse::FrameDetailHandler::AddHex64V(UOSInt frameOfst, UOSInt size, Text::CStringNN name, UInt64 v)
{
	UTF8Char sbuff[19];
	UnsafeArray<UTF8Char> sptr;
	sptr = Text::StrHexVal64V(Text::StrConcatC(sbuff, UTF8STRC("0x")), v);
	this->AddField(frameOfst, size, name, CSTRP(sbuff, sptr));
}

void IO::FileAnalyse::FrameDetailHandler::AddHex8Name(UOSInt frameOfst, Text::CStringNN name, UInt8 v, Text::CString vName)
{
	Text::StringBuilderUTF8 sb;
	sb.AppendC(UTF8STRC("0x"));
	sb.AppendHex8(v);
	Text::CStringNN nns;
	if (vName.SetTo(nns))
	{
		sb.AppendC(UTF8STRC(" ("));
		sb.Append(nns);
		sb.AppendUTF8Char(')');
	}
	else
	{
		sb.AppendC(UTF8STRC(" (Unkonwn)"));
	}
	this->AddField(frameOfst, 1, name, sb.ToCString());
}

void IO::FileAnalyse::FrameDetailHandler::AddHex16Name(UOSInt frameOfst, Text::CStringNN name, UInt16 v, Text::CString vName)
{
	Text::StringBuilderUTF8 sb;
	sb.AppendC(UTF8STRC("0x"));
	sb.AppendHex16(v);
	Text::CStringNN nns;
	if (vName.SetTo(nns))
	{
		sb.AppendC(UTF8STRC(" ("));
		sb.Append(nns);
		sb.AppendUTF8Char(')');
	}
	else
	{
		sb.AppendC(UTF8STRC(" (Unkonwn)"));
	}
	this->AddField(frameOfst, 2, name, sb.ToCString());
}

void IO::FileAnalyse::FrameDetailHandler::AddHex32Name(UOSInt frameOfst, Text::CStringNN name, UInt32 v, Text::CString vName)
{
	Text::StringBuilderUTF8 sb;
	sb.AppendC(UTF8STRC("0x"));
	sb.AppendHex32(v);
	Text::CStringNN nns;
	if (vName.SetTo(nns))
	{
		sb.AppendC(UTF8STRC(" ("));
		sb.Append(nns);
		sb.AppendUTF8Char(')');
	}
	else
	{
		sb.AppendC(UTF8STRC(" (Unkonwn)"));
	}
	this->AddField(frameOfst, 4, name, sb.ToCString());
}

void IO::FileAnalyse::FrameDetailHandler::AddStrC(UOSInt frameOfst, UOSInt size, Text::CStringNN name, UnsafeArray<const UTF8Char> vBuff)
{
	NN<Text::String> s = Text::String::New(vBuff, size);
	this->AddField(frameOfst, size, name, s->ToCString());
	s->Release();
}

void IO::FileAnalyse::FrameDetailHandler::AddStrS(UOSInt frameOfst, UOSInt size, Text::CStringNN name, UnsafeArray<const UTF8Char> vBuff)
{
	UnsafeArray<UTF8Char> sbuff = MemAllocArr(UTF8Char, size + 1);
	UnsafeArray<UTF8Char> sptr;
	sptr = Text::StrConcatS(sbuff, vBuff, size);
	this->AddField(frameOfst, size, name, CSTRP(sbuff, sptr));
	MemFreeArr(sbuff);
}

void IO::FileAnalyse::FrameDetailHandler::AddStrZ(UOSInt frameOfst, Text::CStringNN name, UnsafeArray<const UTF8Char> vBuff)
{
	UOSInt strlen = Text::StrCharCnt(vBuff);
	this->AddField(frameOfst, strlen + 1, name, Text::CStringNN(vBuff, strlen));
}

void IO::FileAnalyse::FrameDetailHandler::AddHexBuff(UOSInt frameOfst, UOSInt size, Text::CStringNN name, UnsafeArray<const UInt8> vBuff, Bool multiLine)
{
	this->AddHexBuff(frameOfst, size, name, vBuff, ' ', multiLine);
}

void IO::FileAnalyse::FrameDetailHandler::AddHexBuff(UOSInt frameOfst, UOSInt size, Text::CStringNN name, UnsafeArray<const UTF8Char> vBuff, UTF8Char seperator, Bool multiLine)
{
	Text::StringBuilderUTF8 sb;
	sb.AppendHexBuff(vBuff, size, seperator, multiLine?Text::LineBreakType::CRLF:Text::LineBreakType::None);
	this->AddField(frameOfst, size, name, sb.ToCString());
}

void IO::FileAnalyse::FrameDetailHandler::AddHexBuff(UOSInt frameOfst, Text::CStringNN name, Data::ByteArrayR vBuff, Bool multiLine)
{
	this->AddHexBuff(frameOfst, name, vBuff, ' ', multiLine);
}

void IO::FileAnalyse::FrameDetailHandler::AddHexBuff(UOSInt frameOfst, Text::CStringNN name, Data::ByteArrayR vBuff, UTF8Char seperator, Bool multiLine)
{
	Text::StringBuilderUTF8 sb;
	sb.AppendHexBuff(vBuff, seperator, multiLine?Text::LineBreakType::CRLF:Text::LineBreakType::None);
	this->AddField(frameOfst, vBuff.GetSize(), name, sb.ToCString());
}

void IO::FileAnalyse::FrameDetailHandler::AddIPv4(UOSInt frameOfst, Text::CStringNN name, UnsafeArray<const UInt8> vBuff)
{
	UTF8Char sbuff[32];
	UnsafeArray<UTF8Char> sptr;
	sptr = Net::SocketUtil::GetIPv4Name(sbuff, ReadNUInt32(&vBuff[0]));
	this->AddField(frameOfst, 4, name, CSTRP(sbuff, sptr));
}

void IO::FileAnalyse::FrameDetailHandler::AddIPv6(UOSInt frameOfst, Text::CStringNN name, UnsafeArray<const UInt8> vBuff)
{
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	Net::SocketUtil::AddressInfo addr;
	Net::SocketUtil::SetAddrInfoV6(addr, vBuff, 0);
	sptr = Net::SocketUtil::GetAddrName(sbuff, addr).Or(sbuff);
	this->AddField(frameOfst, 16, name, CSTRP(sbuff, sptr));
}

void IO::FileAnalyse::FrameDetailHandler::AddMACAddr(UOSInt frameOfst, Text::CStringNN name, UnsafeArray<const UInt8> macBuff, Bool showVendor)
{
	Text::StringBuilderUTF8 sb;
	sb.AppendHexBuff(macBuff, 6, ':', Text::LineBreakType::None);
	if (showVendor)
	{
		NN<const Net::MACInfo::MACEntry> entry;
		sb.AppendC(UTF8STRC(" ("));
		entry = Net::MACInfo::GetMACInfoBuff(macBuff);
		if (entry->nameLen > 0)
		{
			sb.AppendC(entry->name, entry->nameLen);
		}
		else
		{
			sb.AppendC(UTF8STRC("Unknown"));
		}
		sb.AppendUTF8Char(')');
	}
	this->AddField(frameOfst, 6, name, sb.ToCString());
}

void IO::FileAnalyse::FrameDetailHandler::AddNetBIOSName(UOSInt frameOfst, UOSInt size, Text::CStringNN name, Text::CStringNN nbName)
{
	UTF8Char sbuff2[17];
	UnsafeArray<UTF8Char> sptr;
	Text::StringBuilderUTF8 sb;
	sb.Append(nbName);
	if (Net::NetBIOSUtil::GetName(sbuff2, nbName.v).SetTo(sptr))
	{
		sptr[-1] = 0;
		sb.AppendC(UTF8STRC(" ("));
		sb.AppendC(sbuff2, (UOSInt)(sptr - sbuff2 - 1));
		sb.RTrim();
		sb.AppendC(UTF8STRC(")"));
	}
	this->AddField(frameOfst, size, name, sb.ToCString());
}

void IO::FileAnalyse::FrameDetailHandler::AddTextHexBuff(UOSInt frameOfst, UOSInt size, UnsafeArray<const UInt8> vBuff, Bool multiLine)
{
	Text::StringBuilderUTF8 sb;
	sb.AppendHexBuff(vBuff, size, ' ', multiLine?Text::LineBreakType::CRLF:Text::LineBreakType::None);
	this->AddText(frameOfst, sb.ToCString());
}
