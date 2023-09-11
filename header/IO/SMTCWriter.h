#ifndef _SM_IO_SMTCWRITER
#define _SM_IO_SMTCWRITER
#include "IO/FileStream.h"
#include "Net/TCPClient.h"
#include "Sync/Mutex.h"

namespace IO
{
	class SMTCWriter
	{
	private:
		IO::FileStream fs;
		Sync::Mutex mut;

	public:
		SMTCWriter(Text::CStringNN fileName);
		~SMTCWriter();

		Bool IsError() const;
		void TCPConnect(NotNullPtr<Net::TCPClient> cli);
		void TCPDisconnect(NotNullPtr<Net::TCPClient> cli);
		void TCPSend(NotNullPtr<Net::TCPClient> cli, const UInt8 *buff, UOSInt size);
		void TCPRecv(NotNullPtr<Net::TCPClient> cli, const UInt8 *buff, UOSInt size);
	};
}

#endif
