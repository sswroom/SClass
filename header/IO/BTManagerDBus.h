#ifndef _SM_IO_BTMANAGERDBUS
#define _SM_IO_BTMANAGERDBUS
#include "IO/DBusClient.h"
#include "IO/DBusManager.h"

namespace IO
{
	class BTManagerDBus
	{
	private:
		IO::DBusClient *client;
		IO::DBusManager *dbusMgr;
	public:
		BTManagerDBus();
		~BTManagerDBus();
	};
}
#endif
