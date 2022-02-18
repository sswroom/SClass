#ifndef _SM_TEXT_IMIMEOBJ
#define _SM_TEXT_IMIMEOBJ
#include "IO/IStreamData.h"
#include "IO/ParsedObject.h"
#include "IO/Stream.h"
#include "Text/CString.h"

namespace Text
{
	class IMIMEObj : public IO::ParsedObject
	{
	protected:
		IMIMEObj(Text::CString sourceName);
	public:
		virtual ~IMIMEObj();

		virtual Text::CString GetClassName() = 0;
		virtual Text::CString GetContentType() = 0;
		virtual UOSInt WriteStream(IO::Stream *stm) = 0;
		virtual IMIMEObj *Clone() = 0;

		virtual IO::ParserType GetParserType();

		static Text::IMIMEObj *ParseFromData(IO::IStreamData *data, Text::CString contentType);
		static Text::IMIMEObj *ParseFromFile(Text::CString fileName);
	};
}
#endif
