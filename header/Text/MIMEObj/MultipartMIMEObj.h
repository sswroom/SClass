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
			Data::ArrayList<MIMEMessage*> parts;

			void ParsePart(UInt8 *buff, UOSInt buffSize);
			MultipartMIMEObj(NN<Text::String> contentType, Text::String *defMsg, NN<Text::String> boundary);
			MultipartMIMEObj(Text::CString contentType, Text::CString defMsg, Text::CString boundary);
		public:
			MultipartMIMEObj(Text::CString contentType, Text::CString defMsg);
			virtual ~MultipartMIMEObj();

			virtual Text::CStringNN GetClassName() const;
			virtual Text::CStringNN GetContentType() const;
			virtual UOSInt WriteStream(IO::Stream *stm) const;
			virtual IMIMEObj *Clone() const;

			Text::String *GetDefMsg() const;
			UOSInt AddPart(Text::IMIMEObj *obj);
			void SetPartTransferData(UOSInt partIndex, const UInt8 *data, UOSInt dataSize);
			Bool AddPartHeader(UOSInt partIndex, const UTF8Char *name, UOSInt nameLen, const UTF8Char *value, UOSInt valueLen);
			Text::IMIMEObj *GetPartContent(UOSInt partIndex) const;
			MIMEMessage *GetPart(UOSInt partIndex) const;
			UOSInt GetPartCount() const;

			static MultipartMIMEObj *ParseFile(Text::CString contentType, NN<IO::StreamData> data);
		};
	}
}
#endif
