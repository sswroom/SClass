#ifndef _SM_TEXT_XMLREADER
#define _SM_TEXT_XMLREADER
#include "Data/ArrayList.h"
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
		IO::Stream *stm;
		UTF8Char *readBuff;
		UOSInt buffSize;
		UInt8 *rawBuff;
		UOSInt rawBuffSize;
		UOSInt parseOfst;
		OSInt parseError; //Max = 52
		ParseMode mode;

		Data::ArrayList<Text::XMLAttrib *> attrList;
		Text::XMLNode::NodeType nt;
		Data::ArrayList<Text::String*> pathList;
		Text::String *nodeText;
		Text::String *nodeOriText;
		Bool emptyNode;

		void FreeCurrent();
		Bool IsHTMLSkip();
		void InitBuffer();
		UOSInt FillBuffer();
	public:
		XMLReader(Text::EncodingFactory *encFact, IO::Stream *stm, ParseMode mode);
		~XMLReader();

		void GetCurrPath(Text::StringBuilderUTF8 *sb);
		UOSInt GetPathLev();
		Text::XMLNode::NodeType GetNodeType();
		Text::String *GetNodeText(); //TextNode = Value, ElementNode = Name
		Text::String *GetNodeOriText();
		UOSInt GetAttribCount();
		Text::XMLAttrib *GetAttrib(UOSInt index);
		Text::XMLAttrib *GetAttrib(const UTF8Char *name, UOSInt nameLen);

		Bool ReadNext();
		Bool ReadNodeText(Text::StringBuilderUTF8 *sb);
		Bool SkipElement();
		Bool IsElementEmpty();
		Bool IsComplete();
		OSInt GetErrorCode();
		Bool ToString(Text::StringBuilderUTF8 *sb);

		static Bool XMLWellFormat(Text::EncodingFactory *encFact, IO::Stream *stm, UOSInt lev, Text::StringBuilderUTF8 *sb);
	};
}
#endif
