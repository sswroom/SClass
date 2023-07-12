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
			Text::String *fromAddr;
			Data::ArrayListNN<Text::String> recpList;
			Data::ArrayListNN<Text::String> headerList;
			Text::String *contentType;
			UInt8 *content;
			UOSInt contentLen;
			Data::ArrayList<Attachment*> attachments;

			Net::SSLEngine *ssl;
			Crypto::Cert::X509Cert *signCert;
			Crypto::Cert::X509Key *signKey;

			UOSInt GetHeaderIndex(const UTF8Char *name, UOSInt nameLen);
			Bool SetHeader(const UTF8Char *name, UOSInt nameLen, const UTF8Char *val, UOSInt valLen);
			Bool AppendUTF8Header(Text::StringBuilderUTF8 *sb, const UTF8Char *val, UOSInt valLen);
			void GenMultipart(IO::Stream *stm, Text::CString boundary);

			void WriteHeaders(IO::Stream *stm);
			void WriteContents(IO::Stream *stm);
			static UTF8Char *GenBoundary(UTF8Char *sbuff, const UInt8 *data, UOSInt dataLen);
			static void WriteB64Data(IO::Stream *stm, const UInt8 *data, UOSInt dataSize);
			static void AttachmentFree(Attachment *attachment);
		public:
			EmailMessage();
			~EmailMessage();
			
			Bool SetSubject(Text::CString subject);
			Bool SetContent(Text::CString content, Text::CString contentType);
			Bool SetSentDate(Data::DateTime *dt);
			Bool SetMessageId(Text::CString msgId);
			Bool SetFrom(Text::CString name, Text::CString addr);
			Bool AddTo(Text::CString name, Text::CString addr);
			Bool AddToList(Text::CString addrs);
			Bool AddCc(Text::CString name, Text::CString addr);
			Bool AddBcc(Text::CString addr);
			Attachment *AddAttachment(Text::CString fileName);
			Attachment *AddAttachment(const UInt8 *content, UOSInt contentLen, Text::CString fileName);
			Bool AddSignature(Net::SSLEngine *ssl, Crypto::Cert::X509Cert *cert, Crypto::Cert::X509Key *key);

			Bool CompletedMessage();
			Text::String *GetFromAddr();
			const Data::ArrayListNN<Text::String> *GetRecpList();
			Bool WriteToStream(IO::Stream *stm);

			static Bool GenerateMessageID(Text::StringBuilderUTF8 *sb, Text::CString fromAddr);
		};
	}
}
#endif
