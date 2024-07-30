#ifndef _SM_NET_EMAIL_EMAILSENDERCONFIG
#define _SM_NET_EMAIL_EMAILSENDERCONFIG
#include "IO/ConfigFile.h"
#include "Net/TCPClientFactory.h"
#include "Net/Email/EmailSender.h"

namespace Net
{
	namespace Email
	{
		class EmailSenderConfig
		{
		public:
			static Optional<EmailSender> LoadFromConfig(NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, NN<IO::ConfigFile> cfg, Text::CString cfgCategory, NN<IO::LogTool> log);
		};
	}
}
#endif
