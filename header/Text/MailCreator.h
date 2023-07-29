#ifndef _SM_TEXT_MAILCREATOR
#define _SM_TEXT_MAILCREATOR
#include "Data/ArrayListNN.h"
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
		Text::StringBuilderUTF8 toVals;
		Text::StringBuilderUTF8 ccVals;
		Text::IMIMEObj *content;
		Data::ArrayList<Text::IMIMEObj *> attachObj;
		Data::ArrayListNN<Text::String> attachName;

		void AppendStr(NotNullPtr<Text::StringBuilderUTF8> sbc, Text::CString s);
		void AppendStr(NotNullPtr<Text::StringBuilderUTF8> sbc, const WChar *s);
		static Text::IMIMEObj *ParseContentHTML(const UInt8 *buff, UOSInt buffSize, UInt32 codePage, Text::CString htmlPath);
	public:
		MailCreator();
		~MailCreator();

		void SetFrom(const WChar *name, const WChar *address);
		void SetFrom(Text::CString name, Text::CString address);
		void SetReplyTo(const WChar *name, const WChar *address);
		void ToAdd(const WChar *name, const WChar *address);
		void ToAdd(Text::String *name, NotNullPtr<Text::String> address);
		void ToClear();
		void CCAdd(const WChar *name, const WChar *address);
		void CCAdd(Text::String *name, NotNullPtr<Text::String> address);
		void CCClear();
		void SetSubject(const WChar *subj);
		void SetSubject(NotNullPtr<Text::String> subj);

		void SetContentHTML(const WChar *content, Text::CString htmlPath);
		void SetContentHTML(Text::String *content, Text::CString htmlPath);
		void SetContentText(const WChar *content, UInt32 codePage);
		void SetContentText(Text::String *content);
		Bool SetContentFile(Text::CString filePath);
		void AddAttachment(Text::CString fileName);

		Text::MIMEObj::MailMessage *CreateMail();
	};
}
#endif
