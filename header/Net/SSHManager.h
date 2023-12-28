#ifndef _SM_NET_SSHMANAGER
#define _SM_NET_SSHMANAGER
#include "Net/SocketFactory.h"
#include "Net/SSHClient.h"

namespace Net
{
	class SSHManager
	{
	private:
		NotNullPtr<Net::SocketFactory> sockf;
		Int32 error;
	public:
		SSHManager(NotNullPtr<Net::SocketFactory> sockf);
		~SSHManager();

		Bool IsError() const;
		Optional<Net::SSHClient> CreateClient(Text::CStringNN host, UInt16 port, Text::CStringNN userName, Text::CStringNN password);
		Optional<Net::SSHConn> CreateConn(Text::CStringNN host, UInt16 port, Data::Duration timeout);

		static Text::CStringNN ErrorGetName(Int32 errorCode);
	};
}
#endif
