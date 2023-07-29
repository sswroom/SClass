#include "Stdafx.h"
#include "Data/CircularByteBuff.h"

Data::CircularByteBuff::CircularByteBuff(UOSInt maxSize)
{
	this->buffSize = maxSize + 1;
	this->buff = MemAlloc(UInt8, this->buffSize);
	this->indexBegin = 0;
	this->indexEnd = 0;
}

Data::CircularByteBuff::~CircularByteBuff()
{
	MemFree(this->buff);
}

void Data::CircularByteBuff::Clear()
{
	this->indexBegin = 0;
	this->indexEnd = 0;
}

void Data::CircularByteBuff::AppendBytes(const UInt8 *buff, UOSInt buffSize)
{
	if (buffSize >= this->buffSize - 1)
	{
		MemCopyNO(this->buff, &buff[buffSize - this->buffSize + 1], this->buffSize - 1);
		this->indexBegin = 0;
		this->indexEnd = this->buffSize - 1;
	}
	else if (this->indexBegin <= this->indexEnd)
	{
		if (this->indexEnd + buffSize < this->buffSize)
		{
			MemCopyNO(&this->buff[this->indexEnd], buff, buffSize);
			this->indexEnd += buffSize;
		}
		else
		{
			MemCopyNO(&this->buff[this->indexEnd], buff, this->buffSize - this->indexEnd);
			buff += (this->buffSize - this->indexEnd);
			buffSize -= (this->buffSize - this->indexEnd);
			this->indexEnd = 0;
			if (buffSize > 0)
			{
				MemCopyNO(this->buff, buff, buffSize);
				this->indexEnd = buffSize;
			}
			if (this->indexEnd >= this->indexBegin)
			{
				this->indexBegin = this->indexEnd + 1;
				if (this->indexBegin >= this->buffSize)
				{
					this->indexBegin -= this->buffSize;
				}
			}
		}
	}
	else if (buffSize > 0)
	{
		if (this->indexEnd + buffSize < this->buffSize)
		{
			MemCopyNO(&this->buff[this->indexEnd], buff, buffSize);
			this->indexEnd += buffSize;
		}
		else
		{
			MemCopyNO(&this->buff[this->indexEnd], buff, this->buffSize - this->indexEnd);
			buff += (this->buffSize - this->indexEnd);
			buffSize -= (this->buffSize - this->indexEnd);
			this->indexEnd = 0;
			if (buffSize > 0)
			{
				MemCopyNO(this->buff, buff, buffSize);
				this->indexEnd = buffSize;
			}
		}
		this->indexBegin = this->indexEnd + 1;
		if (this->indexBegin >= this->buffSize)
		{
			this->indexBegin -= this->buffSize;
		}
	}
}

UOSInt Data::CircularByteBuff::GetBytes(UInt8 *buff)
{
	if (this->indexBegin <= this->indexEnd)
	{
		MemCopyNO(buff, &this->buff[this->indexBegin], this->indexEnd - this->indexBegin);
		return this->indexEnd - this->indexBegin;
	}
	else
	{
		MemCopyNO(buff, &this->buff[this->indexBegin], this->buffSize - this->indexBegin);
		if (this->indexEnd > 0)
		{
			MemCopyNO(&buff[this->buffSize - this->indexBegin], this->buff, this->indexEnd);
		}
		return this->indexEnd + this->buffSize - this->indexBegin;
	}
}

void Data::CircularByteBuff::ToString(NotNullPtr<Text::StringBuilderUTF8> sb)
{
	if (this->indexBegin <= this->indexEnd)
	{
		sb->AppendHexBuff(&this->buff[this->indexBegin], this->indexEnd - this->indexBegin, ' ', Text::LineBreakType::CRLF);
	}
	else
	{
		UOSInt cnt = this->buffSize - this->indexBegin;
		sb->AppendHexBuff(&this->buff[this->indexBegin], cnt, ' ', Text::LineBreakType::CRLF);
		if (this->indexEnd > 0)
		{
			if (cnt & 15)
			{
				cnt = 16 - (cnt & 15);
				sb->AppendUTF8Char(' ');
				if (this->indexEnd >= cnt)
				{
					sb->AppendHexBuff(this->buff, cnt, ' ', Text::LineBreakType::None);
				}
				else
				{
					sb->AppendHexBuff(this->buff, this->indexEnd, ' ', Text::LineBreakType::None);
				}
			}
			else
			{
				cnt = 0;
			}
			if (this->indexEnd > cnt)
			{
				sb->AppendLB(Text::LineBreakType::CRLF);
				sb->AppendHexBuff(&this->buff[cnt], this->indexEnd - cnt, ' ', Text::LineBreakType::CRLF);
			}
		}
	}

}