#ifndef _SM_TEXT_MIMEOBJ_MAILMESSAGE
#define _SM_TEXT_MIMEOBJ_MAILMESSAGE
#include "Data/ArrayListNN.hpp"
#include "Data/DateTime.h"
#include "IO/StreamData.h"
#include "Text/EncodingFactory.h"
#include "Text/MIMEObject.h"
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
			virtual NN<MIMEObject> Clone() const;

			Bool GetDate(Data::DateTime *dt) const;
			UnsafeArrayOpt<UTF8Char> GetFromAddr(UnsafeArray<UTF8Char> sbuff) const;
			UnsafeArrayOpt<UTF8Char> GetSubject(UnsafeArray<UTF8Char> sbuff) const;
			UnsafeArrayOpt<UTF8Char> GetReplyTo(UnsafeArray<UTF8Char> sbuff) const;
			UOSInt GetRecpList(NN<Data::ArrayListNN<MailAddress>> recpList) const;
			void FreeRecpList(NN<Data::ArrayListNN<MailAddress>> recpList) const;

			Optional<Text::MIMEObj::TextMIMEObj> GetContentText() const;
			Optional<Text::MIMEObject> GetContentMajor() const;
			Optional<Text::MIMEObject> GetAttachment(OSInt index, NN<Text::StringBuilderUTF8> name) const;
			
			Optional<Text::MIMEObject> GetRAWContent() const;

			static Optional<MailMessage> ParseFile(NN<IO::StreamData> fd);

		private:
			UOSInt ParseAddrList(UnsafeArray<const UTF8Char> hdr, UOSInt hdrLen, NN<Data::ArrayListNN<MailAddress>> recpList, AddressType type) const;
		};
	}
}
#endif
