#ifndef _SM_NET_EMAIL_EMAILSENDER
#define _SM_NET_EMAIL_EMAILSENDER
#include "Net/Email/EmailMessage.h"

namespace Net
{
	namespace Email
	{
		class EmailSender
		{
		public:
			virtual ~EmailSender(){};

			virtual Bool Send(NN<Net::Email::EmailMessage> message) = 0;
		};
	}
}
#endif
