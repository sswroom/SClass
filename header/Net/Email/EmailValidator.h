#ifndef _SM_NET_EMAIL_EMAILVALIDATOR
#define _SM_NET_EMAIL_EMAILVALIDATOR
#include "Net/DNSClient.h"

namespace Net
{
	namespace Email
	{
		class EmailValidator
		{
		public:
			enum Status
			{
				S_VALID,
				S_INVALID_FORMAT,
				S_NO_SUCH_ADDR,
				S_FROM_NOT_ACCEPT,
				S_CONN_ERROR,
				S_COMM_ERROR,
				S_DOMAIN_NOT_RESOLVED
			};
		private:
			Net::SocketFactory *sockf;
			Net::DNSClient *dnsClient;
		public:
			EmailValidator(Net::SocketFactory *sockf);
			~EmailValidator();

			Status Validate(const UTF8Char *emailAddr);
			static const UTF8Char *StatusGetName(Status status);
		};
	}
}
#endif