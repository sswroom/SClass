#ifndef _SM_NET_EMAIL_AWSEMAILCLIENT
#define _SM_NET_EMAIL_AWSEMAILCLIENT
#include "IO/LogTool.h"
#include "IO/Writer.h"
#include "Net/Email/EmailMessage.h"
#include "Net/Email/EmailSender.h"

namespace Net
{
	namespace Email
	{
		class AWSEmailClient : public EmailSender
		{
		private:
			class ClassData;
			NN<ClassData> clsData;
			static Int32 initCnt;
		public:
			AWSEmailClient(Text::CStringNN region);
			virtual ~AWSEmailClient();

			void SetProxy(Text::CStringNN proxyHost, UInt16 port, Text::CString username, Text::CString password);

			virtual Bool Send(NN<Net::Email::EmailMessage> message);
		};
	}
}
#endif
