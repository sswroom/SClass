#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayListObj.hpp"
#include "IO/MemoryStream.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/XML.h"
#include "Text/XMLDOM.h"

Text::XMLAttrib::XMLAttrib(Text::CStringNN name, Text::CString value) : XMLNode(Text::XMLNode::NodeType::Attribute)
{
	NN<Text::String> nns;
	this->name = nullptr;
	this->value = nullptr;
	this->valueOri = 0;
	if (name.leng > 0)
	{
		this->name = nns = Text::String::New(name.leng);
		Text::XML::ParseStr(nns, name.v, name.GetEndPtr());
	}
	Text::CStringNN nnvalue;
	if (value.SetTo(nnvalue) && nnvalue.leng > 0)
	{
		this->value = nns = Text::String::New(nnvalue.leng);
		Text::XML::ParseStr(nns, nnvalue.v, nnvalue.GetEndPtr());
		this->valueOri = Text::String::New(nnvalue).Ptr();
	}
}

Text::XMLAttrib::~XMLAttrib()
{
}

Bool Text::XMLAttrib::ToString(NN<Text::StringBuilderUTF8> sb) const
{
	NN<Text::String> nns;
	sb->AppendOpt(this->name);
	if (this->value.SetTo(nns))
	{
		sb->AppendUTF8Char('=');
		if (this->valueOri)
		{
			sb->Append(this->valueOri);
		}
		else
		{
			NN<Text::String> s = Text::XML::ToNewAttrText(UnsafeArray<const UTF8Char>(nns->v));
			sb->Append(s);
			s->Release();
		}
	}
	return true;
}

Text::XMLNode::XMLNode(NodeType nt)
{
	this->nt = nt;
	this->name = nullptr;
	this->value = nullptr;
	this->valueOri = 0;
	this->attribArr = nullptr;
	this->childArr = nullptr;
}

Text::XMLNode::~XMLNode()
{
	UOSInt i;
	NN<XMLAttrib> attr;
	NN<XMLNode> node;
	NN<Data::ArrayListNN<XMLAttrib>> attribArr;
	NN<Data::ArrayListNN<XMLNode>> childArr;

	OPTSTR_DEL(this->name);
	OPTSTR_DEL(this->value);
	SDEL_STRING(this->valueOri);
	if (this->attribArr.SetTo(attribArr))
	{
		i = attribArr->GetCount();
		while (i-- > 0)
		{
			attr = attribArr->GetItemNoCheck(i);
			attr.Delete();
		}

		this->attribArr.Delete();
	}
	if (this->childArr.SetTo(childArr))
	{
		i = childArr->GetCount();
		while (i-- > 0)
		{
			node = childArr->GetItemNoCheck(i);
			node.Delete();
		}

		this->childArr.Delete();
	}
}

Text::XMLNode::NodeType Text::XMLNode::GetNodeType()
{
	return nt;
}

void Text::XMLNode::AddAttrib(NN<XMLAttrib> attr)
{
	NN<Data::ArrayListNN<XMLAttrib>> attribArr;
	if (!this->attribArr.SetTo(attribArr))
	{
		NEW_CLASSNN(attribArr, Data::ArrayListNN<XMLAttrib>());
		this->attribArr = attribArr;
	}
	attribArr->Add(attr);
}

void Text::XMLNode::AddChild(NN<XMLNode> node)
{
	NN<Data::ArrayListNN<XMLNode>> childArr;
	if (!this->childArr.SetTo(childArr))
	{
		NEW_CLASSNN(childArr, Data::ArrayListNN<XMLNode>());
		this->childArr = childArr;
	}
	childArr->Add(node);
}

UOSInt Text::XMLNode::GetAttribCnt()
{
	NN<Data::ArrayListNN<XMLAttrib>> attribArr;
	if (!this->attribArr.SetTo(attribArr))
		return 0;
	return attribArr->GetCount();
}

Optional<Text::XMLAttrib> Text::XMLNode::GetAttrib(UOSInt index)
{
	NN<Data::ArrayListNN<XMLAttrib>> attribArr;
	if (!this->attribArr.SetTo(attribArr))
		return nullptr;
	return attribArr->GetItem(index);
}

Optional<Text::XMLAttrib> Text::XMLNode::GetFirstAttrib(Text::CStringNN attrName)
{
	NN<Data::ArrayListNN<XMLAttrib>> attribArr;
	if (!this->attribArr.SetTo(attribArr))
		return nullptr;
	NN<Text::XMLAttrib> attr;
	NN<Text::String> nns;
	UOSInt i = 0;
	UOSInt cnt = attribArr->GetCount();
	while (i < cnt)
	{
		attr = attribArr->GetItemNoCheck(i++);
		if (attr->name.SetTo(nns) && nns->Equals(attrName))
			return attr;
	}
	return nullptr;
}

UOSInt Text::XMLNode::GetChildCnt() const
{
	NN<Data::ArrayListNN<XMLNode>> childArr;
	if (!this->childArr.SetTo(childArr))
		return 0;
	return childArr->GetCount();
}

NN<Text::XMLNode> Text::XMLNode::GetChildNoCheck(UOSInt index) const
{
	return this->childArr.OrNull()->GetItemNoCheck(index);
}

Optional<Text::XMLNode> Text::XMLNode::GetChild(UOSInt index) const
{
	NN<Data::ArrayListNN<XMLNode>> childArr;
	if (!this->childArr.SetTo(childArr))
		return nullptr;
	return childArr->GetItem(index);
}

void Text::XMLNode::GetInnerXML(NN<Text::StringBuilderUTF8> sb)
{
	NN<Data::ArrayListNN<XMLNode>> childArr;
	NN<Data::ArrayListNN<XMLAttrib>> attribArr;
	NN<Text::XMLNode> n;
	NN<Text::XMLAttrib> attr;
	NN<Text::String> nns;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UOSInt l;
	if (!this->childArr.SetTo(childArr))
		return;
	if (this->nt == Text::XMLNode::NodeType::Document)
	{
		((Text::XMLDocument*)this)->AppendXmlDeclaration(sb);
	}
	i = 0;
	j = childArr->GetCount();
	while (i < j)
	{
		n = childArr->GetItemNoCheck(i);
		if (n->nt == Text::XMLNode::NodeType::Comment)
		{
			sb->AppendC(UTF8STRC("<!--"));
			sb->AppendOpt(n->value);
			sb->AppendC(UTF8STRC("-->"));
		}
		else if (n->nt == Text::XMLNode::NodeType::Text)
		{
			sb->AppendOpt(n->value);
		}
		else if (n->nt == Text::XMLNode::NodeType::CData)
		{
			sb->AppendC(UTF8STRC("<![CDATA["));
			sb->AppendOpt(n->value);
			sb->AppendC(UTF8STRC("]]>"));
		}
		else if (n->nt == Text::XMLNode::NodeType::Element)
		{
			sb->AppendC(UTF8STRC("<"));
			sb->AppendOpt(n->name);
			if (n->attribArr.SetTo(attribArr))
			{
				k = 0;
				l = attribArr->GetCount();
				while (k < l)
				{
					attr = NN<Text::XMLAttrib>::ConvertFrom(attribArr->GetItemNoCheck(k));
					sb->AppendC(UTF8STRC(" "));
					sb->AppendOpt(attr->name);
					if (attr->value.SetTo(nns))
					{
						sb->AppendC(UTF8STRC("=\""));
						sb->Append(nns);
						sb->AppendC(UTF8STRC("\""));
					}
					k++;
				}
			}
			if (n->childArr.NotNull())
			{
				sb->AppendC(UTF8STRC(">"));
				n->GetInnerXML(sb);
				sb->AppendC(UTF8STRC("</"));
				sb->AppendOpt(n->name);
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

void Text::XMLNode::GetInnerText(NN<Text::StringBuilderUTF8> sb)
{
	NN<Data::ArrayListNN<XMLNode>> childArr;
	NN<Text::XMLNode> n;
	UOSInt i;
	UOSInt j;
	if (this->nt == Text::XMLNode::NodeType::Text)
	{
		sb->AppendOpt(this->value);
		return;
	}
	else if (this->nt == Text::XMLNode::NodeType::CData)
	{
		sb->AppendOpt(this->value);
		return;
	}
	else if (!this->childArr.SetTo(childArr))
	{
		return;
	}
	else
	{
		i = 0;
		j = childArr->GetCount();
		while (i < j)
		{
			n = childArr->GetItemNoCheck(i);
			n->GetInnerText(sb);
			i++;
		}
	}
}

UnsafeArray<NN<Text::XMLNode>> Text::XMLNode::SearchNode(Text::CStringNN path, OutParam<UOSInt> cnt)
{
	Data::ArrayListNN<XMLNode> outArr;
	UnsafeArray<NN<Text::XMLNode>> outs;
	UOSInt i;
	SearchNodeBegin(path, outArr, false);

	outs = MemAllocArr(NN<XMLNode>, outArr.GetCount());
	cnt.Set(i = outArr.GetCount());
	while (i-- > 0)
	{
		outs[i] = outArr.GetItemNoCheck(i);
	}
	return outs;
}

Optional<Text::XMLNode> Text::XMLNode::SearchFirstNode(Text::CStringNN path)
{
	Data::ArrayListNN<XMLNode> outArr;
	SearchNodeBegin(path, outArr, true);
	return outArr.GetItem(0);
}

void Text::XMLNode::ReleaseSearch(UnsafeArray<NN<XMLNode>> searchResult)
{
	MemFreeArr(searchResult);
}					

void Text::XMLNode::SearchNodeBegin(Text::CStringNN path, NN<Data::ArrayListNN<XMLNode>> outArr, Bool singleResult)
{
	UTF8Char myPath[256];
	UOSInt i;
	int searchType;
	Data::ArrayListObj<UTF8Char*> reqArr;
	Data::ArrayListNN<XMLNode> currPathArr;
	UTF8Char *src;
	path.ConcatTo(myPath);

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
		
		reqArr.Clear();
		currPathArr.Clear();
		while (true)
		{
			if (*src == '/')
			{
				reqArr.Add(&src[1]);
				*src++ = 0;
			}
			else if (*src == '|')
			{
				*src = 0;
				if (searchType == 0)
				{
					SearchNodeSubElement(*this, reqArr, currPathArr, outArr, searchType, singleResult);
				}
				else
				{
					SearchNodeSub(*this, reqArr, currPathArr, outArr, searchType, singleResult);
				}
				*src = '|';

				i = reqArr.GetCount();
				while (i-- > 0)
				{
					((WChar*)reqArr.GetItem(i))[-1] = '/';
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
					SearchNodeSubElement(*this, reqArr, currPathArr, outArr, searchType, singleResult);
				}
				else
				{
					SearchNodeSub(*this, reqArr, currPathArr, outArr, searchType, singleResult);
				}

				i = reqArr.GetCount();
				while (i-- > 0)
				{
					((WChar*)reqArr.GetItem(i))[-1] = '/';
				}
				break;
			}
			else
			{
				src++;
			}
		}
	}
}

Bool Text::XMLNode::SearchNodeSub(NN<XMLNode> node, NN<Data::ArrayListObj<UTF8Char*>> reqArr, NN<Data::ArrayListNN<XMLNode>> currPathArr, NN<Data::ArrayListNN<XMLNode>> outArr, Int32 searchType, Bool singleResult)
{
	NN<XMLNode> n;
	UTF8Char *reqStr;
	NN<Data::ArrayListNN<XMLNode>> childArr;
	NN<Data::ArrayListNN<XMLAttrib>> attribArr;
	UOSInt i;
	UOSInt j;
	UOSInt k;

	if (reqArr->GetCount() <= currPathArr->GetCount())
	{
		return false;
	}
	reqStr = reqArr->GetItem(currPathArr->GetCount());
	j = currPathArr->GetCount();
	if (reqStr[0] != '@' && node->childArr.SetTo(childArr))
	{
		i = childArr->GetCount();
		while (i-- > 0)
		{
			n = childArr->GetItemNoCheck(i);
			if (n->nt == XMLNode::NodeType::Element)
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
	if (reqStr[0] == '@' && node->attribArr.SetTo(attribArr))
	{
		i = attribArr->GetCount();
		while (i-- > 0)
		{
			n = attribArr->GetItemNoCheck(i);
			
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

Bool Text::XMLNode::SearchNodeSubElement(NN<XMLNode> node, NN<Data::ArrayListObj<UTF8Char*>> reqArr, NN<Data::ArrayListNN<XMLNode>> currPathArr, NN<Data::ArrayListNN<XMLNode>> outArr, Int32 searchType, Bool singleResult)
{
	if (SearchNodeSub(node, reqArr, currPathArr, outArr, searchType, singleResult) && singleResult)
		return true;
	UOSInt i;
	NN<Text::XMLNode> n;
	i = node->GetChildCnt();
	while (i-- > 0)
	{
		if (node->GetChild(i).SetTo(n))
		{
			if (n->GetNodeType() == Text::XMLNode::NodeType::Element)
			{
				if (SearchNodeSubElement(n, reqArr, currPathArr, outArr, searchType, singleResult) && singleResult)
					return true;
			}
		}
	}
	return false;
}

Bool Text::XMLNode::SearchEqual(UOSInt level, NN<Data::ArrayListObj<UTF8Char*>> reqArr, NN<Data::ArrayListNN<XMLNode>> currPathArr)
{
	UTF8Char nameBuff[128];
	UTF8Char condBuff[128];
	Bool hasCond = false;
	UnsafeArray<UTF8Char> req = reqArr->GetItem(level);
	UnsafeArray<UTF8Char> src;
	UnsafeArray<UTF8Char> dest;
	NN<XMLNode> n;
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

	if (!currPathArr->GetItem(level).SetTo(n))
		return false;
	src = Text::String::OrEmpty(n->name)->v;
	dest = nameBuff;
	if (*dest == '@')
	{
		if (n->GetNodeType() != Text::XMLNode::NodeType::Attribute)
			return false;
		dest++;
	}
	else
	{
		if (n->GetNodeType() != Text::XMLNode::NodeType::Element)
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
				if (SearchEval(level, reqArr, currPathArr, n, condBuff, src, leftSB) == false)
					return false;

				dest = src;
				while (*dest++);
				dest--;
				if (SearchEval(level, reqArr, currPathArr, n, src + 1, dest, rightSB) == false)
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

Bool Text::XMLNode::SearchEval(UOSInt level, NN<Data::ArrayListObj<UTF8Char*>> reqArr, NN<Data::ArrayListNN<XMLNode>> currPathArr, NN<Text::XMLNode> n, UnsafeArray<const UTF8Char> nameStart, UnsafeArray<const UTF8Char> nameEnd, NN<Text::StringBuilderUTF8> outSB)
{
	UnsafeArray<const UTF8Char> src;
	UnsafeArray<const UTF8Char> dest;
	NN<XMLAttrib> attr;
	NN<XMLNode> child;
	UOSInt i;
	Bool eq;
	NN<Data::ArrayListNN<XMLAttrib>> attribArr;
	NN<Data::ArrayListNN<XMLNode>> childArr;

	if (*nameStart == '@')
	{
		if (n->attribArr.SetTo(attribArr))
		{
			i = attribArr->GetCount();
			while (i-- > 0)
			{
				attr = attribArr->GetItemNoCheck(i);
				src = &nameStart[1];
				dest = Text::String::OrEmpty(attr->name)->v;
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
					outSB->AppendOpt(attr->value);
					return true;
				}
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
		if (n->childArr.SetTo(childArr))
		{
			i = childArr->GetCount();
			while (i-- > 0)
			{
				child = childArr->GetItemNoCheck(i);
				if (child->nt == Text::XMLNode::NodeType::Element)
				{
					src = nameStart;
					dest = Text::String::OrEmpty(child->name)->v;
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
		}
		return false;
	}
}

Text::CStringNN Text::XMLNode::NodeTypeGetName(NodeType ntype)
{
	switch (ntype)
	{
	case NodeType::Element:
		return CSTR("Element");
	case NodeType::Text:
		return CSTR("Text");
	case NodeType::Document:
		return CSTR("Document");
	case NodeType::Comment:
		return CSTR("Comment");
	case NodeType::Attribute:
		return CSTR("Attribute");
	case NodeType::CData:
		return CSTR("CDATA");
	case NodeType::ElementEnd:
		return CSTR("Element End");
	case NodeType::DocType:
		return CSTR("DocType");
	case NodeType::Unknown:
	default:
		return CSTR("Unknown");
	}

}

UTF8Char *Text::XMLDocument::ParseNode(NN<XMLNode> parentNode, UTF8Char *xmlStart, UTF8Char *xmlEnd)
{
	UTF8Char *currPtr;
	UTF8Char c;
	NN<XMLNode> node;
	Optional<XMLNode> optnode;
	NN<XMLAttrib> attr;
	NN<Text::String> nns;


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
							OPTSTR_DEL(this->encoding);
							this->encoding = nns = Text::String::New((UOSInt)(xmlValEn - xmlValSt));
							Text::XML::ParseStr(nns, xmlValSt, xmlValEn);
						}
						else if (Text::StrEqualsICaseC(UTF8STRC("VERSION"), xmlNameSt, (UOSInt)(xmlNameEn - xmlNameSt)))
						{
							OPTSTR_DEL(this->version);
							this->version = nns = Text::String::New((UOSInt)(xmlValEn - xmlValSt));
							Text::XML::ParseStr(nns, xmlValSt, xmlValEn);
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
							OPTSTR_DEL(this->encoding);
							this->encoding = nns = Text::String::New((UOSInt)(xmlValEn - xmlValSt));
							Text::XML::ParseStr(nns, xmlValSt, xmlValEn);
						}
						else if (Text::StrEqualsICaseC(UTF8STRC("VERSION"), xmlNameSt, (UOSInt)(xmlNameEn - xmlNameSt)))
						{
							OPTSTR_DEL(this->version);
							this->version = nns = Text::String::New((UOSInt)(xmlValEn - xmlValSt));
							Text::XML::ParseStr(nns, xmlValSt, xmlValEn);
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
						NEW_CLASSNN(node, XMLNode(XMLNode::NodeType::Comment));
						node->value = Text::String::New(xmlStart + 4, (UOSInt)(currPtr - xmlStart - 4)).Ptr();
						parentNode->AddChild(node);
						
						return xmlEnd + 3;
					}
				}
				currPtr++;
			}

			//////////////////////////////// File Error /////////////////////////////////////////////////
			NEW_CLASSNN(node, XMLNode(XMLNode::NodeType::Text));
			node->name = nns = Text::String::New((UOSInt)(currPtr - xmlStart));
			Text::XML::ParseStr(nns, xmlStart, currPtr);
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
						NEW_CLASSNN(node, XMLNode(XMLNode::NodeType::CData));
						node->value = Text::String::New(xmlStart + 9, (UOSInt)(currPtr - xmlStart - 9)).Ptr();
						parentNode->AddChild(node);
						return xmlEnd + 3;
					}
				}
				currPtr++;
			}

			//////////////////////////////// File Error /////////////////////////////////////////////////
			NEW_CLASSNN(node, XMLNode(XMLNode::NodeType::Text));
			node->name = nns = Text::String::New((UOSInt)(currPtr - xmlStart));
			Text::XML::ParseStr(nns, xmlStart, currPtr);
			this->AddChild(node);
			
			return xmlEnd;
		}
		else
		{
			lastEq = false;
			lastSpace = true;
			quoted = 0;
			currPtr = &xmlStart[1];
			optnode = nullptr;

			while (true)
			{
				if (currPtr >= xmlEnd)
				{
					if (optnode.SetTo(node))
						parentNode->AddChild(node);
					return xmlEnd;
				}

				c = *currPtr;
				if (!quoted && c == '/' && (optnode.NotNull() || xmlNameSt != 0))
				{
					if (!optnode.SetTo(node))
					{
						NEW_CLASSNN(node, XMLNode(XMLNode::NodeType::Element));
						node->name = nns = Text::String::New((UOSInt)(currPtr - xmlNameSt));
						Text::XML::ParseStr(nns, xmlNameSt, currPtr);
					}
					else if (xmlNameSt != 0)
					{
						if (xmlValSt == 0)
						{
							NEW_CLASSNN(attr, XMLAttrib(CSTRP(xmlNameSt, currPtr), nullptr));
							node->AddAttrib(attr);
						}
						else
						{
							NEW_CLASSNN(attr, XMLAttrib(CSTRP(xmlNameSt, xmlNameEn), CSTRP(xmlValSt, currPtr)));
							node->AddAttrib(attr);
						}
					}

					if (currPtr[1] == '>')
						parentNode->AddChild(node);
					return &currPtr[2];
				}
				if (quoted != 0 && optnode.SetTo(node))
				{
					if (c == quoted)
					{
						xmlValEn = currPtr;

						NEW_CLASSNN(attr, XMLAttrib(CSTRP(xmlNameSt, xmlNameEn), CSTRP(xmlValSt, xmlValEn)));
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
					if (xmlValSt && optnode.SetTo(node))
					{
						xmlValEn = currPtr;

						NEW_CLASSNN(attr, XMLAttrib(CSTRP(xmlNameSt, xmlNameEn), CSTRP(xmlValSt, xmlValEn)));
						node->AddAttrib(attr);

						xmlNameSt = 0;
						xmlValSt = 0;
					}
					else if (xmlNameSt)
					{
						xmlNameEn = currPtr;

						if (!optnode.SetTo(node))
						{
							NEW_CLASSNN(node, XMLNode(XMLNode::NodeType::Element));
							node->name = nns = Text::String::New((UOSInt)(xmlNameEn - xmlNameSt));
							Text::XML::ParseStr(nns, xmlNameSt, xmlNameEn);
							optnode = node;
							xmlNameSt = 0;
							xmlValSt = 0;
						}
					}

					lastSpace = true;

					if (c == '>' && optnode.SetTo(node))
					{
						if (xmlNameSt)
						{
							NEW_CLASSNN(attr, XMLAttrib(CSTRP(xmlNameSt, xmlNameEn), nullptr));
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

							if (optnode.IsNull())
							{
								return xmlEnd;
/*								NEW_CLASS(node, XMLNode(XMLNode::NodeType::Element));
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
						if (optnode.SetTo(node))
						{
							NEW_CLASSNN(attr, XMLAttrib(CSTRP(xmlNameSt, xmlNameEn), nullptr));
							node->AddAttrib(attr);
						}

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
		NEW_CLASSNN(node, XMLNode(XMLNode::NodeType::Text));
		node->value = nns = Text::String::New((UOSInt)(currPtr - xmlStart));
		Text::XML::ParseStr(nns, xmlStart, currPtr);
		parentNode->AddChild(node);
		return currPtr;
	}
	return xmlEnd;
}

Text::XMLDocument::XMLDocument() : XMLNode(Text::XMLNode::NodeType::Document)
{
	this->doc = 0;
	this->encoding = nullptr;
	this->version = nullptr;
}

Text::XMLDocument::~XMLDocument()
{
	if (this->doc)
	{
		MemFree(this->doc);
		this->doc = 0;
	}
	OPTSTR_DEL(this->encoding);
	OPTSTR_DEL(this->version);
}

Bool Text::XMLDocument::ParseBuff(NN<Text::EncodingFactory> encFact, UnsafeArray<const UInt8> buff, UOSInt size)
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
		UnsafeArray<const UInt8> currPos = buff;
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
					attValEnd = currPos.Ptr();
					if (Text::StrEqualsICaseC(UTF8STRC("ENCODING"), attName, (UOSInt)(attNameEnd - attName)))
					{
						src = attVal;
						dest = sbuff;
						while (src < attValEnd)
							*dest++ = *src++;
						*dest = 0;
						Text::Encoding enc(encFact->GetCodePage(CSTRP(sbuff, dest)));
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
					attVal = currPos.Ptr();
					attValEnd = 0;
					lastSpace = false;
					lastEq = false;
				}
			}
			else if (*currPos == ' ')
			{
				if (attVal)
				{
					attValEnd = currPos.Ptr();
					if (Text::StrEqualsICaseC(UTF8STRC("ENCODING"), attName, (UOSInt)(attNameEnd - attName)))
					{
						src = attVal;
						dest = sbuff;
						while (src < attValEnd)
							*dest++ = *src++;
						*dest = 0;
						Text::Encoding enc(encFact->GetCodePage(CSTRP(sbuff, dest)));
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
						attNameEnd = currPos.Ptr();
					}
				}
				lastEq = true;
			}
			else if (lastSpace)
			{
				if (quoted)
				{
					attVal = currPos.Ptr();
					attValEnd = 0;
				}
				else
				{
					attName = currPos.Ptr();
					attNameEnd = 0;
					attVal = 0;
					attValEnd = 0;
				}
				lastSpace = false;
				lastEq = false;
			}
			else if (lastEq)
			{
				attVal = currPos.Ptr();
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
			dest = ParseNode(*this, dest, endPos);
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

Bool Text::XMLDocument::ParseStream(NN<Text::EncodingFactory> encFact, IO::Stream *stm)
{
	Bool parseRes;
	UInt8 buff[2048];
	UnsafeArray<UInt8> mbuff;
	UOSInt buffSize;
	IO::MemoryStream mstm;
	while ((buffSize = stm->Read(BYTEARR(buff))) > 0)
	{
		mstm.Write(Data::ByteArrayR(buff, buffSize));
	}

	mbuff = mstm.GetBuff(buffSize);
	parseRes = this->ParseBuff(encFact, mbuff, buffSize);
	return parseRes;
}

void Text::XMLDocument::AppendXmlDeclaration(NN<Text::StringBuilderUTF8> sb)
{
	NN<Text::String> nns;
	if (this->version.NotNull() || this->encoding.NotNull())
	{
		sb->AppendC(UTF8STRC("<?xml"));
		if (this->version.SetTo(nns))
		{
			sb->AppendC(UTF8STRC(" version=\""));
			sb->Append(nns);
			sb->AppendC(UTF8STRC("\""));
		}
		if (this->encoding.SetTo(nns))
		{
			sb->AppendC(UTF8STRC(" encoding=\""));
			sb->Append(nns);
			sb->AppendC(UTF8STRC("\""));
		}
	}
}
