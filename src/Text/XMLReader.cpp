#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/MemoryStream.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/XML.h"
#include "Text/XMLReader.h"

#define BUFFSIZE 4096

#include <stdio.h>

void Text::XMLReader::FreeCurrent()
{
	SDEL_STRING(this->nodeText);
	SDEL_STRING(this->nodeOriText);
	UOSInt i = this->attrList->GetCount();
	Text::XMLAttrib *attr;
	while (i-- > 0)
	{
		attr = this->attrList->GetItem(i);
		DEL_CLASS(attr);
	}
	this->attrList->Clear();
}

Bool Text::XMLReader::IsHTMLSkip()
{
	if (this->nodeText->EqualsICase(UTF8STRC("META")))
	{
		return true;
	}
	else if (this->nodeText->EqualsICase(UTF8STRC("LINK")))
	{
		return true;
	}
	else if (this->nodeText->EqualsICase(UTF8STRC("IMG")))
	{
		return true;
	}
	else if (this->nodeText->EqualsICase(UTF8STRC("BR")))
	{
		return true;
	}
	else if (this->nodeText->EqualsICase(UTF8STRC("HR")))
	{
		return true;
	}
	else if (this->nodeText->EqualsICase(UTF8STRC("INPUT")))
	{
		return true;
	}
	return false;
}

void Text::XMLReader::InitBuffer()
{
	this->rawBuffSize = this->stm->Read(this->rawBuff, BUFFSIZE);
	if (this->rawBuffSize >= 4)
	{
		if (this->rawBuff[0] == 0xFF && this->rawBuff[1] == 0xFE)
		{
			NEW_CLASS(this->enc, Text::Encoding(1200));
			this->stmEnc = true;
			MemCopyO(this->rawBuff, &this->rawBuff[2], this->rawBuffSize - 2);
			this->rawBuffSize -= 2;
		}
		else if (this->rawBuff[0] == 0xFE && this->rawBuff[1] == 0xFF)
		{
			NEW_CLASS(this->enc, Text::Encoding(1201));
			this->stmEnc = true;
			MemCopyO(this->rawBuff, &this->rawBuff[2], this->rawBuffSize - 2);
			this->rawBuffSize -= 2;
		}
		else if (this->rawBuff[0] == '<' && this->rawBuff[1] == 0 && this->rawBuff[2] != 0 && this->rawBuff[3] == 0)
		{
			NEW_CLASS(this->enc, Text::Encoding(1200));
			this->stmEnc = true;
		}
		else if (this->rawBuff[1] == '<' && this->rawBuff[0] == 0 && this->rawBuff[3] != 0 && this->rawBuff[2] == 0)
		{
			NEW_CLASS(this->enc, Text::Encoding(1201));
			this->stmEnc = true;
		}
	}
	if (this->enc == 0)
	{
		MemCopyNO(this->readBuff, this->rawBuff, this->rawBuffSize);
		this->buffSize = this->rawBuffSize;
	}
}

UOSInt Text::XMLReader::FillBuffer()
{
	if (this->enc && this->stmEnc)
	{
		UOSInt rawReadSize = this->stm->Read(&this->rawBuff[this->rawBuffSize], BUFFSIZE - this->rawBuffSize);
		this->rawBuffSize += rawReadSize;
		if (this->buffSize >= (BUFFSIZE >> 1))
		{
			return 0;
		}
		rawReadSize = (BUFFSIZE >> 1) - this->buffSize;
		if (rawReadSize > this->rawBuffSize)
		{
			rawReadSize = this->rawBuffSize;
		}
		UTF8Char *sptr = this->enc->UTF8FromBytes(&this->readBuff[this->buffSize], this->rawBuff, rawReadSize, &rawReadSize);
		if (rawReadSize == this->rawBuffSize)
		{
			this->rawBuffSize = 0;
		}
		else if (rawReadSize > 0)
		{
			MemCopyO(this->rawBuff, &this->rawBuff[rawReadSize], this->rawBuffSize - rawReadSize);
			this->rawBuffSize -= rawReadSize;
		}
		UOSInt retSize = (UOSInt)(sptr - &this->readBuff[this->buffSize]);
		return retSize;
	}
	else
	{
		return this->stm->Read(&this->readBuff[this->buffSize], BUFFSIZE - this->buffSize);
	}
}

Text::XMLReader::XMLReader(Text::EncodingFactory *encFact, IO::Stream *stm, ParseMode mode)
{
	this->encFact = encFact;
	this->enc = 0;
	this->stm = stm;
	this->stmEnc = false;
	this->mode = mode;
	NEW_CLASS(this->attrList, Data::ArrayList<Text::XMLAttrib*>());
	this->readBuff = MemAlloc(UInt8, BUFFSIZE);
	this->buffSize = 0;
	this->rawBuff = MemAlloc(UInt8, BUFFSIZE);
	this->rawBuffSize = 0;
	this->parseOfst = 0;
	NEW_CLASS(this->pathList, Data::ArrayList<Text::String*>());
	this->nodeText = 0;
	this->nodeOriText = 0;
	this->emptyNode = false;
	this->parseError = 0;
	this->nt = Text::XMLNode::NT_UNKNOWN;
	this->InitBuffer();
}

Text::XMLReader::~XMLReader()
{
	this->FreeCurrent();

	UOSInt i = this->pathList->GetCount();
	while (i-- > 0)
	{
		this->pathList->GetItem(i)->Release();
	}
	DEL_CLASS(this->pathList);
	DEL_CLASS(this->attrList);
	MemFree(this->readBuff);
	MemFree(this->rawBuff);
	SDEL_CLASS(this->enc);
}

void Text::XMLReader::GetCurrPath(Text::StringBuilderUTF *sb)
{
	UOSInt i = 0;
	UOSInt j = this->pathList->GetCount();
	if (j == 0)
	{
		sb->AppendChar('/', 1);
		return;
	}
	while (i < j)
	{
		sb->AppendChar('/', 1);
		sb->Append(this->pathList->GetItem(i));
		i++;
	}
}

UOSInt Text::XMLReader::GetPathLev()
{
	return this->pathList->GetCount();
}

Text::XMLNode::NodeType Text::XMLReader::GetNodeType()
{
	return this->nt;
}

Text::String *Text::XMLReader::GetNodeText()
{
	return this->nodeText;
}

Text::String *Text::XMLReader::GetNodeOriText()
{
	return this->nodeOriText;
}

UOSInt Text::XMLReader::GetAttribCount()
{
	return this->attrList->GetCount();
}

Text::XMLAttrib *Text::XMLReader::GetAttrib(UOSInt index)
{
	return this->attrList->GetItem(index);
}

Text::XMLAttrib *Text::XMLReader::GetAttrib(const UTF8Char *name, UOSInt nameLen)
{
	UOSInt i = this->attrList->GetCount();
	Text::XMLAttrib *attr;
	while (i-- > 0)
	{
		attr = this->attrList->GetItem(i);
		if (attr->name->Equals(name, nameLen))
			return attr;
	}
	return 0;
}

Bool Text::XMLReader::ReadNext()
{
	Bool isHTMLScript = false;
	if (this->nt == Text::XMLNode::NT_ELEMENT && !this->emptyNode)
	{
		if (this->mode == Text::XMLReader::PM_HTML)
		{
			if (this->IsHTMLSkip())
			{

			}
			else if (this->nodeText->EqualsICase(UTF8STRC("LINK")))
			{

			}
			else if (this->nodeText->EqualsICase(UTF8STRC("IMG")))
			{

			}
			else if (this->nodeText->EqualsICase(UTF8STRC("BR")))
			{

			}
			else if (this->nodeText->EqualsICase(UTF8STRC("HR")))
			{

			}
			else if (this->nodeText->EqualsICase(UTF8STRC("INPUT")))
			{

			}
			else if (this->nodeText->EqualsICase(UTF8STRC("SCRIPT")))
			{
				isHTMLScript = true;
				this->pathList->Add(this->nodeText->Clone());
			}
			else
			{
				this->pathList->Add(this->nodeText->Clone());
			}
		}
		else
		{
			this->pathList->Add(this->nodeText->Clone());
		}
	}

	this->nt = Text::XMLNode::NT_UNKNOWN;
	this->FreeCurrent();
	if (this->parseError)
	{
		return false;
	}

	if ((this->buffSize - this->parseOfst) < 128)
	{
		if (this->parseOfst > 0)
		{
			if (this->buffSize <= this->parseOfst)
			{
				this->buffSize = 0;
				this->parseOfst = 0;
			}
			else
			{
				MemCopyO(this->readBuff, &this->readBuff[this->parseOfst], this->buffSize - this->parseOfst);
				this->buffSize -= this->parseOfst;
				this->parseOfst = 0;
			}
		}
		UOSInt readSize = this->FillBuffer();
		if (readSize > 0)
		{
			this->buffSize += readSize;
		}
	}

	if (this->buffSize <= 0)
	{
		return false;
	}
	if (this->readBuff[this->parseOfst] == '<')
	{
		UOSInt lenLeft = this->buffSize - this->parseOfst;
		if (lenLeft >= 4 && Text::StrStartsWith(&this->readBuff[this->parseOfst], (const UTF8Char*)"<!--"))
		{
			this->nt = Text::XMLNode::NT_COMMENT;
			this->parseOfst += 4;
			Text::StringBuilderUTF8 sb;
			while (true)
			{
				if (this->parseOfst + 2 >= this->buffSize)
				{
					if (this->parseOfst < this->buffSize)
					{
						MemCopyO(this->readBuff, &this->readBuff[this->parseOfst], this->buffSize - this->parseOfst);
						this->buffSize -= this->parseOfst;
						this->parseOfst = 0;
					}
					else
					{
						this->parseOfst = 0;
						this->buffSize = 0;
					}
					UOSInt readSize = this->FillBuffer();
					if (readSize <= 0)
					{
						this->parseError = 1;
						return false;
					}
					this->buffSize += readSize;
				}
				if (Text::StrStartsWith(&this->readBuff[this->parseOfst], (const UTF8Char*)"-->"))
				{
					this->parseOfst += 3;
					this->nodeText = Text::String::New(sb.ToString(), sb.GetLength());
					return true;
				}
				sb.AppendChar(this->readBuff[this->parseOfst++], 1);
			}
		}
		else if (lenLeft >= 9 && Text::StrStartsWith(&this->readBuff[this->parseOfst], (const UTF8Char*)"<![CDATA["))
		{
			this->nt = Text::XMLNode::NT_CDATA;
			this->parseOfst += 9;
			IO::MemoryStream mstm(128, UTF8STRC("Text.XMLReader.ReadNextType"));
			while (true)
			{
				if (this->parseOfst + 2 >= this->buffSize)
				{
					if (this->parseOfst < this->buffSize)
					{
						MemCopyO(this->readBuff, &this->readBuff[this->parseOfst], this->buffSize - this->parseOfst);
						this->buffSize -= this->parseOfst;
						this->parseOfst = 0;
					}
					else
					{
						this->parseOfst = 0;
						this->buffSize = 0;
					}
					UOSInt readSize = this->FillBuffer();
					if (readSize <= 0)
					{
						this->parseError = 2;
						return false;
					}
					this->buffSize += readSize;
				}
				if (Text::StrStartsWith(&this->readBuff[this->parseOfst], (const UTF8Char*)"]]>"))
				{
					this->parseOfst += 3;
					UOSInt size;
					UInt8 *buff = mstm.GetBuff(&size);
					this->nodeText = Text::String::New(buff, size);
					return true;
				}
				mstm.Write(&this->readBuff[this->parseOfst++], 1);
			}
		}
		else if (lenLeft >= 2 && this->readBuff[this->parseOfst + 1] == '!')
		{
			if (lenLeft >= 10 && Text::StrStartsWithICase(&this->readBuff[this->parseOfst + 2], (const UTF8Char*)"DOCTYPE "))
			{
				this->nt = Text::XMLNode::NT_DOCTYPE;
				Text::StringBuilderUTF8 sb;
				Bool isEqual = false;
				UTF8Char isQuote = 0;
				UTF8Char c;
				this->parseOfst += 2;
				while (true)
				{
					if (this->parseOfst >= this->buffSize)
					{
						this->parseOfst = 0;
						this->buffSize = 0;
						UOSInt readSize = this->FillBuffer();
						if (readSize <= 0)
						{
							this->parseError = 41;
							return false;
						}
						this->buffSize += readSize;
					}
					c = this->readBuff[this->parseOfst];
					if (isQuote)
					{
						if (c == isQuote)
						{
							isQuote = false;
						}
						else if (c == '&')
						{
							UOSInt l = this->buffSize - this->parseOfst;
							if (l >= 4 && this->readBuff[this->parseOfst + 3] == ';')
							{
								if (Text::StrStartsWith(&this->readBuff[this->parseOfst], (const UTF8Char*)"&lt;"))
								{
									sb.AppendChar('<', 1);
									this->parseOfst += 3;
								}
								else if (Text::StrStartsWith(&this->readBuff[this->parseOfst], (const UTF8Char*)"&gt;"))
								{
									sb.AppendChar('>', 1);
									this->parseOfst += 3;
								}
								else
								{
									this->parseError = 42;
									return false;
								}
							}
							else if (l >= 5 && this->readBuff[this->parseOfst + 4] == ';')
							{
								if (Text::StrStartsWith(&this->readBuff[this->parseOfst], (const UTF8Char*)"&amp;"))
								{
									sb.AppendChar('&', 1);
									this->parseOfst += 4;
								}
								else if (this->readBuff[this->parseOfst + 1] == '#')
								{
									sb.AppendChar(Text::StrHex2UInt8C(&this->readBuff[this->parseOfst + 2]), 1);
									this->parseOfst += 4;
								}
								else
								{
									this->parseError = 43;
									return false;
								}
							}
							else if (l >= 6 && this->readBuff[this->parseOfst + 5] == ';')
							{
								if (Text::StrStartsWith(&this->readBuff[this->parseOfst], (const UTF8Char*)"&quot;"))
								{
									sb.AppendChar('"', 1);
									this->parseOfst += 5;
								}
								else if (Text::StrStartsWith(&this->readBuff[this->parseOfst], (const UTF8Char*)"&apos;"))
								{
									sb.AppendChar('\'', 1);
									this->parseOfst += 5;
								}
								else
								{
									this->parseError = 44;
									return false;
								}
							}
							else
							{
								this->parseError = 45;
								return false;
							}
						}
						else
						{
							sb.AppendChar(c, 1);
						}
					}
					else if (c == ' ' || c == '\r' || c == '\n' || c == '\t')
					{
						if (sb.GetLength() > 0)
						{
							if (this->nodeText == 0)
							{
								this->nodeText = Text::String::New(sb.ToString(), sb.GetLength());
							}
							else if (isEqual)
							{
								Text::XMLAttrib *attr = this->attrList->GetItem(this->attrList->GetCount() - 1);
								SDEL_STRING(attr->value);
								attr->value = Text::String::New(sb.ToString(), sb.GetLength());
								isEqual = false;
							}
							else
							{
								Text::XMLAttrib *attr;
								NEW_CLASS(attr, Text::XMLAttrib(sb.ToString(), sb.GetLength(), 0, 0));
								this->attrList->Add(attr);
							}
							sb.ClearStr();
						}
					}
					else if (c == '>')
					{
						this->parseOfst += 1;
						if (this->nodeText != 0)
						{
							return true;
						}
						else
						{
							this->parseError = 46;
							return false;
						}
					}
					else if (c == '=')
					{
						if (sb.GetLength() > 0)
						{
							Text::XMLAttrib *attr;
							NEW_CLASS(attr, Text::XMLAttrib(sb.ToString(), sb.GetLength(), 0, 0));
							this->attrList->Add(attr);
							sb.ClearStr();
						}
						if (this->nodeText == 0)
						{
							this->parseError = 47;
							return false;
						}
						Text::XMLAttrib *attr = this->attrList->GetItem(this->attrList->GetCount() - 1);
						if (attr == 0)
						{
							this->parseError = 48;
							return false;
						}
						if (attr->value == 0 || attr->value->v[0] == 0)
						{
							isEqual = true;
						}
						else
						{
							this->parseError = 49;
							return false;
						}
					}
					else if (c == '"')
					{
						if (!isEqual)
						{
							this->parseError = 50;
							return false;
						}
						isQuote = '"';
					}
					else if (c == '\'')
					{
						if (!isEqual)
						{
							this->parseError = 51;
							return false;
						}
						isQuote = '\'';
					}
					else
					{
						sb.AppendChar(c, 1);
					}
					this->parseOfst++;
				}
				this->parseError = 52;
				return false;
			}
			else
			{
				this->parseError = 3;
				return false;
			}
		}
		else if (lenLeft >= 2 && this->readBuff[this->parseOfst + 1] == '?')
		{
			this->nt = Text::XMLNode::NT_DOCUMENT;
			Text::StringBuilderUTF8 sb;
			Bool isEqual = false;
			UTF8Char isQuote = 0;
			UTF8Char c;
			this->parseOfst += 2;
			while (true)
			{
				if (this->parseOfst >= this->buffSize)
				{
					this->parseOfst = 0;
					this->buffSize = 0;
					UOSInt readSize = this->FillBuffer();
					if (readSize <= 0)
					{
						this->parseError = 4;
						return false;
					}
					this->buffSize += readSize;
				}
				c = this->readBuff[this->parseOfst];
				if (isQuote)
				{
					if (c == isQuote)
					{
						isQuote = false;
					}
					else if (c == '&')
					{
						UOSInt l = this->buffSize - this->parseOfst;
						if (l >= 4 && this->readBuff[this->parseOfst + 3] == ';')
						{
							if (Text::StrStartsWith(&this->readBuff[this->parseOfst], (const UTF8Char*)"&lt;"))
							{
								sb.AppendChar('<', 1);
								this->parseOfst += 3;
							}
							else if (Text::StrStartsWith(&this->readBuff[this->parseOfst], (const UTF8Char*)"&gt;"))
							{
								sb.AppendChar('>', 1);
								this->parseOfst += 3;
							}
							else
							{
								this->parseError = 5;
								return false;
							}
						}
						else if (l >= 5 && this->readBuff[this->parseOfst + 4] == ';')
						{
							if (Text::StrStartsWith(&this->readBuff[this->parseOfst], (const UTF8Char*)"&amp;"))
							{
								sb.AppendChar('&', 1);
								this->parseOfst += 4;
							}
							else if (this->readBuff[this->parseOfst + 1] == '#')
							{
								sb.AppendChar(Text::StrHex2UInt8C(&this->readBuff[this->parseOfst + 2]), 1);
								this->parseOfst += 4;
							}
							else
							{
								this->parseError = 6;
								return false;
							}
						}
						else if (l >= 6 && this->readBuff[this->parseOfst + 5] == ';')
						{
							if (Text::StrStartsWith(&this->readBuff[this->parseOfst], (const UTF8Char*)"&quot;"))
							{
								sb.AppendChar('"', 1);
								this->parseOfst += 5;
							}
							else if (Text::StrStartsWith(&this->readBuff[this->parseOfst], (const UTF8Char*)"&apos;"))
							{
								sb.AppendChar('\'', 1);
								this->parseOfst += 5;
							}
							else
							{
								this->parseError = 7;
								return false;
							}
						}
						else
						{
							this->parseError = 8;
							return false;
						}
					}
					else
					{
						sb.AppendChar(c, 1);
					}
				}
				else if (c == ' ' || c == '\r' || c == '\n' || c == '\t')
				{
					if (sb.GetLength() > 0)
					{
						if (this->nodeText == 0)
						{
							this->nodeText = Text::String::New(sb.ToString(), sb.GetLength());
						}
						else if (isEqual)
						{
							Text::XMLAttrib *attr = this->attrList->GetItem(this->attrList->GetCount() - 1);
							SDEL_STRING(attr->value);
							attr->value = Text::String::New(sb.ToString(), sb.GetLength());
							isEqual = false;
						}
						else
						{
							Text::XMLAttrib *attr;
							NEW_CLASS(attr, Text::XMLAttrib(sb.ToString(), sb.GetLength(), 0, 0));
							this->attrList->Add(attr);
						}
						sb.ClearStr();
					}
				}
				else if (c == '?')
				{
					if (sb.GetLength() > 0)
					{
						if (this->nodeText == 0)
						{
							this->nodeText = Text::String::New(sb.ToString(), sb.GetLength());
						}
						else if (isEqual)
						{
							Text::XMLAttrib *attr = this->attrList->GetItem(this->attrList->GetCount() - 1);
							SDEL_STRING(attr->value);
							attr->value = Text::String::New(sb.ToString(), sb.GetLength());
							isEqual = false;
						}
						else
						{
							Text::XMLAttrib *attr;
							NEW_CLASS(attr, Text::XMLAttrib(sb.ToString(), sb.GetLength(), 0, 0));
							this->attrList->Add(attr);
						}
						sb.ClearStr();
					}
					if (this->readBuff[this->parseOfst + 1] == '>')
					{
						this->parseOfst += 2;
						if (this->nodeText != 0)
						{
							if (this->encFact && this->nodeText->Equals(UTF8STRC("xml")))
							{
								UOSInt i = this->attrList->GetCount();
								Text::XMLAttrib *attr;
								while (i-- > 0)
								{
									attr = this->attrList->GetItem(i);
									if (attr->name->EqualsICase(UTF8STRC("ENCODING")))
									{
										UInt32 cp = this->encFact->GetCodePage(attr->value->v);
										if (cp && !this->stmEnc)
										{
											SDEL_CLASS(this->enc);
											NEW_CLASS(this->enc, Text::Encoding(cp));
										}
										break;
									}
								}
							}
							return true;
						}
						else
						{
							this->parseError = 9;
							return false;
						}
					}
					else
					{
						this->parseError = 10;
						return false;
					}
				}
				else if (c == '=')
				{
					if (sb.GetLength() > 0)
					{
						Text::XMLAttrib *attr;
						NEW_CLASS(attr, Text::XMLAttrib(sb.ToString(), sb.GetLength(), 0, 0));
						this->attrList->Add(attr);
						sb.ClearStr();
					}
					if (this->nodeText == 0)
					{
						this->parseError = 11;
						return false;
					}
					Text::XMLAttrib *attr = this->attrList->GetItem(this->attrList->GetCount() - 1);
					if (attr == 0)
					{
						this->parseError = 12;
						return false;
					}
					if (attr->value == 0 || attr->value->v[0] == 0)
					{
						isEqual = true;
					}
					else
					{
						this->parseError = 13;
						return false;
					}
				}
				else if (c == '"')
				{
					if (!isEqual)
					{
						this->parseError = 14;
						return false;
					}
					isQuote = '"';
				}
				else if (c == '\'')
				{
					if (!isEqual)
					{
						this->parseError = 15;
						return false;
					}
					isQuote = '\'';
				}
				else
				{
					sb.AppendChar(c, 1);
				}
				this->parseOfst++;
			}
			this->parseError = 16;
			return false;
		}
		else if (lenLeft >= 2 && this->readBuff[this->parseOfst + 1] == '/')
		{
			this->nt = Text::XMLNode::NT_ELEMENTEND;
			Text::StringBuilderUTF8 sb;
			UTF8Char c;
			this->parseOfst += 2;
			while (true)
			{
				if (this->parseOfst >= this->buffSize)
				{
					this->parseOfst = 0;
					this->buffSize = 0;
					UOSInt readSize = this->FillBuffer();
					if (readSize <= 0)
					{
						this->parseError = 17;
						return false;
					}
					this->buffSize += readSize;
				}
				c = this->readBuff[this->parseOfst];
				if (c == ' ' || c == '\r' || c == '\n')
				{
					if (sb.GetLength() > 0)
					{
						if (this->nodeText == 0)
						{
							this->nodeText = Text::String::New(sb.ToString(), sb.GetLength());
						}
						else
						{
							this->parseError = 18;
							return false;
						}
						sb.ClearStr();
					}
				}
				else if (c == '>')
				{
					if (sb.GetLength() > 0)
					{
						if (this->nodeText == 0)
						{
							this->nodeText = Text::String::New(sb.ToString(), sb.GetLength());
						}
						else
						{
							this->parseError = 19;
							return false;
						}
					}
					if (this->nodeText == 0)
					{
						this->parseError = 20;
						return false;
					}
					this->parseOfst++;
					if (this->pathList->GetCount() == 0)
					{
						this->parseError = 21;
						return false;
					}
					if (this->pathList->GetItem(this->pathList->GetCount() - 1)->Equals(this->nodeText))
					{
						this->pathList->RemoveAt(this->pathList->GetCount() - 1)->Release();
						return true;
					}
					else if (this->mode == Text::XMLReader::PM_HTML && this->pathList->GetCount() >= 2 && this->pathList->GetItem(this->pathList->GetCount() - 2)->Equals(this->nodeText))
					{
						this->pathList->RemoveAt(this->pathList->GetCount() - 1)->Release();
						this->pathList->RemoveAt(this->pathList->GetCount() - 1)->Release();
						return true;
					}
					else
					{
						this->parseError = 22;
						return false;
					}
				}
				else
				{
					sb.AppendChar(c, 1);
				}
				this->parseOfst++;
			}
			this->parseError = 23;
			return false;
		}
		else
		{
			this->nt = Text::XMLNode::NT_ELEMENT;
			IO::MemoryStream mstm(128, UTF8STRC("Text.XMLReader.ReadNextType"));
			IO::MemoryStream mstmOri(128, UTF8STRC("Text.XMLReader.ReadNextType"));
			Bool isEqual = false;
			UTF8Char isQuote = 0;
			UTF8Char c;
			UInt8 b[4];
			this->parseOfst += 1;
			while (true)
			{
				if (this->parseOfst >= this->buffSize)
				{
					this->parseOfst = 0;
					this->buffSize = 0;
					UOSInt readSize = this->FillBuffer();
					if (readSize <= 0)
					{
						this->parseError = 24;
						return false;
					}
					this->buffSize += readSize;
				}
				c = this->readBuff[this->parseOfst];
				if (isQuote)
				{
					if (c == isQuote)
					{
						isQuote = 0;
						b[0] = c;
						mstmOri.Write(b, 1);

						if (this->nodeText == 0)
						{
							UOSInt len;
							UInt8 *buff = mstm.GetBuff(&len);
							this->nodeText = Text::String::New(buff, len);
							buff = mstmOri.GetBuff(&len);
							this->nodeOriText = Text::String::New(buff, len);
						}
						else if (isEqual)
						{
							Text::XMLAttrib *attr = this->attrList->GetItem(this->attrList->GetCount() - 1);
							SDEL_STRING(attr->value);
							if (this->enc && !this->stmEnc)
							{
								UOSInt mlen;
								UInt8 *buff = mstm.GetBuff(&mlen);
								UOSInt len = this->enc->CountUTF8Chars(buff, mlen);
								attr->value = Text::String::New(len);
								this->enc->UTF8FromBytes(attr->value->v, buff, mlen, 0);
								attr->value->v[len] = 0;

								buff = mstmOri.GetBuff(&mlen);
								len = this->enc->CountUTF8Chars(buff, mlen);
								attr->valueOri = Text::String::New(len);
								this->enc->UTF8FromBytes(attr->valueOri->v, buff, mlen, 0);
								attr->valueOri->v[len] = 0;
							}
							else
							{
								UOSInt len;
								UInt8 *buff = mstm.GetBuff(&len);
								attr->value = Text::String::New(buff, len);
								buff = mstmOri.GetBuff(&len);
								attr->valueOri = Text::String::New(buff, len);
							}
							isEqual = false;
						}
						else
						{
							Text::XMLAttrib *attr;
							UOSInt len;
							UInt8 *buff = mstm.GetBuff(&len);
							NEW_CLASS(attr, Text::XMLAttrib(buff, len, 0, 0));
							this->attrList->Add(attr);
						}
						mstm.Clear();
						mstmOri.Clear();
					}
					else if (c == '&')
					{
						UOSInt l = this->buffSize - this->parseOfst;
						if (l >= 4 && this->readBuff[this->parseOfst + 3] == ';')
						{
							mstmOri.Write(&this->readBuff[this->parseOfst], 4);
							if (this->mode == Text::XMLReader::PM_HTML && Text::XML::HTMLAppendCharRef(&this->readBuff[this->parseOfst], 4, &mstm))
							{
								this->parseOfst += 3;
							}
							else if (Text::StrStartsWith(&this->readBuff[this->parseOfst], (const UTF8Char*)"&lt;"))
							{
								mstm.Write((const UInt8*)"<", 1);
								this->parseOfst += 3;
							}
							else if (Text::StrStartsWith(&this->readBuff[this->parseOfst], (const UTF8Char*)"&gt;"))
							{
								mstm.Write((const UInt8*)">", 1);
								this->parseOfst += 3;
							}
							else if (this->readBuff[this->parseOfst + 1] == '#')
							{
								UTF8Char sbuff[6];
								UTF32Char wcs;
								this->readBuff[this->parseOfst + 3] = 0;
								wcs = (UTF32Char)Text::StrToInt32(&this->readBuff[this->parseOfst + 2]);
								sbuff[0] = (UTF8Char)wcs;
								mstm.Write(sbuff, 1);
								this->parseOfst += 3;
							}
							else
							{
								this->parseError = 25;
								return false;
							}
						}
						else if (l >= 5 && this->readBuff[this->parseOfst + 4] == ';')
						{
							mstmOri.Write(&this->readBuff[this->parseOfst], 5);
							if (this->mode == Text::XMLReader::PM_HTML && Text::XML::HTMLAppendCharRef(&this->readBuff[this->parseOfst], 5, &mstm))
							{
								this->parseOfst += 4;
							}
							else if (Text::StrStartsWith(&this->readBuff[this->parseOfst], (const UTF8Char*)"&amp;"))
							{
								mstm.Write((const UInt8*)"&", 1);
								this->parseOfst += 4;
							}
							else if (this->readBuff[this->parseOfst + 1] == '#')
							{
								UTF8Char sbuff[6];
								UTF32Char wcs;
								this->readBuff[this->parseOfst + 4] = 0;
								if (this->readBuff[this->parseOfst + 2] == 'x')
								{
									wcs = (UTF32Char)Text::StrHex2Int32C(&this->readBuff[this->parseOfst + 3]);
								}
								else
								{
									wcs = (UTF32Char)Text::StrToInt32(&this->readBuff[this->parseOfst + 2]);
								}
								sbuff[0] = (UTF8Char)wcs;
								mstm.Write(sbuff, 1);
								this->parseOfst += 4;
							}
							else
							{
								this->parseError = 26;
								return false;
							}
						}
						else if (l >= 6 && this->readBuff[this->parseOfst + 5] == ';')
						{
							mstmOri.Write(&this->readBuff[this->parseOfst], 6);
							if (this->mode == Text::XMLReader::PM_HTML && Text::XML::HTMLAppendCharRef(&this->readBuff[this->parseOfst], 6, &mstm))
							{
								this->parseOfst += 5;
							}
							else if (Text::StrStartsWith(&this->readBuff[this->parseOfst], (const UTF8Char*)"&quot;"))
							{
								mstm.Write((const UInt8*)"\"", 1);
								this->parseOfst += 5;
							}
							else if (Text::StrStartsWith(&this->readBuff[this->parseOfst], (const UTF8Char*)"&apos;"))
							{
								mstm.Write((const UInt8*)"\'", 1);
								this->parseOfst += 5;
							}
							else if (this->readBuff[this->parseOfst + 1] == '#')
							{
								UTF8Char sbuff[6];
								UTF32Char wcs;
								this->readBuff[this->parseOfst + 5] = 0;
								if (this->readBuff[this->parseOfst + 2] == 'x')
								{
									wcs = (UTF32Char)Text::StrHex2Int32C(&this->readBuff[this->parseOfst + 3]);
								}
								else
								{
									wcs = (UTF32Char)Text::StrToInt32(&this->readBuff[this->parseOfst + 2]);
								}
								sbuff[0] = (UTF8Char)wcs;
								mstm.Write(sbuff, 1);
								this->parseOfst += 5;
							}
							else
							{
								this->parseError = 27;
								return false;
							}
						}
						else if (this->mode == Text::XMLReader::PM_HTML)
						{
							if (l >= 7 && this->readBuff[this->parseOfst + 6] == ';' && Text::XML::HTMLAppendCharRef(&this->readBuff[this->parseOfst], 7, &mstm))
							{
								mstmOri.Write(&this->readBuff[this->parseOfst], 7);
								this->parseOfst += 6;
							}
							else if (l >= 8 && this->readBuff[this->parseOfst + 7] == ';' && Text::XML::HTMLAppendCharRef(&this->readBuff[this->parseOfst], 8, &mstm))
							{
								mstmOri.Write(&this->readBuff[this->parseOfst], 8);
								this->parseOfst += 7;
							}
							else
							{
								mstmOri.Write((const UInt8*)"&", 1);
								mstm.Write((const UInt8*)"&", 1);
							}
						}
						else
						{
							this->parseError = 28;
							return false;
						}
					}
					else
					{
						b[0] = c;
						mstm.Write(b, 1);
						mstmOri.Write(b, 1);
					}
				}
				else if (c == ' ' || c == '\r' || c == '\n' || c == '\t')
				{
					if (mstm.GetLength() > 0)
					{
						if (this->nodeText == 0)
						{
							UOSInt len;
							UInt8 *buff = mstm.GetBuff(&len);
							this->nodeText = Text::String::New(buff, len);
							buff = mstmOri.GetBuff(&len);
							this->nodeOriText = Text::String::New(buff, len);
						}
						else if (isEqual)
						{
							Text::XMLAttrib *attr = this->attrList->GetItem(this->attrList->GetCount() - 1);
							SDEL_STRING(attr->value);
							if (this->enc && !this->stmEnc)
							{
								UOSInt mlen;
								UInt8 *buff = mstm.GetBuff(&mlen);
								UOSInt len = this->enc->CountUTF8Chars(buff, mlen);
								attr->value = Text::String::New(len);
								this->enc->UTF8FromBytes(attr->value->v, buff, mlen, 0);
								attr->value->v[len] = 0;

								buff = mstmOri.GetBuff(&mlen);
								len = this->enc->CountUTF8Chars(buff, mlen);
								attr->valueOri = Text::String::New(len);
								this->enc->UTF8FromBytes(attr->valueOri->v, buff, mlen, 0);
								attr->valueOri->v[len] = 0;
							}
							else
							{
								UOSInt len;
								UInt8 *buff = mstm.GetBuff(&len);
								attr->value = Text::String::New(buff, len);
								buff = mstmOri.GetBuff(&len);
								attr->valueOri = Text::String::New(buff, len);
							}
							isEqual = false;
						}
						else
						{
							Text::XMLAttrib *attr;
							UOSInt len;
							UInt8 *buff = mstm.GetBuff(&len);
							NEW_CLASS(attr, Text::XMLAttrib(buff, len, 0, 0));
							this->attrList->Add(attr);
						}
						mstm.Clear();
						mstmOri.Clear();
					}
				}
				else if (c == '/')
				{
					if (mstm.GetLength() > 0)
					{
						if (this->nodeText == 0)
						{
							UOSInt len;
							UInt8 *buff = mstm.GetBuff(&len);
							this->nodeText = Text::String::New(buff, len);
							buff = mstmOri.GetBuff(&len);
							this->nodeOriText = Text::String::New(buff, len);
						}
						else if (isEqual)
						{
							Text::XMLAttrib *attr = this->attrList->GetItem(this->attrList->GetCount() - 1);
							SDEL_STRING(attr->value);
							if (this->enc && !this->stmEnc)
							{
								UOSInt mlen;
								UInt8 *buff = mstm.GetBuff(&mlen);
								UOSInt len = this->enc->CountUTF8Chars(buff, mlen);
								attr->value = Text::String::New(len);
								this->enc->UTF8FromBytes(attr->value->v, buff, mlen, 0);
								attr->value->v[len] = 0;

								buff = mstmOri.GetBuff(&mlen);
								len = this->enc->CountUTF8Chars(buff, mlen);
								attr->valueOri = Text::String::New(len);
								this->enc->UTF8FromBytes(attr->valueOri->v, buff, mlen, 0);
								attr->valueOri->v[len] = 0;
							}
							else
							{
								UOSInt len;
								UInt8 *buff = mstm.GetBuff(&len);
								attr->value = Text::String::New(buff, len);
								buff = mstmOri.GetBuff(&len);
								attr->valueOri = Text::String::New(buff, len);
							}
							isEqual = false;
						}
						else
						{
							Text::XMLAttrib *attr;
							UOSInt len;
							UInt8 *buff = mstm.GetBuff(&len);
							NEW_CLASS(attr, Text::XMLAttrib(buff, len, 0, 0));
							this->attrList->Add(attr);
						}
						mstm.Clear();
						mstmOri.Clear();
					}
					if (this->parseOfst + 1 >= this->buffSize)
					{
						this->parseOfst = 0;
						this->buffSize = 1;
						UOSInt readSize = this->FillBuffer();
						if (readSize <= 0)
						{
							this->parseError = 40;
							return false;
						}
						this->buffSize += readSize;
					}
					if (this->readBuff[this->parseOfst + 1] == '>')
					{
						this->parseOfst += 2;
						this->emptyNode = true;
						return this->nodeText != 0;
					}
					else
					{
						this->parseError = 29;
						return false;
					}
				}
				else if (c == '>')
				{
					if (mstm.GetLength() > 0)
					{
						if (this->nodeText == 0)
						{
							UOSInt len;
							UInt8 *buff = mstm.GetBuff(&len);
							this->nodeText = Text::String::New(buff, len);
							buff = mstmOri.GetBuff(&len);
							this->nodeOriText = Text::String::New(buff, len);
						}
						else if (isEqual)
						{
							Text::XMLAttrib *attr = this->attrList->GetItem(this->attrList->GetCount() - 1);
							SDEL_STRING(attr->value);
							if (this->enc && !this->stmEnc)
							{
								UOSInt mlen;
								UInt8 *buff = mstm.GetBuff(&mlen);
								UOSInt len = this->enc->CountUTF8Chars(buff, mlen);
								attr->value = Text::String::New(len);
								this->enc->UTF8FromBytes(attr->value->v, buff, mlen, 0);
								attr->value->v[len] = 0;

								buff = mstmOri.GetBuff(&mlen);
								len = this->enc->CountUTF8Chars(buff, mlen);
								attr->valueOri = Text::String::New(len);
								this->enc->UTF8FromBytes(attr->valueOri->v, buff, mlen, 0);
								attr->valueOri->v[len] = 0;
							}
							else
							{
								UOSInt len;
								UInt8 *buff = mstm.GetBuff(&len);
								attr->value = Text::String::New(buff, len);
								buff = mstmOri.GetBuff(&len);
								attr->valueOri = Text::String::New(buff, len);
							}
							isEqual = false;
						}
						else
						{
							Text::XMLAttrib *attr;
							UOSInt len;
							UInt8 *buff = mstm.GetBuff(&len);
							NEW_CLASS(attr, Text::XMLAttrib(buff, len, 0, 0));
							this->attrList->Add(attr);
						}
						mstm.Clear();
						mstmOri.Clear();
					}
					this->parseOfst += 1;
					this->emptyNode = false;
					return this->nodeText != 0;
				}
				else if (c == '=')
				{
					if (mstm.GetLength() > 0)
					{
						Text::XMLAttrib *attr;
						UOSInt len;
						UInt8 *buff = mstm.GetBuff(&len);
						NEW_CLASS(attr, Text::XMLAttrib(buff, len, 0, 0));
						this->attrList->Add(attr);
						mstm.Clear();
						mstmOri.Clear();
					}
					if (this->nodeText == 0)
					{
						this->parseError = 30;
						return false;
					}
					Text::XMLAttrib *attr = this->attrList->GetItem(this->attrList->GetCount() - 1);
					if (attr == 0)
					{
						this->parseError = 31;
						return false;
					}
					if (attr->value == 0 || attr->value->v[0] == 0)
					{
						isEqual = true;
					}
					else
					{
						this->parseError = 32;
						return false;
					}
				}
				else if (c == '"')
				{
					if (!isEqual)
					{
						this->parseError = 33;
						return false;
					}
					isQuote = '"';
					mstmOri.Write((const UInt8*)"\"", 1);
				}
				else if (c == '\'')
				{
					if (!isEqual)
					{
						this->parseError = 34;
						return false;
					}
					isQuote = '\'';
					mstmOri.Write((const UInt8*)"\'", 1);
				}
				else
				{
					b[0] = c;
					mstm.Write(b, 1);
					mstmOri.Write(b, 1);
				}
				this->parseOfst++;
			}
			this->parseError = 35;
			return false;
		}
	}
	else
	{
		IO::MemoryStream mstm(128, UTF8STRC("Text.XMLReader.ReadNextAny.mstm"));
		IO::MemoryStream mstmOri(128, UTF8STRC("Text.XMLReader.ReadNextAny.mstm"));
		UTF8Char c;
		UInt8 b[1];
		this->nt = Text::XMLNode::NT_TEXT;
		while (true)
		{
			if (this->parseOfst >= this->buffSize)
			{
				this->parseOfst = 0;
				this->buffSize = 0;
				UOSInt readSize = this->FillBuffer();
				if (readSize <= 0)
				{
					if (this->enc && !this->stmEnc)
					{
						UOSInt mlen;
						UInt8 *buff = mstm.GetBuff(&mlen);
						UOSInt len = this->enc->CountUTF8Chars(buff, mlen);
						this->nodeText = Text::String::New(len);
						this->enc->UTF8FromBytes(this->nodeText->v, buff, mlen, 0);
						this->nodeText->v[len] = 0;

						buff = mstmOri.GetBuff(&mlen);
						len = this->enc->CountUTF8Chars(buff, mlen);
						this->nodeOriText = Text::String::New(len);
						this->enc->UTF8FromBytes(this->nodeOriText->v, buff, mlen, 0);
						this->nodeOriText->v[len] = 0;
					}
					else
					{
						UOSInt mlen;
						UInt8 *buff = mstm.GetBuff(&mlen);
						this->nodeText = Text::String::New(buff, mlen);
						buff = mstmOri.GetBuff(&mlen);
						this->nodeOriText = Text::String::New(buff, mlen);
					}
					return true;
				}
				this->buffSize += readSize;
			}
			c = this->readBuff[this->parseOfst];
			if (c == '<')
			{
				if (isHTMLScript && !Text::StrStartsWith(&this->readBuff[this->parseOfst + 1], (const UTF8Char*)"/script>"))
				{
					b[0] = c;
					mstm.Write(b, 1);
					mstmOri.Write(b, 1);
				}
				else
				{
					if (this->enc && !this->stmEnc)
					{
						UOSInt mlen;
						UInt8 *buff = mstm.GetBuff(&mlen);
						UOSInt len = this->enc->CountUTF8Chars(buff, mlen);
						this->nodeText = Text::String::New(len);
						this->enc->UTF8FromBytes(this->nodeText->v, buff, mlen, 0);
						this->nodeText->v[len] = 0;

						buff = mstmOri.GetBuff(&mlen);
						len = this->enc->CountUTF8Chars(buff, mlen);
						this->nodeOriText = Text::String::New(len);
						this->enc->UTF8FromBytes(this->nodeOriText->v, buff, mlen, 0);
						this->nodeOriText->v[len] = 0;
					}
					else
					{
						UOSInt mlen;
						UInt8 *buff = mstm.GetBuff(&mlen);
						this->nodeText = Text::String::New(buff, mlen);
						buff = mstmOri.GetBuff(&mlen);
						this->nodeOriText = Text::String::New(buff, mlen);
					}
					return true;
				}
			}
			else if (c == '&' && !isHTMLScript)
			{
				UOSInt l = this->buffSize - this->parseOfst;
				if (l >= 4 && this->readBuff[this->parseOfst + 3] == ';')
				{
					if (Text::StrStartsWith(&this->readBuff[this->parseOfst], (const UTF8Char*)"&lt;"))
					{
						mstm.Write((const UInt8*)"<", 1);
						mstmOri.Write(&this->readBuff[this->parseOfst], 4);
						this->parseOfst += 3;
					}
					else if (Text::StrStartsWith(&this->readBuff[this->parseOfst], (const UTF8Char*)"&gt;"))
					{
						mstm.Write((const UInt8*)">", 1);
						mstmOri.Write(&this->readBuff[this->parseOfst], 4);
						this->parseOfst += 3;
					}
					else if (this->mode == Text::XMLReader::PM_HTML)
					{
						if (Text::XML::HTMLAppendCharRef(&this->readBuff[this->parseOfst], 4, &mstm))
						{
							mstmOri.Write(&this->readBuff[this->parseOfst], 4);
							this->parseOfst += 3;
						}
						else
						{
							mstm.Write((const UInt8*)"&", 1);
							mstmOri.Write(&this->readBuff[this->parseOfst], 1);
						}
					}
					else
					{
						this->parseError = 36;
						return false;
					}
				}
				else if (l >= 5 && this->readBuff[this->parseOfst + 4] == ';')
				{
					if (Text::StrStartsWith(&this->readBuff[this->parseOfst], (const UTF8Char*)"&amp;"))
					{
						mstm.Write((const UInt8*)"&", 1);
						mstmOri.Write(&this->readBuff[this->parseOfst], 5);
						this->parseOfst += 4;
					}
					else if (this->readBuff[this->parseOfst + 1] == '#')
					{
						b[0] = Text::StrHex2UInt8C(&this->readBuff[this->parseOfst + 2]);
						mstm.Write(b, 1);
						mstmOri.Write(&this->readBuff[this->parseOfst], 5);
						this->parseOfst += 4;
					}
					else if (this->mode == Text::XMLReader::PM_HTML)
					{
						if (Text::XML::HTMLAppendCharRef(&this->readBuff[this->parseOfst], 5, &mstm))
						{
							mstmOri.Write(&this->readBuff[this->parseOfst], 5);
							this->parseOfst += 4;
						}
						else
						{
							mstm.Write((const UInt8*)"&", 1);
							mstmOri.Write(&this->readBuff[this->parseOfst], 1);
						}
					}
					else
					{
						this->parseError = 37;
						return false;
					}
				}
				else if (l >= 6 && this->readBuff[this->parseOfst + 5] == ';')
				{
					if (Text::StrStartsWith(&this->readBuff[this->parseOfst], (const UTF8Char*)"&quot;"))
					{
						mstm.Write((const UInt8*)"\"", 1);
						mstmOri.Write(&this->readBuff[this->parseOfst], 6);
						this->parseOfst += 5;
					}
					else if (Text::StrStartsWith(&this->readBuff[this->parseOfst], (const UTF8Char*)"&apos;"))
					{
						mstm.Write((const UInt8*)"\'", 1);
						mstmOri.Write(&this->readBuff[this->parseOfst], 6);
						this->parseOfst += 5;
					}
					else if (this->mode == Text::XMLReader::PM_HTML)
					{
						if (Text::XML::HTMLAppendCharRef(&this->readBuff[this->parseOfst], 6, &mstm))
						{
							mstmOri.Write(&this->readBuff[this->parseOfst], 6);
							this->parseOfst += 5;
						}
						else
						{
							mstm.Write((const UInt8*)"&", 1);
							mstmOri.Write(&this->readBuff[this->parseOfst], 1);
						}
					}
					else
					{
						this->parseError = 38;
						return false;
					}
				}
				else if (this->mode == Text::XMLReader::PM_HTML)
				{
					mstm.Write((const UInt8*)"&", 1);
					mstmOri.Write((const UInt8*)"&", 1);
				}
				else
				{
					this->parseError = 39;
					return false;
				}
			}
			else
			{
				b[0] = c;
				mstm.Write(b, 1);
				mstmOri.Write(b, 1);
			}
			this->parseOfst++;
		}
	}
}

Bool Text::XMLReader::ReadNodeText(Text::StringBuilderUTF *sb)
{
	if (this->GetNodeType() == Text::XMLNode::NT_ELEMENT)
	{
		if (this->emptyNode)
		{
			return true;
		}
		UOSInt pathLev = this->pathList->GetCount();
		Text::XMLNode::NodeType nt;
		Bool succ = true;
		while ((succ = this->ReadNext()) != false)
		{
			nt = this->GetNodeType();
			if (nt == Text::XMLNode::NT_ELEMENTEND && pathLev == this->pathList->GetCount())
			{
				break;
			}
			else if (nt == Text::XMLNode::NT_TEXT)
			{
				sb->Append(this->GetNodeText());
			}
			else if (nt == Text::XMLNode::NT_CDATA)
			{
				sb->Append(this->GetNodeText());
			}
		}
		return succ;
	}
	else
	{
		return this->ReadNext();
	}
}

Bool Text::XMLReader::SkipElement()
{
	if (this->nt == Text::XMLNode::NT_ELEMENT)
	{
		if (this->emptyNode)
		{
			return true;
		}
		if (this->mode == PM_HTML)
		{
			if (this->IsHTMLSkip())
			{
				return true;
			}
		}
		UOSInt initLev = this->pathList->GetCount();
		Bool succ = true;
		while ((succ = this->ReadNext()) != false)
		{
			if (this->nt == Text::XMLNode::NT_ELEMENTEND && initLev >= this->pathList->GetCount())
			{
				break;
			}
		}
		return succ;
	}
	else
	{
		return this->ReadNext();
	}
}

Bool Text::XMLReader::IsElementEmpty()
{
	return this->nt == Text::XMLNode::NT_ELEMENT && this->emptyNode;
}

Bool Text::XMLReader::IsComplete()
{
	return this->pathList->GetCount() == 0 && this->parseOfst == this->buffSize;	
}

OSInt Text::XMLReader::GetErrorCode()
{
	return this->parseError;
}

Bool Text::XMLReader::ToString(Text::StringBuilderUTF *sb)
{
	UOSInt i;
	UOSInt j;
	Text::XMLAttrib *attr;
	switch (this->nt)
	{
	case Text::XMLNode::NT_DOCUMENT:
		sb->AppendChar('<', 1);
		sb->AppendChar('?', 1);
		sb->Append(this->nodeText);
		i = 0;
		j = this->attrList->GetCount();
		while (i < j)
		{
			attr = this->attrList->GetItem(i);
			sb->AppendChar(' ', 1);
			attr->ToString(sb);
			i++;
		}
		sb->AppendChar('?', 1);
		sb->AppendChar('>', 1);
		return true;
	case Text::XMLNode::NT_ELEMENT:
		sb->AppendChar('<', 1);
		sb->Append(this->nodeText);
		i = 0;
		j = this->attrList->GetCount();
		while (i < j)
		{
			attr = this->attrList->GetItem(i);
			sb->AppendChar(' ', 1);
			attr->ToString(sb);
			i++;
		}

		if (this->emptyNode)
		{
			sb->AppendChar('/', 1);
		}
		sb->AppendChar('>', 1);
		return true;
	case Text::XMLNode::NT_ELEMENTEND:
		sb->AppendChar('<', 1);
		sb->AppendChar('/', 1);
		sb->Append(this->nodeText);
		sb->AppendChar('>', 1);
		return true;
	case Text::XMLNode::NT_TEXT:
		if (this->nodeOriText)
		{
			sb->Append(this->nodeOriText);
		}
		else if (this->mode == Text::XMLReader::PM_XML)
		{
			Text::String *s = Text::XML::ToNewXMLText(this->nodeText->v);
			sb->Append(s);
			s->Release();
		}
		else
		{
			sb->Append(this->nodeText);
		}
		return true;
	case Text::XMLNode::NT_CDATA:
		sb->AppendC(UTF8STRC("<![CDATA["));
		sb->Append(this->nodeText);
		sb->AppendC(UTF8STRC("]]>"));
		return true;
	case Text::XMLNode::NT_COMMENT:
		sb->AppendC(UTF8STRC("<!--"));
		if (this->nodeText)
		{
			sb->Append(this->nodeText);
		}
		sb->AppendC(UTF8STRC("-->"));
		return true;
	case Text::XMLNode::NT_ATTRIBUTE:
	case Text::XMLNode::NT_UNKNOWN:
		break;
	case Text::XMLNode::NT_DOCTYPE:
		sb->AppendChar('<', 1);
		sb->AppendChar('!', 1);
		sb->Append(this->nodeText);
		i = 0;
		j = this->attrList->GetCount();
		while (i < j)
		{
			attr = this->attrList->GetItem(i);
			sb->AppendChar(' ', 1);
			attr->ToString(sb);
			i++;
		}

		if (this->emptyNode)
		{
			sb->AppendChar('/', 1);
		}
		sb->AppendChar('>', 1);
		return true;
	}
	return false;
}

Bool Text::XMLReader::XMLWellFormat(Text::EncodingFactory *encFact, IO::Stream *stm, UOSInt lev, Text::StringBuilderUTF *sb)
{
	Text::XMLReader *reader;
	Bool toWrite;
	Text::XMLNode::NodeType thisNT;
	Text::XMLNode::NodeType lastNT = Text::XMLNode::NT_UNKNOWN;
	NEW_CLASS(reader, Text::XMLReader(encFact, stm, Text::XMLReader::PM_XML));
	while (reader->ReadNext())
	{
		toWrite = true;
		thisNT = reader->GetNodeType();
		if (thisNT == Text::XMLNode::NT_TEXT)
		{
			toWrite = false;
			const UTF8Char *csptr = reader->GetNodeText()->v;
			UTF8Char c;
			while (true)
			{
				c = *csptr++;
				if (c == 0)
					break;
				if (c == '\t' || c == ' ' || c == '\r' || c == '\n')
				{

				}
				else
				{
					toWrite = true;
					break;
				}
			}
		}
		if (toWrite)
		{
			if (lastNT == Text::XMLNode::NT_ELEMENT && (thisNT == Text::XMLNode::NT_TEXT || thisNT == Text::XMLNode::NT_CDATA))
			{

			}
			else if (thisNT == Text::XMLNode::NT_ELEMENTEND && (lastNT == Text::XMLNode::NT_TEXT || lastNT == Text::XMLNode::NT_CDATA))
			{

			}
			else
			{
				if (lastNT == Text::XMLNode::NT_ELEMENT || lastNT == Text::XMLNode::NT_TEXT || lastNT == Text::XMLNode::NT_CDATA)
				{
					sb->AppendC(UTF8STRC("\r\n"));
				}
				sb->AppendChar('\t', reader->GetPathLev() + lev);
			}
			
			reader->ToString(sb);
			if (thisNT != Text::XMLNode::NT_ELEMENT && thisNT != Text::XMLNode::NT_TEXT && thisNT != Text::XMLNode::NT_CDATA)
			{
				sb->AppendC(UTF8STRC("\r\n"));
			}
			lastNT = thisNT;
		}
	}
	DEL_CLASS(reader);
	return true;
}
