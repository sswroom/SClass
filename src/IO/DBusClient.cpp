#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayList.h"
#include "Data/StringCMap.h"
#include "IO/DBusClient.h"
#include "Sync/Interlocked.h"
#include "Text/MyString.h"
#include "Text/StringBuilderC.h"
#include <dbus/dbus.h>

#ifndef DBUS_INTERFACE_OBJECT_MANAGER
#define DBUS_INTERFACE_OBJECT_MANAGER DBUS_INTERFACE_DBUS ".ObjectManager"
#endif

struct IO::DBusClient::PropInfo
{
	const Char *name;
	Int32 type;
	DBusMessage *msg;
};

struct IO::DBusClient::ProxyData
{
	Int32 refCount;
	IO::DBusClient *client;
	const Char *objPath;
	const Char *interface;
	Data::StringCMap<PropInfo*> *propList;
	UOSInt watch;
	PropertyFunction propFunc;
	void *propData;
	ProxyFunction removedFunc;
	void *removedData;
	DBusPendingCall *getAllCall;
	Bool pending;
};

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
	DBusPendingCall *pendingCall;
	DBusPendingCall *getObjectsCall;
	Bool connected;
	IO::DBusManager::MessageFunction signalFunc;
	void *signalData;
	IO::DBusManager::WatchFunction connectFunc;
	void *connectData;
	IO::DBusManager::WatchFunction disconnFunc;
	void *disconnData;
	IO::DBusClient::ProxyFunction proxyAdded;
	IO::DBusClient::ProxyFunction proxyRemoved;
	IO::DBusClient::ClientFunction ready;
	void *readyData;
	IO::DBusClient::PropertyFunction propertyChanged;
	void *userData;
	Data::ArrayList<IO::DBusClient::ProxyData*> *proxyList;
} ClassData;

static DBusHandlerResult DBusClient_MessageFilter(DBusConnection *connection, DBusMessage *message, void *userData)
{
	IO::DBusClient *me = (IO::DBusClient*)userData;
	IO::DBusManager::Message msg(message);
	return (DBusHandlerResult)me->MessageFilter(&msg);
}

void DBusClient_GetAllPropertiesReply(DBusPendingCall *pending, void *userObj)
{
	IO::DBusClient::ProxyData *proxy = (IO::DBusClient::ProxyData*)userObj;
	proxy->client->GetAllPropertiesReply(pending, proxy);
}

void DBusClient_GetManagedObjectsReply(DBusPendingCall *pending, void *userObj)
{
	((IO::DBusClient*)userObj)->GetManagedObjectsReply(pending);
}

void DBusClient_ModifyMatchReply(DBusPendingCall *pending, void *userObj)
{
	((IO::DBusClient*)userObj)->ModifyMatchReply(pending);
}

void IO::DBusClient::OnServiceConnect(IO::DBusManager *dbusManager, void *userData)
{
	IO::DBusClient *me = (IO::DBusClient*)userData;
	ClassData *client = (ClassData*)me->clsData;

	me->Ref();

	client->connected = true;

	me->GetManagedObjects();

	if (client->connectFunc)
		client->connectFunc(dbusManager, client->connectData);

	me->Unref();
}

void IO::DBusClient::OnServiceDisconnect(IO::DBusManager *dbusManager, void *userData)
{
	IO::DBusClient *me = (IO::DBusClient*)userData;
	ClassData *client = (ClassData*)me->clsData;

	client->connected = false;

	LIST_FREE_FUNC(client->proxyList, me->ProxyFree);

	if (client->disconnFunc)
		client->disconnFunc(dbusManager, client->disconnData);
}

Bool IO::DBusClient::OnInterfacesAdded(IO::DBusManager *dbusManager, IO::DBusManager::Message *message, void *userData)
{
	IO::DBusClient *me = (IO::DBusClient*)userData;
	ClassData *client = (ClassData*)me->clsData;

	DBusMessageIter iter;
	const Char *path;

	if (dbus_message_iter_init((DBusMessage*)message->GetHandle(), &iter) == FALSE)
		return TRUE;

	if (dbus_message_iter_get_arg_type(&iter) != DBUS_TYPE_OBJECT_PATH)
		return TRUE;

	dbus_message_iter_get_basic(&iter, &path);
	dbus_message_iter_next(&iter);

	me->Ref();

	me->ParseInterfaces(path, &iter);

	me->Unref();

	return TRUE;
}

Bool IO::DBusClient::OnInterfacesRemoved(IO::DBusManager *dbusManager, IO::DBusManager::Message *message, void *userData)
{
	IO::DBusClient *me = (IO::DBusClient*)userData;
	ClassData *client = (ClassData*)me->clsData;
	DBusMessageIter iter;
	DBusMessageIter entry;
	const Char *path;

	if (dbus_message_iter_init((DBusMessage*)message->GetHandle(), &iter) == FALSE)
		return TRUE;

	if (dbus_message_iter_get_arg_type(&iter) != DBUS_TYPE_OBJECT_PATH)
		return TRUE;

	dbus_message_iter_get_basic(&iter, &path);
	dbus_message_iter_next(&iter);

	if (dbus_message_iter_get_arg_type(&iter) != DBUS_TYPE_ARRAY)
		return TRUE;

	dbus_message_iter_recurse(&iter, &entry);

	me->Ref();

	while (dbus_message_iter_get_arg_type(&entry) == DBUS_TYPE_STRING) {
		const Char *interface;

		dbus_message_iter_get_basic(&entry, &interface);
		me->ProxyRemove(path, interface);
		dbus_message_iter_next(&entry);
	}

	me->Unref();

	return TRUE;
}

Bool IO::DBusClient::OnPropertiesChanged(IO::DBusManager *dbusManager, IO::DBusManager::Message *message, void *userData)
{
	ProxyData *proxy = (ProxyData*)userData;
	IO::DBusClient *me = proxy->client;
	ClassData *client = (ClassData*)me->clsData;
	DBusMessageIter iter, entry;
	const char *interface;

	if (dbus_message_iter_init((DBusMessage*)message->GetHandle(), &iter) == FALSE)
		return true;

	if (dbus_message_iter_get_arg_type(&iter) != DBUS_TYPE_STRING)
		return true;

	dbus_message_iter_get_basic(&iter, &interface);
	dbus_message_iter_next(&iter);

	me->UpdateProperties(proxy, &iter, true);

	dbus_message_iter_next(&iter);

	if (dbus_message_iter_get_arg_type(&iter) != DBUS_TYPE_ARRAY)
		return TRUE;

	dbus_message_iter_recurse(&iter, &entry);

	while (dbus_message_iter_get_arg_type(&entry) == DBUS_TYPE_STRING)
	{
		const char *name;

		dbus_message_iter_get_basic(&entry, &name);

		proxy->propList->Remove(name);

		if (proxy->propFunc)
			proxy->propFunc(proxy, name, NULL, proxy->propData);

		if (client->propertyChanged)
			client->propertyChanged(proxy, name, NULL, client->userData);

		dbus_message_iter_next(&entry);
	}

	return true;
}

void IO::DBusClient::IterAppendIter(void *baseIt, void *itera)
{
	DBusMessageIter *base = (DBusMessageIter*)baseIt;
	DBusMessageIter *iter = (DBusMessageIter*)itera;
	Int32 type;

	type = dbus_message_iter_get_arg_type(iter);

	if (dbus_type_is_basic(type))
	{
		const void *value;

		dbus_message_iter_get_basic(iter, &value);
		dbus_message_iter_append_basic(base, type, &value);
	}
	else if (dbus_type_is_container(type))
	{
		DBusMessageIter iter_sub, base_sub;
		char *sig;

		dbus_message_iter_recurse(iter, &iter_sub);

		switch (type)
		{
		case DBUS_TYPE_ARRAY:
		case DBUS_TYPE_VARIANT:
			sig = dbus_message_iter_get_signature(&iter_sub);
			break;
		default:
			sig = NULL;
			break;
		}

		dbus_message_iter_open_container(base, type, sig, &base_sub);

		if (sig != NULL)
			dbus_free(sig);

		while (dbus_message_iter_get_arg_type(&iter_sub) != DBUS_TYPE_INVALID)
		{
			this->IterAppendIter(&base_sub, &iter_sub);
			dbus_message_iter_next(&iter_sub);
		}

		dbus_message_iter_close_container(base, &base_sub);
	}
}

IO::DBusClient::PropInfo *IO::DBusClient::PropEntryNew(const Char *name, void *itera)
{
	DBusMessageIter *iter = (DBusMessageIter*)itera;
	PropInfo *prop;

	prop = MemAlloc(PropInfo, 1);
	if (prop == NULL)
		return NULL;

	MemClear(prop, sizeof(PropInfo));
	prop->name = Text::StrCopyNew(name);
	prop->type = dbus_message_iter_get_arg_type(iter);

	this->PropEntryUpdate(prop, iter);

	return prop;
}

void IO::DBusClient::PropEntryUpdate(PropInfo *prop, void *itera)
{
	DBusMessageIter *iter = (DBusMessageIter*)itera;
	DBusMessage *msg;
	DBusMessageIter base;

	msg = dbus_message_new(DBUS_MESSAGE_TYPE_METHOD_RETURN);
	if (msg == NULL)
		return;

	dbus_message_iter_init_append(msg, &base);
	this->IterAppendIter(&base, iter);

	if (prop->msg != NULL)
		dbus_message_unref(prop->msg);

	prop->msg = dbus_message_copy(msg);
	dbus_message_unref(msg);	
}

void IO::DBusClient::ParseInterfaces(const Char *path, void *itera)
{
	DBusMessageIter *iter = (DBusMessageIter*)itera;
	DBusMessageIter dict;

	if (dbus_message_iter_get_arg_type(iter) != DBUS_TYPE_ARRAY)
		return;

	dbus_message_iter_recurse(iter, &dict);

	while (dbus_message_iter_get_arg_type(&dict) == DBUS_TYPE_DICT_ENTRY)
	{
		DBusMessageIter entry;
		const Char *interface;

		dbus_message_iter_recurse(&dict, &entry);

		if (dbus_message_iter_get_arg_type(&entry) != DBUS_TYPE_STRING)
			break;

		dbus_message_iter_get_basic(&entry, &interface);
		dbus_message_iter_next(&entry);

		this->ParseProperties(path, interface, &entry);

		dbus_message_iter_next(&dict);
	}	
}

void IO::DBusClient::ParseProperties(const Char *path, const Char *interface, void *itera)
{
	DBusMessageIter *iter = (DBusMessageIter*)itera;
	ProxyData *proxy;

	if (Text::StrEquals(interface, DBUS_INTERFACE_INTROSPECTABLE))
		return;

	if (Text::StrEquals(interface, DBUS_INTERFACE_PROPERTIES))
		return;

	proxy = this->ProxyLookup(0, path, interface);
	if (proxy && !proxy->pending)
	{
		this->UpdateProperties(proxy, iter, false);
		return;
	}

	if (!proxy)
	{
		proxy = this->ProxyNew(path, interface);
		if (proxy == NULL)
			return;
	}

	this->UpdateProperties(proxy, iter, false);

	this->ProxyAdded(proxy);
}

void IO::DBusClient::UpdateProperties(ProxyData *proxy, void *itera, Bool sendChanged)
{
	DBusMessageIter *iter = (DBusMessageIter*)itera;
	DBusMessageIter dict;

	if (dbus_message_iter_get_arg_type(iter) != DBUS_TYPE_ARRAY)
		return;

	dbus_message_iter_recurse(iter, &dict);

	while (dbus_message_iter_get_arg_type(&dict) == DBUS_TYPE_DICT_ENTRY) {
		DBusMessageIter entry;
		const char *name;

		dbus_message_iter_recurse(&dict, &entry);

		if (dbus_message_iter_get_arg_type(&entry) != DBUS_TYPE_STRING)
			break;

		dbus_message_iter_get_basic(&entry, &name);
		dbus_message_iter_next(&entry);

		this->AddProperty(proxy, name, &entry, sendChanged);

		dbus_message_iter_next(&dict);
	}
}

void IO::DBusClient::RefreshProperties()
{
	ClassData *client = (ClassData*)this->clsData;
	UOSInt i = client->proxyList->GetCount();
	ProxyData *proxy;
	while (i-- > 0)
	{
		proxy = client->proxyList->GetItem(i);
		if (proxy->pending)
		{
			this->GetAllProperties(proxy);
		}
	}
}

void IO::DBusClient::GetAllProperties(ProxyData *proxy)
{
	ClassData *client = (ClassData*)proxy;
	const Char *serviceName = client->serviceName;
	DBusMessage *msg;

	if (proxy->getAllCall)
		return;

	msg = dbus_message_new_method_call(serviceName, proxy->objPath, DBUS_INTERFACE_PROPERTIES, "GetAll");
	if (msg == NULL)
		return;

	dbus_message_append_args(msg, DBUS_TYPE_STRING, &proxy->interface, DBUS_TYPE_INVALID);

	if (!client->dbusMgr->SendMessageWithReply(msg, (void**)&proxy->getAllCall, -1))
	{
		dbus_message_unref(msg);
		return;
	}

	dbus_pending_call_set_notify(proxy->getAllCall, DBusClient_GetAllPropertiesReply, proxy, NULL);

	dbus_message_unref(msg);
}

void IO::DBusClient::AddProperty(ProxyData *proxy, const Char *name, void *itera, Bool sendChanged)
{
	DBusMessageIter *iter = (DBusMessageIter*)itera;
	ClassData *client = (ClassData*)this->clsData;
	DBusMessageIter value;
	PropInfo *prop;

	if (dbus_message_iter_get_arg_type(iter) != DBUS_TYPE_VARIANT)
		return;

	dbus_message_iter_recurse(iter, &value);

	prop = proxy->propList->Get(name);
	if (prop != 0)
	{
		this->PropEntryUpdate(prop, &value);
	}
	else
	{
		prop = this->PropEntryNew(name, &value);
		if (prop == NULL)
			return;
		proxy->propList->Put(prop->name, prop);
	}

	if (proxy->propFunc)
		proxy->propFunc(proxy, name, &value, proxy->propData);

	if (!sendChanged)
		return;

	if (client->propertyChanged)
		client->propertyChanged(proxy, name, &value, client->userData);
}

void IO::DBusClient::GetManagedObjects()
{
	ClassData *client = (ClassData*)this->clsData;
	DBusMessage *msg;

	if (!client->connected)
		return;

	if ((!client->proxyAdded && !client->proxyRemoved) || !client->rootPath)
	{
		this->RefreshProperties();
		return;
	}

	if (client->getObjectsCall != NULL)
		return;

	msg = dbus_message_new_method_call(client->serviceName, client->rootPath, DBUS_INTERFACE_OBJECT_MANAGER, "GetManagedObjects");
	if (msg == NULL)
		return;

	dbus_message_append_args(msg, DBUS_TYPE_INVALID);

	if (!client->dbusMgr->SendMessageWithReply(msg, (void**)&client->getObjectsCall, -1))
	{
		dbus_message_unref(msg);
		return;
	}

	dbus_pending_call_set_notify(client->getObjectsCall, DBusClient_GetManagedObjectsReply, client, NULL);

	dbus_message_unref(msg);
}

void IO::DBusClient::ParseManagedObjects(void *mesg)
{
	DBusMessage *msg = (DBusMessage*)mesg;
	DBusMessageIter iter;
	DBusMessageIter dict;

	if (dbus_message_iter_init(msg, &iter) == FALSE)
		return;

	if (dbus_message_iter_get_arg_type(&iter) != DBUS_TYPE_ARRAY)
		return;

	dbus_message_iter_recurse(&iter, &dict);

	while (dbus_message_iter_get_arg_type(&dict) == DBUS_TYPE_DICT_ENTRY)
	{
		DBusMessageIter entry;
		const Char *path;

		dbus_message_iter_recurse(&dict, &entry);

		if (dbus_message_iter_get_arg_type(&entry) != DBUS_TYPE_OBJECT_PATH)
			break;

		dbus_message_iter_get_basic(&entry, &path);
		dbus_message_iter_next(&entry);

		this->ParseInterfaces(path, &entry);

		dbus_message_iter_next(&dict);
	}
}


IO::DBusClient::ProxyData *IO::DBusClient::ProxyNew(const Char *path, const Char *interface)
{
	ClassData *client = (ClassData*)this->clsData;
	ProxyData *proxy;

	proxy = MemAlloc(ProxyData, 1);
	if (proxy == 0)
		return 0;
	MemClear(proxy, sizeof(ProxyData));
	proxy->client = this;
	proxy->objPath = Text::StrCopyNew(path);
	proxy->interface = Text::StrCopyNew(interface);

	NEW_CLASS(proxy->propList, Data::StringCMap<PropInfo*>());
	proxy->watch = client->dbusMgr->AddPropertiesWatch(client->serviceName, proxy->objPath, proxy->interface, OnPropertiesChanged, proxy, NULL);
	proxy->pending = TRUE;
	
	client->proxyList->Add(proxy);

	return this->ProxyRef(proxy);
}

void IO::DBusClient::ProxyAdded(ProxyData *proxy)
{
	if (!proxy->pending)
		return;

	ClassData *client = (ClassData*)this->clsData;
	if (client->proxyAdded)
		client->proxyAdded(proxy, client->userData);

	proxy->pending = false;
}

void IO::DBusClient::ProxyFree(ProxyData *proxy)
{
	if (proxy->client)
	{
		ClassData *client = (ClassData*)this->clsData;

		if (proxy->getAllCall != NULL) {
			dbus_pending_call_cancel(proxy->getAllCall);
			dbus_pending_call_unref(proxy->getAllCall);
			proxy->getAllCall = NULL;
		}

		if (client->proxyRemoved)
			client->proxyRemoved(proxy, client->userData);

		client->dbusMgr->RemoveWatch(proxy->watch);

		//g_hash_table_remove_all(proxy->prop_list);

		proxy->client = NULL;
	}

	if (proxy->removedFunc)
		proxy->removedFunc(proxy, proxy->removedData);

	this->ProxyUnref(proxy);
}

void IO::DBusClient::ProxyRemove(const Char *path, const Char *interface)
{
	ClassData *client = (ClassData*)this->clsData;
	ProxyData *proxy;
	UOSInt i = client->proxyList->GetCount();
	while (i-- > 0)
	{
		proxy = client->proxyList->GetItem(i);
		if (Text::StrEqualsN(proxy->interface, interface)&& Text::StrEqualsN(proxy->objPath, path))
		{
			client->proxyList->RemoveAt(i);
			this->ProxyFree(proxy);
			break;
		}
	}
}

IO::DBusClient::ProxyData *IO::DBusClient::ProxyLookup(UOSInt *index, const Char *path, const Char *interface)
{
	UOSInt i;
	UOSInt j;
	if (index)
	{
		i = *index;
	}
	else
	{
		i = 0;
		index = &i;
	}

	if (interface == 0)
	{
		return 0;
	}

	ProxyData *proxy;
	ClassData *client = (ClassData*)this->clsData;
	j = client->proxyList->GetCount();
	while (i < j)
	{
		proxy = client->proxyList->GetItem(i);
		i++;

		if (Text::StrEquals(proxy->interface, interface) && Text::StrEquals(proxy->objPath, path))
		{
			*index = i;
			return proxy;
		}
	}
	return 0;
}

IO::DBusClient::ProxyData *IO::DBusClient::ProxyRef(ProxyData *proxy)
{
	if (proxy == 0)
		return 0;

	Sync::Interlocked::Increment(&proxy->refCount);
	return proxy;
}

void IO::DBusClient::ProxyUnref(ProxyData *proxy)
{
	if (proxy == 0)
		return;

	if (Sync::Interlocked::Decrement(&proxy->refCount) > 0)
		return;

	if (proxy->getAllCall != NULL) {
		dbus_pending_call_cancel(proxy->getAllCall);
		dbus_pending_call_unref(proxy->getAllCall);
	}

	//g_hash_table_destroy(proxy->prop_list);

	SDEL_TEXT(proxy->objPath);
	SDEL_TEXT(proxy->interface);

	MemFree(proxy);
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
	if (dbus_connection_add_filter((DBusConnection*)dbusMgr->GetHandle(), DBusClient_MessageFilter, this, NULL) == FALSE)
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

	client->watch = client->dbusMgr->AddServiceWatch(service, OnServiceConnect, OnServiceDisconnect, this, 0);

	if (rootPath == 0)
	{
		this->Ref();
		return;
	}

	client->addedWatch = client->dbusMgr->AddSignalWatch(service, client->rootPath, DBUS_INTERFACE_OBJECT_MANAGER, "InterfacesAdded", OnInterfacesAdded, this, NULL);
	client->removedWatch = client->dbusMgr->AddSignalWatch(service, client->rootPath, DBUS_INTERFACE_OBJECT_MANAGER, "InterfacesRemoved", OnInterfacesRemoved, this, NULL);

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
	NEW_CLASS(client->proxyList, Data::ArrayList<ProxyData*>());

	this->Ref();
}

IO::DBusClient::~DBusClient()
{
	ClassData *data = (ClassData*)this->clsData;
	if (data)
	{
		if (data->pendingCall != NULL)
		{
			dbus_pending_call_cancel(data->pendingCall);
			dbus_pending_call_unref(data->pendingCall);
		}

		if (data->getObjectsCall != NULL)
		{
			dbus_pending_call_cancel(data->getObjectsCall);
			dbus_pending_call_unref(data->getObjectsCall);
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

		LIST_FREE_FUNC(data->proxyList, this->ProxyFree);
		DEL_CLASS(data->proxyList);

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

IO::DBusManager::HandlerResult IO::DBusClient::MessageFilter(IO::DBusManager::Message *message)
{
	ClassData *client = (ClassData*)this->clsData;
	const Char *sender;
	const Char *path;
	const Char *interface;

	if (message->GetType() != IO::DBusManager::MT_SIGNAL)
		return IO::DBusManager::HR_NOT_YET_HANDLED;

	sender = message->GetSender();
	if (sender == NULL)
		return IO::DBusManager::HR_NOT_YET_HANDLED;

	path = message->GetPath();
	interface = message->GetInterface();

	if (Text::StrStartsWith(path, client->basePath) == FALSE)
		return IO::DBusManager::HR_NOT_YET_HANDLED;

	if (Text::StrEquals(interface, DBUS_INTERFACE_PROPERTIES))
		return IO::DBusManager::HR_NOT_YET_HANDLED;

	if (client->signalFunc)
		client->signalFunc(client->dbusMgr, message, client->signalData);

	return IO::DBusManager::HR_NOT_YET_HANDLED;
}

Bool IO::DBusClient::ModifyMatch(const Char *member, const Char *rule)
{
	ClassData *data = (ClassData*)this->clsData;
	DBusMessage *msg;
	DBusPendingCall *call;

	msg = dbus_message_new_method_call(DBUS_SERVICE_DBUS, DBUS_PATH_DBUS, DBUS_INTERFACE_DBUS, member);
	if (msg == NULL)
		return false;

	dbus_message_append_args(msg, DBUS_TYPE_STRING, &rule, DBUS_TYPE_INVALID);

	if (!data->dbusMgr->SendMessageWithReply(msg, (void**)&call, -1))
	{
		dbus_message_unref(msg);
		return false;
	}

	dbus_pending_call_set_notify(call, DBusClient_ModifyMatchReply, this, NULL);
	dbus_pending_call_unref(call);

	dbus_message_unref(msg);

	return true;
}

void IO::DBusClient::GetAllPropertiesReply(void *pending, ProxyData *proxy)
{
	ClassData *client = (ClassData*)this->clsData;
	DBusMessage *reply = dbus_pending_call_steal_reply((DBusPendingCall*)pending);
	DBusMessageIter iter;
	DBusError error;

	this->Ref();

	dbus_error_init(&error);

	if (dbus_set_error_from_message(&error, reply) == TRUE)
	{
		dbus_error_free(&error);
	}
	else
	{
		dbus_message_iter_init(reply, &iter);
		this->UpdateProperties(proxy, &iter, FALSE);
	}

	this->ProxyAdded(proxy);

	dbus_message_unref(reply);

	dbus_pending_call_unref(proxy->getAllCall);
	proxy->getAllCall = NULL;

	this->Unref();
}

void IO::DBusClient::GetManagedObjectsReply(void *pending)
{
	ClassData *client = (ClassData*)this->clsData;
	DBusMessage *reply = dbus_pending_call_steal_reply((DBusPendingCall*)pending);
	DBusError error;

	this->Ref();

	dbus_error_init(&error);

	if (dbus_set_error_from_message(&error, reply) == TRUE)
	{
		dbus_error_free(&error);
	}
	else
	{
		this->ParseManagedObjects(reply);
	}

	if (client->ready)
		client->ready(this, client->readyData);

	dbus_message_unref(reply);

	dbus_pending_call_unref(client->getObjectsCall);
	client->getObjectsCall = NULL;

	this->RefreshProperties();

	this->Unref();
}

void IO::DBusClient::ModifyMatchReply(void *pending)
{

}
