#ifndef _SM_TEXT_MIMEOBJ_MIMEMESSAGE
#define _SM_TEXT_MIMEOBJ_MIMEMESSAGE
#include "Data/ArrayListStringNN.h"
#include "IO/StreamData.h"
#include "Text/MIMEObject.h"
#include "Text/String.h"

namespace Text
{
	namespace MIMEObj
	{
		class MIMEMessage : public Text::MIMEObject
		{
		protected:
			Data::ArrayListStringNN headerName;
			Data::ArrayListStringNN headerValue;
			Optional<Text::MIMEObject> content;
			UInt8 *transferData;
			UOSInt transferSize;

		public:
			MIMEMessage();
			MIMEMessage(Optional<Text::MIMEObject> content);
			virtual ~MIMEMessage();

			virtual Text::CStringNN GetClassName() const;
			virtual Text::CStringNN GetContentType() const;
			virtual UOSInt WriteStream(NN<IO::Stream> stm) const;
			virtual NN<MIMEObject> Clone() const;

			void SetContent(Optional<Text::MIMEObject> content);
			Optional<Text::MIMEObject> GetContent() const;
			void SetTransferData(const UInt8 *data, UOSInt dataSize);

			void AddHeader(Text::CStringNN name, Text::CStringNN value);
			void AddHeader(NN<Text::String> name, NN<Text::String> value);
			Optional<Text::String> GetHeader(UnsafeArray<const UTF8Char> name, UOSInt nameLen) const;
			UOSInt GetHeaderCount() const;
			Optional<Text::String> GetHeaderName(UOSInt index) const;
			Optional<Text::String> GetHeaderValue(UOSInt index) const;

			Bool ParseFromData(NN<IO::StreamData> fd);

			static UnsafeArray<UTF8Char> ParseHeaderStr(UnsafeArray<UTF8Char> sbuff, UnsafeArray<const UTF8Char> value);
		};
	}
}
#endif
