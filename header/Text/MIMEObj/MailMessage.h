#ifndef _SM_TEXT_MIMEOBJ_MAILMESSAGE
#define _SM_TEXT_MIMEOBJ_MAILMESSAGE
#include "Data/ArrayList.h"
#include "Data/DateTime.h"
#include "IO/IStreamData.h"
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
				Text::String *address;
			} MailAddress;

		public:
			MailMessage();
			virtual ~MailMessage();

			virtual Text::CString GetClassName() const;
			virtual IMIMEObj *Clone() const;

			Bool GetDate(Data::DateTime *dt) const;
			UTF8Char *GetFromAddr(UTF8Char *sbuff) const;
			UTF8Char *GetSubject(UTF8Char *sbuff) const;
			UTF8Char *GetReplyTo(UTF8Char *sbuff) const;
			UOSInt GetRecpList(Data::ArrayList<MailAddress*> *recpList) const;
			void FreeRecpList(Data::ArrayList<MailAddress*> *recpList) const;

			Text::MIMEObj::TextMIMEObj *GetContentText() const;
			Text::IMIMEObj *GetContentMajor() const;
			Text::IMIMEObj *GetAttachment(OSInt index, Text::StringBuilderUTF8 *name) const;
			
			Text::IMIMEObj *GetRAWContent() const;

			static MailMessage *ParseFile(IO::IStreamData *fd);

		private:
			UOSInt ParseAddrList(const UTF8Char *hdr, UOSInt hdrLen, Data::ArrayList<MailAddress*> *recpList, AddressType type) const;
		};
	}
}
#endif
