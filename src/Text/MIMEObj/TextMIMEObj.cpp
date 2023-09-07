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
	UTF8Char sbuff[64];
	UTF8Char *sptr;
	sbuff[0] = 0;
	sbc.AppendC(UTF8STRC("text/plain; charset="));
	sptr = Text::EncodingFactory::GetInternetName(sbuff, this->codePage);
	sbc.AppendP(sbuff, sptr);
	this->contType = Text::String::New(sbc.ToString(), sbc.GetLength()).Ptr();
}

Text::MIMEObj::TextMIMEObj::TextMIMEObj(const UInt8 *textBuff, UOSInt buffSize, UInt32 codePage) : Text::IMIMEObj(CSTR("text/plain"))
{
	this->contType = 0;
	this->codePage = codePage;
	this->textBuff = MemAlloc(UInt8, buffSize);
	this->buffSize = buffSize;
	MemCopyNO(this->textBuff, textBuff, buffSize);
	this->BuildContentType();
}

Text::MIMEObj::TextMIMEObj::TextMIMEObj(const WChar *txt, UInt32 codePage) : Text::IMIMEObj(CSTR("text/plain"))
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
	this->contType->Release();
}

Text::CStringNN Text::MIMEObj::TextMIMEObj::GetClassName() const
{
	return CSTR("TextMIMEObj");
}

Text::CStringNN Text::MIMEObj::TextMIMEObj::GetContentType() const
{
	return this->contType->ToCString();
}

UOSInt Text::MIMEObj::TextMIMEObj::WriteStream(IO::Stream *stm) const
{
	return stm->Write(this->textBuff, this->buffSize);
}

Text::IMIMEObj *Text::MIMEObj::TextMIMEObj::Clone() const
{
	Text::MIMEObj::TextMIMEObj *txt;
	NEW_CLASS(txt, Text::MIMEObj::TextMIMEObj(this->textBuff, this->buffSize, this->codePage));
	return txt;
}

void Text::MIMEObj::TextMIMEObj::GetText(NotNullPtr<Text::StringBuilderUTF8> sb) const
{
	Text::Encoding enc(this->codePage);
	UOSInt strLen;
	UTF8Char *sbuff;
	UTF8Char *sptr;
	strLen = enc.CountUTF8Chars(this->textBuff, this->buffSize);
	sbuff = MemAlloc(UTF8Char, strLen + 1);
	sptr = enc.UTF8FromBytes(sbuff, this->textBuff, this->buffSize, 0);
	sb->AppendC(sbuff, (UOSInt)(sptr - sbuff));
	MemFree(sbuff);
}

UInt32 Text::MIMEObj::TextMIMEObj::GetCodePage() const
{
	return this->codePage;
}
