#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Text/MIMEObj/MIMEMessage.h"
#include "Text/MIMEObj/UnknownMIMEObj.h"

Text::MIMEObj::UnknownMIMEObj::UnknownMIMEObj(UInt8 *dataBuff, UOSInt buffSize, Text::CStringNN contentType) : Text::IMIMEObj(contentType)
{
	this->buffSize = buffSize;
	this->dataBuff = MemAlloc(UInt8, buffSize);
	this->contType = Text::String::New(contentType);
	MemCopyNO(this->dataBuff, dataBuff, buffSize);
	const UTF8Char *tmpPtr = Text::StrCopyNewC(contentType.v, this->contType->leng).Ptr();
	UOSInt i;
	UOSInt j;
	Text::PString sarr[2];
	UTF8Char sbuff[256];
	UTF8Char *sptr;
	i = Text::StrSplitTrimP(sarr, 2, {(UTF8Char*)tmpPtr, this->contType->leng}, ';');
	while (i == 2)
	{
		i = Text::StrSplitTrimP(sarr, 2, sarr[1], ';');
		if (Text::StrStartsWithC(sarr[0].v, sarr[0].leng, UTF8STRC("name=")))
		{
			j = sarr[0].leng;
			if (sarr[0].v[5] == '"' && sarr[0].v[j - 1] == '"')
			{
				sarr[0].v[j - 1] = 0;
				sptr = Text::MIMEObj::MIMEMessage::ParseHeaderStr(sbuff, &sarr[0].v[6]);
			}
			else
			{
				sptr = Text::MIMEObj::MIMEMessage::ParseHeaderStr(sbuff, &sarr[0].v[5]);
			}
			this->SetSourceName(CSTRP(sbuff, sptr));
		}

	}
	Text::StrDelNew(tmpPtr);
}

Text::MIMEObj::UnknownMIMEObj::~UnknownMIMEObj()
{
	MemFree(this->dataBuff);
	this->contType->Release();
}

Text::CStringNN Text::MIMEObj::UnknownMIMEObj::GetClassName() const
{
	return CSTR("UnknownMIMEObj");
}

Text::CStringNN Text::MIMEObj::UnknownMIMEObj::GetContentType() const
{
	return this->contType->ToCString();
}

UOSInt Text::MIMEObj::UnknownMIMEObj::WriteStream(IO::Stream *stm) const
{
	return stm->Write(this->dataBuff, this->buffSize);
}

Text::IMIMEObj *Text::MIMEObj::UnknownMIMEObj::Clone() const
{
	Text::MIMEObj::UnknownMIMEObj *newObj;
	NEW_CLASS(newObj, Text::MIMEObj::UnknownMIMEObj(this->dataBuff, this->buffSize, this->contType->ToCString()));
	return newObj;
}

const UInt8 *Text::MIMEObj::UnknownMIMEObj::GetRAWData(UOSInt *dataSize) const
{
	*dataSize = this->buffSize;
	return this->dataBuff;
}
