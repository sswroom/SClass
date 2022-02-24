#include "Stdafx.h"
#include "MyMemory.h"
#include "Crypto/Encrypt/Base64.h"
#include "Text/Encoding.h"
#include "Text/EncodingFactory.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/MIMEObj/MIMEHeader.h"

Text::MIMEObj::MIMEHeader::MIMEHeader()
{
	NEW_CLASS(this->headerName, Data::ArrayList<Text::String *>());
	NEW_CLASS(this->headerValue, Data::ArrayList<Text::String *>());
}

Text::MIMEObj::MIMEHeader::~MIMEHeader()
{
	UOSInt i;
	i = this->headerName->GetCount();
	while (i-- > 0)
	{
		this->headerName->GetItem(i)->Release();
		this->headerValue->GetItem(i)->Release();
	}
	DEL_CLASS(this->headerName);
	DEL_CLASS(this->headerValue);
}


void Text::MIMEObj::MIMEHeader::AddHeader(const UTF8Char *name, UOSInt nameLen, const UTF8Char *value, UOSInt valueLen)
{
	this->headerName->Add(Text::String::New(name, nameLen));
	this->headerValue->Add(Text::String::New(value, valueLen));
}

void Text::MIMEObj::MIMEHeader::AddHeader(Text::String *name, Text::String *value)
{
	this->headerName->Add(name->Clone());
	this->headerValue->Add(value->Clone());
}

Text::String *Text::MIMEObj::MIMEHeader::GetHeader(const UTF8Char *name, UOSInt nameLen)
{
	UOSInt i;
	UOSInt j;
	i = 0;
	j = this->headerName->GetCount();
	while (i < j)
	{
		if (this->headerName->GetItem(i)->Equals(name, nameLen))
			return this->headerValue->GetItem(i);
		i++;
	}
	return 0;
}

UOSInt Text::MIMEObj::MIMEHeader::GetHeaderCount()
{
	return this->headerName->GetCount();
}

Text::String *Text::MIMEObj::MIMEHeader::GetHeaderName(UOSInt index)
{
	return this->headerName->GetItem(index);
}

Text::String *Text::MIMEObj::MIMEHeader::GetHeaderValue(UOSInt index)
{
	return this->headerValue->GetItem(index);
}

UTF8Char *Text::MIMEObj::MIMEHeader::ParseHeaderStr(UTF8Char *sbuff, const UTF8Char *value)
{
	Text::StringBuilderUTF8 sb;
	Text::StringBuilderUTF8 sbc;
	UTF8Char c;
	Bool err;
	while (true)
	{
		c = *value++;
		if (c == '=')
		{
			if (*value == '?')
			{
				err = false;
				value++;
				sb.ClearStr();
				while (true)
				{
					c = *value++;
					if (c == '?')
					{
						if (*value == 'B' && value[1] == '?')
						{
							value += 2;
							break;
						}
						else if (*value == 'b' && value[1] == '?')
						{
							value += 2;
							break;
						}
						else
						{
							err = true;
							break;
						}
					}
					else if (c == 0)
					{
						err = true;
						break;
					}
					else
					{
						sb.AppendUTF8Char(c);
					}
				}
				if (!err)
				{
					sbc.ClearStr();
					while (true)
					{
						c = *value++;
						if (c == '?')
						{
							if (*value == '=')
							{
								value++;
								break;
							}
							else
							{
								err = true;
								break;
							}
						}
						else if (c == 0)
						{
							err = true;
							break;
						}
						else
						{
							sbc.AppendUTF8Char(c);
						}
					}
				}
				if (!err)
				{
					Text::EncodingFactory encFact;
					UInt32 cp = encFact.GetCodePage(sb.ToCString());
					if (cp == 0)
					{
						err = true;
					}
					else
					{
						Text::Encoding enc(cp);
						Crypto::Encrypt::Base64 b64;
						UOSInt buffSize = (sbc.GetLength() >> 2) * 3;
						UOSInt outSize;
						UInt8 *tmpBuff = MemAlloc(UInt8, buffSize);
						outSize = b64.Decrypt((const UInt8*)sbc.ToString(), sbc.GetLength(), tmpBuff, 0);
						sbuff = enc.UTF8FromBytes(sbuff, tmpBuff, outSize, 0);
						MemFree(tmpBuff);
					}
				}
			}
			else
			{
				*sbuff++ = c;
			}
		}
		else
		{
			if (c == 0)
			{
				*sbuff = 0;
				break;
			}
			*sbuff++ = c;
		}
	}
	return sbuff;
}

