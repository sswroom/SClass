#ifndef _SM_TEXT_MAILCREATOR
#define _SM_TEXT_MAILCREATOR
#include "Text/MIMEObj/MailMessage.h"
#include "Text/StringBuilderUTF8.h"

namespace Text
{
	class MailCreator
	{
	private:
		Text::String *from;
		Text::String *replyTo;
		Text::String *subject;
		Text::StringBuilderUTF8 *toVals;
		Text::StringBuilderUTF8 *ccVals;
		Text::IMIMEObj *content;
		Data::ArrayList<Text::IMIMEObj *> *attachObj;
		Data::ArrayList<const UTF8Char *> *attachName;

		void AppendStr(Text::StringBuilderUTF8 *sbc, const UTF8Char *s);
		void AppendStr(Text::StringBuilderUTF8 *sbc, const WChar *s);
		static Text::IMIMEObj *ParseContentHTML(UInt8 *buff, UOSInt buffSize, UInt32 codePage, const UTF8Char *htmlPath);
	public:
		MailCreator();
		~MailCreator();

		void SetFrom(const WChar *name, const WChar *address);
		void SetReplyTo(const WChar *name, const WChar *address);
		void ToAdd(const WChar *name, const WChar *address);
		void ToClear();
		void CCAdd(const WChar *name, const WChar *address);
		void CCClear();
		void SetSubject(const WChar *subj);

		void SetContentHTML(const WChar *content, const UTF8Char *htmlPath);
		void SetContentText(const WChar *content, UInt32 codePage);
		Bool SetContentFile(const UTF8Char *filePath);
		void AddAttachment(const UTF8Char *fileName);

		Text::MIMEObj::MailMessage *CreateMail();
	};
}
#endif
