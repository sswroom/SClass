#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Text/MIMEObj/MIMEMessage.h"
#include "Text/MIMEObj/UnknownMIMEObj.h"

Text::MIMEObj::UnknownMIMEObj::UnknownMIMEObj(UnsafeArray<UInt8> dataBuff, UOSInt buffSize, Text::CStringNN contentType) : Text::MIMEObject(contentType)
{
	this->buffSize = buffSize;
	this->dataBuff = MemAllocArr(UInt8, buffSize);
	this->contType = Text::String::New(contentType);
	this->dataBuff.CopyFromNO(dataBuff, buffSize);
	UnsafeArray<const UTF8Char> tmpPtr = Text::StrCopyNewC(contentType.v, this->contType->leng);
	UOSInt i;
	UOSInt j;
	Text::PString sarr[2];
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	i = Text::StrSplitTrimP(sarr, 2, Text::PString(UnsafeArray<UTF8Char>::ConvertFrom(tmpPtr), this->contType->leng), ';');
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
	MemFreeArr(this->dataBuff);
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

UOSInt Text::MIMEObj::UnknownMIMEObj::WriteStream(NN<IO::Stream> stm) const
{
	return stm->Write(Data::ByteArrayR(this->dataBuff, this->buffSize));
}

NN<Text::MIMEObject> Text::MIMEObj::UnknownMIMEObj::Clone() const
{
	NN<Text::MIMEObj::UnknownMIMEObj> newObj;
	NEW_CLASSNN(newObj, Text::MIMEObj::UnknownMIMEObj(this->dataBuff, this->buffSize, this->contType->ToCString()));
	return newObj;
}

UnsafeArray<const UInt8> Text::MIMEObj::UnknownMIMEObj::GetRAWData(OutParam<UOSInt> dataSize) const
{
	dataSize.Set(this->buffSize);
	return this->dataBuff;
}
