#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MailBase64Stream.h"

Text::MailBase64Stream::MailBase64Stream(IO::Stream *stm) : IO::Stream(stm->GetSourceNameObj())
{
	this->lineBuffSize = 0;
	this->lineCnt = 0;
	this->stm = stm;
	NEW_CLASS(this->b64, Crypto::Encrypt::Base64());
}

Text::MailBase64Stream::~MailBase64Stream()
{
	UInt8 b64buff[74];
	if (this->lineBuffSize > 0)
	{
		UOSInt i = b64->Encrypt(this->lineBuff, this->lineBuffSize, (UInt8*)b64buff, 0);
		this->stm->Write(b64buff, i);
	}
	DEL_CLASS(this->b64);
}

Bool Text::MailBase64Stream::IsDown()
{
	return this->stm->IsDown();
}

UOSInt Text::MailBase64Stream::Read(UInt8 *buff, UOSInt size)
{
	return 0;
}

UOSInt Text::MailBase64Stream::Write(const UInt8 *buff, UOSInt size)
{
	UOSInt oriSize = size;
	UInt8 b64buff[74];
	if (this->lineBuffSize + size >= 54)
	{
		MemCopyNO(&this->lineBuff[this->lineBuffSize], buff, 54 - this->lineBuffSize);
		b64->Encrypt(this->lineBuff, 54, b64buff, 0);
		b64buff[72] = 13;
		b64buff[73] = 10;
		this->stm->Write(b64buff, 74);
		buff += 54 - this->lineBuffSize;
		size -= (54 - this->lineBuffSize);
		this->lineBuffSize = 0;
		this->lineCnt++;
	}
	while (size >= 54)
	{
		b64->Encrypt(buff, 54, b64buff, 0);
		b64buff[72] = 13;
		b64buff[73] = 10;
		this->stm->Write(b64buff, 74);
		size -= 54;
		buff += 54;
		this->lineCnt++;
	}
	if (size > 0)
	{
		MemCopyNO(&this->lineBuff[this->lineBuffSize], buff, size);
		this->lineBuffSize += size;
	}
	return oriSize;
}

Int32 Text::MailBase64Stream::Flush()
{
	return 0;
}

void Text::MailBase64Stream::Close()
{
}

Bool Text::MailBase64Stream::Recover()
{
	return this->stm->Recover();
}

UOSInt Text::MailBase64Stream::GetWriteCount()
{
	UOSInt blk = this->lineBuffSize / 3;
	if (this->lineBuffSize > blk * 3)
		blk++;
	return (this->lineCnt * 74) + (blk * 4);
}

