#ifndef _SM_IO_DBUSCLIENT
#define _SM_IO_DBUSCLIENT
#include "IO/DBusManager.h"

namespace IO
{
	class DBusClient
	{
	public:
		struct PropInfo;
		struct ProxyData;
		typedef void (* ClientFunction) (IO::DBusClient *client, void *user_data);
		typedef void (* ProxyFunction) (ProxyData *proxy, void *user_data);
		typedef void (* PropertyFunction) (ProxyData *proxy, const Char *name, void *iter, void *user_data);
	private:
		struct ClassData;
		ClassData *clsData;

		static void OnServiceConnect(IO::DBusManager *dbusManager, void *userData);
		static void OnServiceDisconnect(IO::DBusManager *dbusManager, void *userData);
		static Bool OnInterfacesAdded(IO::DBusManager *dbusManager, IO::DBusManager::Message *message, void *userData);
		static Bool OnInterfacesRemoved(IO::DBusManager *dbusManager, IO::DBusManager::Message *message, void *userData);
		static Bool OnPropertiesChanged(IO::DBusManager *dbusManager, IO::DBusManager::Message *message, void *userData);

		void IterAppendIter(void *baseIt, void *itera);
		PropInfo *PropEntryNew(const Char *name, void *itera);
		void PropEntryUpdate(PropInfo *prop, void *itera);

		void ParseInterfaces(const Char *path, void *itera);
		void ParseProperties(const Char *path, const Char *interface, void *itera);
		void UpdateProperties(ProxyData *proxy, void *itera, Bool sendChanged);
		void RefreshProperties();
		void GetAllProperties(ProxyData *proxy);
		void AddProperty(ProxyData *proxy, const Char *name, void *itera, Bool sendChanged);
		void GetManagedObjects();
		void ParseManagedObjects(void *mesg);

		ProxyData *ProxyNew(const Char *path, const Char *interface);
		void ProxyAdded(ProxyData *proxy);
		void ProxyFree(ProxyData *proxy);
		void ProxyRemove(const Char *path, const Char *interface);
		ProxyData *ProxyLookup(UIntOS *index, const Char *path, const Char *interface);
		ProxyData *ProxyRef(ProxyData *proxy);
		void ProxyUnref(ProxyData *proxy);
	public:
		DBusClient(IO::DBusManager *dbusMgr, const Char *service, const Char *path, const Char *rootPath);
		~DBusClient();

		DBusClient *Ref();
		void Unref();

		IO::DBusManager::HandlerResult MessageFilter(IO::DBusManager::Message *message);
		Bool ModifyMatch(const Char *member, const Char *rule);


//		void SetConnectWatch(connect_handler, NULL);
//		void SetDisconnectWatch(disconnect_handler, NULL);
//		void SetSignalWatch(message_handler, NULL);
//		void SetProxyHandlers(proxy_added, proxy_removed, property_changed, NULL);
//		void SetReadyWatch(client_ready, NULL);

		void GetAllPropertiesReply(void *pending, ProxyData *proxy);
		void GetManagedObjectsReply(void *pending);
		void ModifyMatchReply(void *pending);
	};
}
#endif
