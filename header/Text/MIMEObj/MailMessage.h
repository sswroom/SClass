#ifndef _SM_TEXT_MIMEOBJ_MAILMESSAGE
#define _SM_TEXT_MIMEOBJ_MAILMESSAGE
#include "Data/ArrayList.h"
#include "Data/DateTime.h"
#include "IO/StreamData.h"
#include "Text/EncodingFactory.h"
#include "Text/IMIMEObj.h"
#include "Text/String.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/MIMEObj/MIMEMessage.h"
#include "Text/MIMEObj/TextMIMEObj.h"

namespace Text
{
	namespace MIMEObj
	{
		class MailMessage : public Text::MIMEObj::MIMEMessage
		{
		public:
			typedef enum
			{
				AT_TO,
				AT_CC
			} AddressType;

			typedef struct
			{
				AddressType type;
				Text::String *name;
				NotNullPtr<Text::String> address;
			} MailAddress;

		public:
			MailMessage();
			virtual ~MailMessage();

			virtual Text::CStringNN GetClassName() const;
			virtual IMIMEObj *Clone() const;

			Bool GetDate(Data::DateTime *dt) const;
			UTF8Char *GetFromAddr(UTF8Char *sbuff) const;
			UTF8Char *GetSubject(UTF8Char *sbuff) const;
			UTF8Char *GetReplyTo(UTF8Char *sbuff) const;
			UOSInt GetRecpList(NotNullPtr<Data::ArrayList<MailAddress*>> recpList) const;
			void FreeRecpList(NotNullPtr<Data::ArrayList<MailAddress*>> recpList) const;

			Text::MIMEObj::TextMIMEObj *GetContentText() const;
			Text::IMIMEObj *GetContentMajor() const;
			Text::IMIMEObj *GetAttachment(OSInt index, NotNullPtr<Text::StringBuilderUTF8> name) const;
			
			Text::IMIMEObj *GetRAWContent() const;

			static MailMessage *ParseFile(NotNullPtr<IO::StreamData> fd);

		private:
			UOSInt ParseAddrList(const UTF8Char *hdr, UOSInt hdrLen, NotNullPtr<Data::ArrayList<MailAddress*>> recpList, AddressType type) const;
		};
	}
}
#endif
