#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Text/MIMEObj/MIMEHeader.h"
#include "Text/MIMEObj/UnknownMIMEObj.h"

Text::MIMEObj::UnknownMIMEObj::UnknownMIMEObj(UInt8 *dataBuff, UOSInt buffSize, const UTF8Char *contentType) : Text::IMIMEObj(contentType)
{
	this->buffSize = buffSize;
	this->dataBuff = MemAlloc(UInt8, buffSize);
	this->contType = Text::StrCopyNew(contentType);
	MemCopyNO(this->dataBuff, dataBuff, buffSize);
	const UTF8Char *tmpPtr = Text::StrCopyNew(contentType);
	UOSInt i;
	UOSInt j;
	UTF8Char *sarr[2];
	UTF8Char sbuff[256];
	UTF8Char *sptr;
	i = Text::StrSplitTrim(sarr, 2, (UTF8Char*)tmpPtr, ';');
	while (i == 2)
	{
		i = Text::StrSplitTrim(sarr, 2, sarr[1], ';');
		if (Text::StrStartsWith(sarr[0], (const UTF8Char*)"name="))
		{
			j = Text::StrCharCnt(sarr[0]);
			if (sarr[0][5] == '"' && sarr[0][j - 1] == '"')
			{
				sarr[0][j - 1] = 0;
				sptr = Text::MIMEObj::MIMEHeader::ParseHeaderStr(sbuff, &sarr[0][6]);
			}
			else
			{
				sptr = Text::MIMEObj::MIMEHeader::ParseHeaderStr(sbuff, &sarr[0][5]);
			}
			this->SetSourceName(sbuff, (UOSInt)(sptr - sbuff));
		}

	}
	Text::StrDelNew(tmpPtr);
}

Text::MIMEObj::UnknownMIMEObj::~UnknownMIMEObj()
{
	MemFree(this->dataBuff);
	Text::StrDelNew(this->contType);
}

const UTF8Char *Text::MIMEObj::UnknownMIMEObj::GetClassName()
{
	return (const UTF8Char*)"UnknownMIMEObj";
}

const UTF8Char *Text::MIMEObj::UnknownMIMEObj::GetContentType()
{
	return this->contType;
}

UOSInt Text::MIMEObj::UnknownMIMEObj::WriteStream(IO::Stream *stm)
{
	return stm->Write(this->dataBuff, this->buffSize);
}

Text::IMIMEObj *Text::MIMEObj::UnknownMIMEObj::Clone()
{
	Text::MIMEObj::UnknownMIMEObj *newObj;
	NEW_CLASS(newObj, Text::MIMEObj::UnknownMIMEObj(this->dataBuff, this->buffSize, this->contType));
	return newObj;
}

const UInt8 *Text::MIMEObj::UnknownMIMEObj::GetRAWData(UOSInt *dataSize)
{
	*dataSize = this->buffSize;
	return this->dataBuff;
}
