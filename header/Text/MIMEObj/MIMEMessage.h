#ifndef _SM_TEXT_MIMEOBJ_MIMEMESSAGE
#define _SM_TEXT_MIMEOBJ_MIMEMESSAGE
#include "Data/ArrayList.h"
#include "IO/IStreamData.h"
#include "Text/IMIMEObj.h"
#include "Text/String.h"

namespace Text
{
	namespace MIMEObj
	{
		class MIMEMessage : public Text::IMIMEObj
		{
		protected:
			Data::ArrayList<Text::String *> headerName;
			Data::ArrayList<Text::String *> headerValue;
			Text::IMIMEObj *content;
			UInt8 *transferData;
			UOSInt transferSize;

		public:
			MIMEMessage();
			MIMEMessage(Text::IMIMEObj *content);
			virtual ~MIMEMessage();

			virtual Text::CString GetClassName();
			virtual Text::CString GetContentType();
			virtual UOSInt WriteStream(IO::Stream *stm);
			virtual IMIMEObj *Clone();

			void SetContent(Text::IMIMEObj *content);
			Text::IMIMEObj *GetContent();
			void SetTransferData(const UInt8 *data, UOSInt dataSize);

			void AddHeader(const UTF8Char *name, UOSInt nameLen, const UTF8Char *value, UOSInt valueLen);
			void AddHeader(Text::String *name, Text::String *value);
			Text::String *GetHeader(const UTF8Char *name, UOSInt nameLen);
			UOSInt GetHeaderCount();
			Text::String *GetHeaderName(UOSInt index);
			Text::String *GetHeaderValue(UOSInt index);

			Bool ParseFromData(IO::IStreamData *fd);

			static UTF8Char *ParseHeaderStr(UTF8Char *sbuff, const UTF8Char *value);
		};
	}
}
#endif