#ifndef _SM_TEXT_MAILCREATOR
#define _SM_TEXT_MAILCREATOR
#include "Data/ArrayListNN.hpp"
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
		Optional<Text::MIMEObject> content;
		Data::ArrayListNN<Text::MIMEObject> attachObj;
		Data::ArrayListStringNN attachName;

		void AppendStr(NN<Text::StringBuilderUTF8> sbc, Text::CStringNN s);
		void AppendStr(NN<Text::StringBuilderUTF8> sbc, const WChar *s);
		static Optional<Text::MIMEObject> ParseContentHTML(UnsafeArray<const UInt8> buff, UIntOS buffSize, UInt32 codePage, Text::CStringNN htmlPath);
	public:
		MailCreator();
		~MailCreator();

		void SetFrom(const WChar *name, const WChar *address);
		void SetFrom(Text::CString name, Text::CString address);
		void SetReplyTo(const WChar *name, const WChar *address);
		void ToAdd(const WChar *name, const WChar *address);
		void ToAdd(Text::String *name, NN<Text::String> address);
		void ToClear();
		void CCAdd(const WChar *name, const WChar *address);
		void CCAdd(Text::String *name, NN<Text::String> address);
		void CCClear();
		void SetSubject(const WChar *subj);
		void SetSubject(NN<Text::String> subj);

		void SetContentHTML(const WChar *content, Text::CStringNN htmlPath);
		void SetContentHTML(NN<Text::String> content, Text::CStringNN htmlPath);
		void SetContentText(const WChar *content, UInt32 codePage);
		void SetContentText(NN<Text::String> content);
		Bool SetContentFile(Text::CStringNN filePath);
		void AddAttachment(Text::CStringNN fileName);

		NN<Text::MIMEObj::MailMessage> CreateMail();
	};
}
#endif
