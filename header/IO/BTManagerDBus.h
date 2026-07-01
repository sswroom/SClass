#ifndef _SM_IO_BTMANAGERDBUS
#define _SM_IO_BTMANAGERDBUS
#include "IO/DBusClient.h"
#include "IO/DBusManager.h"

namespace IO
{
	class BTManagerDBus
	{
	private:
		Optional<IO::DBusClient> client;
		NN<IO::DBusManager> dbusMgr;
	public:
		BTManagerDBus();
		~BTManagerDBus();
	};
}
#endif
