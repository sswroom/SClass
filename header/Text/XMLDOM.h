#ifndef _SM_TEXT_XMLDOM
#define _SM_TEXT_XMLDOM
#include "Data/ArrayListNN.hpp"
#include "Data/ArrayListObj.hpp"
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
		Optional<Text::String> name;
		Optional<Text::String> value;
		Text::String *valueOri;

	protected:
		Optional<Data::ArrayListNN<XMLNode>> childArr;
		Optional<Data::ArrayListNN<XMLAttrib>> attribArr;
		NodeType nt;

	public:
		XMLNode(NodeType nt);
		virtual ~XMLNode();
		NodeType GetNodeType();

		void AddAttrib(NN<XMLAttrib> attr);
		void AddChild(NN<XMLNode> node);
		UIntOS GetAttribCnt();
		Optional<XMLAttrib> GetAttrib(UIntOS index);
		Optional<XMLAttrib> GetFirstAttrib(Text::CStringNN attrName);

		UIntOS GetChildCnt() const;
		NN<XMLNode> GetChildNoCheck(UIntOS index) const;
		Optional<XMLNode> GetChild(UIntOS index) const;

		UnsafeArray<NN<XMLNode>> SearchNode(Text::CStringNN path, OutParam<UIntOS> cnt);
		Optional<XMLNode> SearchFirstNode(Text::CStringNN path);
		void ReleaseSearch(UnsafeArray<NN<XMLNode>> searchResult);

		void GetInnerXML(NN<Text::StringBuilderUTF8> sb);
		void GetInnerText(NN<Text::StringBuilderUTF8> sb);
	private:
		void SearchNodeBegin(Text::CStringNN path, NN<Data::ArrayListNN<XMLNode>> outArr, Bool singleResult);
		Bool SearchNodeSub(NN<XMLNode> node, NN<Data::ArrayListObj<UTF8Char*>> reqArr, NN<Data::ArrayListNN<XMLNode>> currPathArr, NN<Data::ArrayListNN<XMLNode>> outArr, Int32 searchType, Bool singleResult);
		Bool SearchNodeSubElement(NN<XMLNode> node, NN<Data::ArrayListObj<UTF8Char*>> reqArr, NN<Data::ArrayListNN<XMLNode>> currPathArr, NN<Data::ArrayListNN<XMLNode>> outArr, Int32 searchType, Bool singleResult);
		Bool SearchEqual(UIntOS level, NN<Data::ArrayListObj<UTF8Char*>> reqArr, NN<Data::ArrayListNN<XMLNode>> currPathArr);
		Bool SearchEval(UIntOS level, NN<Data::ArrayListObj<UTF8Char*>> reqArr, NN<Data::ArrayListNN<XMLNode>> currPathArr, NN<XMLNode> n, UnsafeArray<const UTF8Char> nameStart, UnsafeArray<const UTF8Char> nameEnd, NN<Text::StringBuilderUTF8> outSB);
	public:
		static Text::CStringNN NodeTypeGetName(NodeType ntype);
	};

	class XMLAttrib : public XMLNode
	{
	public:
		XMLAttrib(Text::CStringNN name, Text::CString value);
		virtual ~XMLAttrib();

		virtual Bool ToString(NN<Text::StringBuilderUTF8> sb) const;
	};


	class XMLDocument : public XMLNode
	{
	private:
		UTF8Char *doc;
		UIntOS docLeng;
		Optional<Text::String> version;
		Optional<Text::String> encoding;

	private:
		UTF8Char *ParseNode(NN<XMLNode> parentNode, UTF8Char *xmlStart, UTF8Char *xmlEnd);
	public:
		XMLDocument();
		virtual ~XMLDocument();
		Bool ParseBuff(NN<Text::EncodingFactory> encFact, UnsafeArray<const UInt8> buff, UIntOS size);
		Bool ParseStream(NN<Text::EncodingFactory> encFact, IO::Stream *stm);
		void AppendXmlDeclaration(NN<Text::StringBuilderUTF8> sb);
	};
}
#endif
