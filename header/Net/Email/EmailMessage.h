#ifndef _SM_NET_EMAIL_EMAILMESSAGE
#define _SM_NET_EMAIL_EMAILMESSAGE
#include "Crypto/Cert/X509Cert.h"
#include "Data/ArrayList.h"
#include "Data/ArrayListStringNN.h"
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
				NN<Text::String> addr;
			};

			struct Attachment
			{
				UInt8 *content;
				UOSInt contentLen;
				NN<Text::String> contentId;
				NN<Text::String> fileName;
				Data::DateTime createTime;
				Data::DateTime modifyTime;
				Bool isInline;
			};
			
		private:
			Optional<EmailAddress> fromAddr;
			Data::ArrayListNN<EmailAddress> recpList;
			Data::ArrayListStringNN headerList;
			Text::String *contentType;
			UInt8 *content;
			UOSInt contentLen;
			Data::ArrayListNN<Attachment> attachments;

			Optional<Net::SSLEngine> ssl;
			Crypto::Cert::X509Cert *signCert;
			Crypto::Cert::X509Key *signKey;

			UOSInt GetHeaderIndex(Text::CStringNN name);
			Bool SetHeader(Text::CStringNN name, Text::CStringNN val);
			Bool AppendUTF8Header(NN<Text::StringBuilderUTF8> sb, Text::CStringNN val);
			void GenMultipart(NN<IO::Stream> stm, Text::CStringNN boundary);

			void WriteHeaders(NN<IO::Stream> stm);
			void WriteContents(NN<IO::Stream> stm);
			static UnsafeArray<UTF8Char> GenBoundary(UnsafeArray<UTF8Char> sbuff, UnsafeArray<const UInt8> data, UOSInt dataLen);
			static void WriteB64Data(NN<IO::Stream> stm, UnsafeArray<const UInt8> data, UOSInt dataSize);
			static void AttachmentFree(NN<Attachment> attachment);
			static void EmailAddressFree(NN<EmailAddress> recipient);
			static NN<EmailAddress> EmailAddressCreate(RecipientType type, Text::CString name, Text::CStringNN addr);
		public:
			EmailMessage();
			~EmailMessage();
			
			Bool SetSubject(Text::CStringNN subject);
			Bool SetContent(Text::CStringNN content, Text::CStringNN contentType);
			Bool SetSentDate(NN<Data::DateTime> dt);
			Bool SetSentDate(Data::Timestamp ts);
			Bool SetMessageId(Text::CStringNN msgId);
			Bool SetFrom(Text::CString name, Text::CStringNN addr);
			Bool AddTo(Text::CString name, Text::CStringNN addr);
			Bool AddToList(Text::CStringNN addrs);
			Bool AddCc(Text::CString name, Text::CStringNN addr);
			Bool AddBcc(Text::CStringNN addr);
			void AddCustomHeader(Text::CStringNN name, Text::CStringNN value);
			Optional<Attachment> AddAttachment(Text::CStringNN fileName);
			NN<Attachment> AddAttachment(UnsafeArray<const UInt8> content, UOSInt contentLen, Text::CStringNN fileName);
			Bool AddSignature(Optional<Net::SSLEngine> ssl, Crypto::Cert::X509Cert *cert, Crypto::Cert::X509Key *key);

			Bool CompletedMessage();
			Optional<EmailAddress> GetFrom();
			NN<const Data::ArrayListNN<EmailAddress>> GetRecpList();
			Bool WriteToStream(NN<IO::Stream> stm);

			static Bool GenerateMessageID(NN<Text::StringBuilderUTF8> sb, Text::CStringNN fromAddr);
		};
	}
}
#endif
