#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/MemoryStream.h"
#include "Text/HTMLUtil.h"
#include "Text/JSText.h"
#include "Text/XMLReader.h"

Bool Text::HTMLUtil::HTMLWellFormat(Text::EncodingFactory *encFact, IO::Stream *stm, UOSInt lev, Text::StringBuilderUTF *sb)
{
	Text::XMLReader *reader;
	Text::XMLAttrib *attr;
	UOSInt i;
	UOSInt j;
	Bool toWrite;
	Text::XMLNode::NodeType thisNT;
	OSInt elementType = 0;
	const UTF8Char *csptr;
	UOSInt strLen;
	NEW_CLASS(reader, Text::XMLReader(encFact, stm, Text::XMLReader::PM_HTML));
	while (reader->ReadNext())
	{
		toWrite = true;
		thisNT = reader->GetNodeType();
		if (thisNT == Text::XMLNode::NT_TEXT)
		{
			toWrite = false;
			const UTF8Char *csptr = reader->GetNodeOriText();
			if (csptr == 0)
			{
				csptr = reader->GetNodeText();
			}
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
			if (thisNT == Text::XMLNode::NT_TEXT && elementType == 1) //Style
			{
				csptr = reader->GetNodeText();
				strLen = Text::StrCharCnt(csptr);
				CSSWellFormat(csptr, strLen, reader->GetPathLev(), sb);
			}
			else if (thisNT == Text::XMLNode::NT_TEXT && elementType == 2) //JavaScript
			{
				csptr = reader->GetNodeText();
				strLen = Text::StrCharCnt(csptr);
				Text::JSText::JSWellFormat(csptr, strLen, reader->GetPathLev(), sb);
				sb->Append((const UTF8Char*)"\r\n");
			}
			else if (thisNT == Text::XMLNode::NT_TEXT && elementType == 3) //JSON
			{
				csptr = reader->GetNodeText();
				strLen = Text::StrCharCnt(csptr);
				Text::JSText::JSONWellFormat(csptr, strLen, reader->GetPathLev(), sb);
				sb->Append((const UTF8Char*)"\r\n");
			}
			else if (thisNT == Text::XMLNode::NT_TEXT)
			{
				csptr = reader->GetNodeOriText();
				if (csptr == 0)
				{
					csptr = reader->GetNodeText();
				}
				UTF8Char c;
				while (true)
				{
					c = *csptr;
					if (c == 0)
						break;
					if (c == '\t' || c == ' ' || c == '\r' || c == '\n')
					{
						csptr++;
					}
					else
					{
						break;
					}
				}
				strLen = Text::StrCharCnt(csptr);
				if (strLen > 0)
				{
					while (true)
					{
						c = csptr[strLen - 1];
						if (c == '\t' || c == ' ' || c == '\r' || c == '\n')
						{
							strLen--;
						}
						else
						{
							break;
						}
					}
					sb->AppendChar('\t', reader->GetPathLev() + lev);
					sb->AppendC(csptr, strLen);
				}
				sb->Append((const UTF8Char*)"\r\n");
			}
			else
			{
				sb->AppendChar('\t', reader->GetPathLev() + lev);
				reader->ToString(sb);
				sb->Append((const UTF8Char*)"\r\n");
			}

			elementType = 0;
			if (thisNT == Text::XMLNode::NT_ELEMENT)
			{
				csptr = reader->GetNodeText();
				if (Text::StrEqualsICase(csptr, (const UTF8Char*)"STYLE"))
				{
					elementType = 1;
				}
				else if (Text::StrEqualsICase(csptr, (const UTF8Char*)"SCRIPT"))
				{
					elementType = 2;
					i = 0;
					j = reader->GetAttribCount();
					while (i < j)
					{
						attr = reader->GetAttrib(i);
						if (Text::StrEquals(attr->name, (const UTF8Char*)"type") && attr->value != 0)
						{
							if (Text::StrEquals(attr->value, (const UTF8Char*)"application/ld+json"))
							{
								elementType = 3;
							}
							break;
						}
						i++;
					}
				}
			}
		}
	}
	DEL_CLASS(reader);
	return true;
}

Bool Text::HTMLUtil::CSSWellFormat(const UInt8 *buff, UOSInt buffSize, UOSInt lev, Text::StringBuilderUTF *sb)
{
	UInt8 c;
	Bool lineStart = true;
	Bool lastIsWS = true;
	UOSInt currLev = 0;
	while (buffSize > 0)
	{
		c = *buff;
		if (c == ' ' || c == '\t' || c == '\r' || c == '\n')
		{
			if (!lastIsWS)
			{
				sb->AppendChar(' ', 1);
			}
			lastIsWS = true;
		}
		else if (c == '{')
		{
			if (!lastIsWS)
			{
				sb->AppendChar(' ', 1);
			}
			else if (lineStart)
			{
				sb->AppendChar('\t', lev + currLev);
			}
			sb->AppendChar('{', 1);
			sb->AppendChar('\r', 1);
			sb->AppendChar('\n', 1);
			currLev++;
			lineStart = true;
			lastIsWS = true;
		}
		else if (c == '}')
		{
			if (!lineStart)
			{
				sb->AppendChar('\r', 1);
				sb->AppendChar('\n', 1);
			}
			currLev--;
			sb->AppendChar('\t', lev + currLev);
			sb->AppendChar('}', 1);
			sb->AppendChar('\r', 1);
			sb->AppendChar('\n', 1);
			lineStart = true;
			lastIsWS = true;
		}
		else if (c == ';')
		{
			if (lineStart)
			{
				sb->AppendChar('\t', lev + currLev);
			}
			sb->AppendChar(';', 1);
			sb->AppendChar('\r', 1);
			sb->AppendChar('\n', 1);
			lineStart = true;
			lastIsWS = true;
		}
		else
		{
			if (lineStart)
			{
				lineStart = false;
				sb->AppendChar('\t', lev + currLev);
			}
			sb->AppendChar(c, 1);
			lastIsWS = false;
		}
		buff++;
		buffSize--;
	}
	return currLev == 0;
}

Bool Text::HTMLUtil::HTMLGetText(Text::EncodingFactory *encFact, const UInt8 *buff, UOSInt buffSize, Bool singleLine, Text::StringBuilderUTF *sb, Data::ArrayList<const UTF8Char *> *imgList)
{
	Text::XMLReader *reader;
	IO::MemoryStream *mstm;
	IO::MemoryStream *wmstm;
	UOSInt len;
	Text::XMLNode::NodeType nt;
	Int32 lastType = 0;
	Bool lastIsSpace = true;
	const UTF8Char *csptr;
	UTF8Char c;
	NEW_CLASS(wmstm, IO::MemoryStream((const UTF8Char*)"Text.HTMLUtil.HTMLGetText.wmstm"));
	NEW_CLASS(mstm, IO::MemoryStream((UInt8*)buff, buffSize, (const UTF8Char*)"Text.HTMLUtil.HTMLGetText.mstm"));
	NEW_CLASS(reader, Text::XMLReader(encFact, mstm, Text::XMLReader::PM_HTML));
	while (reader->ReadNext())
	{
		nt = reader->GetNodeType();
		if (nt == Text::XMLNode::NT_TEXT)
		{
			if (lastType == 0)
			{
				csptr = reader->GetNodeText();
				while (true)
				{
					c = *csptr;
					if (c == 0)
					{
						break;
					}
					else if (c == ' ' || c == '\t' || c == '\r' || c == '\n')
					{
						if (!lastIsSpace)
						{
							lastIsSpace = true;
							wmstm->Write((const UTF8Char*)" ", 1);
						}
					}
					else if (c == '&')
					{
						if (Text::StrStartsWith(csptr, (const UTF8Char*)"&nbsp;"))
						{
							if (singleLine)
							{
								if (!lastIsSpace)
								{
									lastIsSpace = true;
									wmstm->Write((const UTF8Char*)" ", 1);
								}
							}
							else
							{
								wmstm->Write((const UTF8Char*)" ", 1);
								lastIsSpace = true;
							}
							csptr += 5;
						}
						else
						{
							wmstm->Write((const UTF8Char*)"&", 1);
							lastIsSpace = false;
						}
					}
					else
					{
						wmstm->Write(csptr, 1);
						lastIsSpace = false;
					}
					csptr++;
				}
			}
		}
		else if (nt == Text::XMLNode::NT_ELEMENT)
		{
			csptr = reader->GetNodeText();
			if (Text::StrEqualsICase(csptr, (const UTF8Char*)"SCRIPT"))
			{
				lastType = 1;
			}
			else if (Text::StrEqualsICase(csptr, (const UTF8Char*)"STYLE"))
			{
				lastType = 2;
			}
			else if (Text::StrEqualsICase(csptr, (const UTF8Char*)"BR") || Text::StrEqualsICase(csptr, (const UTF8Char*)"P"))
			{
				if (singleLine)
				{
					if (!lastIsSpace)
					{
						wmstm->Write((const UTF8Char*)" ", 1);
					}					
				}
				else
				{
					wmstm->Write((const UTF8Char*)"\r\n", 2);
				}
				lastIsSpace = true;
				lastType = 0;
			}
			else if (Text::StrEqualsICase(csptr, (const UTF8Char*)"IMG"))
			{
				if (imgList)
				{
					UOSInt i = 0;
					UOSInt j = reader->GetAttribCount();
					Text::XMLAttrib *attr;
					while (i < j)
					{
						attr = reader->GetAttrib(i);
						if (Text::StrEqualsICase(attr->name, (const UTF8Char*)"SRC") && attr->value)
						{
							imgList->Add(Text::StrCopyNew(attr->value));
						}
						i++;
					}
				}
				lastType = 0;
			}
			else
			{
				lastType = 0;
			}
		}
		else
		{
			lastType = 0;
		}
	}
	DEL_CLASS(reader);
	DEL_CLASS(mstm);
	len = (UOSInt)wmstm->GetLength();
	sb->AppendC(wmstm->GetBuff(&len), len);
	DEL_CLASS(wmstm);
	return true;
}
