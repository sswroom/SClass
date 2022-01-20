#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayList.h"
#include "IO/MemoryStream.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/XML.h"
#include "Text/XMLDOM.h"

Text::XMLAttrib::XMLAttrib(const UTF8Char *name, UOSInt nameLen, const UTF8Char *value, UOSInt valueLen) : XMLNode(Text::XMLNode::NT_ATTRIBUTE)
{
	this->name = 0;
	this->value = 0;
	this->valueOri = 0;
	if (nameLen > 0)
	{
		this->name = Text::String::New(nameLen);
		Text::XML::ParseStr(this->name, name, name + nameLen);
	}
	if (valueLen > 0)
	{
		this->value = Text::String::New(valueLen);
		Text::XML::ParseStr(this->value, value, value + valueLen);
		this->valueOri = Text::String::New(value, valueLen);
	}
}

Text::XMLAttrib::~XMLAttrib()
{
	SDEL_STRING(this->name);
	SDEL_STRING(this->value);
	SDEL_STRING(this->valueOri);
}

Bool Text::XMLAttrib::ToString(Text::StringBuilderUTF *sb)
{
	sb->Append(this->name);
	if (this->value)
	{
		sb->AppendChar('=', 1);
		if (this->valueOri)
		{
			sb->Append(this->valueOri);
		}
		else
		{
			Text::String *s = Text::XML::ToNewAttrText(this->value->v);
			sb->Append(s);
			s->Release();
		}
	}
	return true;
}

Text::XMLNode::XMLNode(NodeType nt)
{
	this->nt = nt;
	this->name = 0;
	this->value = 0;
	this->valueOri = 0;
	this->attribArr = 0;
	this->childArr = 0;
}

Text::XMLNode::~XMLNode()
{
	UOSInt i;
	XMLAttrib *attr;
	XMLNode *node;

	SDEL_STRING(this->name);
	SDEL_STRING(this->value);
	SDEL_STRING(this->valueOri);
	if (this->attribArr)
	{
		i = attribArr->GetCount();
		while (i-- > 0)
		{
			attr = attribArr->GetItem(i);
			DEL_CLASS(attr);
		}

		DEL_CLASS(this->attribArr);
		this->attribArr = 0;
	}
	if (this->childArr)
	{
		i = childArr->GetCount();
		while (i-- > 0)
		{
			node = childArr->GetItem(i);
			DEL_CLASS(node);
		}

		DEL_CLASS(this->childArr);
		this->childArr = 0;
	}
}

Text::XMLNode::NodeType Text::XMLNode::GetNodeType()
{
	return nt;
}

void Text::XMLNode::AddAttrib(XMLAttrib *attr)
{
	if (this->attribArr == 0)
	{
		NEW_CLASS(this->attribArr, Data::ArrayList<XMLAttrib*>());
	}
	this->attribArr->Add(attr);
}

void Text::XMLNode::AddChild(XMLNode *node)
{
	if (this->childArr == 0)
	{
		NEW_CLASS(this->childArr, Data::ArrayList<XMLNode*>());
	}
	this->childArr->Add(node);
}

UOSInt Text::XMLNode::GetAttribCnt()
{
	if (this->attribArr == 0)
		return 0;
	return this->attribArr->GetCount();
}

Text::XMLAttrib *Text::XMLNode::GetAttrib(UOSInt index)
{
	if (this->attribArr == 0)
		return 0;
	return this->attribArr->GetItem(index);
}

Text::XMLAttrib *Text::XMLNode::GetFirstAttrib(const UTF8Char *attrName, UOSInt nameLen)
{
	if (this->attribArr == 0)
		return 0;
	Text::XMLAttrib *attr;
	UOSInt i = 0;
	UOSInt cnt = this->attribArr->GetCount();
	while (i < cnt)
	{
		attr = this->attribArr->GetItem(i++);
		if (attr->name->Equals(attrName, nameLen))
			return attr;
	}
	return 0;
}

UOSInt Text::XMLNode::GetChildCnt()
{
	if (this->childArr == 0)
		return 0;
	return this->childArr->GetCount();
}

Text::XMLNode *Text::XMLNode::GetChild(UOSInt index)
{
	if (this->childArr == 0)
		return 0;
	return this->childArr->GetItem(index);
}

void Text::XMLNode::GetInnerXML(Text::StringBuilderUTF *sb)
{
	Text::XMLNode *n;
	Text::XMLAttrib *attr;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UOSInt l;
	if (this->childArr == 0)
		return;
	if (this->nt == Text::XMLNode::NT_DOCUMENT)
	{
		((Text::XMLDocument*)this)->AppendXmlDeclaration(sb);
	}
	i = 0;
	j = this->childArr->GetCount();
	while (i < j)
	{
		n = (Text::XMLNode*)this->childArr->GetItem(i);
		if (n->nt == Text::XMLNode::NT_COMMENT)
		{
			sb->AppendC(UTF8STRC("<!--"));
			sb->Append(n->value);
			sb->AppendC(UTF8STRC("-->"));
		}
		else if (n->nt == Text::XMLNode::NT_TEXT)
		{
			sb->Append(n->value);
		}
		else if (n->nt == Text::XMLNode::NT_CDATA)
		{
			sb->AppendC(UTF8STRC("<![CDATA["));
			sb->Append(n->value);
			sb->AppendC(UTF8STRC("]]>"));
		}
		else if (n->nt == Text::XMLNode::NT_ELEMENT)
		{
			sb->AppendC(UTF8STRC("<"));
			sb->Append(n->name);
			if (n->attribArr)
			{
				k = 0;
				l = n->attribArr->GetCount();
				while (k < l)
				{
					attr = (Text::XMLAttrib*)n->attribArr->GetItem(k);
					sb->AppendC(UTF8STRC(" "));
					sb->Append(attr->name);
					if (attr->value)
					{
						sb->AppendC(UTF8STRC("=\""));
						sb->Append(attr->value);
						sb->AppendC(UTF8STRC("\""));
					}
					k++;
				}
			}
			if (n->childArr)
			{
				sb->AppendC(UTF8STRC(">"));
				n->GetInnerXML(sb);
				sb->AppendC(UTF8STRC("</"));
				sb->Append(n->name);
				sb->AppendC(UTF8STRC(">"));
			}
			else
			{
				sb->AppendC(UTF8STRC("/>"));
			}
		}
		i++;
	}
}

void Text::XMLNode::GetInnerText(Text::StringBuilderUTF *sb)
{
	Text::XMLNode *n;
	UOSInt i;
	UOSInt j;
	if (this->nt == Text::XMLNode::NT_TEXT)
	{
		sb->Append(this->value);
		return;
	}
	else if (this->nt == Text::XMLNode::NT_CDATA)
	{
		sb->Append(this->value);
		return;
	}
	else if (this->childArr == 0)
	{
		return;
	}
	else
	{
		i = 0;
		j = this->childArr->GetCount();
		while (i < j)
		{
			n = (Text::XMLNode*)this->childArr->GetItem(i);
			n->GetInnerText(sb);
			i++;
		}
	}
}

Text::XMLNode **Text::XMLNode::SearchNode(const UTF8Char *path, UOSInt *cnt)
{
	Data::ArrayList<XMLNode*> *outArr;
	Text::XMLNode **outs;
	UOSInt i;
	NEW_CLASS(outArr, Data::ArrayList<XMLNode*>());
	SearchNodeBegin(path, outArr, false);

	outs = MemAlloc(XMLNode*, outArr->GetCount());
	*cnt = i = outArr->GetCount();
	while (i-- > 0)
	{
		outs[i] = (XMLNode*)outArr->GetItem(i);
	}
	DEL_CLASS(outArr);
	return outs;
}

Text::XMLNode *Text::XMLNode::SearchFirstNode(const UTF8Char *path)
{
	Data::ArrayList<XMLNode*> *outArr;
	NEW_CLASS(outArr, Data::ArrayList<XMLNode*>());
	SearchNodeBegin(path, outArr, true);
	Text::XMLNode *node = outArr->GetItem(0);
	DEL_CLASS(outArr);
	return node;
}

void Text::XMLNode::ReleaseSearch(XMLNode **searchResult)
{
	MemFree(searchResult);
}					

void Text::XMLNode::SearchNodeBegin(const UTF8Char *path, Data::ArrayList<XMLNode*> *outArr, Bool singleResult)
{
	UTF8Char myPath[256];
	UOSInt i;
	int searchType;
	Data::ArrayList<UTF8Char*> *reqArr;
	Data::ArrayList<XMLNode*> *currPathArr;
	UTF8Char *src;
	Text::StrConcat(myPath, path);

	NEW_CLASS(reqArr, Data::ArrayList<UTF8Char*>());
	NEW_CLASS(currPathArr, Data::ArrayList<XMLNode*>());

	src = myPath;
	while (true)
	{
		if (*src != '/')
			break;
		if (src[1] == '/')
		{
			searchType = 0;
			src += 1;
		}
		else
		{
			searchType = 1;
		}
		
		reqArr->Clear();
		currPathArr->Clear();
		while (true)
		{
			if (*src == '/')
			{
				reqArr->Add(&src[1]);
				*src++ = 0;
			}
			else if (*src == '|')
			{
				*src = 0;
				if (searchType == 0)
				{
					SearchNodeSubElement(this, reqArr, currPathArr, outArr, searchType, singleResult);
				}
				else
				{
					SearchNodeSub(this, reqArr, currPathArr, outArr, searchType, singleResult);
				}
				*src = '|';

				i = reqArr->GetCount();
				while (i-- > 0)
				{
					((WChar*)reqArr->GetItem(i))[-1] = '/';
				}
				src++;
				if (*src != '/')
					break;
				if (src[1] == '/')
				{
					searchType = 0;
					src += 1;
				}
				else
				{
					searchType = 1;
				}
			}
			else if (*src == 0)
			{
				if (searchType == 0)
				{
					SearchNodeSubElement(this, reqArr, currPathArr, outArr, searchType, singleResult);
				}
				else
				{
					SearchNodeSub(this, reqArr, currPathArr, outArr, searchType, singleResult);
				}

				i = reqArr->GetCount();
				while (i-- > 0)
				{
					((WChar*)reqArr->GetItem(i))[-1] = '/';
				}
				break;
			}
			else
			{
				src++;
			}
		}
	}
	

	DEL_CLASS(reqArr);
	DEL_CLASS(currPathArr);
}

Bool Text::XMLNode::SearchNodeSub(XMLNode *node, Data::ArrayList<UTF8Char*> *reqArr, Data::ArrayList<XMLNode*> *currPathArr, Data::ArrayList<XMLNode*> *outArr, Int32 searchType, Bool singleResult)
{
	XMLNode *n;
	UTF8Char *reqStr;
	UOSInt i;
	UOSInt j;
	UOSInt k;

	if (reqArr->GetCount() <= currPathArr->GetCount())
	{
		return false;
	}
	reqStr = reqArr->GetItem(currPathArr->GetCount());
	j = currPathArr->GetCount();
	if (reqStr[0] != '@' && node->childArr)
	{
		i = node->childArr->GetCount();
		while (i-- > 0)
		{
			n = node->childArr->GetItem(i);
			if (n->nt == XMLNode::NT_ELEMENT)
			{
				currPathArr->Add(n);
				if (currPathArr->GetCount() == reqArr->GetCount())
				{
					if (SearchEqual(currPathArr->GetCount() - 1, reqArr, currPathArr))
					{
						outArr->Add(n);
						if (singleResult)
							return true;
					}
				}
				else
				{
					if (SearchEqual(currPathArr->GetCount() - 1, reqArr, currPathArr))
					{
						if (SearchNodeSub(n, reqArr, currPathArr, outArr, searchType, singleResult) && singleResult)
							return true;
					}
				}
				currPathArr->RemoveAt(j);
			}
		}
	}
	if (reqStr[0] == '@' && node->attribArr)
	{
		i = node->attribArr->GetCount();
		while (i-- > 0)
		{
			n = node->attribArr->GetItem(i);
			
			currPathArr->Add(n);
			if (currPathArr->GetCount() == reqArr->GetCount())
			{
				Bool isEqual = true;
				k = currPathArr->GetCount();
				while (k-- > 0 && isEqual)
				{
					isEqual = SearchEqual(k, reqArr, currPathArr);
				}
				if (isEqual)
				{
					outArr->Add(n);
					if (singleResult)
						return true;
				}
			}
			else
			{
				if (SearchNodeSub(n, reqArr, currPathArr, outArr, searchType, singleResult) && singleResult)
					return true;
			}
			currPathArr->RemoveAt(j);
		}
	}
	return false;
}

Bool Text::XMLNode::SearchNodeSubElement(XMLNode *node, Data::ArrayList<UTF8Char*> *reqArr, Data::ArrayList<XMLNode*> *currPathArr, Data::ArrayList<XMLNode*> *outArr, Int32 searchType, Bool singleResult)
{
	if (SearchNodeSub(node, reqArr, currPathArr, outArr, searchType, singleResult) && singleResult)
		return true;
	UOSInt i;
	Text::XMLNode *n;
	i = node->GetChildCnt();
	while (i-- > 0)
	{
		n = node->GetChild(i);
		if (n->GetNodeType() == Text::XMLNode::NT_ELEMENT)
		{
			if (SearchNodeSubElement(n, reqArr, currPathArr, outArr, searchType, singleResult) && singleResult)
				return true;
		}
	}
	return false;
}

Bool Text::XMLNode::SearchEqual(UOSInt level, Data::ArrayList<UTF8Char*> *reqArr, Data::ArrayList<XMLNode*> *currPathArr)
{
	UTF8Char nameBuff[128];
	UTF8Char condBuff[128];
	Bool hasCond = false;
	UTF8Char *req = reqArr->GetItem(level);
	UTF8Char *src;
	UTF8Char *dest;
	XMLNode *n;
	src = req;
	dest = nameBuff;
	while (*src)
	{
		if (*src == '[')
		{
			*dest = 0;
			hasCond = true;
			dest = condBuff;
			src++;
		}
		else if (*src == ']')
		{
			break;
		}
		else
		{
			*dest++ = *src++;
		}
	}
	*dest = 0;

	n = (XMLNode*)currPathArr->GetItem(level);
	src = n->name->v;
	dest = nameBuff;
	if (*dest == '@')
	{
		if (n->GetNodeType() != Text::XMLNode::NT_ATTRIBUTE)
			return false;
		dest++;
	}
	else
	{
		if (n->GetNodeType() != Text::XMLNode::NT_ELEMENT)
			return false;
	}
	while (*src && *dest)
	{
		if (*src++ != *dest++)
			return false;
	}
	if (*src != *dest)
		return false;

	if (hasCond)
	{
		src = &condBuff[0];
		while (true)
		{
			if (*src == '=')
			{
				Text::StringBuilderUTF8 leftSB;
				Text::StringBuilderUTF8 rightSB;
				/////////////////////////////////////////
				if (SearchEval(level, reqArr, currPathArr, n, condBuff, src, &leftSB) == false)
					return false;

				dest = src;
				while (*dest++);
				dest--;
				if (SearchEval(level, reqArr, currPathArr, n, src + 1, dest, &rightSB) == false)
					return false;

				return Text::StrEqualsC(leftSB.ToString(), leftSB.GetLength(), rightSB.ToString(), rightSB.GetLength());
			}
			else if (*src == 0)
			{
				if (condBuff[0] == '@')
				{
					///////////////////////////////////////////////
				}
				else
				{
					return false;
				}
			}
			else if (*src == '>')
			{
				///////////////////////////////
				return false;
			}
			else if (*src == '<')
			{
				//////////////////////////////////
				return false;
			}
			src++;
		}
	}
	return true;
}

Bool Text::XMLNode::SearchEval(UOSInt level, Data::ArrayList<UTF8Char*> *reqArr, Data::ArrayList<XMLNode*> *currPathArr, Text::XMLNode *n, const UTF8Char *nameStart, const UTF8Char *nameEnd, Text::StringBuilderUTF *outSB)
{
	const UTF8Char *src;
	const UTF8Char *dest;
	XMLAttrib *attr;
	XMLNode *child;
	UOSInt i;
	Bool eq;

	if (*nameStart == '@')
	{
		i = n->attribArr->GetCount();
		while (i-- > 0)
		{
			attr = n->attribArr->GetItem(i);
			src = &nameStart[1];
			dest = attr->name->v;
			eq = true;
			while (src < nameEnd)
			{
				if (*src++ != *dest++)
				{
					eq = false;
					break;
				}
			}
			if (eq)
			{
				outSB->Append(attr->value);
				return true;
			}
		}
		return false;
	}
	else if (*nameStart == '\'' && nameEnd[-1] == '\'')
	{
		outSB->AppendC(&nameStart[1], (UOSInt)(nameEnd - nameStart - 2));
		return true;
	}
	else
	{
		i = n->childArr->GetCount();
		while (i-- > 0)
		{
			child = n->childArr->GetItem(i);
			if (child->nt == Text::XMLNode::NT_ELEMENT)
			{
				src = nameStart;
				dest = child->name->v;
				eq = true;
				while (src < nameEnd)
				{
					if (*src++ != *dest++)
					{
						eq = false;
						break;
					}
				}
				if (eq && *dest == 0)
				{
					child->GetInnerXML(outSB);
					return true;
				}
			}
		}
		return false;
	}
}

const UTF8Char *Text::XMLNode::NodeTypeGetName(NodeType ntype)
{
	switch (ntype)
	{
	case NT_ELEMENT:
		return (const UTF8Char*)"Element";
	case NT_TEXT:
		return (const UTF8Char*)"Text";
	case NT_DOCUMENT:
		return (const UTF8Char*)"Document";
	case NT_COMMENT:
		return (const UTF8Char*)"Comment";
	case NT_ATTRIBUTE:
		return (const UTF8Char*)"Attribute";
	case NT_CDATA:
		return (const UTF8Char*)"CDATA";
	case NT_ELEMENTEND:
		return (const UTF8Char*)"Element End";
	case NT_DOCTYPE:
		return (const UTF8Char*)"DocType";
	case NT_UNKNOWN:
	default:
		return (const UTF8Char*)"Unknown";
	}

}

UTF8Char *Text::XMLDocument::ParseNode(XMLNode *parentNode, UTF8Char *xmlStart, UTF8Char *xmlEnd)
{
	UTF8Char *currPtr;
	UTF8Char c;
	XMLNode *node;
	XMLAttrib *attr;


	Bool lastSpace;
	Bool lastEq;
	UTF8Char quoted;
	UTF8Char *xmlNameSt = 0;
	UTF8Char *xmlNameEn = 0;
	UTF8Char *xmlValSt = 0;
	UTF8Char *xmlValEn;

	if (*xmlStart == '<')
	{
		if (xmlStart[1] == '?')
		{
			lastSpace = true;
			lastEq = false;
			quoted = 0;
			currPtr = &xmlStart[2];
			while (true)
			{
				if (currPtr >= xmlEnd)
					return xmlEnd;
				c = *currPtr;
				if (c == '>')
				{
					return currPtr + 1;
				}
				if (quoted != 0)
				{
					if (c == quoted)
					{
						xmlValEn = currPtr;
						if (Text::StrEqualsICaseC(UTF8STRC("ENCODING"), xmlNameSt, (UOSInt)(xmlNameEn - xmlNameSt)))
						{
							SDEL_STRING(this->encoding);
							this->encoding = Text::String::New((UOSInt)(xmlValEn - xmlValSt));
							Text::XML::ParseStr(this->encoding, xmlValSt, xmlValEn);
						}
						else if (Text::StrEqualsICaseC(UTF8STRC("VERSION"), xmlNameSt, (UOSInt)(xmlNameEn - xmlNameSt)))
						{
							SDEL_STRING(this->version);
							this->version = Text::String::New((UOSInt)(xmlValEn - xmlValSt));
							Text::XML::ParseStr(this->version, xmlValSt, xmlValEn);
						}

						xmlNameSt = 0;
						xmlValSt = 0;
						quoted = 0;
						lastSpace = true;
					}
					else
					{
						if (lastSpace)
						{
							xmlValSt = currPtr;
							xmlValEn = 0;
							lastSpace = false;
							lastEq = false;
						}
					}
				}
				else if (c == '"')
				{
					if (lastEq)
					{
						quoted = '"';
						lastSpace = true;
					}
					else
					{
						////////////////////////
						return 0;
					}
				}
				else if (c == '\'')
				{
					if (lastEq)
					{
						quoted = '\'';
						lastSpace = true;
					}
					else
					{
						////////////////////////
						return 0;
					}
				}
				else if (c == ' ' || c == '\r' || c == '\n' || c == '\t')
				{
					if (xmlValSt)
					{
						xmlValEn = currPtr;
						if (Text::StrEqualsICaseC(UTF8STRC("ENCODING"), xmlNameSt, (UOSInt)(xmlNameEn - xmlNameSt)))
						{
							SDEL_STRING(this->encoding);
							this->encoding = Text::String::New((UOSInt)(xmlValEn - xmlValSt));
							Text::XML::ParseStr(this->encoding, xmlValSt, xmlValEn);
						}
						else if (Text::StrEqualsICaseC(UTF8STRC("VERSION"), xmlNameSt, (UOSInt)(xmlNameEn - xmlNameSt)))
						{
							SDEL_STRING(this->version);
							this->version = Text::String::New((UOSInt)(xmlValEn - xmlValSt));
							Text::XML::ParseStr(this->version, xmlValSt, xmlValEn);
						}

						xmlNameSt = 0;
						xmlValSt = 0;
					}

					lastSpace = true;
					xmlNameSt = 0;
				}
				else if (c == '=')
				{
					if (xmlNameSt)
					{
						if (xmlNameEn == 0)
						{
							xmlNameEn = currPtr;
						}
					}
					lastEq = true;
				}
				else if (lastSpace)
				{
					if (quoted)
					{
						xmlValSt = currPtr;
						xmlValEn = 0;
					}
					else
					{
						xmlNameSt = currPtr;
						xmlNameEn = 0;
						xmlValSt = 0;
						xmlValEn = 0;
					}
					lastSpace = false;
					lastEq = false;
				}
				else if (lastEq)
				{
					xmlValSt = currPtr;
					xmlValEn = 0;
					lastSpace = false;
					lastEq = false;
				}
				currPtr++;
			}
		}
		else if (xmlStart[1] == '!' && xmlStart[2] == '-' && xmlStart[3] == '-')
		{
			currPtr = xmlStart + 4;
			while (currPtr < xmlEnd)
			{
				c = *currPtr;
				if (c == '-')
				{
					if (currPtr[1] == '-' && currPtr[2] == '>')
					{
						NEW_CLASS(node, XMLNode(XMLNode::NT_COMMENT));
						node->value = Text::String::New(xmlStart + 4, (UOSInt)(currPtr - xmlStart - 4));
						parentNode->AddChild(node);
						
						return xmlEnd + 3;
					}
				}
				currPtr++;
			}

			//////////////////////////////// File Error /////////////////////////////////////////////////
			NEW_CLASS(node, XMLNode(XMLNode::NT_TEXT));
			node->name = Text::String::New((UOSInt)(currPtr - xmlStart));
			Text::XML::ParseStr(node->name, xmlStart, currPtr);
			this->AddChild(node);
			
			return xmlEnd;
		}
		else if (xmlStart[1] == '!' && xmlStart[2] == '[' && xmlStart[3] == 'C' && xmlStart[4] == 'D' && xmlStart[5] == 'A' && xmlStart[6] == 'T' && xmlStart[7] == 'A' && xmlStart[8] == '[')
		{
			currPtr = xmlStart + 9;
			while (currPtr < xmlEnd)
			{
				c = *currPtr;
				if (c == ']')
				{
					if (currPtr[1] == ']' && currPtr[2] == '>')
					{
						NEW_CLASS(node, XMLNode(XMLNode::NT_CDATA));
						node->value = Text::String::New(xmlStart + 9, (UOSInt)(currPtr - xmlStart - 9));
						parentNode->AddChild(node);
						return xmlEnd + 3;
					}
				}
				currPtr++;
			}

			//////////////////////////////// File Error /////////////////////////////////////////////////
			NEW_CLASS(node, XMLNode(XMLNode::NT_TEXT));
			node->name = Text::String::New((UOSInt)(currPtr - xmlStart));
			Text::XML::ParseStr(node->name, xmlStart, currPtr);
			this->AddChild(node);
			
			return xmlEnd;
		}
		else
		{
			lastEq = false;
			lastSpace = true;
			quoted = 0;
			currPtr = &xmlStart[1];
			node = 0;

			while (true)
			{
				if (currPtr >= xmlEnd)
				{
					if (node)
						parentNode->AddChild(node);
					return xmlEnd;
				}

				c = *currPtr;
				if (!quoted && c == '/' && (node != 0 || xmlNameSt != 0))
				{
					if (node == 0)
					{
						NEW_CLASS(node, XMLNode(XMLNode::NT_ELEMENT));
						node->name = Text::String::New((UOSInt)(currPtr - xmlNameSt));
						Text::XML::ParseStr(node->name, xmlNameSt, currPtr);
					}
					else if (xmlNameSt != 0)
					{
						if (xmlValSt == 0)
						{
							NEW_CLASS(attr, XMLAttrib(xmlNameSt, (UOSInt)(currPtr - xmlNameSt), (const UTF8Char*)"", 0));
							node->AddAttrib(attr);
						}
						else
						{
							NEW_CLASS(attr, XMLAttrib(xmlNameSt, (UOSInt)(xmlNameEn - xmlNameSt), xmlValSt, (UOSInt)(currPtr - xmlValSt)));
							node->AddAttrib(attr);
						}
					}

					if (currPtr[1] == '>')
						parentNode->AddChild(node);
					return &currPtr[2];
				}
				if (quoted != 0)
				{
					if (c == quoted)
					{
						xmlValEn = currPtr;

						NEW_CLASS(attr, XMLAttrib(xmlNameSt, (UOSInt)(xmlNameEn - xmlNameSt), xmlValSt, (UOSInt)(xmlValEn - xmlValSt)));
						node->AddAttrib(attr);

						xmlNameSt = 0;
						xmlValSt = 0;
						quoted = 0;
						lastSpace = true;
						lastEq = false;
					}
					else
					{
						if (lastSpace)
						{
							xmlValSt = currPtr;
							xmlValEn = 0;
							lastSpace = false;
							lastEq = false;
						}
					}
				}
				else if (c == '"')
				{
					if (lastEq)
					{
						quoted = '"';
						lastSpace = true;
						xmlValSt = currPtr + 1;
					}
					else
					{
						////////////////////////
						return xmlEnd;
					}
				}
				else if (c == '\'')
				{
					if (lastEq)
					{
						quoted = '\'';
						lastSpace = true;
						xmlValSt = currPtr + 1;
					}
					else
					{
						////////////////////////
						return xmlEnd;
					}
				}
				else if (c == ' ' || c == '\t' || c == '>' || c == '\r' || c == '\n')
				{
					if (xmlValSt)
					{
						xmlValEn = currPtr;

						NEW_CLASS(attr, XMLAttrib(xmlNameSt, (UOSInt)(xmlNameEn - xmlNameSt), xmlValSt, (UOSInt)(xmlValEn - xmlValSt)));
						node->AddAttrib(attr);

						xmlNameSt = 0;
						xmlValSt = 0;
					}
					else if (xmlNameSt)
					{
						xmlNameEn = currPtr;

						if (node == 0)
						{
							NEW_CLASS(node, XMLNode(XMLNode::NT_ELEMENT));
							node->name = Text::String::New((UOSInt)(xmlNameEn - xmlNameSt));
							Text::XML::ParseStr(node->name, xmlNameSt, xmlNameEn);
							xmlNameSt = 0;
							xmlValSt = 0;
						}
					}

					lastSpace = true;

					if (c == '>')
					{
						if (xmlNameSt)
						{
							NEW_CLASS(attr, XMLAttrib(xmlNameSt, (UOSInt)(xmlNameEn - xmlNameSt), (const UTF8Char*)"", 0));
							node->AddAttrib(attr);
							xmlNameSt = 0;
						}

						parentNode->AddChild(node);

						currPtr++;
						while (currPtr < xmlEnd)
						{
							if (*currPtr == '<' && currPtr[1] == '/')
							{
								while (currPtr < xmlEnd)
								{
									if (*currPtr++ == '>')
										break;
								}
								return currPtr;
							}
							else
							{
								currPtr = ParseNode(node, currPtr, xmlEnd);
							}
						}
						return currPtr;
					}
				}
				else if (c == '=')
				{
					if (lastEq || xmlValSt)
					{
						//////////////////////////
						return xmlEnd;
					}
					else if (xmlNameSt)
					{
						if (xmlNameEn == 0)
						{
							xmlNameEn = currPtr;

							if (node == 0)
							{
								return xmlEnd;
/*								NEW_CLASS(node, XMLNode(XMLNode::NT_ELEMENT));
								currPtr2 = node->name = MemAlloc(WChar, xmlNameEn - xmlNameSt + 1);
								while (xmlNameSt < xmlNameEn)
								{
									*currPtr2++ = *xmlNameSt++;
								}
								*currPtr2 = 0;*/
							}
						}
					}
					else
					{
						//////////////////////////
						return xmlEnd;
					}
					lastEq = true;
				}
				else if (lastSpace)
				{
					if (lastEq)
					{
						xmlValSt = currPtr;
						xmlValEn = 0;
					}
					else if (xmlNameSt)
					{
						NEW_CLASS(attr, XMLAttrib(xmlNameSt, (UOSInt)(xmlNameEn - xmlNameSt), (const UTF8Char*)"", 0));
						node->AddAttrib(attr);

						xmlNameSt = currPtr;
						xmlNameEn = 0;
						xmlValSt = 0;
						xmlValEn = 0;
					}
					else
					{
						xmlNameSt = currPtr;
						xmlNameEn = 0;
						xmlValSt = 0;
						xmlValEn = 0;
					}
					lastSpace = false;
					lastEq = false;
				}
				else if (lastEq)
				{
					xmlValSt = currPtr;
					xmlValEn = 0;
					lastSpace = false;
					lastEq = false;
				}

				currPtr++;
			}
		}
	}
	else
	{
		currPtr = xmlStart;
		while (currPtr < xmlEnd)
		{
			c = *currPtr;
			if (c == '<')
				break;
			currPtr++;
		}
		NEW_CLASS(node, XMLNode(XMLNode::NT_TEXT));
		node->value = Text::String::New((UOSInt)(currPtr - xmlStart));
		Text::XML::ParseStr(node->value, xmlStart, currPtr);
		parentNode->AddChild(node);
		return currPtr;
	}
	return xmlEnd;
}

Text::XMLDocument::XMLDocument() : XMLNode(Text::XMLNode::NT_DOCUMENT)
{
	this->doc = 0;
	this->encoding = 0;
	this->version = 0;
}

Text::XMLDocument::~XMLDocument()
{
	if (this->doc)
	{
		MemFree(this->doc);
		this->doc = 0;
	}
	SDEL_STRING(this->encoding);
	SDEL_STRING(this->version);
}

Bool Text::XMLDocument::ParseBuff(Text::EncodingFactory *encFact, const UInt8 *buff, UOSInt size)
{
	UTF8Char sbuff[32];
	UTF8Char *newDoc = 0;
	UTF8Char *dest;
	const UInt8 *src;

	if (size < 4)
	{
		return true;
	}
	else if (buff[0] == 0xfe && buff[1] == 0xff)
	{
		Text::Encoding enc(1201);
		this->docLeng = enc.CountUTF8Chars(buff + 2, size - 2);
		newDoc = MemAlloc(UTF8Char, this->docLeng);
		enc.UTF8FromBytes(newDoc, buff + 2, size - 2, 0);
		newDoc[this->docLeng] = 0;
	}
	else if (buff[0] == 0xff && buff[1] == 0xfe)
	{
		Text::Encoding enc(1200);
		this->docLeng = enc.CountUTF8Chars(buff + 2, size - 2);
		newDoc = MemAlloc(UTF8Char, this->docLeng);
		enc.UTF8FromBytes(newDoc, buff + 2, size - 2, 0);
		newDoc[this->docLeng] = 0;
	}
	else if (buff[0] == 0xEF && buff[1] == 0xBB && buff[2] == 0xBF)
	{
		this->docLeng = size - 3;
		newDoc = MemAlloc(UTF8Char, docLeng + 1);
		Text::StrConcatC(newDoc, &buff[3], size - 3);
	}
	else if (buff[0] == '<' && buff[1] == '?' && buff[2] == 'x' && buff[3] == 'm' && buff[4] == 'l' && buff[5] == ' ')
	{
		const UInt8 *currPos = buff;
		const UInt8 *attName = 0;
		const UInt8 *attNameEnd = 0;
		const UInt8 *attVal = 0;
		const UInt8 *attValEnd = 0;
		UTF8Char quoted = 0;
		Bool lastSpace = false;
		Bool lastEq = false;
//		Bool tagStart = false;
		currPos = &buff[5];
		while (true)
		{
			if ((UOSInt)(currPos - buff) > size)
				return false;
			if (*currPos == '>')
			{
				break;
			}
			if ((*currPos == '"' || *currPos == '\'') && (quoted == 0 || quoted == *currPos))
			{
				if (quoted)
				{
					attValEnd = currPos;
					if (Text::StrEqualsICaseC(UTF8STRC("ENCODING"), attName, (UOSInt)(attNameEnd - attName)))
					{
						src = attVal;
						dest = sbuff;
						while (src < attValEnd)
							*dest++ = *src++;
						*dest = 0;
						Text::Encoding enc(encFact->GetCodePage(sbuff));
						docLeng = enc.CountUTF8Chars(buff, size);
						newDoc = MemAlloc(UTF8Char, docLeng + 1);
						enc.UTF8FromBytes(newDoc, buff, size, 0);
						break;
					}

					attName = 0;
					attVal = 0;
					quoted = 0;
					lastSpace = true;
				}
				else if (lastEq)
				{
					quoted = *currPos;
					lastSpace = true;
				}
				else
				{
					////////////////////////
					return false;
				}
			}
			else if (quoted)
			{
				if (lastSpace)
				{
					attVal = currPos;
					attValEnd = 0;
					lastSpace = false;
					lastEq = false;
				}
			}
			else if (*currPos == ' ')
			{
				if (attVal)
				{
					attValEnd = currPos;
					if (Text::StrEqualsICaseC(UTF8STRC("ENCODING"), attName, (UOSInt)(attNameEnd - attName)))
					{
						src = attVal;
						dest = sbuff;
						while (src < attValEnd)
							*dest++ = *src++;
						*dest = 0;
						Text::Encoding enc(encFact->GetCodePage(sbuff));
						this->docLeng = enc.CountUTF8Chars(buff, size);
						newDoc = MemAlloc(UTF8Char, docLeng + 1);
						enc.UTF8FromBytes(newDoc, buff, size, 0);
						break;
					}
					attName = 0;
					attVal = 0;
				}

				lastSpace = true;
				attName = 0;
			}
			else if (*currPos == '=')
			{
				if (attName)
				{
					if (attNameEnd == 0)
					{
						attNameEnd = currPos;
					}
				}
				lastEq = true;
			}
			else if (lastSpace)
			{
				if (quoted)
				{
					attVal = currPos;
					attValEnd = 0;
				}
				else
				{
					attName = currPos;
					attNameEnd = 0;
					attVal = 0;
					attValEnd = 0;
				}
				lastSpace = false;
				lastEq = false;
			}
			else if (lastEq)
			{
				attVal = currPos;
				attValEnd = 0;
				lastSpace = false;
				lastEq = false;
			}
			currPos++;
		}
		if (newDoc == 0)
		{
			Text::Encoding enc;
			this->docLeng = enc.CountUTF8Chars(buff, size);
			newDoc = MemAlloc(UTF8Char, docLeng + 1);
			enc.UTF8FromBytes(newDoc, buff, size, 0);
		}
	}
	else
	{
		Text::Encoding enc;
		this->docLeng = enc.CountUTF8Chars(buff, size);
		newDoc = MemAlloc(UTF8Char, docLeng + 1);
		enc.UTF8FromBytes(newDoc, buff, size, 0);
	}

	if (newDoc)
	{
		if (doc)
		{
			MemFree(doc);
		}
		UTF8Char *endPos;
		doc = newDoc;
		endPos = doc + docLeng;
		dest = doc;
		while (dest && dest < endPos)
		{
			dest = ParseNode(this, dest, endPos);
		}
		return true;
	}
	else
	{
//		printf("cannot parse the buffer\n");
		return false;
	}
	return false;
}

Bool Text::XMLDocument::ParseStream(Text::EncodingFactory *encFact, IO::Stream *stm)
{
	Bool parseRes;
	UInt8 buff[2048];
	IO::MemoryStream *mstm;
	UInt8 *mbuff;
	UOSInt buffSize;
	NEW_CLASS(mstm, IO::MemoryStream(UTF8STRC("Text.XMLDocument.ParseStream")));
	while ((buffSize = stm->Read(buff, 2048)) > 0)
	{
		mstm->Write(buff, buffSize);
	}

	mbuff = mstm->GetBuff(&buffSize);
	parseRes = this->ParseBuff(encFact, mbuff, buffSize);
	DEL_CLASS(mstm);
	return parseRes;
}

void Text::XMLDocument::AppendXmlDeclaration(Text::StringBuilderUTF *sb)
{
	if (this->version || this->encoding)
	{
		sb->AppendC(UTF8STRC("<?xml"));
		if (this->version)
		{
			sb->AppendC(UTF8STRC(" version=\""));
			sb->Append(this->version);
			sb->AppendC(UTF8STRC("\""));
		}
		if (this->encoding)
		{
			sb->AppendC(UTF8STRC(" encoding=\""));
			sb->Append(this->encoding);
			sb->AppendC(UTF8STRC("\""));
		}
	}
}
