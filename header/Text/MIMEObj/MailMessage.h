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
				NN<Text::String> address;
			} MailAddress;

		public:
			MailMessage();
			virtual ~MailMessage();

			virtual Text::CStringNN GetClassName() const;
			virtual NN<IMIMEObj> Clone() const;

			Bool GetDate(Data::DateTime *dt) const;
			UTF8Char *GetFromAddr(UTF8Char *sbuff) const;
			UTF8Char *GetSubject(UTF8Char *sbuff) const;
			UTF8Char *GetReplyTo(UTF8Char *sbuff) const;
			UOSInt GetRecpList(NN<Data::ArrayListNN<MailAddress>> recpList) const;
			void FreeRecpList(NN<Data::ArrayListNN<MailAddress>> recpList) const;

			Optional<Text::MIMEObj::TextMIMEObj> GetContentText() const;
			Optional<Text::IMIMEObj> GetContentMajor() const;
			Optional<Text::IMIMEObj> GetAttachment(OSInt index, NN<Text::StringBuilderUTF8> name) const;
			
			Optional<Text::IMIMEObj> GetRAWContent() const;

			static Optional<MailMessage> ParseFile(NN<IO::StreamData> fd);

		private:
			UOSInt ParseAddrList(const UTF8Char *hdr, UOSInt hdrLen, NN<Data::ArrayListNN<MailAddress>> recpList, AddressType type) const;
		};
	}
}
#endif
