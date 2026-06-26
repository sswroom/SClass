#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayListArr.hpp"
#include "Data/StringMapNN.hpp"
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
	UnsafeArray<const Char> name;
	Int32 type;
	DBusMessage *msg;
};

struct IO::DBusClient::ProxyData
{
	Int32 refCount;
	NN<IO::DBusClient> client;
	const Char *objPath;
	const Char *interface;
	Data::StringMapNN<PropInfo> *propList;
	UIntOS watch;
	PropertyFunction propFunc;
	void *propData;
	ProxyFunction removedFunc;
	void *removedData;
	DBusPendingCall *getAllCall;
	Bool pending;
};

struct IO::DBusClient::ClassData
{
	Int32 refCount;
	NN<IO::DBusManager> dbusMgr;
	DBusConnection *dbusConn;
	UnsafeArrayOpt<const UTF8Char> serviceName;
	const Char *basePath;
	UnsafeArrayOpt<const UTF8Char> rootPath;
	Data::ArrayListArr<const UTF8Char> *matchRules;
	UIntOS watch;
	UIntOS addedWatch;
	UIntOS removedWatch;
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
	Data::ArrayListNN<IO::DBusClient::ProxyData> *proxyList;
};

static DBusHandlerResult DBusClient_MessageFilter(DBusConnection *connection, DBusMessage *message, void *userData)
{
	IO::DBusClient *me = (IO::DBusClient*)userData;
	IO::DBusManager::Message msg(message);
	return (DBusHandlerResult)me->MessageFilter(msg);
}

void DBusClient_GetAllPropertiesReply(DBusPendingCall *pending, void *userObj)
{
	NN<IO::DBusClient::ProxyData> proxy;
	if (proxy.Set((IO::DBusClient::ProxyData*)userObj))
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

void IO::DBusClient::OnServiceConnect(NN<IO::DBusManager> dbusManager, AnyType userData)
{
	NN<IO::DBusClient> me = userData.GetNN<IO::DBusClient>();
	NN<ClassData> client;
	if (!me->clsData.SetTo(client))
		return;

	me->Ref();

	client->connected = true;

	me->GetManagedObjects();

	if (client->connectFunc)
		client->connectFunc(dbusManager, client->connectData);

	me->Unref();
}

void IO::DBusClient::OnServiceDisconnect(NN<IO::DBusManager> dbusManager, AnyType userData)
{
	NN<IO::DBusClient> me = userData.GetNN<IO::DBusClient>();
	NN<ClassData> client;
	if (!me->clsData.SetTo(client))
		return;

	client->connected = false;

	client->proxyList->FreeAll(ProxyFree);

	if (client->disconnFunc)
		client->disconnFunc(dbusManager, client->disconnData);
}

Bool IO::DBusClient::OnInterfacesAdded(NN<IO::DBusManager> dbusManager, NN<IO::DBusManager::Message> message, AnyType userData)
{
	NN<IO::DBusClient> me = userData.GetNN<IO::DBusClient>();

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

Bool IO::DBusClient::OnInterfacesRemoved(NN<IO::DBusManager> dbusManager, NN<IO::DBusManager::Message> message, AnyType userData)
{
	NN<IO::DBusClient> me = userData.GetNN<IO::DBusClient>();
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

Bool IO::DBusClient::OnPropertiesChanged(NN<IO::DBusManager> dbusManager, NN<IO::DBusManager::Message> message, AnyType userData)
{
	NN<ProxyData> proxy = userData.GetNN<ProxyData>();
	NN<IO::DBusClient> me = proxy->client;
	NN<ClassData> client;
	if (!me->clsData.SetTo(client))
		return false;
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

		proxy->propList->RemoveC(Text::CStringNN::FromPtr((const UTF8Char*)name));

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

Optional<IO::DBusClient::PropInfo> IO::DBusClient::PropEntryNew(UnsafeArray<const Char> name, void *itera)
{
	DBusMessageIter *iter = (DBusMessageIter*)itera;
	NN<PropInfo> prop;

	prop = MemAllocNN(PropInfo);

	prop.ZeroContent();
	prop->name = Text::StrCopyNew(name);
	prop->type = dbus_message_iter_get_arg_type(iter);

	this->PropEntryUpdate(prop, iter);

	return prop;
}

void IO::DBusClient::PropEntryUpdate(NN<PropInfo> prop, void *itera)
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
	NN<ProxyData> proxy;
	Optional<ProxyData> optProxy;

	if (Text::StrEquals(interface, DBUS_INTERFACE_INTROSPECTABLE))
		return;

	if (Text::StrEquals(interface, DBUS_INTERFACE_PROPERTIES))
		return;

	optProxy = this->ProxyLookup(0, path, interface);
	if (optProxy.SetTo(proxy) && !proxy->pending)
	{
		this->UpdateProperties(proxy, iter, false);
		return;
	}

	if (!optProxy.SetTo(proxy))
	{
		if (!this->ProxyNew(path, interface).SetTo(proxy))
			return;
	}

	this->UpdateProperties(proxy, iter, false);

	this->ProxyAdded(proxy);
}

void IO::DBusClient::UpdateProperties(NN<ProxyData> proxy, void *itera, Bool sendChanged)
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

		if (name)
		{
			this->AddProperty(proxy, (const UTF8Char*)name, &entry, sendChanged);
		}

		dbus_message_iter_next(&dict);
	}
}

void IO::DBusClient::RefreshProperties()
{
	NN<ClassData> client = this->clsData;
	UIntOS i = client->proxyList->GetCount();
	NN<ProxyData> proxy;
	while (i-- > 0)
	{
		proxy = client->proxyList->GetItemNoCheck(i);
		if (proxy->pending)
		{
			this->GetAllProperties(proxy);
		}
	}
}

void IO::DBusClient::GetAllProperties(NN<ProxyData> proxy)
{
	NN<ClassData> client = this->clsData;
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

	dbus_pending_call_set_notify(proxy->getAllCall, DBusClient_GetAllPropertiesReply, proxy.Ptr(), NULL);

	dbus_message_unref(msg);
}

void IO::DBusClient::AddProperty(NN<ProxyData> proxy, UnsafeArray<const UTF8Char> name, void *itera, Bool sendChanged)
{
	DBusMessageIter *iter = (DBusMessageIter*)itera;
	NN<ClassData> client;
	if (!this->clsData.SetTo(client))
		return;
	DBusMessageIter value;
	NN<PropInfo> prop;

	if (dbus_message_iter_get_arg_type(iter) != DBUS_TYPE_VARIANT)
		return;

	dbus_message_iter_recurse(iter, &value);

	if (proxy->propList->GetC(Text::CStringNN::FromPtr(name)).SetTo(prop))
	{
		this->PropEntryUpdate(prop, &value);
	}
	else
	{
		if (!this->PropEntryNew(name, &value).SetTo(prop))
			return;
		proxy->propList->PutC(Text::CStringNN::FromPtr(prop->name), prop);
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
	NN<ClassData> client;
	if (!this->clsData.SetTo(client))
		return;
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

	dbus_pending_call_set_notify(client->getObjectsCall, DBusClient_GetManagedObjectsReply, client.Ptr(), NULL);

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


Optional<IO::DBusClient::ProxyData> IO::DBusClient::ProxyNew(const Char *path, const Char *interface)
{
	NN<ClassData> client;
	if (!this->clsData.SetTo(client))
		return nullptr;
	NN<ProxyData> proxy;

	proxy = MemAllocNN(ProxyData);
	proxy.ZeroContent();
	proxy->client = this;
	proxy->objPath = Text::StrCopyNew(path);
	proxy->interface = Text::StrCopyNew(interface);

	NEW_CLASS(proxy->propList, Data::StringMapNN<PropInfo>());
	proxy->watch = client->dbusMgr->AddPropertiesWatch(client->serviceName, proxy->objPath, proxy->interface, OnPropertiesChanged, proxy.Ptr(), NULL);
	proxy->pending = TRUE;
	
	client->proxyList->Add(proxy);

	return this->ProxyRef(proxy);
}

void IO::DBusClient::ProxyAdded(NN<ProxyData> proxy)
{
	if (!proxy->pending)
		return;

	NN<ClassData> client;
	if (!this->clsData.SetTo(client))
		return;
	if (client->proxyAdded)
		client->proxyAdded(proxy, client->userData);

	proxy->pending = false;
}

void __stdcall IO::DBusClient::ProxyFree(NN<ProxyData> proxy)
{
	if (proxy->client)
	{
		NN<ClassData> client = this->clsData;

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

	ProxyUnref(proxy);
}

void IO::DBusClient::ProxyRemove(const Char *path, const Char *interface)
{
	NN<ClassData> client = this->clsData;
	NN<ProxyData> proxy;
	UIntOS i = client->proxyList->GetCount();
	while (i-- > 0)
	{
		proxy = client->proxyList->GetItemNoCheck(i);
		if (Text::StrEqualsN(proxy->interface, interface) && Text::StrEqualsN(proxy->objPath, path))
		{
			client->proxyList->RemoveAt(i);
			this->ProxyFree(proxy);
			break;
		}
	}
}

IO::DBusClient::ProxyData *IO::DBusClient::ProxyLookup(UIntOS *index, const Char *path, const Char *interface)
{
	UIntOS i;
	UIntOS j;
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
	ClassData *client = this->clsData;
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

NN<IO::DBusClient::ProxyData> IO::DBusClient::ProxyRef(NN<ProxyData> proxy)
{
	Sync::Interlocked::IncrementI32(proxy->refCount);
	return proxy;
}

void __stdcall IO::DBusClient::ProxyUnref(NN<ProxyData> proxy)
{
	if (Sync::Interlocked::DecrementI32(proxy->refCount) > 0)
		return;

	if (proxy->getAllCall != NULL) {
		dbus_pending_call_cancel(proxy->getAllCall);
		dbus_pending_call_unref(proxy->getAllCall);
	}

	//g_hash_table_destroy(proxy->prop_list);

	SDEL_TEXT(proxy->objPath);
	SDEL_TEXT(proxy->interface);

	MemFreeNN(proxy);
}

IO::DBusClient::DBusClient(NN<IO::DBusManager> dbusMgr, UnsafeArray<const UTF8Char> service, UnsafeArrayOpt<const UTF8Char> path, UnsafeArrayOpt<const UTF8Char> rootPath)
{
	this->clsData = nullptr;
	if (dbusMgr->GetHandle() == 0)
	{
		return;
	}
	NN<ClassData> client = MemAllocNN(ClassData);
	client.ZeroContent();
	client->basePath = 0;
	if (dbus_connection_add_filter((DBusConnection*)dbusMgr->GetHandle(), DBusClient_MessageFilter, this, NULL) == FALSE)
	{
		MemFreeNN(client);
		return;
	}
	UIntOS i;
	UIntOS j;
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

	client->addedWatch = client->dbusMgr->AddSignalWatch(service, client->rootPath, CSTR(DBUS_INTERFACE_OBJECT_MANAGER).v, U8STR("InterfacesAdded"), OnInterfacesAdded, this, NULL);
	client->removedWatch = client->dbusMgr->AddSignalWatch(service, client->rootPath, CSTR(DBUS_INTERFACE_OBJECT_MANAGER).v, U8STR("InterfacesRemoved"), OnInterfacesRemoved, this, NULL);

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
	NN<ClassData> data;
	if (this->clsData.SetTo(data))
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

		UIntOS i;
		UIntOS j;
		i = 0;
		j = data->matchRules->GetCount();
		while (i < j)
		{
			this->ModifyMatch(U8STR("RemoveMatch"), data->matchRules->GetItemNoCheck(i));
			Text::StrDelNew(data->matchRules->GetItemNoCheck(i));
			i++;
		}
		DEL_CLASS(data->matchRules);
		dbus_connection_remove_filter(data->dbusConn, DBusClient_MessageFilter, data.Ptr());

		NNLIST_CALL_FUNC(data->proxyList, this->ProxyFree);
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

		MemFreeNN(data);
	}
}

IO::DBusClient *IO::DBusClient::Ref()
{
	NN<ClassData> data;
	if (this->clsData.SetTo(data))
	{
		Sync::Interlocked::IncrementI32(data->refCount);
		return this;
	}
	return 0;
}

void IO::DBusClient::Unref()
{
	NN<ClassData> data;
	if (this->clsData.SetTo(data))
	{
		if (Sync::Interlocked::DecrementI32(data->refCount) == 0)
		{
			DEL_CLASS(this);
		}
	}
}

IO::DBusManager::HandlerResult IO::DBusClient::MessageFilter(NN<IO::DBusManager::Message> message)
{
	NN<ClassData> client;
	if (!this->clsData.SetTo(client))
		return IO::DBusManager::HR_NOT_YET_HANDLED;
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

Bool IO::DBusClient::ModifyMatch(UnsafeArray<const UTF8Char> member, UnsafeArray<const UTF8Char> rule)
{
	NN<ClassData> data;
	if (!this->clsData.SetTo(data))
		return false;
	DBusMessage *msg;
	DBusPendingCall *call;

	msg = dbus_message_new_method_call(DBUS_SERVICE_DBUS, DBUS_PATH_DBUS, DBUS_INTERFACE_DBUS, (const Char*)member.Ptr());
	if (msg == NULL)
		return false;

	const Char *ruleStr = (const Char*)rule.Ptr();
	dbus_message_append_args(msg, DBUS_TYPE_STRING, &ruleStr, DBUS_TYPE_INVALID);

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

void IO::DBusClient::GetAllPropertiesReply(void *pending, NN<ProxyData> proxy)
{
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
	NN<ClassData> client;
	if (!this->clsData.SetTo(client))
		return;
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
