#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayList.h"
#include "IO/SeekableStream.h"
#include "IO/StreamReader.h"
#include "Sync/Event.h"
#include "Text/Encoding.h"
#include "Text/MyStringW.h"

#define BUFFSIZE 16384

void IO::StreamReader::FillBuffer()
{
	UOSInt i;
	UnsafeArray<UTF8Char> dest;

	if (stm->CanSeek())
	{
		UInt64 currPos = ((IO::SeekableStream*)stm.Ptr())->GetPosition();
		if (this->lastPos != currPos)
		{
			buffSize = 0;
			cSize = 0;
			cPos = 0;
		}
	}

	if (cPos)
	{
		if (cSize > cPos)
		{
			i = cSize - cPos;
			MemCopyO(&cbuff[0], &cbuff[cPos], i);
			cSize = i;
			cPos = 0;
		}
		else
		{
			cPos = 0;
			cSize = 0;
		}
	}

	buffSize += stm->Read(Data::ByteArray(&buff[buffSize], BUFFSIZE - buffSize));
	if (stm->CanSeek())
	{
		lastPos = ((IO::SeekableStream*)stm.Ptr())->GetPosition();
	}
	if (buffSize <= 0)
	{
		return;
	}
	UOSInt convSize = BUFFSIZE - cSize;
	if (convSize > buffSize)
		convSize = buffSize;
	if (convSize)
	{
		dest = this->enc.UTF8FromBytes(&cbuff[cSize], buff, convSize, i);
		cSize = (UOSInt)(dest - cbuff);
		MemCopyO(buff, &buff[i], buffSize - i);
		buffSize -= i;
	}
}

void IO::StreamReader::CheckHeader()
{
	if (buffSize != 0)
		return;
	buffSize += stm->Read(Data::ByteArray(&buff[buffSize], 4 - buffSize));
	if (buffSize >= 3 && buff[0] == 0xef && buff[1] == 0xbb && buff[2] == 0xbf)
	{
		this->enc.SetCodePage(65001);
		buff[0] = buff[3];
		buffSize -= 3;
	}
	else if (buffSize >= 2 && buff[0] == 0xff && buff[1] == 0xfe)
	{
		this->enc.SetCodePage(1200);
		buff[0] = buff[2];
		buff[1] = buff[3];
		buffSize -= 2;
	}
	else if (buffSize >= 2 && buff[0] == 0xfe && buff[1] == 0xff)
	{
		this->enc.SetCodePage(1201);
		buff[0] = buff[2];
		buff[1] = buff[3];
		buffSize -= 2;
	}
	if (stm->CanSeek())
	{
		this->lastPos = ((IO::SeekableStream*)stm.Ptr())->GetPosition();
	}
	else
	{
		this->lastPos = 0;
	}
}

IO::StreamReader::StreamReader(NN<IO::Stream> stm)
{
	this->stm = stm;
	this->buff = MemAlloc(UInt8, BUFFSIZE);
	this->buffSize = 0;
	this->cbuff = MemAlloc(UTF8Char, BUFFSIZE + 1);
	this->cSize = 0;
	this->cPos = 0;
	if (stm->CanSeek())
	{
		this->lastPos = ((IO::SeekableStream*)stm.Ptr())->GetPosition();
	}
	else
	{
		this->lastPos = 0;
	}
	this->lineBreak = Text::LineBreakType::None;
	CheckHeader();
	FillBuffer();
}

IO::StreamReader::StreamReader(NN<IO::Stream> stm, UInt32 codePage) : enc(codePage)
{
	this->stm = stm;
	this->buff = MemAlloc(UInt8, BUFFSIZE);
	this->buffSize = 0;
	this->cbuff = MemAllocArr(UTF8Char, BUFFSIZE + 1);
	this->cSize = 0;
	this->cPos = 0;
	if (stm->CanSeek())
	{
		this->lastPos = ((IO::SeekableStream*)stm.Ptr())->GetPosition();
	}
	else
	{
		this->lastPos = 0;
	}
	this->lineBreak = Text::LineBreakType::None;
	CheckHeader();
	FillBuffer();
}

IO::StreamReader::~StreamReader()
{
	MemFree(this->buff);
	MemFreeArr(this->cbuff);
}

void IO::StreamReader::Close()
{
	this->stm->Close();
}

UnsafeArrayOpt<UTF8Char> IO::StreamReader::ReadLine(UnsafeArray<UTF8Char> buff, UOSInt maxCharCnt)
{
	UnsafeArray<UTF8Char> dest = buff;
	Bool tmp = false;
	if (stm->CanSeek())
	{
		UInt64 currPos = ((IO::SeekableStream*)stm.Ptr())->GetPosition();
		if (this->lastPos != currPos)
		{
			this->FillBuffer();
		}
	}
	UOSInt currPos = cPos;
	UOSInt currSize = cSize;
	UTF8Char c;
	while (true)
	{
		currPos = cPos;
		currSize = cSize;
		UnsafeArray<UTF8Char> src = &cbuff[currPos];
		while (currPos < currSize)
		{
			c = *src;
			if (c == 13)
			{
				if (!tmp && src + 1 == &cbuff[currSize])
				{
					tmp = true;
					break;
				}
				if (src[1] == 10)
				{
					*dest = 0;
					this->cPos = currPos + 2;
					this->lineBreak = Text::LineBreakType::CRLF;
					return dest;
				}
				else
				{
					*dest = 0;
					this->cPos = currPos + 1;
					this->lineBreak = Text::LineBreakType::CR;
					return dest;
				}
			}
			else if (c == 10)
			{
				*dest = 0;
				this->cPos = currPos + 1;
				this->lineBreak = Text::LineBreakType::LF;
				return dest;
			}
			else if (maxCharCnt <= 0)
			{
				*dest = 0;
				this->lineBreak = Text::LineBreakType::None;
				this->cPos = currPos;
				return dest;
			}

			*dest++ = c;
			src++;
			currPos++;
			maxCharCnt--;
		}
		this->cPos = currPos;
		FillBuffer();
		if (cSize <= cPos)
		{
			if (buff == dest)
			{
				return 0;
			}
			else
			{
				*dest = 0;
				this->lineBreak = Text::LineBreakType::None;
				return dest;
			}
		}
	}
	return 0;
}

Bool IO::StreamReader::ReadLine(NN<Text::StringBuilderUTF8> sb, UOSInt maxCharCnt)
{
	sb->AllocLeng(maxCharCnt);
	UnsafeArray<UTF8Char> endPtr;
	if (this->ReadLine(sb->GetEndPtr(), maxCharCnt).SetTo(endPtr))
	{
		sb->SetEndPtr(endPtr);
		return true;
	}
	return false;
}

UnsafeArray<UTF8Char> IO::StreamReader::GetLastLineBreak(UnsafeArray<UTF8Char> buff)
{
	if (this->lineBreak == Text::LineBreakType::CR)
	{
		*buff++ = 13;
	}
	else if (this->lineBreak == Text::LineBreakType::LF)
	{
		*buff++ = 10;
	}
	else if (this->lineBreak == Text::LineBreakType::CRLF)
	{
		*buff++ = 13;
		*buff++ = 10;
	}
	*buff = 0;
	return buff;
}

Bool IO::StreamReader::GetLastLineBreak(NN<Text::StringBuilderUTF8> sb)
{
	sb->AppendLB(this->lineBreak);
	return true;
}

Bool IO::StreamReader::IsLineBreak()
{
	return this->lineBreak != Text::LineBreakType::None;
}

Bool IO::StreamReader::ReadToEnd(NN<Text::StringBuilderUTF8> sb)
{
	Bool succ = false;
	while (this->ReadLine(sb, BUFFSIZE))
	{
		succ = this->GetLastLineBreak(sb);
	}
	return succ;
}
