#ifndef _SM_NET_SSHMANAGER
#define _SM_NET_SSHMANAGER
#include "Net/SocketFactory.h"
#include "Net/SSHClient.h"

namespace Net
{
	class SSHManager
	{
	private:
		static UInt32 useCnt;
		NN<Net::SocketFactory> sockf;
		Int32 error;
	public:
		SSHManager(NN<Net::SocketFactory> sockf);
		~SSHManager();

		Bool IsError() const;
		Int32 GetError() const;
		Optional<Net::SSHClient> CreateClient(Text::CStringNN host, UInt16 port, Text::CStringNN userName, Text::CStringNN password);
		Optional<Net::SSHConn> CreateConn(Text::CStringNN host, UInt16 port, Data::Duration timeout);

		static Text::CStringNN ErrorGetName(Int32 errorCode);
	};
}
#endif
