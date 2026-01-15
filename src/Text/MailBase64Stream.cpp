#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MailBase64Stream.h"

Text::MailBase64Stream::MailBase64Stream(NN<IO::Stream> stm) : IO::Stream(stm->GetSourceNameObj())
{
	this->lineBuffSize = 0;
	this->lineCnt = 0;
	this->stm = stm;
}

Text::MailBase64Stream::~MailBase64Stream()
{
	UInt8 b64buff[74];
	if (this->lineBuffSize > 0)
	{
		UIntOS i = b64.Encrypt(this->lineBuff, this->lineBuffSize, (UInt8*)b64buff);
		this->stm->Write(Data::ByteArrayR(b64buff, i));
	}
}

Bool Text::MailBase64Stream::IsDown() const
{
	return this->stm->IsDown();
}

UIntOS Text::MailBase64Stream::Read(const Data::ByteArray &buff)
{
	return 0;
}

UIntOS Text::MailBase64Stream::Write(Data::ByteArrayR buff)
{
	UIntOS oriSize = buff.GetSize();
	UInt8 b64buff[74];
	if (this->lineBuffSize + buff.GetSize() >= 54)
	{
		MemCopyNO(&this->lineBuff[this->lineBuffSize], buff.Arr().Ptr(), 54 - this->lineBuffSize);
		this->b64.Encrypt(this->lineBuff, 54, b64buff);
		b64buff[72] = 13;
		b64buff[73] = 10;
		this->stm->Write(Data::ByteArrayR(b64buff, 74));
		buff += 54 - this->lineBuffSize;
		this->lineBuffSize = 0;
		this->lineCnt++;
	}
	while (buff.GetSize() >= 54)
	{
		this->b64.Encrypt(buff.Arr(), 54, b64buff);
		b64buff[72] = 13;
		b64buff[73] = 10;
		this->stm->Write(Data::ByteArrayR(b64buff, 74));
		buff += 54;
		this->lineCnt++;
	}
	if (buff.GetSize() > 0)
	{
		MemCopyNO(&this->lineBuff[this->lineBuffSize], buff.Arr().Ptr(), buff.GetSize());
		this->lineBuffSize += buff.GetSize();
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

UIntOS Text::MailBase64Stream::GetWriteCount()
{
	UIntOS blk = this->lineBuffSize / 3;
	if (this->lineBuffSize > blk * 3)
		blk++;
	return (this->lineCnt * 74) + (blk * 4);
}

