#ifndef _SM_IO_DBUSCLIENT
#define _SM_IO_DBUSCLIENT
#include "IO/DBusManager.h"

namespace IO
{
	class DBusClient
	{
	private:
		void *clsData;
	public:
		DBusClient(IO::DBusManager *dbusMgr, const Char *service, const Char *path, const Char *rootPath);
		~DBusClient();

		DBusClient *Ref();
		void Unref();

		Bool ModifyMatch(const Char *member, const Char *rule);


//		void SetConnectWatch(connect_handler, NULL);
//		void SetDisconnectWatch(disconnect_handler, NULL);
//		void SetSignalWatch(message_handler, NULL);
//		void SetProxyHandlers(proxy_added, proxy_removed, property_changed, NULL);
//		void SetReadyWatch(client_ready, NULL);

	};
}
#endif
