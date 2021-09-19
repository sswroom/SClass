#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "IO/FileAnalyse/FrameDetailHandler.h"
#include "Net/MACInfo.h"
#include "Net/SocketUtil.h"
#include "Text/StringBuilderUTF8.h"

void IO::FileAnalyse::FrameDetailHandler::AddBool(UInt32 frameOfst, const Char *name, UInt8 v)
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
		Text::StrHexByte(Text::StrConcat(sbuff, (const UTF8Char*)"0x"), v);
		this->AddField(frameOfst, 1, (const UTF8Char*)name, sbuff);
	}
}

void IO::FileAnalyse::FrameDetailHandler::AddInt64(UInt32 frameOfst, const Char *name, Int64 v)
{
	UTF8Char sbuff[24];
	Text::StrInt64(sbuff, v);
	this->AddField(frameOfst, 8, (const UTF8Char*)name, sbuff);
}

void IO::FileAnalyse::FrameDetailHandler::AddInt(UInt32 frameOfst, UInt32 size, const Char *name, OSInt v)
{
	UTF8Char sbuff[16];
	Text::StrOSInt(sbuff, v);
	this->AddField(frameOfst, size, (const UTF8Char*)name, sbuff);
}

void IO::FileAnalyse::FrameDetailHandler::AddUInt(UInt32 frameOfst, UInt32 size, const Char *name, UOSInt v)
{
	UTF8Char sbuff[16];
	Text::StrUOSInt(sbuff, v);
	this->AddField(frameOfst, size, (const UTF8Char*)name, sbuff);
}

void IO::FileAnalyse::FrameDetailHandler::AddUIntName(UInt32 frameOfst, UInt32 size, const Char *name, UOSInt v, const UTF8Char *vName)
{
	Text::StringBuilderUTF8 sb;
	sb.AppendUOSInt(v);
	if (vName)
	{
		sb.Append((const UTF8Char*)" (");
		sb.Append(vName);
		sb.AppendChar(')', 1);
	}
	else
	{
		sb.Append((const UTF8Char*)" (Unkonwn)");
	}
	this->AddField(frameOfst, size, (const UTF8Char*)name, sb.ToString());
}

void IO::FileAnalyse::FrameDetailHandler::AddHex8(UInt32 frameOfst, const Char *name, UInt8 v)
{
	UTF8Char sbuff[16];
	Text::StrHexByte(Text::StrConcat(sbuff, (const UTF8Char*)"0x"), v);
	this->AddField(frameOfst, 1, (const UTF8Char*)name, sbuff);
}

void IO::FileAnalyse::FrameDetailHandler::AddHex16(UInt32 frameOfst, const Char *name, UInt16 v)
{
	UTF8Char sbuff[16];
	Text::StrHexVal16(Text::StrConcat(sbuff, (const UTF8Char*)"0x"), v);
	this->AddField(frameOfst, 2, (const UTF8Char*)name, sbuff);
}

void IO::FileAnalyse::FrameDetailHandler::AddHex24(UInt32 frameOfst, const Char *name, UInt32 v)
{
	UTF8Char sbuff[16];
	Text::StrHexVal24(Text::StrConcat(sbuff, (const UTF8Char*)"0x"), v);
	this->AddField(frameOfst, 3, (const UTF8Char*)name, sbuff);
}

void IO::FileAnalyse::FrameDetailHandler::AddHex32(UInt32 frameOfst, const Char *name, UInt32 v)
{
	UTF8Char sbuff[16];
	Text::StrHexVal32(Text::StrConcat(sbuff, (const UTF8Char*)"0x"), v);
	this->AddField(frameOfst, 4, (const UTF8Char*)name, sbuff);
}

void IO::FileAnalyse::FrameDetailHandler::AddHex8Name(UInt32 frameOfst, const Char *name, UInt8 v, const UTF8Char *vName)
{
	Text::StringBuilderUTF8 sb;
	sb.Append((const UTF8Char*)"0x");
	sb.AppendHex8(v);
	if (vName)
	{
		sb.Append((const UTF8Char*)" (");
		sb.Append(vName);
		sb.AppendChar(')', 1);
	}
	else
	{
		sb.Append((const UTF8Char*)" (Unkonwn)");
	}
	this->AddField(frameOfst, 1, (const UTF8Char*)name, sb.ToString());
}

void IO::FileAnalyse::FrameDetailHandler::AddHex16Name(UInt32 frameOfst, const Char *name, UInt16 v, const UTF8Char *vName)
{
	Text::StringBuilderUTF8 sb;
	sb.Append((const UTF8Char*)"0x");
	sb.AppendHex16(v);
	if (vName)
	{
		sb.Append((const UTF8Char*)" (");
		sb.Append(vName);
		sb.AppendChar(')', 1);
	}
	else
	{
		sb.Append((const UTF8Char*)" (Unkonwn)");
	}
	this->AddField(frameOfst, 2, (const UTF8Char*)name, sb.ToString());
}

void IO::FileAnalyse::FrameDetailHandler::AddStrC(UInt32 frameOfst, UInt32 size, const Char *name, const UTF8Char *vBuff)
{
	const UTF8Char *csptr = Text::StrCopyNewC(vBuff, size);
	this->AddField(frameOfst, size, (const UTF8Char*)name, csptr);
	Text::StrDelNew(csptr);
}

void IO::FileAnalyse::FrameDetailHandler::AddHexBuff(UInt32 frameOfst, UInt32 size, const Char *name, const UTF8Char *vBuff, Bool multiLine)
{
	Text::StringBuilderUTF8 sb;
	sb.AppendHexBuff(vBuff, size, ' ', multiLine?Text::LBT_CRLF:Text::LBT_NONE);
	this->AddField(frameOfst, size, (const UTF8Char*)name, sb.ToString());
}

void IO::FileAnalyse::FrameDetailHandler::AddIPv4(UInt32 frameOfst, const Char *name, const UInt8 *vBuff)
{
	UTF8Char sbuff[32];
	Net::SocketUtil::GetIPv4Name(sbuff, ReadNUInt32(vBuff));
	this->AddField(frameOfst, 4, (const UTF8Char*)name, sbuff);
}

void IO::FileAnalyse::FrameDetailHandler::AddMACAddr(UInt32 frameOfst, const Char *name, const UInt8 *macBuff, Bool showVendor)
{
	Text::StringBuilderUTF8 sb;
	sb.AppendHexBuff(macBuff, 6, ':', Text::LBT_NONE);
	if (showVendor)
	{
		const Net::MACInfo::MACEntry *entry;
		sb.Append((const UTF8Char*)" (");
		entry = Net::MACInfo::GetMACInfoBuff(macBuff);
		if (entry && entry->name[0])
		{
			sb.Append((const UTF8Char*)entry->name);
		}
		else
		{
			sb.Append((const UTF8Char*)"Unknown");
		}
		sb.AppendChar(')', 1);
	}
	this->AddField(frameOfst, 6, (const UTF8Char*)name, sb.ToString());
}
