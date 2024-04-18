#ifndef _SM_NET_EMAIL_EMAILSENDERCONFIG
#define _SM_NET_EMAIL_EMAILSENDERCONFIG
#include "IO/ConfigFile.h"
#include "Net/SocketFactory.h"
#include "Net/Email/EmailSender.h"

namespace Net
{
	namespace Email
	{
		class EmailSenderConfig
		{
		public:
			static Optional<EmailSender> LoadFromConfig(NotNullPtr<Net::SocketFactory> sockf, Optional<Net::SSLEngine> ssl, NotNullPtr<IO::ConfigFile> cfg, Text::CString cfgCategory, NotNullPtr<IO::LogTool> log);
		};
	}
}
#endif
