#ifndef _SM_NET_EMAIL_EMAILMESSAGE
#define _SM_NET_EMAIL_EMAILMESSAGE
#include "Crypto/Cert/X509Cert.h"
#include "Data/ArrayList.hpp"
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
				UnsafeArray<UInt8> content;
				NN<Text::String> contentType;
				UOSInt contentLen;
				NN<Text::String> contentId;
				NN<Text::String> fileName;
				Data::Timestamp createTime;
				Data::Timestamp modifyTime;
				Bool isInline;
			};
			
		private:
			Optional<EmailAddress> fromAddr;
			Data::ArrayListNN<EmailAddress> recpList;
			Data::ArrayListStringNN headerList;
			Optional<Text::String> contentType;
			UnsafeArrayOpt<UInt8> content;
			UOSInt contentLen;
			Data::ArrayListNN<Attachment> attachments;

			Optional<Net::SSLEngine> ssl;
			Optional<Crypto::Cert::X509Cert> signCert;
			Optional<Crypto::Cert::X509Key> signKey;

			UOSInt GetHeaderIndex(Text::CStringNN name);
			Bool SetHeader(Text::CStringNN name, Text::CStringNN val);
			Bool AppendUTF8Header(NN<Text::StringBuilderUTF8> sb, Text::CStringNN val);
			void GenMultipart(NN<IO::Stream> stm, Text::CStringNN boundary);

			void WriteHeaders(NN<IO::Stream> stm);
			Bool WriteContents(NN<IO::Stream> stm);
			static UnsafeArray<UTF8Char> GenBoundary(UnsafeArray<UTF8Char> sbuff, UnsafeArray<const UInt8> data, UOSInt dataLen);
			static void WriteB64Data(NN<IO::Stream> stm, UnsafeArray<const UInt8> data, UOSInt dataSize);
			static void __stdcall AttachmentFree(NN<Attachment> attachment);
			static void __stdcall EmailAddressFree(NN<EmailAddress> recipient);
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
			Bool AddCcList(Text::CStringNN addrs);
			Bool AddBcc(Text::CStringNN addr);
			Bool AddBccList(Text::CStringNN addrs);
			void AddCustomHeader(Text::CStringNN name, Text::CStringNN value);
			Optional<Attachment> AddAttachment(Text::CStringNN fileName);
			NN<Attachment> AddAttachment(UnsafeArray<const UInt8> content, UOSInt contentLen, Text::CStringNN fileName);
			Bool AddSignature(Optional<Net::SSLEngine> ssl, Optional<Crypto::Cert::X509Cert> cert, Optional<Crypto::Cert::X509Key> key);
			UOSInt AttachmentGetCount() const;
			Optional<Attachment> AttachmentGetItem(UOSInt index) const;

			Bool CompletedMessage();
			Optional<EmailAddress> GetFrom();
			NN<const Data::ArrayListNN<EmailAddress>> GetRecpList();
			Text::CString GetSubject();
			Optional<Text::String> GetContentType();
			UnsafeArrayOpt<UInt8> GetContent(OutParam<UOSInt> contentLeng);
			Bool WriteToStream(NN<IO::Stream> stm);

			static Bool GenerateMessageID(NN<Text::StringBuilderUTF8> sb, Text::CStringNN fromAddr);
		};
	}
}
#endif
