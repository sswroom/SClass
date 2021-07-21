#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/Encoding.h"
#include "Text/EncodingFactory.h"
#include "Text/MyStringW.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/MIMEObj/TextMIMEObj.h"

void Text::MIMEObj::TextMIMEObj::BuildContentType()
{
	Text::StringBuilderUTF8 sbc;
	UTF8Char u8buff[64];
	u8buff[0] = 0;
	sbc.Append((const UTF8Char*)"text/plain; charset=");
	Text::EncodingFactory::GetInternetName(u8buff, this->codePage);
	sbc.Append(u8buff);
	this->contType = Text::StrCopyNew(sbc.ToString());
}

Text::MIMEObj::TextMIMEObj::TextMIMEObj(UInt8 *textBuff, UOSInt buffSize, UInt32 codePage) : Text::IMIMEObj((const UTF8Char*)"text/plain")
{
	this->contType = 0;
	this->codePage = codePage;
	this->textBuff = MemAlloc(UInt8, buffSize);
	this->buffSize = buffSize;
	MemCopyNO(this->textBuff, textBuff, buffSize);
	this->BuildContentType();
}

Text::MIMEObj::TextMIMEObj::TextMIMEObj(const WChar *txt, UInt32 codePage) : Text::IMIMEObj((const UTF8Char*)"text/plain")
{
	Text::Encoding enc(codePage);
	UOSInt strLen;
	this->contType = 0;
	this->codePage = codePage;
	strLen = Text::StrCharCnt(txt);
	this->buffSize = enc.WCountBytesC(txt, strLen);
	this->textBuff = MemAlloc(UInt8, this->buffSize);
	enc.WToBytesC(this->textBuff, txt, strLen);
	this->BuildContentType();
}

Text::MIMEObj::TextMIMEObj::~TextMIMEObj()
{
	MemFree(this->textBuff);
	Text::StrDelNew(this->contType);
}

const UTF8Char *Text::MIMEObj::TextMIMEObj::GetClassName()
{
	return (const UTF8Char*)"TextMIMEObj";
}

const UTF8Char *Text::MIMEObj::TextMIMEObj::GetContentType()
{
	return (const UTF8Char *)this->contType;
}

UOSInt Text::MIMEObj::TextMIMEObj::WriteStream(IO::Stream *stm)
{
	return stm->Write(this->textBuff, this->buffSize);
}

Text::IMIMEObj *Text::MIMEObj::TextMIMEObj::Clone()
{
	Text::MIMEObj::TextMIMEObj *txt;
	NEW_CLASS(txt, Text::MIMEObj::TextMIMEObj(this->textBuff, this->buffSize, this->codePage));
	return txt;
}

void Text::MIMEObj::TextMIMEObj::GetText(Text::StringBuilderUTF *sb)
{
	Text::Encoding enc(this->codePage);
	UOSInt strLen;
	UTF8Char *sbuff;
	strLen = enc.CountUTF8Chars(this->textBuff, this->buffSize);
	sbuff = MemAlloc(UTF8Char, strLen + 1);
	enc.UTF8FromBytes(sbuff, this->textBuff, this->buffSize, 0);
	sb->Append(sbuff);
	MemFree(sbuff);
}

UInt32 Text::MIMEObj::TextMIMEObj::GetCodePage()
{
	return this->codePage;
}
