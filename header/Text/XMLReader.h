#ifndef _SM_TEXT_XMLREADER
#define _SM_TEXT_XMLREADER
#include "Data/ArrayList.h"
#include "Data/ArrayListStringNN.h"
#include "IO/Stream.h"
#include "Text/Encoding.h"
#include "Text/EncodingFactory.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/XMLDOM.h"

namespace Text
{
	class XMLReader
	{
	public:
		typedef enum
		{
			PM_XML,
			PM_HTML
		} ParseMode;
	private:
		Optional<Text::Encoding> enc;
		Bool stmEnc;
		Optional<Text::EncodingFactory> encFact;
		NN<IO::Stream> stm;
		UnsafeArray<UTF8Char> readBuff;
		UOSInt buffSize;
		UnsafeArray<UInt8> rawBuff;
		UOSInt rawBuffSize;
		UOSInt parseOfst;
		UOSInt parseError; //Max = 52
		ParseMode mode;

		Data::ArrayListNN<Text::XMLAttrib> attrList;
		Text::XMLNode::NodeType nt;
		Data::ArrayListStringNN pathList;
		Optional<Text::String> nodeText;
		Optional<Text::String> nodeOriText;
		Optional<Text::String> ns;
		Data::FastStringMapNN<Text::String> nsMap;
		Bool emptyNode;
		Text::StringBuilderUTF8 sbTmp;

		void FreeCurrent();
		Bool IsHTMLSkip();
		void InitBuffer();
		UOSInt FillBuffer();
		void ParseElementNS();
	public:
		XMLReader(Optional<Text::EncodingFactory> encFact, NN<IO::Stream> stm, ParseMode mode);
		~XMLReader();

		void GetCurrPath(NN<Text::StringBuilderUTF8> sb) const;
		UOSInt GetPathLev() const;
		Text::XMLNode::NodeType GetNodeType() const;
		Optional<Text::String> GetNodeText() const; //TextNode = Value, ElementNode = Name
		NN<Text::String> GetNodeTextNN() const; //TextNode = Value, ElementNode = Name
		Optional<Text::String> GetNodeOriText() const;
		Optional<Text::String> GetNamespace() const;
		Text::CStringNN GetElementName() const;
		UOSInt GetAttribCount() const;
		NN<Text::XMLAttrib> GetAttribNoCheck(UOSInt index) const;
		Optional<Text::XMLAttrib> GetAttrib(UOSInt index) const;
		Optional<Text::XMLAttrib> GetAttrib(Text::CStringNN name) const;

		Bool ReadNext();
		Bool ReadNodeText(NN<Text::StringBuilderUTF8> sb);
//		Bool NextElement();
		Optional<Text::String> NextElementName();
		Bool SkipElement();
		Bool IsElementEmpty() const;
		Bool IsComplete() const;
		Bool HasError() const;
		UOSInt GetErrorCode() const;
		Bool ToString(NN<Text::StringBuilderUTF8> sb) const;

		static Bool XMLWellFormat(Optional<Text::EncodingFactory> encFact, NN<IO::Stream> stm, UOSInt lev, NN<Text::StringBuilderUTF8> sb);
	};
}
#endif
