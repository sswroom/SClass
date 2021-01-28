#include "Stdafx.h"
#include "MyMemory.h"
#include "Crypto/Encrypt/Base64.h"
#include "Text/Encoding.h"
#include "Text/EncodingFactory.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/MIMEObj/MIMEHeader.h"

Text::MIMEObj::MIMEHeader::MIMEHeader()
{
	NEW_CLASS(this->headerName, Data::ArrayList<const UTF8Char *>());
	NEW_CLASS(this->headerValue, Data::ArrayList<const UTF8Char *>());
}

Text::MIMEObj::MIMEHeader::~MIMEHeader()
{
	OSInt i;
	i = this->headerName->GetCount();
	while (i-- > 0)
	{
		Text::StrDelNew(this->headerName->GetItem(i));
		Text::StrDelNew(this->headerValue->GetItem(i));
	}
	DEL_CLASS(this->headerName);
	DEL_CLASS(this->headerValue);
}


void Text::MIMEObj::MIMEHeader::AddHeader(const UTF8Char *name, const UTF8Char *value)
{
	this->headerName->Add(Text::StrCopyNew(name));
	this->headerValue->Add(Text::StrCopyNew(value));
}

const UTF8Char *Text::MIMEObj::MIMEHeader::GetHeader(const UTF8Char *name)
{
	OSInt i;
	OSInt j;
	i = 0;
	j = this->headerName->GetCount();
	while (i < j)
	{
		if (Text::StrEquals(name, this->headerName->GetItem(i)))
			return this->headerValue->GetItem(i);
		i++;
	}
	return 0;
}

OSInt Text::MIMEObj::MIMEHeader::GetHeaderCount()
{
	return this->headerName->GetCount();
}

const UTF8Char *Text::MIMEObj::MIMEHeader::GetHeaderName(OSInt index)
{
	return this->headerName->GetItem(index);
}

const UTF8Char *Text::MIMEObj::MIMEHeader::GetHeaderValue(OSInt index)
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
						sb.AppendChar(c, 1);
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
							sbc.AppendChar(c, 1);
						}
					}
				}
				if (!err)
				{
					Text::EncodingFactory encFact;
					Int32 cp = encFact.GetCodePage(sb.ToString());
					if (cp == 0)
					{
						err = true;
					}
					else
					{
						Text::Encoding enc(cp);
						Crypto::Encrypt::Base64 b64;
						OSInt buffSize = (sbc.GetLength() >> 2) * 3;
						OSInt outSize;
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

