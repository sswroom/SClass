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

void Media::AudioFormat::ToString(Text::StringBuilderUTF *sb)
{
	sb->Append((const UTF8Char*)"Foramt Id = 0x");
	sb->AppendHex32V(this->formatId);
	sb->Append((const UTF8Char*)" (");
	sb->Append(GetNameFormatId(this->formatId));
	sb->Append((const UTF8Char*)")");
	sb->Append((const UTF8Char*)"\r\nFrequency = ");
	sb->AppendU32(this->frequency);
	sb->Append((const UTF8Char*)" Hz\r\nBit Per Sample = ");
	sb->AppendU16(this->bitpersample);
	sb->Append((const UTF8Char*)"\r\nNumber of channels = ");
	sb->AppendU16(this->nChannels);
	sb->Append((const UTF8Char*)"\r\nBit Rate = ");
	sb->AppendU32(this->bitRate);
	sb->Append((const UTF8Char*)"\r\nAlign = ");
	sb->AppendU32(this->align);
	sb->Append((const UTF8Char*)"\r\nOther = ");
	sb->AppendU32(this->other);
	sb->Append((const UTF8Char*)"\r\nInteger Type = ");
	sb->Append(GetNameIntType(this->intType));
	sb->Append((const UTF8Char*)"\r\nExtra Size = ");
	sb->AppendU32(this->extraSize);
}

const UTF8Char *Media::AudioFormat::GetNameFormatId(UInt32 formatId)
{
	switch (formatId)
	{
	case 1:
		return (const UTF8Char*)"LPCM (Int)";
	case 3:
		return (const UTF8Char*)"LPCM (Float)";
	case 0x7:
		return (const UTF8Char*)"G.711 mu-Law";
	case 0x50:
		return (const UTF8Char*)"MPEG-1 Audio Layer 2";
	case 0xFF:
		return (const UTF8Char*)"AAC";
	case 0x2080:
		return (const UTF8Char*)"ADX";
	case 0x2081:
		return (const UTF8Char*)"PSSA";
	case 0x2082:
		return (const UTF8Char*)"XA";
	default:
		return (const UTF8Char*)"Unknown";
	}
}

const UTF8Char *Media::AudioFormat::GetNameIntType(IntType intType)
{
	switch (intType)
	{
	case IT_NORMAL:
		return (const UTF8Char*)"Little Endian";
	case IT_BIGENDIAN:
		return (const UTF8Char*)"Big Endian";
	case IT_BIGENDIAN16:
		return (const UTF8Char*)"Big Endian 16-bit";
	default:
		return (const UTF8Char*)"Unknown";
	}
}
