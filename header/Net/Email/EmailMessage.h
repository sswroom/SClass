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
			Text::String *fromAddr;
			Data::ArrayList<const UTF8Char*> *recpList;
			Data::ArrayList<Text::String*> *headerList;
			UInt8 *content;
			UOSInt contentLen;

			UOSInt GetHeaderIndex(const UTF8Char *name, UOSInt nameLen);
			Bool SetHeader(const UTF8Char *name, UOSInt nameLen, const UTF8Char *val, UOSInt valLen);
			Bool AppendUTF8Header(Text::StringBuilderUTF8 *sb, const UTF8Char *val, UOSInt valLen);
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

			Bool CompletedMessage();
			Text::String *GetFromAddr();
			Data::ArrayList<const UTF8Char*> *GetRecpList();
			Bool WriteToStream(IO::Stream *stm);

			static Bool GenerateMessageID(Text::StringBuilderUTF8 *sb, Text::CString fromAddr);
		};
	}
}
#endif
