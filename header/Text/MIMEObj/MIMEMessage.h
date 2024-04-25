#ifndef _SM_TEXT_MIMEOBJ_MIMEMESSAGE
#define _SM_TEXT_MIMEOBJ_MIMEMESSAGE
#include "Data/ArrayListStringNN.h"
#include "IO/StreamData.h"
#include "Text/IMIMEObj.h"
#include "Text/String.h"

namespace Text
{
	namespace MIMEObj
	{
		class MIMEMessage : public Text::IMIMEObj
		{
		protected:
			Data::ArrayListStringNN headerName;
			Data::ArrayListStringNN headerValue;
			Text::IMIMEObj *content;
			UInt8 *transferData;
			UOSInt transferSize;

		public:
			MIMEMessage();
			MIMEMessage(Text::IMIMEObj *content);
			virtual ~MIMEMessage();

			virtual Text::CStringNN GetClassName() const;
			virtual Text::CStringNN GetContentType() const;
			virtual UOSInt WriteStream(IO::Stream *stm) const;
			virtual IMIMEObj *Clone() const;

			void SetContent(Text::IMIMEObj *content);
			Text::IMIMEObj *GetContent() const;
			void SetTransferData(const UInt8 *data, UOSInt dataSize);

			void AddHeader(const UTF8Char *name, UOSInt nameLen, const UTF8Char *value, UOSInt valueLen);
			void AddHeader(NN<Text::String> name, NN<Text::String> value);
			Optional<Text::String> GetHeader(const UTF8Char *name, UOSInt nameLen) const;
			UOSInt GetHeaderCount() const;
			Optional<Text::String> GetHeaderName(UOSInt index) const;
			Optional<Text::String> GetHeaderValue(UOSInt index) const;

			Bool ParseFromData(NN<IO::StreamData> fd);

			static UTF8Char *ParseHeaderStr(UTF8Char *sbuff, const UTF8Char *value);
		};
	}
}
#endif
