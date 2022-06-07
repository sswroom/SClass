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

			virtual Text::CString GetClassName();
			virtual IMIMEObj *Clone();

			Bool GetDate(Data::DateTime *dt);
			UTF8Char *GetFromAddr(UTF8Char *sbuff);
			UTF8Char *GetSubject(UTF8Char *sbuff);
			UTF8Char *GetReplyTo(UTF8Char *sbuff);
			UOSInt GetRecpList(Data::ArrayList<MailAddress*> *recpList);
			void FreeRecpList(Data::ArrayList<MailAddress*> *recpList);

			Text::MIMEObj::TextMIMEObj *GetContentText();
			Text::IMIMEObj *GetContentMajor();
			Text::IMIMEObj *GetAttachment(OSInt index, Text::StringBuilderUTF8 *name);
			
			Text::IMIMEObj *GetRAWContent();

			static MailMessage *ParseFile(IO::IStreamData *fd);

		private:
			UOSInt ParseAddrList(const UTF8Char *hdr, UOSInt hdrLen, Data::ArrayList<MailAddress*> *recpList, AddressType type);
		};
	}
}
#endif
