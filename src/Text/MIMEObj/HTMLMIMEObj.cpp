#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/Encoding.h"
#include "Text/EncodingFactory.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/MIMEObj/HTMLMIMEObj.h"

void Text::MIMEObj::HTMLMIMEObj::BuildContentType()
{
	Text::StringBuilderUTF8 sbc;
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	sbuff[0] = 0;
	sbc.AppendC(UTF8STRC("text/html; charset="));
	sptr = Text::EncodingFactory::GetInternetName(sbuff, this->codePage);
	sbc.AppendP(sbuff, sptr);
	this->contType = Text::String::New(sbc.ToString(), sbc.GetLength()).Ptr();
}

Text::MIMEObj::HTMLMIMEObj::HTMLMIMEObj(UnsafeArray<const UInt8> textBuff, UOSInt buffSize, UInt32 codePage) : Text::IMIMEObj(CSTR("text/html"))
{
	this->contType = 0;
	this->codePage = codePage;
	this->textBuff = MemAlloc(UInt8, buffSize);
	this->buffSize = buffSize;
	MemCopyNO(this->textBuff, textBuff.Ptr(), buffSize);
	this->BuildContentType();
}

Text::MIMEObj::HTMLMIMEObj::~HTMLMIMEObj()
{
	MemFree(this->textBuff);
	this->contType->Release();
}

Text::CStringNN Text::MIMEObj::HTMLMIMEObj::GetClassName() const
{
	return CSTR("HTMLMIMEObj");
}

Text::CStringNN Text::MIMEObj::HTMLMIMEObj::GetContentType() const
{
	return this->contType->ToCString();
}

UOSInt Text::MIMEObj::HTMLMIMEObj::WriteStream(NN<IO::Stream> stm) const
{
	return stm->Write(this->textBuff, this->buffSize);
}

NN<Text::IMIMEObj> Text::MIMEObj::HTMLMIMEObj::Clone() const
{
	NN<Text::MIMEObj::HTMLMIMEObj> txt;
	NEW_CLASSNN(txt, Text::MIMEObj::HTMLMIMEObj(this->textBuff, this->buffSize, this->codePage));
	return txt;
}

void Text::MIMEObj::HTMLMIMEObj::GetText(NN<Text::StringBuilderUTF8> sb) const
{
	Text::Encoding enc(this->codePage);
	UOSInt strLen;
	UnsafeArray<UTF8Char> sbuff;
	UnsafeArray<UTF8Char> sptr;
	strLen = enc.CountUTF8Chars(this->textBuff, this->buffSize);
	sbuff = MemAllocArr(UTF8Char, strLen + 1);
	sptr = enc.UTF8FromBytes(sbuff, this->textBuff, this->buffSize, 0);
	sb->AppendC(sbuff, (UOSInt)(sptr - sbuff));
	MemFreeArr(sbuff);
}

UInt32 Text::MIMEObj::HTMLMIMEObj::GetCodePage() const
{
	return this->codePage;
}
