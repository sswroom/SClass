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
		typedef void (* ProxyFunction) (NN<ProxyData> proxy, void *user_data);
		typedef void (* PropertyFunction) (NN<ProxyData> proxy, UnsafeArray<const UTF8Char> name, void *iter, void *user_data);
	private:
		struct ClassData;
		Optional<ClassData> clsData;

		static void OnServiceConnect(NN<IO::DBusManager> dbusManager, AnyType userData);
		static void OnServiceDisconnect(NN<IO::DBusManager> dbusManager, AnyType userData);
		static Bool OnInterfacesAdded(NN<IO::DBusManager> dbusManager, NN<IO::DBusManager::Message> message, AnyType userData);
		static Bool OnInterfacesRemoved(NN<IO::DBusManager> dbusManager, NN<IO::DBusManager::Message> message, AnyType userData);
		static Bool OnPropertiesChanged(NN<IO::DBusManager> dbusManager, NN<IO::DBusManager::Message> message, AnyType userData);

		void IterAppendIter(void *baseIt, void *itera);
		Optional<PropInfo> PropEntryNew(UnsafeArray<const Char> name, void *itera);
		void PropEntryUpdate(NN<PropInfo> prop, void *itera);

		void ParseInterfaces(const Char *path, void *itera);
		void ParseProperties(const Char *path, const Char *interface, void *itera);
		void UpdateProperties(NN<ProxyData> proxy, void *itera, Bool sendChanged);
		void RefreshProperties();
		void GetAllProperties(NN<ProxyData> proxy);
		void AddProperty(NN<ProxyData> proxy, UnsafeArray<const UTF8Char> name, void *itera, Bool sendChanged);
		void GetManagedObjects();
		void ParseManagedObjects(void *mesg);

		Optional<ProxyData> ProxyNew(const Char *path, const Char *interface);
		void ProxyAdded(NN<ProxyData> proxy);
		static void __stdcall  ProxyFree(NN<ProxyData> proxy);
		void ProxyRemove(const Char *path, const Char *interface);
		ProxyData *ProxyLookup(UIntOS *index, const Char *path, const Char *interface);
		NN<ProxyData> ProxyRef(NN<ProxyData> proxy);
		static void __stdcall ProxyUnref(NN<ProxyData> proxy);
	public:
		DBusClient(NN<IO::DBusManager> dbusMgr, UnsafeArray<const UTF8Char> service, UnsafeArrayOpt<const UTF8Char> path, UnsafeArrayOpt<const UTF8Char> rootPath);
		~DBusClient();

		DBusClient *Ref();
		void Unref();

		IO::DBusManager::HandlerResult MessageFilter(NN<IO::DBusManager::Message> message);
		Bool ModifyMatch(UnsafeArray<const UTF8Char> member, UnsafeArray<const UTF8Char> rule);


//		void SetConnectWatch(connect_handler, NULL);
//		void SetDisconnectWatch(disconnect_handler, NULL);
//		void SetSignalWatch(message_handler, NULL);
//		void SetProxyHandlers(proxy_added, proxy_removed, property_changed, NULL);
//		void SetReadyWatch(client_ready, NULL);

		void GetAllPropertiesReply(void *pending, NN<ProxyData> proxy);
		void GetManagedObjectsReply(void *pending);
		void ModifyMatchReply(void *pending);
	};
}
#endif
