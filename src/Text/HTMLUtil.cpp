#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/MemoryReadingStream.h"
#include "IO/MemoryStream.h"
#include "Text/HTMLUtil.h"
#include "Text/JSText.h"
#include "Text/XMLReader.h"

Bool Text::HTMLUtil::HTMLWellFormat(Optional<Text::EncodingFactory> encFact, NN<IO::Stream> stm, UOSInt lev, NN<Text::StringBuilderUTF8> sb)
{
	NN<Text::XMLAttrib> attr;
	UOSInt i;
	UOSInt j;
	Bool toWrite;
	Text::XMLNode::NodeType thisNT;
	OSInt elementType = 0;
	UnsafeArray<const UTF8Char> csptr;
	NN<Text::String> s;
	UOSInt strLen;
	Text::XMLReader reader(encFact, stm, Text::XMLReader::PM_HTML);
	while (reader.ReadNext())
	{
		toWrite = true;
		thisNT = reader.GetNodeType();
		if (thisNT == Text::XMLNode::NodeType::Text)
		{
			toWrite = false;
			if (!s.Set(reader.GetNodeOriText()))
			{
				s = reader.GetNodeTextNN();
			}
			csptr = s->v;
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
			if (thisNT == Text::XMLNode::NodeType::Text && elementType == 1) //Style
			{
				s = reader.GetNodeTextNN();
				CSSWellFormat(s->v, s->leng, reader.GetPathLev(), sb);
			}
			else if (thisNT == Text::XMLNode::NodeType::Text && elementType == 2) //JavaScript
			{
				s = reader.GetNodeTextNN();
				Text::JSText::JSWellFormat(s->v, s->leng, reader.GetPathLev(), sb);
				sb->AppendC(UTF8STRC("\r\n"));
			}
			else if (thisNT == Text::XMLNode::NodeType::Text && elementType == 3) //JSON
			{
				s = reader.GetNodeTextNN();
				Text::JSText::JSONWellFormat(s->v, s->leng, reader.GetPathLev(), sb);
				sb->AppendC(UTF8STRC("\r\n"));
			}
			else if (thisNT == Text::XMLNode::NodeType::Text)
			{
				if (!s.Set(reader.GetNodeOriText()))
				{
					s = reader.GetNodeTextNN();
				}
				csptr = s->v;
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
				strLen = (UOSInt)(UnsafeArray<const UTF8Char>(s->v) + s->leng - csptr);
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
					sb->AppendChar('\t', reader.GetPathLev() + lev);
					sb->AppendC(csptr, strLen);
				}
				sb->AppendC(UTF8STRC("\r\n"));
			}
			else
			{
				sb->AppendChar('\t', reader.GetPathLev() + lev);
				reader.ToString(sb);
				sb->AppendC(UTF8STRC("\r\n"));
			}

			elementType = 0;
			if (thisNT == Text::XMLNode::NodeType::Element)
			{
				s = reader.GetNodeTextNN();
				if (s->EqualsICase(UTF8STRC("style")))
				{
					elementType = 1;
				}
				else if (s->EqualsICase(UTF8STRC("script")))
				{
					elementType = 2;
					i = 0;
					j = reader.GetAttribCount();
					while (i < j)
					{
						attr = reader.GetAttribNoCheck(i);
						if (attr->name->Equals(UTF8STRC("type")) && attr->value != 0)
						{
							if (attr->value->Equals(UTF8STRC("application/ld+json")))
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
	return true;
}

Bool Text::HTMLUtil::CSSWellFormat(UnsafeArray<const UInt8> buff, UOSInt buffSize, UOSInt lev, NN<Text::StringBuilderUTF8> sb)
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
				sb->AppendUTF8Char(' ');
			}
			lastIsWS = true;
		}
		else if (c == '{')
		{
			if (!lastIsWS)
			{
				sb->AppendUTF8Char(' ');
			}
			else if (lineStart)
			{
				sb->AppendChar('\t', lev + currLev);
			}
			sb->AppendUTF8Char('{');
			sb->AppendUTF8Char('\r');
			sb->AppendUTF8Char('\n');
			currLev++;
			lineStart = true;
			lastIsWS = true;
		}
		else if (c == '}')
		{
			if (!lineStart)
			{
				sb->AppendUTF8Char('\r');
				sb->AppendUTF8Char('\n');
			}
			currLev--;
			sb->AppendChar('\t', lev + currLev);
			sb->AppendUTF8Char('}');
			sb->AppendUTF8Char('\r');
			sb->AppendUTF8Char('\n');
			lineStart = true;
			lastIsWS = true;
		}
		else if (c == ';')
		{
			if (lineStart)
			{
				sb->AppendChar('\t', lev + currLev);
			}
			sb->AppendUTF8Char(';');
			sb->AppendUTF8Char('\r');
			sb->AppendUTF8Char('\n');
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
			sb->AppendUTF8Char(c);
			lastIsWS = false;
		}
		buff++;
		buffSize--;
	}
	return currLev == 0;
}

Bool Text::HTMLUtil::HTMLGetText(Optional<Text::EncodingFactory> encFact, UnsafeArray<const UInt8> buff, UOSInt buffSize, Bool singleLine, NN<Text::StringBuilderUTF8> sb, Data::ArrayListStringNN *imgList)
{
	UOSInt len;
	Text::XMLNode::NodeType nt;
	Int32 lastType = 0;
	Bool lastIsSpace = true;
	UnsafeArray<const UTF8Char> csptr;
	UnsafeArray<const UTF8Char> csptrEnd;
	NN<Text::String> s;
	UTF8Char c;
	IO::MemoryStream wmstm;
	IO::MemoryReadingStream mstm(buff, buffSize);
	Text::XMLReader reader(encFact, mstm, Text::XMLReader::PM_HTML);
	while (reader.ReadNext())
	{
		nt = reader.GetNodeType();
		if (nt == Text::XMLNode::NodeType::Text)
		{
			if (lastType == 0)
			{
				s = reader.GetNodeTextNN();
				csptr = s->v;
				csptrEnd = &s->v[s->leng];
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
							wmstm.Write(Data::ByteArrayR((const UInt8*)" ", 1));
						}
					}
					else if (c == '&')
					{
						if (Text::StrStartsWithC(csptr, (UOSInt)(csptrEnd - csptr), UTF8STRC("&nbsp;")))
						{
							if (singleLine)
							{
								if (!lastIsSpace)
								{
									lastIsSpace = true;
									wmstm.Write(Data::ByteArrayR((const UInt8*)" ", 1));
								}
							}
							else
							{
								wmstm.Write(Data::ByteArrayR((const UInt8*)" ", 1));
								lastIsSpace = true;
							}
							csptr += 5;
						}
						else
						{
							wmstm.Write(Data::ByteArrayR((const UInt8*)"&", 1));
							lastIsSpace = false;
						}
					}
					else
					{
						wmstm.Write(Data::ByteArrayR(csptr, 1));
						lastIsSpace = false;
					}
					csptr++;
				}
			}
		}
		else if (nt == Text::XMLNode::NodeType::Element)
		{
			s = reader.GetNodeTextNN();
			if (s->EqualsICase(UTF8STRC("SCRIPT")))
			{
				lastType = 1;
			}
			else if (s->EqualsICase(UTF8STRC("STYLE")))
			{
				lastType = 2;
			}
			else if (s->EqualsICase(UTF8STRC("BR")) || s->EqualsICase(UTF8STRC("P")))
			{
				if (singleLine)
				{
					if (!lastIsSpace)
					{
						wmstm.Write(Data::ByteArrayR((const UInt8*)" ", 1));
					}					
				}
				else
				{
					wmstm.Write(Data::ByteArrayR((const UInt8*)"\r\n", 2));
				}
				lastIsSpace = true;
				lastType = 0;
			}
			else if (s->EqualsICase(UTF8STRC("IMG")))
			{
				if (imgList)
				{
					UOSInt i = 0;
					UOSInt j = reader.GetAttribCount();
					NN<Text::XMLAttrib> attr;
					while (i < j)
					{
						attr = reader.GetAttribNoCheck(i);
						if (attr->name->EqualsICase(UTF8STRC("SRC")) && attr->value)
						{
							imgList->Add(attr->value->Clone());
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
	len = (UOSInt)wmstm.GetLength();
	sb->AppendC(wmstm.GetBuff(), len);
	return true;
}

Bool Text::HTMLUtil::XMLWellFormat(UnsafeArray<const UInt8> buff, UOSInt buffSize, UOSInt lev, NN<Text::StringBuilderUTF8> sb)
{
	UOSInt startOfst = 0;
	UOSInt currOfst = 0;
	UTF8Char lastC = 0;
	UTF8Char c;
	UInt8 startType = 0;
	while (currOfst < buffSize)
	{
		c = buff[currOfst];
		if (c == '<')
		{
			if (startType == 0)
			{
				if (startOfst < currOfst)
				{
					sb->AppendC(&buff[startOfst], currOfst - startOfst);
				}
				if (buff[currOfst + 1] == '?')
				{
					startOfst = currOfst;
					startType = 1;
				}
				else if (buff[currOfst + 1] == '/')
				{
					startOfst = currOfst;
					startType = 3;
				}
				else
				{
					startOfst = currOfst;
					startType = 2;
				}
			}
			else if (startType == 4 && buff[currOfst + 1] == '/')
			{
				startType = 5;
			}
		}
		else if (c == '>')
		{
			if (startType == 1)
			{
				if (lev > 0)
				{
					sb->AppendChar(' ', lev << 1);
				}
				sb->AppendC(&buff[startOfst], currOfst - startOfst + 1);
				sb->AppendC(UTF8STRC("\r\n"));
				startType = 0;
				startOfst = currOfst + 1;
			}
			else if (startType == 2)
			{
				if (lastC == '/')
				{
					if (lev > 0)
					{
						sb->AppendChar(' ', lev << 1);
					}
					sb->AppendC(&buff[startOfst], currOfst - startOfst + 1);
					sb->AppendC(UTF8STRC("\r\n"));
					startType = 0;
					startOfst = currOfst + 1;
				}
				else if (buff[currOfst + 1] == '<' && buff[currOfst + 2] == '/')
				{
					startType = 5;
				}
				else if (buff[currOfst + 1] == '<')
				{
					if (lev > 0)
					{
						sb->AppendChar(' ', lev << 1);
					}
					lev++;
					sb->AppendC(&buff[startOfst], currOfst - startOfst + 1);
					sb->AppendC(UTF8STRC("\r\n"));
					startType = 0;
					startOfst = currOfst + 1;
				}
				else
				{
					startType = 4;
				}
			}
			else if (startType == 5)
			{
				if (lev > 0)
				{
					sb->AppendChar(' ', lev << 1);
				}
				sb->AppendC(&buff[startOfst], currOfst - startOfst + 1);
				sb->AppendC(UTF8STRC("\r\n"));
				startType = 0;
				startOfst = currOfst + 1;
			}
			else if (startType == 3)
			{
				if (lev > 0)
				{
					lev--;
					if (lev > 0)
					{
						sb->AppendChar(' ', lev << 1);
					}
				}
				sb->AppendC(&buff[startOfst], currOfst - startOfst + 1);
				sb->AppendC(UTF8STRC("\r\n"));
				startType = 0;
				startOfst = currOfst + 1;
			}
		}

		currOfst++;
		lastC = c;
	}
	if (startOfst < buffSize)
	{
		sb->AppendC(&buff[startOfst], buffSize - startOfst);
	}
	return true;
}
