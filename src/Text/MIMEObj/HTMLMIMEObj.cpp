#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/Encoding.h"
#include "Text/EncodingFactory.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/MIMEObj/HTMLMIMEObj.h"

void Text::MIMEObj::HTMLMIMEObj::BuildContentType()
{
	Text::StringBuilderUTF8 sbc;
	UTF8Char u8buff[64];
	u8buff[0] = 0;
	sbc.AppendC(UTF8STRC("text/html; charset="));
	Text::EncodingFactory::GetInternetName(u8buff, this->codePage);
	sbc.Append(u8buff);
	this->contType = Text::String::New(sbc.ToString(), sbc.GetLength());
}

Text::MIMEObj::HTMLMIMEObj::HTMLMIMEObj(UInt8 *textBuff, OSInt buffSize, Int32 codePage) : Text::IMIMEObj((const UTF8Char*)"text/html")
{
	this->contType = 0;
	this->codePage = codePage;
	this->textBuff = MemAlloc(UInt8, buffSize);
	this->buffSize = buffSize;
	MemCopyNO(this->textBuff, textBuff, buffSize);
	this->BuildContentType();
}

Text::MIMEObj::HTMLMIMEObj::~HTMLMIMEObj()
{
	MemFree(this->textBuff);
	this->contType->Release();
}

Text::CString Text::MIMEObj::HTMLMIMEObj::GetClassName()
{
	return {UTF8STRC("HTMLMIMEObj")};
}

Text::CString Text::MIMEObj::HTMLMIMEObj::GetContentType()
{
	return {this->contType->v, this->contType->leng};
}

UOSInt Text::MIMEObj::HTMLMIMEObj::WriteStream(IO::Stream *stm)
{
	return stm->Write(this->textBuff, this->buffSize);
}

Text::IMIMEObj *Text::MIMEObj::HTMLMIMEObj::Clone()
{
	Text::MIMEObj::HTMLMIMEObj *txt;
	NEW_CLASS(txt, Text::MIMEObj::HTMLMIMEObj(this->textBuff, this->buffSize, this->codePage));
	return txt;
}

void Text::MIMEObj::HTMLMIMEObj::GetText(Text::StringBuilderUTF *sb)
{
	Text::Encoding enc(this->codePage);
	OSInt strLen;
	UTF8Char *sbuff;
	strLen = enc.CountUTF8Chars(this->textBuff, this->buffSize);
	sbuff = MemAlloc(UTF8Char, strLen + 1);
	enc.UTF8FromBytes(sbuff, this->textBuff, this->buffSize, 0);
	sb->Append(sbuff);
	MemFree(sbuff);
}

Int32 Text::MIMEObj::HTMLMIMEObj::GetCodePage()
{
	return this->codePage;
}
