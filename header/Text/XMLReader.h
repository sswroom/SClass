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
		Text::Encoding *enc;
		Bool stmEnc;
		Text::EncodingFactory *encFact;
		NotNullPtr<IO::Stream> stm;
		UTF8Char *readBuff;
		UOSInt buffSize;
		UInt8 *rawBuff;
		UOSInt rawBuffSize;
		UOSInt parseOfst;
		UOSInt parseError; //Max = 52
		ParseMode mode;

		Data::ArrayList<Text::XMLAttrib *> attrList;
		Text::XMLNode::NodeType nt;
		Data::ArrayListStringNN pathList;
		Text::String *nodeText;
		Text::String *nodeOriText;
		Bool emptyNode;
		Text::StringBuilderUTF8 sbTmp;

		void FreeCurrent();
		Bool IsHTMLSkip();
		void InitBuffer();
		UOSInt FillBuffer();
	public:
		XMLReader(Text::EncodingFactory *encFact, NotNullPtr<IO::Stream> stm, ParseMode mode);
		~XMLReader();

		void GetCurrPath(NotNullPtr<Text::StringBuilderUTF8> sb) const;
		UOSInt GetPathLev() const;
		Text::XMLNode::NodeType GetNodeType() const;
		Optional<Text::String> GetNodeText() const; //TextNode = Value, ElementNode = Name
		NotNullPtr<Text::String> GetNodeTextNN() const; //TextNode = Value, ElementNode = Name
		Text::String *GetNodeOriText() const;
		UOSInt GetAttribCount() const;
		Text::XMLAttrib *GetAttrib(UOSInt index) const;
		Text::XMLAttrib *GetAttrib(const UTF8Char *name, UOSInt nameLen) const;

		Bool ReadNext();
		Bool ReadNodeText(NotNullPtr<Text::StringBuilderUTF8> sb);
//		Bool NextElement();
		Optional<Text::String> NextElementName();
		Bool SkipElement();
		Bool IsElementEmpty() const;
		Bool IsComplete() const;
		Bool HasError() const;
		UOSInt GetErrorCode() const;
		Bool ToString(NotNullPtr<Text::StringBuilderUTF8> sb) const;

		static Bool XMLWellFormat(Text::EncodingFactory *encFact, NotNullPtr<IO::Stream> stm, UOSInt lev, NotNullPtr<Text::StringBuilderUTF8> sb);
	};
}
#endif
