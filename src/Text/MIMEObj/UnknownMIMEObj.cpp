#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Text/MIMEObj/MIMEHeader.h"
#include "Text/MIMEObj/UnknownMIMEObj.h"

Text::MIMEObj::UnknownMIMEObj::UnknownMIMEObj(UInt8 *dataBuff, UOSInt buffSize, const UTF8Char *contentType) : Text::IMIMEObj(contentType)
{
	this->buffSize = buffSize;
	this->dataBuff = MemAlloc(UInt8, buffSize);
	this->contType = Text::String::NewNotNull(contentType);
	MemCopyNO(this->dataBuff, dataBuff, buffSize);
	const UTF8Char *tmpPtr = Text::StrCopyNewC(contentType, this->contType->leng);
	UOSInt i;
	UOSInt j;
	Text::PString sarr[2];
	UTF8Char sbuff[256];
	UTF8Char *sptr;
	i = Text::StrSplitTrimP(sarr, 2, (UTF8Char*)tmpPtr, this->contType->leng, ';');
	while (i == 2)
	{
		i = Text::StrSplitTrimP(sarr, 2, sarr[1].v, sarr[1].len, ';');
		if (Text::StrStartsWithC(sarr[0].v, sarr[0].len, UTF8STRC("name=")))
		{
			j = sarr[0].len;
			if (sarr[0].v[5] == '"' && sarr[0].v[j - 1] == '"')
			{
				sarr[0].v[j - 1] = 0;
				sptr = Text::MIMEObj::MIMEHeader::ParseHeaderStr(sbuff, &sarr[0].v[6]);
			}
			else
			{
				sptr = Text::MIMEObj::MIMEHeader::ParseHeaderStr(sbuff, &sarr[0].v[5]);
			}
			this->SetSourceName(sbuff, (UOSInt)(sptr - sbuff));
		}

	}
	Text::StrDelNew(tmpPtr);
}

Text::MIMEObj::UnknownMIMEObj::~UnknownMIMEObj()
{
	MemFree(this->dataBuff);
	this->contType->Release();
}

Text::CString Text::MIMEObj::UnknownMIMEObj::GetClassName()
{
	return {UTF8STRC("UnknownMIMEObj")};
}

Text::CString Text::MIMEObj::UnknownMIMEObj::GetContentType()
{
	return {this->contType->v, this->contType->leng};
}

UOSInt Text::MIMEObj::UnknownMIMEObj::WriteStream(IO::Stream *stm)
{
	return stm->Write(this->dataBuff, this->buffSize);
}

Text::IMIMEObj *Text::MIMEObj::UnknownMIMEObj::Clone()
{
	Text::MIMEObj::UnknownMIMEObj *newObj;
	NEW_CLASS(newObj, Text::MIMEObj::UnknownMIMEObj(this->dataBuff, this->buffSize, this->contType->v));
	return newObj;
}

const UInt8 *Text::MIMEObj::UnknownMIMEObj::GetRAWData(UOSInt *dataSize)
{
	*dataSize = this->buffSize;
	return this->dataBuff;
}
