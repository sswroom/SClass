#ifndef _SM_TEXT_IMIMEOBJ
#define _SM_TEXT_IMIMEOBJ
#include "IO/StreamData.h"
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

		virtual Text::CString GetClassName() const = 0;
		virtual Text::CString GetContentType() const = 0;
		virtual UOSInt WriteStream(IO::Stream *stm) const = 0;
		virtual IMIMEObj *Clone() const = 0;

		virtual IO::ParserType GetParserType() const;

		static Text::IMIMEObj *ParseFromData(NotNullPtr<IO::StreamData> data, Text::CString contentType);
		static Text::IMIMEObj *ParseFromFile(Text::CString fileName);
	};
}
#endif
