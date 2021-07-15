#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayList.h"
#include "IO/DBusClient.h"
#include "Sync/Interlocked.h"
#include "Text/MyString.h"
#include "Text/StringBuilderC.h"
#include <dbus/dbus.h>

typedef struct
{
	Int32 refCount;
	IO::DBusManager *dbusMgr;
	DBusConnection *dbusConn;
	const Char *serviceName;
	const Char *basePath;
	const Char *rootPath;
	Data::ArrayList<const Char *> *matchRules;
	UOSInt watch;
	UOSInt addedWatch;
	UOSInt removedWatch;
	DBusPendingCall *pending_call;
	DBusPendingCall *get_objects_call;
	Bool connected;
	IO::DBusManager::MessageFunction signalFunc;
	void *signalData;
	IO::DBusManager::WatchFunction connectFunc;
	void *connectData;
	IO::DBusManager::WatchFunction disconnFunc;
	void *disconnData;
//	GDBusProxyFunction proxy_added;
//	GDBusProxyFunction proxy_removed;
//	GDBusClientFunction ready;
//	void *ready_data;
//	GDBusPropertyFunction property_changed;
//	void *user_data;
//	GList *proxy_list;
} ClassData;

static DBusHandlerResult DBusClient_MessageFilter(DBusConnection *connection, DBusMessage *message, void *userData)
{
	ClassData *client = (ClassData*)userData;
	const Char *sender;
	const Char *path;
	const Char *interface;

	if (dbus_message_get_type(message) != DBUS_MESSAGE_TYPE_SIGNAL)
		return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;

	sender = dbus_message_get_sender(message);
	if (sender == NULL)
		return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;

	path = dbus_message_get_path(message);
	interface = dbus_message_get_interface(message);

	if (Text::StrStartsWith(path, client->basePath) == FALSE)
		return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;

	if (Text::StrEquals(interface, DBUS_INTERFACE_PROPERTIES))
		return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;

	if (client->signalFunc)
		client->signalFunc(connection, message, client->signalData);

	return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
}


IO::DBusClient::DBusClient(IO::DBusManager *dbusMgr, const Char *service, const Char *path, const Char *rootPath)
{
	this->clsData = 0;
	if (dbusMgr == 0 || dbusMgr->GetHandle() == 0 || service == 0)
	{
		return;
	}
	ClassData *client = MemAlloc(ClassData, 1);
	MemClear(client, sizeof(ClassData));
	client->basePath = 0;
	if (dbus_connection_add_filter((DBusConnection*)dbusMgr->GetHandle(), DBusClient_MessageFilter, client, NULL) == FALSE)
	{
		MemFree(client);
		return;
	}
	UOSInt i;
	UOSInt j;
	this->clsData = client;
	client->refCount = 0;
	client->dbusMgr = dbusMgr->Ref();
	client->dbusConn = (DBusConnection*)dbusMgr->GetHandle();
	client->serviceName = Text::StrCopyNew(service);
	client->basePath = SCOPY_TEXT(path);
	client->rootPath = SCOPY_TEXT(rootPath);
	client->connected = false;

	NEW_CLASS(client->matchRules, Data::ArrayList<const Char*>());

	client->watch = g_dbus_add_service_watch(connection, service, service_connect, service_disconnect, client, NULL);

	if (rootPath == 0)
	{
		this->Ref();
		return;
	}

	client->addedWatch = g_dbus_add_signal_watch(connection, service, client->rootPath, DBUS_INTERFACE_OBJECT_MANAGER, "InterfacesAdded", interfaces_added, client, NULL);
	client->removedWatch = g_dbus_add_signal_watch(connection, service, client->rootPath, DBUS_INTERFACE_OBJECT_MANAGER, "InterfacesRemoved", interfaces_removed, client, NULL);

	Text::StringBuilderC sb;
	sb.Append("type='signal',sender='");
	sb.Append(client->serviceName);
	sb.Append("',path_namespace='");
	sb.Append(client->basePath);
	sb.Append("'");
	client->matchRules->Add(Text::StrCopyNew(sb.ToString()));

	i = 0;
	j = client->matchRules->GetCount();
	while (i < j)
	{
		this->ModifyMatch("AddMatch", client->matchRules->GetItem(i));
		i++;
	}

	this->Ref();
}

IO::DBusClient::~DBusClient()
{
	ClassData *data = (ClassData*)this->clsData;
	if (data)
	{
		if (data->pending_call != NULL)
		{
			dbus_pending_call_cancel(data->pending_call);
			dbus_pending_call_unref(data->pending_call);
		}

		if (data->get_objects_call != NULL) {
			dbus_pending_call_cancel(data->get_objects_call);
			dbus_pending_call_unref(data->get_objects_call);
		}

		UOSInt i;
		UOSInt j;
		i = 0;
		j = data->matchRules->GetCount();
		while (i < j)
		{
			this->ModifyMatch("RemoveMatch", data->matchRules->GetItem(i));
			Text::StrDelNew(data->matchRules->GetItem(i));
			i++;
		}
		DEL_CLASS(data->matchRules);
		dbus_connection_remove_filter(data->dbusConn, DBusClient_MessageFilter, data);

		//g_list_free_full(client->proxy_list, proxy_free);

		if (data->disconnFunc && data->connected)
			data->disconnFunc(data->dbusMgr, data->disconnData);

		data->dbusMgr->RemoveWatch(data->watch);
		data->dbusMgr->RemoveWatch(data->addedWatch);
		data->dbusMgr->RemoveWatch(data->removedWatch);

		data->dbusMgr->Unref();

		SDEL_TEXT(data->serviceName);
		SDEL_TEXT(data->basePath);
		SDEL_TEXT(data->rootPath);

		MemFree(data);
	}
}

IO::DBusClient *IO::DBusClient::Ref()
{
	ClassData *data = (ClassData*)this->clsData;
	if (data)
	{
		Sync::Interlocked::Increment(&data->refCount);
		return this;
	}
	return 0;
}

void IO::DBusClient::Unref()
{
	ClassData *data = (ClassData*)this->clsData;
	if (data)
	{
		if (Sync::Interlocked::Decrement(&data->refCount) == 0)
		{
			DEL_CLASS(this);
		}
	}
}

Bool IO::DBusClient::ModifyMatch(const Char *member, const Char *rule)
{
	ClassData *data = (ClassData*)this->clsData;

}
