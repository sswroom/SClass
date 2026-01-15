#ifndef _SM_TEXT_MIMEOBJECT
#define _SM_TEXT_MIMEOBJECT
#include "IO/StreamData.h"
#include "IO/ParsedObject.h"
#include "IO/Stream.h"
#include "Text/CString.h"

namespace Text
{
	class MIMEObject : public IO::ParsedObject
	{
	protected:
		MIMEObject(Text::CStringNN sourceName);
	public:
		virtual ~MIMEObject();

		virtual Text::CStringNN GetClassName() const = 0;
		virtual Text::CStringNN GetContentType() const = 0;
		virtual UIntOS WriteStream(NN<IO::Stream> stm) const = 0;
		virtual NN<MIMEObject> Clone() const = 0;

		virtual IO::ParserType GetParserType() const;

		static Optional<Text::MIMEObject> ParseFromData(NN<IO::StreamData> data, Text::CStringNN contentType);
		static Optional<Text::MIMEObject> ParseFromFile(Text::CStringNN fileName);
	};
}
#endif
