#ifndef _SM_NET_EMAIL_EMAILMESSAGE
#define _SM_NET_EMAIL_EMAILMESSAGE
#include "Crypto/Cert/X509Cert.h"
#include "Data/ArrayList.h"
#include "Data/ArrayListNN.h"
#include "Data/DateTime.h"
#include "Data/Timestamp.h"
#include "IO/Stream.h"
#include "Net/SSLEngine.h"
#include "Text/String.h"
#include "Text/StringBuilderUTF8.h"

namespace Net
{
	namespace Email
	{
		class EmailMessage
		{
		public:
			enum class RecipientType
			{
				From,
				To,
				Cc,
				Bcc
			};

			struct EmailAddress
			{
				RecipientType type;
				Optional<Text::String> name;
				NotNullPtr<Text::String> addr;
			};

			struct Attachment
			{
				UInt8 *content;
				UOSInt contentLen;
				NotNullPtr<Text::String> contentId;
				NotNullPtr<Text::String> fileName;
				Data::DateTime createTime;
				Data::DateTime modifyTime;
				Bool isInline;
			};
			
		private:
			Optional<EmailAddress> fromAddr;
			Data::ArrayListNN<EmailAddress> recpList;
			Data::ArrayListNN<Text::String> headerList;
			Text::String *contentType;
			UInt8 *content;
			UOSInt contentLen;
			Data::ArrayListNN<Attachment> attachments;

			Optional<Net::SSLEngine> ssl;
			Crypto::Cert::X509Cert *signCert;
			Crypto::Cert::X509Key *signKey;

			UOSInt GetHeaderIndex(const UTF8Char *name, UOSInt nameLen);
			Bool SetHeader(const UTF8Char *name, UOSInt nameLen, const UTF8Char *val, UOSInt valLen);
			Bool AppendUTF8Header(NotNullPtr<Text::StringBuilderUTF8> sb, const UTF8Char *val, UOSInt valLen);
			void GenMultipart(NotNullPtr<IO::Stream> stm, Text::CString boundary);

			void WriteHeaders(NotNullPtr<IO::Stream> stm);
			void WriteContents(NotNullPtr<IO::Stream> stm);
			static UTF8Char *GenBoundary(UTF8Char *sbuff, const UInt8 *data, UOSInt dataLen);
			static void WriteB64Data(NotNullPtr<IO::Stream> stm, const UInt8 *data, UOSInt dataSize);
			static void AttachmentFree(NotNullPtr<Attachment> attachment);
			static void EmailAddressFree(NotNullPtr<EmailAddress> recipient);
			static NotNullPtr<EmailAddress> EmailAddressCreate(RecipientType type, Text::CString name, Text::CStringNN addr);
		public:
			EmailMessage();
			~EmailMessage();
			
			Bool SetSubject(Text::CStringNN subject);
			Bool SetContent(Text::CStringNN content, Text::CStringNN contentType);
			Bool SetSentDate(NotNullPtr<Data::DateTime> dt);
			Bool SetMessageId(Text::CString msgId);
			Bool SetFrom(Text::CString name, Text::CStringNN addr);
			Bool AddTo(Text::CString name, Text::CStringNN addr);
			Bool AddToList(Text::CStringNN addrs);
			Bool AddCc(Text::CString name, Text::CStringNN addr);
			Bool AddBcc(Text::CStringNN addr);
			void AddCustomHeader(Text::CStringNN name, Text::CStringNN value);
			Optional<Attachment> AddAttachment(Text::CStringNN fileName);
			NotNullPtr<Attachment> AddAttachment(const UInt8 *content, UOSInt contentLen, Text::CString fileName);
			Bool AddSignature(Optional<Net::SSLEngine> ssl, Crypto::Cert::X509Cert *cert, Crypto::Cert::X509Key *key);

			Bool CompletedMessage();
			Optional<EmailAddress> GetFrom();
			NotNullPtr<const Data::ArrayListNN<EmailAddress>> GetRecpList();
			Bool WriteToStream(NotNullPtr<IO::Stream> stm);

			static Bool GenerateMessageID(NotNullPtr<Text::StringBuilderUTF8> sb, Text::CString fromAddr);
		};
	}
}
#endif
