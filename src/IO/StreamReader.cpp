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
	WChar *src;
	WChar *dest;

	if (stm->CanSeek())
	{
		UInt64 currPos = ((IO::SeekableStream*)stm)->GetPosition();
		if (this->lastPos != currPos)
		{
			buffSize = 0;
			wcSize = 0;
			wcPos = 0;
		}
	}

	if (wcPos)
	{
		if (wcSize > wcPos)
		{
			i = wcSize - wcPos;
			src = &wcbuff[wcPos];
			dest = &wcbuff[0];
			wcSize = i;
			wcPos = 0;
			while (i-- > 0)
			{
				*dest++ = *src++;
			}
		}
		else
		{
			wcPos = 0;
			wcSize = 0;
		}
	}

	buffSize += stm->Read(&buff[buffSize], BUFFSIZE - buffSize);
	if (stm->CanSeek())
	{
		lastPos = ((IO::SeekableStream*)stm)->GetPosition();
	}
	if (buffSize <= 0)
		return;
	UOSInt convSize = BUFFSIZE - wcSize;
	if (convSize > buffSize)
		convSize = buffSize;
	if (convSize)
	{
		dest = this->enc->WFromBytes(&wcbuff[wcSize], buff, convSize, &i);
		if (dest)
		{
			wcSize = (UOSInt)(dest - wcbuff);
			MemCopyO(buff, &buff[i], buffSize - i);
			buffSize -= i;
		}
		else
		{
			//wcSize = 0;
		}
	}
}

void IO::StreamReader::CheckHeader()
{
	if (buffSize != 0)
		return;
	buffSize += stm->Read(&buff[buffSize], 4 - buffSize);
	if (buff[0] == 0xef && buff[1] == 0xbb && buff[2] == 0xbf)
	{
		enc->SetCodePage(65001);
		buff[0] = buff[3];
		buffSize -= 3;
	}
	else if (buff[0] == 0xff && buff[1] == 0xfe)
	{
		enc->SetCodePage(1200);
		buff[0] = buff[2];
		buff[1] = buff[3];
		buffSize -= 2;
	}
	else if (buff[0] == 0xfe && buff[1] == 0xff)
	{
		enc->SetCodePage(1201);
		buff[0] = buff[2];
		buff[1] = buff[3];
		buffSize -= 2;
	}
	if (stm->CanSeek())
	{
		this->lastPos = ((IO::SeekableStream*)stm)->GetPosition();
	}
	else
	{
		this->lastPos = 0;
	}
}

IO::StreamReader::StreamReader(IO::Stream *stm)
{
	this->stm = stm;
	this->buff = MemAlloc(UInt8, BUFFSIZE);
	this->buffSize = 0;
	this->wcbuff = MemAlloc(WChar, BUFFSIZE + 1);
	this->wcSize = 0;
	this->wcPos = 0;
	if (stm->CanSeek())
	{
		this->lastPos = ((IO::SeekableStream*)stm)->GetPosition();
	}
	else
	{
		this->lastPos = 0;
	}
	this->lineBreak = 0;
	NEW_CLASS(this->enc, Text::Encoding());
	CheckHeader();
	FillBuffer();
}

IO::StreamReader::StreamReader(IO::Stream *stm, Int32 codePage)
{
	this->stm = stm;
	this->buff = MemAlloc(UInt8, BUFFSIZE);
	this->buffSize = 0;
	this->wcbuff = MemAlloc(WChar, BUFFSIZE + 1);
	this->wcSize = 0;
	this->wcPos = 0;
	if (stm->CanSeek())
	{
		this->lastPos = ((IO::SeekableStream*)stm)->GetPosition();
	}
	else
	{
		this->lastPos = 0;
	}
	this->lineBreak = 0;
	NEW_CLASS(this->enc, Text::Encoding(codePage));
	CheckHeader();
	FillBuffer();
}

IO::StreamReader::~StreamReader()
{
	MemFree(this->buff);
	MemFree(this->wcbuff);
	if (this->enc)
	{
		DEL_CLASS(this->enc);
	}
}

void IO::StreamReader::Close()
{
	this->stm->Close();
}

Int32 IO::StreamReader::Peek()
{
	if (wcPos < wcSize)
	{
		return wcbuff[wcPos];
	}
	else
	{
		FillBuffer();
		if (wcPos < wcSize)
		{
			return wcbuff[wcPos];
		}
		else
		{
			return 0;
		}
	}
}

WChar IO::StreamReader::Read()
{
	if (wcPos < wcSize)
	{
		return wcbuff[wcPos++];
	}
	else
	{
		FillBuffer();
		if (wcPos < wcSize)
		{
			return wcbuff[wcPos++];
		}
		else
		{
			return 0;
		}
	}
}

WChar *IO::StreamReader::Read(WChar *buff, UOSInt charCnt)
{
	WChar *dest = buff;
	while (true)
	{
		WChar *src = &wcbuff[wcPos];
		while (wcPos < wcSize)
		{
			if (charCnt-- <= 0)
			{
				*dest = 0;
				return dest;
			}
			*dest++ = *src++;
			wcPos++;
		}
		FillBuffer();
		if (wcSize <= wcPos)
		{
			if (buff == dest)
			{
				return 0;
			}
			else
			{
				*dest = 0;
				return dest;
			}
		}
	}
	return 0;
}

WChar *IO::StreamReader::ReadLine(WChar *buff)
{
	WChar *dest = buff;
	Bool tmp = false;
	while (true)
	{
		WChar *src = &wcbuff[wcPos];
		while (wcPos < wcSize)
		{
			if (*src == 13)
			{
				if (!tmp && src + 1 == &wcbuff[wcSize])
				{
					tmp = true;
					break;
				}
				if (src[1] == 10)
				{
					*dest = 0;
					wcPos += 2;
					this->lineBreak = 3;
					return dest;
				}
				else
				{
					*dest = 0;
					wcPos += 1;
					this->lineBreak = 1;
					return dest;
				}
			}
			else if (*src == 10)
			{
				*dest = 0;
				wcPos += 1;
				this->lineBreak = 2;
				return dest;
			}
			*dest++ = *src++;
			wcPos++;
		}
		FillBuffer();
		if (wcSize <= wcPos)
		{
			if (buff == dest)
			{
				return 0;
			}
			else
			{
				*dest = 0;
				this->lineBreak = 0;
				return dest;
			}
		}
	}
	return 0;
}

UTF8Char *IO::StreamReader::ReadLine(UTF8Char *buff, UOSInt maxCharCnt)
{
#if _WCHAR_SIZE == 2
	UTF8Char *dest = buff;
	Bool tmp = false;
	UOSInt currPos = wcPos;
	UOSInt currSize = wcSize;
	WChar c;
	UInt32 code;
	while (true)
	{
		currPos = wcPos;
		currSize = wcSize;
		WChar *src = &wcbuff[currPos];
		while (currPos < currSize)
		{
			c = *src;
			if (c == 13)
			{
				if (!tmp && src + 1 == &wcbuff[currSize])
				{
					tmp = true;
					break;
				}
				if (src[1] == 10)
				{
					*dest = 0;
					this->wcPos = currPos + 2;
					this->lineBreak = 3;
					return dest;
				}
				else
				{
					*dest = 0;
					this->wcPos = currPos + 1;
					this->lineBreak = 1;
					return dest;
				}
			}
			else if (c == 10)
			{
				*dest = 0;
				this->wcPos = currPos + 1;
				this->lineBreak = 2;
				return dest;
			}
			else if (c < 0x80)
			{
				if (maxCharCnt <= 0)
				{
					*dest = 0;
					this->lineBreak = 0;
					this->wcPos = currPos;
					return dest;
				}
				*dest++ = (UInt8)c;
				maxCharCnt--;
				src++;
				currPos++;
			}
			else if (c < 0x800)
			{
				if (maxCharCnt <= 1)
				{
					*dest = 0;
					this->lineBreak = 0;
					this->wcPos = currPos;
					return dest;
				}
				*dest++ = 0xc0 | (c >> 6);
				*dest++ = 0x80 | (c & 0x3f);
				maxCharCnt -= 2;
				src++;
				currPos++;
			}
			else if (c >= 0xd800 && c < 0xdc00)
			{
				if (currPos + 1 >= currSize)
				{
					if (currPos == 0)
					{
						if (maxCharCnt <= 2)
						{
							*dest = 0;
							this->lineBreak = 0;
							this->wcPos = currPos;
							return dest;
						}
						*dest++ = 0xe0 | (c >> 12);
						*dest++ = 0x80 | ((c >> 6) & 0x3f);
						*dest++ = 0x80 | (c & 0x3f);
						maxCharCnt -= 3;
						src++;
						currPos++;
					}
					else
					{
						break;
					}
				}
				else if (src[1] >= 0xdc00 && src[1] < 0xe000)
				{
					code = 0x10000 + ((UTF32Char)(c - 0xd800) << 10) + (src[1] - 0xdc00);
					if (code < 0x200000)
					{
						if (maxCharCnt <= 3)
						{
							*dest = 0;
							this->lineBreak = 0;
							this->wcPos = currPos;
							return dest;
						}
						*dest++ = 0xf0 | (code >> 18);
						*dest++ = 0x80 | ((code >> 12) & 0x3f);
						*dest++ = 0x80 | ((code >> 6) & 0x3f);
						*dest++ = 0x80 | (code & 0x3f);
						maxCharCnt -= 4;
					}
					else if (code < 0x4000000)
					{
						if (maxCharCnt <= 4)
						{
							*dest = 0;
							this->lineBreak = 0;
							this->wcPos = currPos;
							return dest;
						}
						*dest++ = 0xf8 | (code >> 24);
						*dest++ = 0x80 | ((code >> 18) & 0x3f);
						*dest++ = 0x80 | ((code >> 12) & 0x3f);
						*dest++ = 0x80 | ((code >> 6) & 0x3f);
						*dest++ = 0x80 | (code & 0x3f);
						maxCharCnt -= 5;
					}
					else
					{
						if (maxCharCnt <= 5)
						{
							*dest = 0;
							this->lineBreak = 0;
							this->wcPos = currPos;
							return dest;
						}
						*dest++ = 0xfc | (code >> 30);
						*dest++ = 0x80 | ((code >> 24) & 0x3f);
						*dest++ = 0x80 | ((code >> 18) & 0x3f);
						*dest++ = 0x80 | ((code >> 12) & 0x3f);
						*dest++ = 0x80 | ((code >> 6) & 0x3f);
						*dest++ = 0x80 | (code & 0x3f);
						maxCharCnt -= 6;
					}
					src += 2;
					currPos += 2;
				}
				else
				{
					if (maxCharCnt <= 2)
					{
						*dest = 0;
						this->lineBreak = 0;
						this->wcPos = currPos;
						return dest;
					}
					*dest++ = 0xe0 | (c >> 12);
					*dest++ = 0x80 | ((c >> 6) & 0x3f);
					*dest++ = 0x80 | (c & 0x3f);
					maxCharCnt -= 3;
					src++;
					currPos++;
				}
			}
			else
			{
				if (maxCharCnt <= 2)
				{
					*dest = 0;
					this->lineBreak = 0;
					this->wcPos = currPos;
					return dest;
				}
				*dest++ = 0xe0 | (c >> 12);
				*dest++ = 0x80 | ((c >> 6) & 0x3f);
				*dest++ = 0x80 | (c & 0x3f);
				maxCharCnt -= 3;
				src++;
				currPos++;
			}
		}
		this->wcPos = currPos;
		FillBuffer();
		if (wcSize <= wcPos)
		{
			if (buff == dest)
			{
				return 0;
			}
			else
			{
				*dest = 0;
				this->lineBreak = 0;
				return dest;
			}
		}
	}
	return 0;
#elif _WCHAR_SIZE == 4
	UTF8Char *dest = buff;
	Bool tmp = false;
	UOSInt currPos = wcPos;
	UOSInt currSize = wcSize;
	WChar c;
	while (true)
	{
		currPos = wcPos;
		currSize = wcSize;
		WChar *src = &wcbuff[currPos];
		while (currPos < currSize)
		{
			c = *src;
			if (c == 13)
			{
				if (!tmp && src + 1 == &wcbuff[currSize])
				{
					tmp = true;
					break;
				}
				if (src[1] == 10)
				{
					*dest = 0;
					this->wcPos = currPos + 2;
					this->lineBreak = 3;
					return dest;
				}
				else
				{
					*dest = 0;
					this->wcPos = currPos + 1;
					this->lineBreak = 1;
					return dest;
				}
			}
			else if (c == 10)
			{
				*dest = 0;
				this->wcPos = currPos + 1;
				this->lineBreak = 2;
				return dest;
			}
			else if (c < 0x80)
			{
				if (maxCharCnt <= 0)
				{
					*dest = 0;
					this->lineBreak = 0;
					this->wcPos = currPos;
					return dest;
				}
				*dest++ = (UInt8)c;
				maxCharCnt--;
			}
			else if (c < 0x800)
			{
				if (maxCharCnt <= 1)
				{
					*dest = 0;
					this->lineBreak = 0;
					this->wcPos = currPos;
					return dest;
				}
				*dest++ = (UTF8Char)(0xc0 | (c >> 6));
				*dest++ = (UTF8Char)(0x80 | (c & 0x3f));
				maxCharCnt -= 2;
			}
			else if (c < 0x10000)
			{
				if (maxCharCnt <= 2)
				{
					*dest = 0;
					this->lineBreak = 0;
					this->wcPos = currPos;
					return dest;
				}
				*dest++ = (UTF8Char)(0xe0 | (c >> 12));
				*dest++ = (UTF8Char)(0x80 | ((c >> 6) & 0x3f));
				*dest++ = (UTF8Char)(0x80 | (c & 0x3f));
				maxCharCnt -= 3;
			}
			else if (c < 0x200000)
			{
				if (maxCharCnt <= 3)
				{
					*dest = 0;
					this->lineBreak = 0;
					this->wcPos = currPos;
					return dest;
				}
				*dest++ = (UTF8Char)(0xf0 | (c >> 18));
				*dest++ = (UTF8Char)(0x80 | ((c >> 12) & 0x3f));
				*dest++ = (UTF8Char)(0x80 | ((c >> 6) & 0x3f));
				*dest++ = (UTF8Char)(0x80 | (c & 0x3f));
				maxCharCnt -= 4;
			}
			else if (c < 0x4000000)
			{
				if (maxCharCnt <= 4)
				{
					*dest = 0;
					this->lineBreak = 0;
					this->wcPos = currPos;
					return dest;
				}
				*dest++ = (UTF8Char)(0xf8 | (c >> 24));
				*dest++ = (UTF8Char)(0x80 | ((c >> 18) & 0x3f));
				*dest++ = (UTF8Char)(0x80 | ((c >> 12) & 0x3f));
				*dest++ = (UTF8Char)(0x80 | ((c >> 6) & 0x3f));
				*dest++ = (UTF8Char)(0x80 | (c & 0x3f));
				maxCharCnt -= 5;
			}
			else
			{
				if (maxCharCnt <= 5)
				{
					*dest = 0;
					this->lineBreak = 0;
					this->wcPos = currPos;
					return dest;
				}
				*dest++ = (UTF8Char)(0xfc | (c >> 30));
				*dest++ = (UTF8Char)(0x80 | ((c >> 24) & 0x3f));
				*dest++ = (UTF8Char)(0x80 | ((c >> 18) & 0x3f));
				*dest++ = (UTF8Char)(0x80 | ((c >> 12) & 0x3f));
				*dest++ = (UTF8Char)(0x80 | ((c >> 6) & 0x3f));
				*dest++ = (UTF8Char)(0x80 | (c & 0x3f));
				maxCharCnt -= 6;
			}

			src++;
			currPos++;
		}
		this->wcPos = currPos;
		FillBuffer();
		if (wcSize <= wcPos)
		{
			if (buff == dest)
			{
				return 0;
			}
			else
			{
				*dest = 0;
				this->lineBreak = 0;
				return dest;
			}
		}
	}
	return 0;
#endif
}

WChar *IO::StreamReader::ReadLine(WChar *buff, UOSInt maxCharCnt)
{
	WChar *dest = buff;
	Bool tmp = false;
	UOSInt currPos = wcPos;
	UOSInt currSize = wcSize;
	WChar c;
	while (true)
	{
		currPos = wcPos;
		currSize = wcSize;
		WChar *src = &wcbuff[currPos];
		while (currPos < currSize)
		{
			c = *src;
			if (c == 13)
			{
				if (!tmp && src + 1 == &wcbuff[currSize])
				{
					tmp = true;
					break;
				}
				if (src[1] == 10)
				{
					*dest = 0;
					this->wcPos = currPos + 2;
					this->lineBreak = 3;
					return dest;
				}
				else
				{
					*dest = 0;
					this->wcPos = currPos + 1;
					this->lineBreak = 1;
					return dest;
				}
			}
			else if (c == 10)
			{
				*dest = 0;
				this->wcPos = currPos + 1;
				this->lineBreak = 2;
				return dest;
			}
			else if (maxCharCnt <= 0)
			{
				*dest = 0;
				this->lineBreak = 0;
				this->wcPos = currPos;
				return dest;
			}

			*dest++ = c;
			src++;
			currPos++;
			maxCharCnt--;
		}
		this->wcPos = currPos;
		FillBuffer();
		if (wcSize <= wcPos)
		{
			if (buff == dest)
			{
				return 0;
			}
			else
			{
				*dest = 0;
				this->lineBreak = 0;
				return dest;
			}
		}
	}
	return 0;
}

Bool IO::StreamReader::ReadLine(Text::StringBuilderUTF *sb, UOSInt maxCharCnt)
{
	WChar *wptr = MemAlloc(WChar, maxCharCnt + 1);
	WChar *end = ReadLine(wptr, maxCharCnt);
	if (end == 0)
	{
		MemFree(wptr);
		return false;
	}
	const UTF8Char *csptr = Text::StrToUTF8New(wptr);
	sb->Append(csptr);
	Text::StrDelNew(csptr);
	MemFree(wptr);
	return true;
}

UTF8Char *IO::StreamReader::GetLastLineBreak(UTF8Char *buff)
{
	if (this->lineBreak == 1)
	{
		*buff++ = 13;
	}
	else if (this->lineBreak == 2)
	{
		*buff++ = 10;
	}
	else if (this->lineBreak == 3)
	{
		*buff++ = 13;
		*buff++ = 10;
	}
	*buff = 0;
	return buff;
}

Bool IO::StreamReader::GetLastLineBreak(Text::StringBuilderUTF *sb)
{
	if (this->lineBreak == 1)
	{
		sb->AppendChar('\r', 1);
	}
	else if (this->lineBreak == 2)
	{
		sb->AppendChar('\n', 1);
	}
	else if (this->lineBreak == 2)
	{
		sb->AppendC((const UTF8Char*)"\r\n", 2);
	}
	return true;
}

WChar *IO::StreamReader::GetLastLineBreak(WChar *buff)
{
	if (this->lineBreak == 1)
	{
		*buff++ = 13;
	}
	else if (this->lineBreak == 2)
	{
		*buff++ = 10;
	}
	else if (this->lineBreak == 3)
	{
		*buff++ = 13;
		*buff++ = 10;
	}
	*buff = 0;
	return buff;
}

WChar *IO::StreamReader::ReadToEnd(WChar *buff)
{
	WChar *src;
	WChar *dest;
	FillBuffer();
	dest = buff;
	while (wcSize > wcPos)
	{
		src = &wcbuff[wcPos];
		while (wcPos < wcSize)
		{
			*dest++ = *src++;
			wcPos++;
		}
		FillBuffer();
	}
	*dest = 0;
	return dest;
}

Bool IO::StreamReader::ReadToEnd(Text::StringBuilderUTF *sb)
{
	Bool succ = false;
	WChar *wptr = MemAlloc(WChar, 4096);
	WChar *sptr;
	const UTF8Char *csptr;
	while (true)
	{
		sptr = ReadLine(wptr, 4093);
		if (sptr == 0)
		{
			break;
		}
		succ = true;
		sptr = this->GetLastLineBreak(sptr);
		csptr = Text::StrToUTF8New(wptr);
		sb->Append(csptr);
		Text::StrDelNew(csptr);
	}
	MemFree(wptr);
	return succ;
}
