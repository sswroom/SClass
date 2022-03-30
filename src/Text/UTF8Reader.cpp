#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayList.h"
#include "IO/SeekableStream.h"
#include "Sync/Event.h"
#include "Text/UTF8Reader.h"

//#define VERBOSE
#ifdef VERBOSE
#include <stdio.h>
#endif
#define BUFFSIZE 16384

void Text::UTF8Reader::FillBuffer()
{
	UOSInt readSize;
	if (this->stm->CanSeek())
	{
		UInt64 currPos = ((IO::SeekableStream*)this->stm)->GetPosition();
		if (this->lastPos != currPos)
		{
			this->buffSize = 0;
			this->currOfst = 0;
		}
	}

#ifdef VERBOSE
	printf("UTF8Reader.FB ofst = %d, size = %d\r\n", (UInt32)this->currOfst, (UInt32)this->buffSize);
#endif
	if (this->currOfst == this->buffSize)
	{
		this->buffSize = 0;
		this->currOfst = 0;
	}
	else if (this->currOfst > 0)
	{
		MemCopyO(this->buff, &this->buff[this->currOfst], this->buffSize - this->currOfst);
		this->buffSize -= this->currOfst;
		this->currOfst = 0;
	}
	readSize = this->stm->Read(&this->buff[this->buffSize], BUFFSIZE - this->buffSize);;
#ifdef VERBOSE
	printf("UTF8Reader.FB read %d bytes, ofst = %d, size = %d\r\n", (UInt32)readSize, (UInt32)this->currOfst, (UInt32)this->buffSize);
#endif
	this->buffSize += readSize;
	if (stm->CanSeek())
	{
		this->lastPos = ((IO::SeekableStream*)stm)->GetPosition();
	}
}

void Text::UTF8Reader::CheckHeader()
{
	if (this->buffSize != 0)
		return;
	UOSInt readSize;
	readSize = this->stm->Read(&this->buff[this->buffSize], 4 - this->buffSize);
#ifdef VERBOSE
	printf("UTF8Reader.CH read %d bytes\r\n", (UInt32)readSize);
#endif
	this->buffSize += readSize;
	if (this->buffSize >= 3 && this->buff[0] == 0xef && this->buff[1] == 0xbb && this->buff[2] == 0xbf)
	{
		this->buff[0] = this->buff[3];
		this->currOfst = 0;
		this->buffSize -= 3;
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

Text::UTF8Reader::UTF8Reader(IO::Stream *stm)
{
	this->stm = stm;
	this->buff = MemAlloc(UInt8, BUFFSIZE);
	this->buffSize = 0;
	this->currOfst = 0;
	if (stm->CanSeek())
	{
		this->lastPos = ((IO::SeekableStream*)stm)->GetPosition();
	}
	else
	{
		this->lastPos = 0;
	}
	this->lineBreak = 0;
	this->CheckHeader();
	this->FillBuffer();
}

Text::UTF8Reader::~UTF8Reader()
{
	MemFree(this->buff);
}

void Text::UTF8Reader::Close()
{
	this->stm->Close();
}

UTF32Char Text::UTF8Reader::Peek()
{
	UInt32 ret;
	if (this->currOfst < this->buffSize)
	{
		UTF8Char c = this->buff[this->currOfst];
		if ((c & 0x80) == 0)
		{
			return c;
		}
		else if ((c & 0xe0) == 0xc0)
		{
			if (this->buffSize - this->currOfst >= 2)
			{
				ret = ((UInt32)(c & 0x1f) << 6) | (UInt32)(this->buff[this->currOfst + 1] & 0x3f);
				return (UTF32Char)ret;
			}
		}
		else if ((c & 0xf0) == 0xe0)
		{
			if (this->buffSize - this->currOfst >= 3)
			{
				ret = ((UInt32)(c & 0x0f) << 12) | ((UInt32)(this->buff[this->currOfst + 1] & 0x3f) << 6) | (UInt32)(this->buff[this->currOfst + 2] & 0x3f);
				return (UTF32Char)ret;
			}
		}
		else if ((c & 0xf8) == 0xf0)
		{
			if (this->buffSize - this->currOfst >= 4)
			{
				ret = ((UInt32)(c & 0x7) << 18) | ((UInt32)(this->buff[this->currOfst + 1] & 0x3f) << 12) | ((UInt32)(this->buff[this->currOfst + 2] & 0x3f) << 6) | (UInt32)(this->buff[this->currOfst + 3] & 0x3f);
				return (UTF32Char)ret;
			}
		}
		else if ((c & 0xfc) == 0xf8)
		{
			if (this->buffSize - this->currOfst >= 5)
			{
				ret = ((UInt32)(c & 0x3) << 24) | ((UInt32)(this->buff[this->currOfst + 1] & 0x3f) << 18) | ((UInt32)(this->buff[this->currOfst + 2] & 0x3f) << 12) | ((UInt32)(this->buff[this->currOfst + 3] & 0x3f) << 6) | (UInt32)(this->buff[this->currOfst + 4] & 0x3f);
				return (UTF32Char)ret;
			}
		}
		else
		{
			if (this->buffSize - this->currOfst >= 6)
			{
				ret = ((UInt32)(c & 0x1) << 30) | ((UInt32)(this->buff[this->currOfst + 1] & 0x3f) << 24) | ((UInt32)(this->buff[this->currOfst + 2] & 0x3f) << 18) | ((UInt32)(this->buff[this->currOfst + 3] & 0x3f) << 12) | ((UInt32)(this->buff[this->currOfst + 4] & 0x3f) << 6) | (UInt32)(this->buff[this->currOfst + 5] & 0x3f);
				return (UTF32Char)ret;
			}
		}
	}
	this->FillBuffer();
	if (this->currOfst < this->buffSize)
	{
		UTF8Char c = this->buff[this->currOfst];
		if ((c & 0x80) == 0)
		{
			return c;
		}
		else if ((c & 0xe0) == 0xc0)
		{
			if (this->buffSize - this->currOfst >= 2)
			{
				ret = ((UInt32)(c & 0x1f) << 6) | (UInt32)(this->buff[this->currOfst + 1] & 0x3f);
				return (UTF32Char)ret;
			}
		}
		else if ((c & 0xf0) == 0xe0)
		{
			if (this->buffSize - this->currOfst >= 3)
			{
				ret = ((UInt32)(c & 0x0f) << 12) | ((UInt32)(this->buff[this->currOfst + 1] & 0x3f) << 6) | (UInt32)(this->buff[this->currOfst + 2] & 0x3f);
				return (UTF32Char)ret;
			}
		}
		else if ((c & 0xf8) == 0xf0)
		{
			if (this->buffSize - this->currOfst >= 4)
			{
				ret = ((UInt32)(c & 0x7) << 18) | ((UInt32)(this->buff[this->currOfst + 1] & 0x3f) << 12) | ((UInt32)(this->buff[this->currOfst + 2] & 0x3f) << 6) | (UInt32)(this->buff[this->currOfst + 3] & 0x3f);
				return (UTF32Char)ret;
			}
		}
		else if ((c & 0xfc) == 0xf8)
		{
			if (this->buffSize - this->currOfst >= 5)
			{
				ret = ((UInt32)(c & 0x3) << 24) | ((UInt32)(this->buff[this->currOfst + 1] & 0x3f) << 18) | ((UInt32)(this->buff[this->currOfst + 2] & 0x3f) << 12) | ((UInt32)(this->buff[this->currOfst + 3] & 0x3f) << 6) | (UInt32)(this->buff[this->currOfst + 4] & 0x3f);
				return (UTF32Char)ret;
			}
		}
		else
		{
			if (this->buffSize - this->currOfst >= 6)
			{
				ret = ((UInt32)(c & 0x1) << 30) | ((UInt32)(this->buff[this->currOfst + 1] & 0x3f) << 24) | ((UInt32)(this->buff[this->currOfst + 2] & 0x3f) << 18) | ((UInt32)(this->buff[this->currOfst + 3] & 0x3f) << 12) | ((UInt32)(this->buff[this->currOfst + 4] & 0x3f) << 6) | (UInt32)(this->buff[this->currOfst + 5] & 0x3f);
				return (UTF32Char)ret;
			}
		}
	}
	return 0;
}

UTF32Char Text::UTF8Reader::Read()
{
	UInt32 ret;
	if (this->currOfst < this->buffSize)
	{
		UTF8Char c = this->buff[this->currOfst];
		if ((c & 0x80) == 0)
		{
			this->currOfst += 1;
			return c;
		}
		else if ((c & 0xe0) == 0xc0)
		{
			if (this->buffSize - this->currOfst >= 2)
			{
				ret = ((UInt32)(c & 0x1f) << 6) | (UInt32)(this->buff[this->currOfst + 1] & 0x3f);
				this->currOfst += 2;
				return (UTF32Char)ret;
			}
		}
		else if ((c & 0xf0) == 0xe0)
		{
			if (this->buffSize - this->currOfst >= 3)
			{
				ret = ((UInt32)(c & 0x0f) << 12) | ((UInt32)(this->buff[this->currOfst + 1] & 0x3f) << 6) | (UInt32)(this->buff[this->currOfst + 2] & 0x3f);
				this->currOfst += 3;
				return (UTF32Char)ret;
			}
		}
		else if ((c & 0xf8) == 0xf0)
		{
			if (this->buffSize - this->currOfst >= 4)
			{
				ret = ((UInt32)(c & 0x7) << 18) | ((UInt32)(this->buff[this->currOfst + 1] & 0x3f) << 12) | ((UInt32)(this->buff[this->currOfst + 2] & 0x3f) << 6) | (UInt32)(this->buff[this->currOfst + 3] & 0x3f);
				this->currOfst += 4;
				return (UTF32Char)ret;
			}
		}
		else if ((c & 0xfc) == 0xf8)
		{
			if (this->buffSize - this->currOfst >= 5)
			{
				ret = ((UInt32)(c & 0x3) << 24) | ((UInt32)(this->buff[this->currOfst + 1] & 0x3f) << 18) | ((UInt32)(this->buff[this->currOfst + 2] & 0x3f) << 12) | ((UInt32)(this->buff[this->currOfst + 3] & 0x3f) << 6) | (UInt32)(this->buff[this->currOfst + 4] & 0x3f);
				this->currOfst += 5;
				return (UTF32Char)ret;
			}
		}
		else
		{
			if (this->buffSize - this->currOfst >= 6)
			{
				ret = ((UInt32)(c & 0x1) << 30) | ((UInt32)(this->buff[this->currOfst + 1] & 0x3f) << 24) | ((UInt32)(this->buff[this->currOfst + 2] & 0x3f) << 18) | ((UInt32)(this->buff[this->currOfst + 3] & 0x3f) << 12) | ((UInt32)(this->buff[this->currOfst + 4] & 0x3f) << 6) | (UInt32)(this->buff[this->currOfst + 5] & 0x3f);
				this->currOfst += 6;
				return (UTF32Char)ret;
			}
		}
	}
	this->FillBuffer();
	if (this->currOfst < this->buffSize)
	{
		UTF8Char c = this->buff[this->currOfst];
		if ((c & 0x80) == 0)
		{
			this->currOfst += 1;
			return c;
		}
		else if ((c & 0xe0) == 0xc0)
		{
			if (this->buffSize - this->currOfst >= 2)
			{
				ret = ((UInt32)(c & 0x1f) << 6) | (UInt32)(this->buff[this->currOfst + 1] & 0x3f);
				this->currOfst += 2;
				return (UTF32Char)ret;
			}
		}
		else if ((c & 0xf0) == 0xe0)
		{
			if (this->buffSize - this->currOfst >= 3)
			{
				ret = ((UInt32)(c & 0x0f) << 12) | ((UInt32)(this->buff[this->currOfst + 1] & 0x3f) << 6) | (UInt32)(this->buff[this->currOfst + 2] & 0x3f);
				this->currOfst += 3;
				return (UTF32Char)ret;
			}
		}
		else if ((c & 0xf8) == 0xf0)
		{
			if (this->buffSize - this->currOfst >= 4)
			{
				ret = ((UInt32)(c & 0x7) << 18) | ((UInt32)(this->buff[this->currOfst + 1] & 0x3f) << 12) | ((UInt32)(this->buff[this->currOfst + 2] & 0x3f) << 6) | (UInt32)(this->buff[this->currOfst + 3] & 0x3f);
				this->currOfst += 4;
				return (UTF32Char)ret;
			}
		}
		else if ((c & 0xfc) == 0xf8)
		{
			if (this->buffSize - this->currOfst >= 5)
			{
				ret = ((UInt32)(c & 0x3) << 24) | ((UInt32)(this->buff[this->currOfst + 1] & 0x3f) << 18) | ((UInt32)(this->buff[this->currOfst + 2] & 0x3f) << 12) | ((UInt32)(this->buff[this->currOfst + 3] & 0x3f) << 6) | (UInt32)(this->buff[this->currOfst + 4] & 0x3f);
				this->currOfst += 5;
				return (UTF32Char)ret;
			}
		}
		else
		{
			if (this->buffSize - this->currOfst >= 6)
			{
				ret = ((UInt32)(c & 0x1) << 30) | ((UInt32)(this->buff[this->currOfst + 1] & 0x3f) << 24) | ((UInt32)(this->buff[this->currOfst + 2] & 0x3f) << 18) | ((UInt32)(this->buff[this->currOfst + 3] & 0x3f) << 12) | ((UInt32)(this->buff[this->currOfst + 4] & 0x3f) << 6) | (UInt32)(this->buff[this->currOfst + 5] & 0x3f);
				this->currOfst += 6;
				return (UTF32Char)ret;
			}
		}
	}
	return 0;
}

Bool Text::UTF8Reader::ReadLine(Text::StringBuilderUTF8 *sb, UOSInt maxCharCnt)
{
	if (this->currOfst >= this->buffSize)
	{
		this->FillBuffer();
		if (this->currOfst >= this->buffSize)
			return false;
	}

	this->lineBreak = 0;
	UOSInt currSize = 0;
	UOSInt writeSize = 0;
	UOSInt charSize;
	UOSInt currOfst = this->currOfst;
	UOSInt buffSize = this->buffSize;
	UOSInt nextCheckSize = buffSize - currOfst;
	if (maxCharCnt < nextCheckSize)
	{
		nextCheckSize = maxCharCnt;
	}
	while (true)
	{
		if (currSize >= nextCheckSize)
		{
			if (currSize >= maxCharCnt)
			{
				sb->AppendC((const UTF8Char*)&this->buff[currOfst], currSize);
				this->currOfst = currOfst + currSize;
				return true;
			}
			else //if (this->currOfst + currSize >= this->buffSize)
			{
				if (currSize > 0)
				{
					sb->AppendC((const UTF8Char*)&this->buff[currOfst], currSize);
					this->currOfst = currOfst + currSize;
					maxCharCnt -= currSize;
					writeSize += currSize;
					currSize = 0;
				}
				else
				{
					this->currOfst = currOfst;
				}
				this->FillBuffer();
				if (this->currOfst >= this->buffSize)
					return true;
				currOfst = this->currOfst;
				buffSize = this->buffSize;
				nextCheckSize = buffSize - currOfst;
				if (maxCharCnt < nextCheckSize)
				{
					nextCheckSize = maxCharCnt;
				}
			}
		}

		UTF8Char c = this->buff[currOfst + currSize];
		if ((c & 0x80) == 0)
		{
			switch (c)
			{
			case 10:
				sb->AppendC((const UTF8Char*)&this->buff[currOfst], currSize);
				this->currOfst = currOfst + currSize + 1;
				this->lineBreak = 2;
				return true;
			case 13:
				sb->AppendC((const UTF8Char*)&this->buff[currOfst], currSize);
				this->currOfst = currOfst + currSize + 1;
				if (this->currOfst < this->buffSize && this->buff[this->currOfst] == 10)
				{
					this->lineBreak = 3;
					this->currOfst += 1;
				}
				else
				{
					this->lineBreak = 1;
				}
				return true;
			default:
				currSize += 1;
				break;
			}
		}
		else
		{
			if ((c & 0xe0) == 0xc0)
			{	
				charSize = 2;
			}
			else if ((c & 0xf0) == 0xe0)
			{
				charSize = 3;
			}
			else if ((c & 0xf8) == 0xf0)
			{
				charSize = 4;
			}
			else if ((c & 0xfc) == 0xf8)
			{
				charSize = 5;
			}
			else
			{
				charSize = 6;
			}

			if (maxCharCnt < currSize + charSize)
			{
				sb->AppendC((const UTF8Char*)&this->buff[currOfst], currSize);
				this->currOfst = currOfst + currSize;
				return true;
			}
			else if (buffSize - currOfst < currSize + charSize)
			{
				if (currSize > 0)
				{
					sb->AppendC((const UTF8Char*)&this->buff[currOfst], currSize);
					this->currOfst = currOfst + currSize;
					maxCharCnt -= currSize;
					writeSize += currSize;
					currSize = 0;
				}
				else
				{
					this->currOfst = currOfst;
				}
				this->FillBuffer();
				if (this->buffSize - this->currOfst < currSize + charSize)
				{
					return writeSize > 0;
				}
				currOfst = this->currOfst;
				buffSize = this->buffSize;
			}
			nextCheckSize = buffSize - currOfst;
			if (maxCharCnt < nextCheckSize)
			{
				nextCheckSize = maxCharCnt;
			}
			currSize += charSize;
		}
	}
	return true;
}

UTF8Char *Text::UTF8Reader::ReadLine(UTF8Char *sbuff, UOSInt maxCharCnt)
{
#if defined(VERBOSE)
	printf("UTF8Reader.RL: ofst = %d, size = %d\r\n", (UInt32)this->currOfst, (UInt32)this->buffSize);
#endif
	if (this->currOfst >= this->buffSize)
	{
		this->FillBuffer();
		if (this->currOfst >= this->buffSize)
			return 0;
	}

	this->lineBreak = 0;
	UOSInt currSize = 0;
	UOSInt writeSize = 0;
	UOSInt charSize;
	UOSInt currOfst = this->currOfst;
	UOSInt buffSize = this->buffSize;
	UOSInt nextCheckSize = buffSize - currOfst;
	if (maxCharCnt < nextCheckSize)
	{
		nextCheckSize = maxCharCnt;
	}
	while (true)
	{
		if (currSize >= nextCheckSize)
		{
			if (currSize >= maxCharCnt)
			{
				sbuff = Text::StrConcatC(sbuff, (const UTF8Char*)&this->buff[currOfst], currSize);
				this->currOfst = currOfst + currSize;
				return sbuff;
			}
			else //if (this->currOfst + currSize >= this->buffSize)
			{
				if (currSize > 0)
				{
					sbuff = Text::StrConcatC(sbuff, (const UTF8Char*)&this->buff[currOfst], currSize);
					this->currOfst = currOfst + currSize;
					maxCharCnt -= currSize;
					writeSize += currSize;
					currSize = 0;
				}
				else
				{
					this->currOfst = currOfst;
				}
				this->FillBuffer();
				if (this->currOfst >= this->buffSize)
					return sbuff;
				currOfst = this->currOfst;
				buffSize = this->buffSize;
			}
			nextCheckSize = buffSize - currOfst;
			if (maxCharCnt < nextCheckSize)
			{
				nextCheckSize = maxCharCnt;
			}
		}

		UTF8Char c = this->buff[currOfst + currSize];
		if ((c & 0x80) == 0)
		{
			if (c == 10)
			{
				sbuff = Text::StrConcatC(sbuff, (const UTF8Char*)&this->buff[currOfst], currSize);
				this->currOfst = currOfst + currSize + 1;
				this->lineBreak = 2;
				return sbuff;
			}
			else if (c == 13)
			{
				sbuff = Text::StrConcatC(sbuff, (const UTF8Char*)&this->buff[currOfst], currSize);
				this->currOfst = currOfst + currSize + 1;
				if (this->currOfst < this->buffSize && this->buff[this->currOfst] == 10)
				{
					this->lineBreak = 3;
					this->currOfst += 1;
				}
				else
				{
					this->lineBreak = 1;
				}
				return sbuff;
			}
			currSize += 1;
		}
		else
		{
			if ((c & 0xe0) == 0xc0)
			{	
				charSize = 2;
			}
			else if ((c & 0xf0) == 0xe0)
			{
				charSize = 3;
			}
			else if ((c & 0xf8) == 0xf0)
			{
				charSize = 4;
			}
			else if ((c & 0xfc) == 0xf8)
			{
				charSize = 5;
			}
			else
			{
				charSize = 6;
			}

			if (maxCharCnt - currSize < charSize)
			{
				sbuff = Text::StrConcatC(sbuff, (const UTF8Char*)&this->buff[currOfst], currSize);
				this->currOfst = currOfst + currSize;
				return sbuff;
			}
			else if (buffSize - currOfst < currSize + charSize)
			{
				if (currSize > 0)
				{
					sbuff = Text::StrConcatC(sbuff, (const UTF8Char*)&this->buff[currOfst], currSize);
					this->currOfst = currOfst + currSize;
					maxCharCnt -= currSize;
					writeSize += currSize;
					currSize = 0;
				}
				else
				{
					this->currOfst = currOfst;
				}
				this->FillBuffer();
				if (this->buffSize - this->currOfst < currSize + charSize)
				{
					if (writeSize <= 0)
						return 0;
					return sbuff;
				}
				buffSize = this->buffSize;
				currOfst = this->currOfst;
			}
			nextCheckSize = buffSize - currOfst;
			if (maxCharCnt < nextCheckSize)
			{
				nextCheckSize = maxCharCnt;
			}
			currSize += charSize;
		}
	}
	return sbuff;
}

UTF8Char *Text::UTF8Reader::GetLastLineBreak(UTF8Char *buff)
{
	switch (this->lineBreak)
	{
	case 1:
		*buff++ = 13;
		break;
	case 2:
		*buff++ = 10;
		break;
	case 3:
		buff[0] = 13;
		buff[1] = 10;
		buff += 2;
		break;
	}
	*buff = 0;
	return buff;
}

Bool Text::UTF8Reader::GetLastLineBreak(Text::StringBuilderUTF8 *sb)
{
	switch (this->lineBreak)
	{
	case 1:
		sb->AppendUTF8Char('\r');
		return true;
	case 2:
		sb->AppendUTF8Char('\n');
		return true;
	case 3:
		sb->AppendC(UTF8STRC("\r\n"));
		return true;
	}
	return true;
}

Bool Text::UTF8Reader::ReadToEnd(Text::StringBuilderUTF8 *sb)
{
	Bool succ = false;
	while (this->ReadLine(sb, BUFFSIZE))
	{
		succ = this->GetLastLineBreak(sb);
	}
	return succ;
}
