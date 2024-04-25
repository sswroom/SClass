#ifndef _SM_NET_EMAIL_EMAILTEMPLATE
#define _SM_NET_EMAIL_EMAILTEMPLATE
#include "Data/FastStringMapNN.h"
#include "IO/LogTool.h"
#include "Net/Email/EmailMessage.h"
#include "Text/StringBuilderUTF8.h"

namespace Net
{
	namespace Email
	{
		// Template format:
		// [@xxxx] Attribute Text
		// [^xxxx] Form Encoded Text
		// [#xxxx] HTML Text
		// [$xxxx] Direct HTML Tag
		// [xxxx]  Subject Text
		// [[ output [ character
		class EmailTemplate
		{
		private:
			NN<Text::String> subject;
			NN<Text::String> content;
			Bool htmlContent;

			Bool ParseTemplate(NN<Text::StringBuilderUTF8> sbOut, Text::CStringNN tpl, NN<Data::FastStringMapNN<Text::String>> items, NN<IO::LogTool> log);
		public:
			EmailTemplate(Text::CStringNN subject, Text::CStringNN content, Bool htmlContent);
			~EmailTemplate();

			Bool FillEmailMessage(NN<Net::Email::EmailMessage> msg, NN<Data::FastStringMapNN<Text::String>> items, NN<IO::LogTool> log);

			static Optional<EmailTemplate> LoadFromFile(Text::CStringNN fileName, Bool htmlContent);
		};
	}
}
#endif
