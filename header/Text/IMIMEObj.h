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
		IMIMEObj(Text::CStringNN sourceName);
	public:
		virtual ~IMIMEObj();

		virtual Text::CStringNN GetClassName() const = 0;
		virtual Text::CStringNN GetContentType() const = 0;
		virtual UOSInt WriteStream(IO::Stream *stm) const = 0;
		virtual IMIMEObj *Clone() const = 0;

		virtual IO::ParserType GetParserType() const;

		static Text::IMIMEObj *ParseFromData(NotNullPtr<IO::StreamData> data, Text::CStringNN contentType);
		static Text::IMIMEObj *ParseFromFile(Text::CStringNN fileName);
	};
}
#endif
