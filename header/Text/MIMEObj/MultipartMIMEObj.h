#ifndef _SM_TEXT_MIMEOBJ_MULTIPARTMIMEOBJ
#define _SM_TEXT_MIMEOBJ_MULTIPARTMIMEOBJ
#include "Data/ArrayList.h"
#include "IO/StreamData.h"
#include "Text/IMIMEObj.h"
#include "Text/MIMEObj/MIMEMessage.h"

namespace Text
{
	namespace MIMEObj
	{
		class MultipartMIMEObj : public Text::IMIMEObj
		{
		private:
			NN<Text::String> contentType;
			NN<Text::String> boundary;
			Text::String *defMsg;
			Data::ArrayListNN<MIMEMessage> parts;

			void ParsePart(UInt8 *buff, UOSInt buffSize);
			MultipartMIMEObj(NN<Text::String> contentType, Text::String *defMsg, NN<Text::String> boundary);
			MultipartMIMEObj(Text::CStringNN contentType, Text::CString defMsg, Text::CStringNN boundary);
		public:
			MultipartMIMEObj(Text::CStringNN contentType, Text::CString defMsg);
			virtual ~MultipartMIMEObj();

			virtual Text::CStringNN GetClassName() const;
			virtual Text::CStringNN GetContentType() const;
			virtual UOSInt WriteStream(NN<IO::Stream> stm) const;
			virtual NN<IMIMEObj> Clone() const;

			Text::String *GetDefMsg() const;
			UOSInt AddPart(NN<Text::IMIMEObj> obj);
			void SetPartTransferData(UOSInt partIndex, const UInt8 *data, UOSInt dataSize);
			Bool AddPartHeader(UOSInt partIndex, Text::CStringNN name, Text::CStringNN value);
			Optional<Text::IMIMEObj> GetPartContent(UOSInt partIndex) const;
			Optional<MIMEMessage> GetPart(UOSInt partIndex) const;
			UOSInt GetPartCount() const;

			static Optional<MultipartMIMEObj> ParseFile(Text::CStringNN contentType, NN<IO::StreamData> data);
		};
	}
}
#endif
