#ifndef _SM_TEXT_XMLREADER
#define _SM_TEXT_XMLREADER
#include "Data/ArrayList.h"
#include "IO/Stream.h"
#include "Text/Encoding.h"
#include "Text/EncodingFactory.h"
#include "Text/StringBuilderUTF.h"
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
		Text::EncodingFactory *encFact;
		IO::Stream *stm;
		UInt8 *readBuff;
		OSInt buffSize;
		OSInt parseOfst;
		OSInt parseError; //Max = 52
		ParseMode mode;

		Data::ArrayList<Text::XMLAttrib *> *attrList;
		Text::XMLNode::NodeType nt;
		Data::ArrayList<const UTF8Char*> *pathList;
		const UTF8Char *nodeText;
		const UTF8Char *nodeOriText;
		Bool emptyNode;

		void FreeCurrent();
		Bool IsHTMLSkip();
	public:
		XMLReader(Text::EncodingFactory *encFact, IO::Stream *stm, ParseMode mode);
		~XMLReader();

		void GetCurrPath(Text::StringBuilderUTF *sb);
		OSInt GetPathLev();
		Text::XMLNode::NodeType GetNodeType();
		const UTF8Char *GetNodeText(); //TextNode = Value, ElementNode = Name
		const UTF8Char *GetNodeOriText();
		OSInt GetAttribCount();
		Text::XMLAttrib *GetAttrib(OSInt index);
		Text::XMLAttrib *GetAttrib(const UTF8Char *name);

		Bool ReadNext();
		Bool ReadNodeText(Text::StringBuilderUTF *sb);
		Bool SkipElement();
		Bool IsElementEmpty();
		Bool IsComplete();
		OSInt GetErrorCode();
		Bool ToString(Text::StringBuilderUTF *sb);

		static Bool XMLWellFormat(Text::EncodingFactory *encFact, IO::Stream *stm, OSInt lev, Text::StringBuilderUTF *sb);
	};
}
#endif
