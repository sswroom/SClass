#ifndef _SM_TEXT_XMLDOM
#define _SM_TEXT_XMLDOM
#include "Data/ArrayList.h"
#include "IO/Stream.h"
#include "Text/CString.h"
#include "Text/EncodingFactory.h"
#include "Text/String.h"
#include "Text/StringBuilderUTF8.h"

namespace Text
{
	class XMLAttrib;

	class XMLNode
	{
	public:
		enum class NodeType
		{
			Unknown,
			Element,
			Text,
			Document,
			Comment,
			Attribute,
			CData,
			ElementEnd,
			DocType
		};
	public:
		Text::String *name;
		Text::String *value;
		Text::String *valueOri;

	protected:
		Data::ArrayList<XMLNode*> *childArr;
		Data::ArrayList<XMLAttrib*> *attribArr;
		NodeType nt;

	public:
		XMLNode(NodeType nt);
		virtual ~XMLNode();
		NodeType GetNodeType();

		void AddAttrib(XMLAttrib *attr);
		void AddChild(XMLNode *node);
		UOSInt GetAttribCnt();
		XMLAttrib *GetAttrib(UOSInt index);
		XMLAttrib *GetFirstAttrib(const UTF8Char *attrName, UOSInt nameLen);

		UOSInt GetChildCnt();
		XMLNode *GetChild(UOSInt index);

		XMLNode **SearchNode(Text::CString path, UOSInt *cnt);
		XMLNode *SearchFirstNode(Text::CString path);
		void ReleaseSearch(XMLNode** searchResult);

		void GetInnerXML(NotNullPtr<Text::StringBuilderUTF8> sb);
		void GetInnerText(NotNullPtr<Text::StringBuilderUTF8> sb);
	private:
		void SearchNodeBegin(Text::CString path, Data::ArrayList<XMLNode*> *outArr, Bool singleResult);
		Bool SearchNodeSub(XMLNode *node, Data::ArrayList<UTF8Char*> *reqArr, Data::ArrayList<XMLNode*> *currPathArr, Data::ArrayList<XMLNode*> *outArr, Int32 searchType, Bool singleResult);
		Bool SearchNodeSubElement(XMLNode *node, Data::ArrayList<UTF8Char*> *reqArr, Data::ArrayList<XMLNode*> *currPathArr, Data::ArrayList<XMLNode*> *outArr, Int32 searchType, Bool singleResult);
		Bool SearchEqual(UOSInt level, Data::ArrayList<UTF8Char*> *reqArr, Data::ArrayList<XMLNode*> *currPathArr);
		Bool SearchEval(UOSInt level, Data::ArrayList<UTF8Char*> *reqArr, Data::ArrayList<XMLNode*> *currPathArr, XMLNode *n, const UTF8Char *nameStart, const UTF8Char *nameEnd, NotNullPtr<Text::StringBuilderUTF8> outSB);
	public:
		static Text::CStringNN NodeTypeGetName(NodeType ntype);
	};

	class XMLAttrib : public XMLNode
	{
	public:
		XMLAttrib(const UTF8Char *name, UOSInt nameLen, const UTF8Char *value, UOSInt valueLen);
		virtual ~XMLAttrib();

		virtual Bool ToString(NotNullPtr<Text::StringBuilderUTF8> sb) const;
	};


	class XMLDocument : public XMLNode
	{
	private:
		UTF8Char *doc;
		UOSInt docLeng;
		Text::String *version;
		Text::String *encoding;

	private:
		UTF8Char *ParseNode(XMLNode *parentNode, UTF8Char *xmlStart, UTF8Char *xmlEnd);
	public:
		XMLDocument();
		virtual ~XMLDocument();
		Bool ParseBuff(Text::EncodingFactory *encFact, const UInt8 *buff, UOSInt size);
		Bool ParseStream(Text::EncodingFactory *encFact, IO::Stream *stm);
		void AppendXmlDeclaration(NotNullPtr<Text::StringBuilderUTF8> sb);
	};
}
#endif
