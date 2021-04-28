#ifndef _SM_TEXT_MIMEOBJ_MAILMESSAGE
#define _SM_TEXT_MIMEOBJ_MAILMESSAGE
#include "Data/ArrayList.h"
#include "Data/DateTime.h"
#include "IO/IStreamData.h"
#include "Text/EncodingFactory.h"
#include "Text/IMIMEObj.h"
#include "Text/StringBuilderUTF.h"
#include "Text/MIMEObj/MIMEHeader.h"
#include "Text/MIMEObj/TextMIMEObj.h"

namespace Text
{
	namespace MIMEObj
	{
		class MailMessage : public Text::IMIMEObj, public Text::MIMEObj::MIMEHeader
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
				const UTF8Char *name;
				const UTF8Char *address;
			} MailAddress;

		private:
			Text::IMIMEObj *content;
		public:
			MailMessage();
			virtual ~MailMessage();

			virtual const UTF8Char *GetClassName();
			virtual const UTF8Char *GetContentType();
			virtual UOSInt WriteStream(IO::Stream *stm);
			virtual IMIMEObj *Clone();

			void SetContent(Text::IMIMEObj *content);

			Bool GetDate(Data::DateTime *dt);
			UTF8Char *GetFromAddr(UTF8Char *sbuff);
			UTF8Char *GetSubject(UTF8Char *sbuff);
			UTF8Char *GetReplyTo(UTF8Char *sbuff);
			UOSInt GetRecpList(Data::ArrayList<MailAddress*> *recpList);
			void FreeRecpList(Data::ArrayList<MailAddress*> *recpList);

			Text::MIMEObj::TextMIMEObj *GetContentText();
			Text::IMIMEObj *GetContentMajor();
			Text::IMIMEObj *GetAttachment(OSInt index, Text::StringBuilderUTF *name);
			
			Text::IMIMEObj *GetRAWContent();

			static MailMessage *ParseFile(IO::IStreamData *fd);

		private:
			UOSInt ParseAddrList(const UTF8Char *hdr, Data::ArrayList<MailAddress*> *recpList, AddressType type);
		};
	}
}
#endif
