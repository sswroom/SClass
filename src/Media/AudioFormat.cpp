#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/AudioFormat.h"

Media::AudioFormat::AudioFormat()
{
	this->formatId = 0;
	this->align = 0;
	this->other = 0;
	this->intType = Media::AudioFormat::IT_NORMAL;
	this->extra = 0;
	this->extraSize = 0;
}

Media::AudioFormat::~AudioFormat()
{
	if (this->extra)
	{
		MemFree(this->extra);
		this->extra = 0;
		this->extraSize = 0;
	}
}

void Media::AudioFormat::FromWAVEFORMATEX(UInt8 *fmt)
{
	this->formatId = *(UInt16*)fmt;
	this->nChannels = *(UInt16*)&fmt[2];
	this->frequency = *(UInt32*)&fmt[4];
	this->bitpersample = *(UInt16*)&fmt[14];
	this->bitRate = (*(UInt32*)&fmt[8]) << 3;
	this->align = *(UInt16*)&fmt[12];
	this->other = 0;
	this->intType = Media::AudioFormat::IT_NORMAL;
	this->extraSize = 0;
	if (this->extra)
	{
		MemFree(this->extra);
		this->extra = 0;
	}
	if (this->formatId == 1)
	{
		this->extra = 0;
		this->extraSize = 0;
	}
	else
	{
		this->extraSize = *(UInt16*)&fmt[16];
		if (this->extraSize > 0)
		{
			this->extra = MemAlloc(UInt8, this->extraSize);
			MemCopyNO(this->extra, &fmt[18], this->extraSize);
		}
	}
}

void Media::AudioFormat::Clear()
{
	if (this->extra)
	{
		MemFree(this->extra);
		this->extra = 0;
		this->extraSize = 0;
	}
	this->formatId = 0;
	this->nChannels = 0;
	this->frequency = 0;
	this->bitpersample = 0;
	this->bitRate = 0;
	this->align = 0;
	this->other = 0;
	this->intType = Media::AudioFormat::IT_NORMAL;
	this->extraSize = 0;
	this->extra = 0;
}

void Media::AudioFormat::FromAudioFormat(Media::AudioFormat *fmt)
{
	this->formatId = fmt->formatId;
	this->nChannels = fmt->nChannels;
	this->frequency = fmt->frequency;
	this->bitpersample = fmt->bitpersample;
	this->bitRate = fmt->bitRate;
	this->align = fmt->align;
	this->other = fmt->other;
	this->intType = fmt->intType;
	this->extraSize = fmt->extraSize;
	if (this->extra)
	{
		MemFree(this->extra);
		this->extra = 0;
	}
	if (this->extraSize > 0)
	{
		this->extra = MemAlloc(UInt8, this->extraSize);
		MemCopyNO(this->extra, fmt->extra, this->extraSize);
	}
}

void Media::AudioFormat::ToString(NotNullPtr<Text::StringBuilderUTF8> sb) const
{
	sb->AppendC(UTF8STRC("Foramt Id = 0x"));
	sb->AppendHex32V(this->formatId);
	sb->AppendC(UTF8STRC(" ("));
	sb->Append(GetNameFormatId(this->formatId));
	sb->AppendC(UTF8STRC(")"));
	sb->AppendC(UTF8STRC("\r\nFrequency = "));
	sb->AppendU32(this->frequency);
	sb->AppendC(UTF8STRC(" Hz\r\nBit Per Sample = "));
	sb->AppendU16(this->bitpersample);
	sb->AppendC(UTF8STRC("\r\nNumber of channels = "));
	sb->AppendU16(this->nChannels);
	sb->AppendC(UTF8STRC("\r\nBit Rate = "));
	sb->AppendU32(this->bitRate);
	sb->AppendC(UTF8STRC("\r\nAlign = "));
	sb->AppendU32(this->align);
	sb->AppendC(UTF8STRC("\r\nOther = "));
	sb->AppendU32(this->other);
	sb->AppendC(UTF8STRC("\r\nInteger Type = "));
	sb->Append(GetNameIntType(this->intType));
	sb->AppendC(UTF8STRC("\r\nExtra Size = "));
	sb->AppendU32(this->extraSize);
}

Text::CString Media::AudioFormat::GetNameFormatId(UInt32 formatId)
{
	switch (formatId)
	{
	case 1:
		return CSTR("LPCM (Int)");
	case 3:
		return CSTR("LPCM (Float)");
	case 0x7:
		return CSTR("G.711 mu-Law");
	case 0x50:
		return CSTR("MPEG-1 Audio Layer 2");
	case 0xFF:
		return CSTR("AAC");
	case 0x2080:
		return CSTR("ADX");
	case 0x2081:
		return CSTR("PSSA");
	case 0x2082:
		return CSTR("XA");
	default:
		return CSTR("Unknown");
	}
}

Text::CString Media::AudioFormat::GetNameIntType(IntType intType)
{
	switch (intType)
	{
	case IT_NORMAL:
		return CSTR("Little Endian");
	case IT_BIGENDIAN:
		return CSTR("Big Endian");
	case IT_BIGENDIAN16:
		return CSTR("Big Endian 16-bit");
	default:
		return CSTR("Unknown");
	}
}
