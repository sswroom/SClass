#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayList.h"
#include "IO/SeekableStream.h"
#include "Sync/Event.h"
#include "Text/UTF8Reader.h"

#define BUFFSIZE 16384

void Text::UTF8Reader::FillBuffer()
{
	if (this->stm->CanSeek())
	{
		Int64 currPos = ((IO::SeekableStream*)this->stm)->GetPosition();
		if (this->lastPos != currPos)
		{
			this->buffSize = 0;
			this->currOfst = 0;
		}
	}

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
	this->buffSize += this->stm->Read(&this->buff[this->buffSize], BUFFSIZE - this->buffSize);
	if (stm->CanSeek())
	{
		this->lastPos = ((IO::SeekableStream*)stm)->GetPosition();
	}
}

void Text::UTF8Reader::CheckHeader()
{
	if (this->buffSize != 0)
		return;
	this->buffSize += this->stm->Read(&this->buff[this->buffSize], 4 - this->buffSize);
	if (this->buff[0] == 0xef && this->buff[1] == 0xbb && this->buff[2] == 0xbf)
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
	UTF32Char ret;
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
				return ret;
			}
		}
		else if ((c & 0xf0) == 0xe0)
		{
			if (this->buffSize - this->currOfst >= 3)
			{
				ret = ((UInt32)(c & 0x0f) << 12) | ((UInt32)(this->buff[this->currOfst + 1] & 0x3f) << 6) | (UInt32)(this->buff[this->currOfst + 2] & 0x3f);
				return ret;
			}
		}
		else if ((c & 0xf8) == 0xf0)
		{
			if (this->buffSize - this->currOfst >= 4)
			{
				ret = ((UInt32)(c & 0x7) << 18) | ((UInt32)(this->buff[this->currOfst + 1] & 0x3f) << 12) | ((UInt32)(this->buff[this->currOfst + 2] & 0x3f) << 6) | (UInt32)(this->buff[this->currOfst + 3] & 0x3f);
				return ret;
			}
		}
		else if ((c & 0xfc) == 0xf8)
		{
			if (this->buffSize - this->currOfst >= 5)
			{
				ret = ((UInt32)(c & 0x3) << 24) | ((UInt32)(this->buff[this->currOfst + 1] & 0x3f) << 18) | ((UInt32)(this->buff[this->currOfst + 2] & 0x3f) << 12) | ((UInt32)(this->buff[this->currOfst + 3] & 0x3f) << 6) | (UInt32)(this->buff[this->currOfst + 4] & 0x3f);
				return ret;
			}
		}
		else
		{
			if (this->buffSize - this->currOfst >= 6)
			{
				ret = ((UInt32)(c & 0x1) << 30) | ((UInt32)(this->buff[this->currOfst + 1] & 0x3f) << 24) | ((UInt32)(this->buff[this->currOfst + 2] & 0x3f) << 18) | ((UInt32)(this->buff[this->currOfst + 3] & 0x3f) << 12) | ((UInt32)(this->buff[this->currOfst + 4] & 0x3f) << 6) | (UInt32)(this->buff[this->currOfst + 5] & 0x3f);
				return ret;
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
				return ret;
			}
		}
		else if ((c & 0xf0) == 0xe0)
		{
			if (this->buffSize - this->currOfst >= 3)
			{
				ret = ((UInt32)(c & 0x0f) << 12) | ((UInt32)(this->buff[this->currOfst + 1] & 0x3f) << 6) | (UInt32)(this->buff[this->currOfst + 2] & 0x3f);
				return ret;
			}
		}
		else if ((c & 0xf8) == 0xf0)
		{
			if (this->buffSize - this->currOfst >= 4)
			{
				ret = ((UInt32)(c & 0x7) << 18) | ((UInt32)(this->buff[this->currOfst + 1] & 0x3f) << 12) | ((UInt32)(this->buff[this->currOfst + 2] & 0x3f) << 6) | (UInt32)(this->buff[this->currOfst + 3] & 0x3f);
				return ret;
			}
		}
		else if ((c & 0xfc) == 0xf8)
		{
			if (this->buffSize - this->currOfst >= 5)
			{
				ret = ((UInt32)(c & 0x3) << 24) | ((UInt32)(this->buff[this->currOfst + 1] & 0x3f) << 18) | ((UInt32)(this->buff[this->currOfst + 2] & 0x3f) << 12) | ((UInt32)(this->buff[this->currOfst + 3] & 0x3f) << 6) | (UInt32)(this->buff[this->currOfst + 4] & 0x3f);
				return ret;
			}
		}
		else
		{
			if (this->buffSize - this->currOfst >= 6)
			{
				ret = ((UInt32)(c & 0x1) << 30) | ((UInt32)(this->buff[this->currOfst + 1] & 0x3f) << 24) | ((UInt32)(this->buff[this->currOfst + 2] & 0x3f) << 18) | ((UInt32)(this->buff[this->currOfst + 3] & 0x3f) << 12) | ((UInt32)(this->buff[this->currOfst + 4] & 0x3f) << 6) | (UInt32)(this->buff[this->currOfst + 5] & 0x3f);
				return ret;
			}
		}
	}
	return 0;
}

UTF32Char Text::UTF8Reader::Read()
{
	UTF32Char ret;
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
				return ret;
			}
		}
		else if ((c & 0xf0) == 0xe0)
		{
			if (this->buffSize - this->currOfst >= 3)
			{
				ret = ((UInt32)(c & 0x0f) << 12) | ((UInt32)(this->buff[this->currOfst + 1] & 0x3f) << 6) | (UInt32)(this->buff[this->currOfst + 2] & 0x3f);
				this->currOfst += 3;
				return ret;
			}
		}
		else if ((c & 0xf8) == 0xf0)
		{
			if (this->buffSize - this->currOfst >= 4)
			{
				ret = ((UInt32)(c & 0x7) << 18) | ((UInt32)(this->buff[this->currOfst + 1] & 0x3f) << 12) | ((UInt32)(this->buff[this->currOfst + 2] & 0x3f) << 6) | (UInt32)(this->buff[this->currOfst + 3] & 0x3f);
				this->currOfst += 4;
				return ret;
			}
		}
		else if ((c & 0xfc) == 0xf8)
		{
			if (this->buffSize - this->currOfst >= 5)
			{
				ret = ((UInt32)(c & 0x3) << 24) | ((UInt32)(this->buff[this->currOfst + 1] & 0x3f) << 18) | ((UInt32)(this->buff[this->currOfst + 2] & 0x3f) << 12) | ((UInt32)(this->buff[this->currOfst + 3] & 0x3f) << 6) | (UInt32)(this->buff[this->currOfst + 4] & 0x3f);
				this->currOfst += 5;
				return ret;
			}
		}
		else
		{
			if (this->buffSize - this->currOfst >= 6)
			{
				ret = ((UInt32)(c & 0x1) << 30) | ((UInt32)(this->buff[this->currOfst + 1] & 0x3f) << 24) | ((UInt32)(this->buff[this->currOfst + 2] & 0x3f) << 18) | ((UInt32)(this->buff[this->currOfst + 3] & 0x3f) << 12) | ((UInt32)(this->buff[this->currOfst + 4] & 0x3f) << 6) | (UInt32)(this->buff[this->currOfst + 5] & 0x3f);
				this->currOfst += 6;
				return ret;
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
				return ret;
			}
		}
		else if ((c & 0xf0) == 0xe0)
		{
			if (this->buffSize - this->currOfst >= 3)
			{
				ret = ((UInt32)(c & 0x0f) << 12) | ((UInt32)(this->buff[this->currOfst + 1] & 0x3f) << 6) | (UInt32)(this->buff[this->currOfst + 2] & 0x3f);
				this->currOfst += 3;
				return ret;
			}
		}
		else if ((c & 0xf8) == 0xf0)
		{
			if (this->buffSize - this->currOfst >= 4)
			{
				ret = ((UInt32)(c & 0x7) << 18) | ((UInt32)(this->buff[this->currOfst + 1] & 0x3f) << 12) | ((UInt32)(this->buff[this->currOfst + 2] & 0x3f) << 6) | (UInt32)(this->buff[this->currOfst + 3] & 0x3f);
				this->currOfst += 4;
				return ret;
			}
		}
		else if ((c & 0xfc) == 0xf8)
		{
			if (this->buffSize - this->currOfst >= 5)
			{
				ret = ((UInt32)(c & 0x3) << 24) | ((UInt32)(this->buff[this->currOfst + 1] & 0x3f) << 18) | ((UInt32)(this->buff[this->currOfst + 2] & 0x3f) << 12) | ((UInt32)(this->buff[this->currOfst + 3] & 0x3f) << 6) | (UInt32)(this->buff[this->currOfst + 4] & 0x3f);
				this->currOfst += 5;
				return ret;
			}
		}
		else
		{
			if (this->buffSize - this->currOfst >= 6)
			{
				ret = ((UInt32)(c & 0x1) << 30) | ((UInt32)(this->buff[this->currOfst + 1] & 0x3f) << 24) | ((UInt32)(this->buff[this->currOfst + 2] & 0x3f) << 18) | ((UInt32)(this->buff[this->currOfst + 3] & 0x3f) << 12) | ((UInt32)(this->buff[this->currOfst + 4] & 0x3f) << 6) | (UInt32)(this->buff[this->currOfst + 5] & 0x3f);
				this->currOfst += 6;
				return ret;
			}
		}
	}
	return 0;
}

Bool Text::UTF8Reader::ReadLine(Text::StringBuilderUTF *sb, OSInt maxCharCnt)
{
	if (this->currOfst >= this->buffSize)
	{
		this->FillBuffer();
		if (this->currOfst >= this->buffSize)
			return false;
	}

	this->lineBreak = 0;
	OSInt currSize = 0;
	OSInt writeSize = 0;
	OSInt charSize;
	while (true)
	{
		if (currSize >= maxCharCnt)
		{
			sb->AppendC((const UTF8Char*)&this->buff[this->currOfst], currSize);
			this->currOfst += currSize;
			return true;
		}
		if (this->currOfst + currSize >= this->buffSize)
		{
			if (currSize > 0)
			{
				sb->AppendC((const UTF8Char*)&this->buff[this->currOfst], currSize);
				this->currOfst += currSize;
				maxCharCnt -= currSize;
				writeSize += currSize;
				currSize = 0;
			}
			this->FillBuffer();
			if (this->currOfst >= this->buffSize)
				return true;
		}

		UTF8Char c = this->buff[this->currOfst + currSize];
		if ((c & 0x80) == 0)
		{
			if (c == 10)
			{
				sb->AppendC((const UTF8Char*)&this->buff[this->currOfst], currSize);
				this->currOfst += currSize + 1;
				this->lineBreak = 2;
				return true;
			}
			else if (c == 13)
			{
				sb->AppendC((const UTF8Char*)&this->buff[this->currOfst], currSize);
				this->currOfst += currSize + 1;
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
				sb->AppendC((const UTF8Char*)&this->buff[this->currOfst], currSize);
				this->currOfst += currSize;
				return true;
			}
			else if (this->buffSize - this->currOfst < currSize + charSize)
			{
				if (currSize > 0)
				{
					sb->AppendC((const UTF8Char*)&this->buff[this->currOfst], currSize);
					this->currOfst += currSize;
					maxCharCnt -= currSize;
					writeSize += currSize;
					currSize = 0;
				}
				this->FillBuffer();
				if (this->buffSize - this->currOfst < currSize + charSize)
				{
					return writeSize > 0;
				}
			}
			currSize += charSize;
		}
	}
	return true;
}

UTF8Char *Text::UTF8Reader::ReadLine(UTF8Char *u8buff, OSInt maxCharCnt)
{
	if (this->currOfst >= this->buffSize)
	{
		this->FillBuffer();
		if (this->currOfst >= this->buffSize)
			return 0;
	}

	this->lineBreak = 0;
	OSInt currSize = 0;
	OSInt writeSize = 0;
	OSInt charSize;
	while (true)
	{
		if (currSize >= maxCharCnt)
		{
			u8buff = Text::StrConcatC(u8buff, (const UTF8Char*)&this->buff[this->currOfst], currSize);
			this->currOfst += currSize;
			return u8buff;
		}
		if (this->currOfst + currSize >= this->buffSize)
		{
			if (currSize > 0)
			{
				u8buff = Text::StrConcatC(u8buff, (const UTF8Char*)&this->buff[this->currOfst], currSize);
				this->currOfst += currSize;
				maxCharCnt -= currSize;
				writeSize += currSize;
				currSize = 0;
			}
			this->FillBuffer();
			if (this->currOfst >= this->buffSize)
				return u8buff;
		}

		UTF8Char c = this->buff[this->currOfst + currSize];
		if ((c & 0x80) == 0)
		{
			if (c == 10)
			{
				u8buff = Text::StrConcatC(u8buff, (const UTF8Char*)&this->buff[this->currOfst], currSize);
				this->currOfst += currSize + 1;
				this->lineBreak = 2;
				return u8buff;
			}
			else if (c == 13)
			{
				u8buff = Text::StrConcatC(u8buff, (const UTF8Char*)&this->buff[this->currOfst], currSize);
				this->currOfst += currSize + 1;
				if (this->currOfst < this->buffSize && this->buff[this->currOfst] == 10)
				{
					this->lineBreak = 3;
					this->currOfst += 1;
				}
				else
				{
					this->lineBreak = 1;
				}
				return u8buff;
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
				u8buff = Text::StrConcatC(u8buff, (const UTF8Char*)&this->buff[this->currOfst], currSize);
				this->currOfst += currSize;
				return u8buff;
			}
			else if (this->buffSize - this->currOfst < currSize + charSize)
			{
				if (currSize > 0)
				{
					u8buff = Text::StrConcatC(u8buff, (const UTF8Char*)&this->buff[this->currOfst], currSize);
					this->currOfst += currSize;
					maxCharCnt -= currSize;
					writeSize += currSize;
					currSize = 0;
				}
				this->FillBuffer();
				if (this->buffSize - this->currOfst < currSize + charSize)
				{
					if (writeSize <= 0)
						return 0;
					return u8buff;
				}
			}
			currSize += charSize;
		}
	}
	return u8buff;
}

WChar *Text::UTF8Reader::GetLastLineBreak(WChar *buff)
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

Bool Text::UTF8Reader::GetLastLineBreak(Text::StringBuilderUTF *sb)
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

Bool Text::UTF8Reader::ReadToEnd(Text::StringBuilderUTF *sb)
{
	Bool succ = false;
	while (this->ReadLine(sb, BUFFSIZE))
	{
		succ = this->GetLastLineBreak(sb);
	}
	return succ;
}
