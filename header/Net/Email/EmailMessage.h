#ifndef _SM_NET_EMAIL_EMAILMESSAGE
#define _SM_NET_EMAIL_EMAILMESSAGE
#include "Data/ArrayList.h"
#include "Data/DateTime.h"
#include "IO/Stream.h"
#include "Text/String.h"
#include "Text/StringBuilderUTF8.h"

namespace Net
{
	namespace Email
	{
		class EmailMessage
		{
		private:
			const UTF8Char *fromAddr;
			Data::ArrayList<const UTF8Char*> *recpList;
			Data::ArrayList<Text::String*> *headerList;
			UInt8 *content;
			UOSInt contentLen;

			UOSInt GetHeaderIndex(const UTF8Char *name, UOSInt nameLen);
			Bool SetHeader(const UTF8Char *name, UOSInt nameLen, const UTF8Char *val);
			Bool AppendUTF8Header(Text::StringBuilderUTF8 *sb, const UTF8Char *val);
		public:
			EmailMessage();
			~EmailMessage();
			
			Bool SetSubject(const UTF8Char *subject);
			Bool SetContent(const UTF8Char *content, const Char *contentType);
			Bool SetSentDate(Data::DateTime *dt);
			Bool SetMessageId(const UTF8Char *msgId);
			Bool SetFrom(const UTF8Char *name, const UTF8Char *addr);
			Bool AddTo(const UTF8Char *name, const UTF8Char *addr);
			Bool AddToList(const UTF8Char *addrs);
			Bool AddCc(const UTF8Char *name, const UTF8Char *addr);
			Bool AddBcc(const UTF8Char *addr);

			Bool CompletedMessage();
			const UTF8Char *GetFromAddr();
			Data::ArrayList<const UTF8Char*> *GetRecpList();
			Bool WriteToStream(IO::Stream *stm);

			static Bool GenerateMessageID(Text::StringBuilderUTF8 *sb, const UTF8Char *fromAddr);
		};
	}
}
#endif
