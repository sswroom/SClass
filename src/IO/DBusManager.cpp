#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayList.h"
#include "IO/DBusManager.h"
#include "Sync/Interlocked.h"
#include "Text/MyString.h"
#include <dbus/dbus.h>
#include <glib.h>
#include <stdio.h>

typedef void (* GDBusWatchFunction) (DBusConnection *connection, void *user_data);

struct DBusManagerTimeoutHandler {
	guint id;
	DBusTimeout *timeout;
};

struct DBusManagerWatchInfo {
	guint id;
	DBusWatch *watch;
	DBusConnection *conn;
};

struct DBusManagerDisconnectData {
	GDBusWatchFunction function;
	void *user_data;
};

struct DBusManagerGenericData {
	unsigned int refcount;
	DBusConnection *conn;
	char *path;
	GSList *interfaces;
	GSList *objects;
	GSList *added;
	GSList *removed;
	guint process_id;
	gboolean pending_prop;
	char *introspect;
	struct generic_data *parent;
};

typedef struct
{
	Int32 refCount;
	DBusConnection *conn;
	DBusManagerGenericData *root;
	Data::ArrayList<IO::DBusManager::Listener*> *listeners;
} ClassData;


static gboolean DBusManager_DisconnectedSignal(DBusConnection *conn, DBusMessage *msg, void *data)
{
	struct DBusManagerDisconnectData *dc_data = (DBusManagerDisconnectData*)data;

	//error("Got disconnected from the system message bus");

	dc_data->function(conn, dc_data->user_data);

	dbus_connection_unref(conn);

	return TRUE;
}

static gboolean DBusManager_MessageDispatch(void *data)
{
	DBusConnection *conn = (DBusConnection*)data;

	/* Dispatch messages */
	while (dbus_connection_dispatch(conn) == DBUS_DISPATCH_DATA_REMAINS);

	dbus_connection_unref(conn);

	return FALSE;
}

static inline void DBusManager_QueueDispatch(DBusConnection *conn, DBusDispatchStatus status)
{
	if (status == DBUS_DISPATCH_DATA_REMAINS)
		g_idle_add(DBusManager_MessageDispatch, dbus_connection_ref(conn));
}

static gboolean DBusManager_WatchFunc(GIOChannel *chan, GIOCondition cond, gpointer data)
{
	struct DBusManagerWatchInfo *info = (DBusManagerWatchInfo*)data;
	unsigned int flags = 0;
	DBusDispatchStatus status;
	DBusConnection *conn;

	if (cond & G_IO_IN)  flags |= DBUS_WATCH_READABLE;
	if (cond & G_IO_OUT) flags |= DBUS_WATCH_WRITABLE;
	if (cond & G_IO_HUP) flags |= DBUS_WATCH_HANGUP;
	if (cond & G_IO_ERR) flags |= DBUS_WATCH_ERROR;

	/* Protect connection from being destroyed by dbus_watch_handle */
	conn = dbus_connection_ref(info->conn);

	dbus_watch_handle(info->watch, flags);

	status = dbus_connection_get_dispatch_status(conn);
	DBusManager_QueueDispatch(conn, status);

	dbus_connection_unref(conn);

	return TRUE;
}

static void DBusManager_WatchInfoFree(void *data)
{
	struct DBusManagerWatchInfo *info = (DBusManagerWatchInfo*)data;

	if (info->id > 0) {
		g_source_remove(info->id);
		info->id = 0;
	}

	dbus_connection_unref(info->conn);

	g_free(info);
}

static dbus_bool_t DBusManager_OnAddWatch(DBusWatch *watch, void *data)
{
	IO::DBusManager *me = (IO::DBusManager*)data;
	DBusConnection *conn = (DBusConnection*)me->GetHandle();
	GIOCondition cond = (GIOCondition)(G_IO_HUP | G_IO_ERR);
	GIOChannel *chan;
	struct DBusManagerWatchInfo *info;
	unsigned int flags;
	int fd;

	if (!dbus_watch_get_enabled(watch))
		return TRUE;

	info = g_new0(struct DBusManagerWatchInfo, 1);

	fd = dbus_watch_get_unix_fd(watch);
	chan = g_io_channel_unix_new(fd);

	info->watch = watch;
	info->conn = dbus_connection_ref(conn);

	dbus_watch_set_data(watch, info, DBusManager_WatchInfoFree);

	flags = dbus_watch_get_flags(watch);

	if (flags & DBUS_WATCH_READABLE) cond = (GIOCondition)(cond | G_IO_IN);
	if (flags & DBUS_WATCH_WRITABLE) cond = (GIOCondition)(cond | G_IO_OUT);

	info->id = g_io_add_watch(chan, cond, DBusManager_WatchFunc, info);

	g_io_channel_unref(chan);

	return TRUE;
}

static void DBusManager_OnRemoveWatch(DBusWatch *watch, void *data)
{
	if (dbus_watch_get_enabled(watch))
		return;

	/* will trigger watch_info_free() */
	dbus_watch_set_data(watch, NULL, NULL);
}

static void DBusManager_OnWatchToggled(DBusWatch *watch, void *data)
{
	/* Because we just exit on OOM, enable/disable is
	 * no different from add/remove */
	if (dbus_watch_get_enabled(watch))
		DBusManager_OnAddWatch(watch, data);
	else
		DBusManager_OnRemoveWatch(watch, data);
}

static gboolean DBusManager_TimeoutHandlerDispatch(gpointer data)
{
	struct DBusManagerTimeoutHandler *handler = (DBusManagerTimeoutHandler*)data;

	handler->id = 0;

	/* if not enabled should not be polled by the main loop */
	if (!dbus_timeout_get_enabled(handler->timeout))
		return FALSE;

	dbus_timeout_handle(handler->timeout);

	return FALSE;
}

static void DBusManager_TimeoutHandlerFree(void *data)
{
	struct DBusManagerTimeoutHandler *handler = (DBusManagerTimeoutHandler*)data;

	if (handler->id > 0) {
		g_source_remove(handler->id);
		handler->id = 0;
	}

	g_free(handler);
}

static dbus_bool_t DBusManager_OnAddTimeout(DBusTimeout *timeout, void *data)
{
	int interval = dbus_timeout_get_interval(timeout);
	struct DBusManagerTimeoutHandler *handler;

	if (!dbus_timeout_get_enabled(timeout))
		return TRUE;

	handler = g_new0(struct DBusManagerTimeoutHandler, 1);
	handler->timeout = timeout;
	dbus_timeout_set_data(timeout, handler, DBusManager_TimeoutHandlerFree);
	handler->id = g_timeout_add(interval, DBusManager_TimeoutHandlerDispatch,	handler);

	return TRUE;
}

static void DBusManager_OnRemoveTimeout(DBusTimeout *timeout, void *data)
{
	dbus_timeout_set_data(timeout, NULL, NULL);
}

static void DBusManager_OnTimeoutToggled(DBusTimeout *timeout, void *data)
{
	if (dbus_timeout_get_enabled(timeout))
		DBusManager_OnAddTimeout(timeout, data);
	else
		DBusManager_OnRemoveTimeout(timeout, data);
}


static void DBusManager_OnDispatchStatus(DBusConnection *conn, DBusDispatchStatus status, void *data)
{
	if (!dbus_connection_get_is_connected(conn))
		return;

	DBusManager_QueueDispatch(conn, status);
}

/*static struct DBusManagerGenericData *DBusManager_ObjectPathRef(DBusConnection *connection, const char *path)
{
	struct DBusManagerGenericData *data;
	if (dbus_connection_get_object_path_data(connection, path, (void **) &data) == TRUE)
	{
		if (data != NULL)
		{
			data->refcount++;
			return data;
		}
	}

	data = g_new0(struct DBusManagerGenericData, 1);
	data->conn = dbus_connection_ref(connection);
	data->path = g_strdup(path);
	data->refcount = 1;

	data->introspect = g_strdup(DBUS_INTROSPECT_1_0_XML_DOCTYPE_DECL_NODE "<node></node>");

	if (!dbus_connection_register_object_path(connection, path, &generic_table, data)) {
		dbus_connection_unref(data->conn);
		g_free(data->path);
		g_free(data->introspect);
		g_free(data);
		return NULL;
	}

	invalidate_parent_data(connection, path);

	add_interface(data, DBUS_INTERFACE_INTROSPECTABLE, introspect_methods, NULL, NULL, data, NULL);

	return data;
}

static void DBusManager_ObjectPathUnref(DBusConnection *connection, const char *path)
{
	struct DBusManagerGenericData *data = NULL;

	if (dbus_connection_get_object_path_data(connection, path, (void **) &data) == FALSE)
		return;

	if (data == NULL)
		return;

	data->refcount--;

	if (data->refcount > 0)
		return;

	remove_interface(data, DBUS_INTERFACE_INTROSPECTABLE);
	remove_interface(data, DBUS_INTERFACE_PROPERTIES);

	invalidate_parent_data(data->conn, data->path);

	dbus_connection_unregister_object_path(data->conn, data->path);
}

gboolean DBusManager_AttachObjectManager(ClassData *clsData)
{
	struct DBusManagerGenericData *data;

	data = DBusManager_ObjectPathRef(clsData->conn, "/");
	if (data == NULL)
		return FALSE;

	DBusManager_AddInterface(data, DBUS_INTERFACE_OBJECT_MANAGER, manager_methods, manager_signals, NULL, data, NULL);
	clsData->root = data;

	return TRUE;
}

gboolean DBusManager_detach_object_manager(ClassData *clsData)
{
	if (!g_dbus_unregister_interface(connection, "/", DBUS_INTERFACE_OBJECT_MANAGER))
		return FALSE;

	clsData->root = 0;
	return TRUE;
}*/

IO::DBusManager::DBusManager(DBusType dbType, const Char *name)
{
	ClassData *data = MemAlloc(ClassData, 1);
	this->clsData = data;
	data->refCount = 1;
	data->root = 0;
	data->listeners = 0;
	DBusBusType type;
	switch (dbType)
	{
		case DBT_SESSION:
			type = DBUS_BUS_SESSION;
			break;
		case DBT_STARTER:
			type = DBUS_BUS_STARTER;
			break;
		case DBT_SYSTEM:
		default:
			type = DBUS_BUS_SYSTEM;
			break;
	}
	data->conn = dbus_bus_get(type, 0);
	if (data->conn == 0)
	{
		return;
	}
	if (name)
	{
		int result = dbus_bus_request_name(data->conn, name, DBUS_NAME_FLAG_DO_NOT_QUEUE, 0);
		if (result != DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER)
		{
			dbus_connection_unref(data->conn);
			data->conn = 0;
			return;
		}
	}
	dbus_connection_set_watch_functions(data->conn, DBusManager_OnAddWatch, DBusManager_OnRemoveWatch, DBusManager_OnWatchToggled, this, 0);
	dbus_connection_set_timeout_functions(data->conn, DBusManager_OnAddTimeout, DBusManager_OnRemoveTimeout, DBusManager_OnTimeoutToggled, this, 0);
	dbus_connection_set_dispatch_status_function(data->conn, DBusManager_OnDispatchStatus, this, 0);

	DBusDispatchStatus status;
	status = dbus_connection_get_dispatch_status(data->conn);
	DBusManager_QueueDispatch(data->conn, status);

//	DBusManager_AttachObjectManager(data);
}

IO::DBusManager::~DBusManager()
{
	ClassData *data = (ClassData*)this->clsData;
	if (data->conn)
	{
		dbus_connection_unref(data->conn);
	}
	MemFree(data);
}

Bool IO::DBusManager::IsError()
{
	ClassData *data = (ClassData*)this->clsData;
	return data->conn == 0;
}

IO::DBusManager *IO::DBusManager::Ref()
{
	ClassData *data = (ClassData*)this->clsData;
	if (data)
	{
		Sync::Interlocked::Increment(&data->refCount);
	}
}

void IO::DBusManager::Unref()
{
	ClassData *data = (ClassData*)this->clsData;
	if (data)
	{
		if (Sync::Interlocked::Decrement(&data->refCount) <= 0)
		{
			DEL_CLASS(this);
		}
	}
}

void *IO::DBusManager::GetHandle()
{
	ClassData *data = (ClassData*)this->clsData;
	return data->conn;
}

void IO::DBusManager::ServiceDataFree(ServiceData *data)
{
	ListenerCallbacks *callback = data->callback;

	if (data->call)
		dbus_pending_call_unref((DBusPendingCall*)data->call);

	if (data->id)
		g_source_remove(data->id); ////////////////////////

	Text::StrDelNew(data->name);
	MemFree(data);

	callback->data = 0;
}

void IO::DBusManager::ListenerBuildRule(Listener *listener, Text::StringBuilderC *sb)
{
	sb->Append("type='signal'");
	const Char *sender = listener->name?listener->name:listener->owner;

	if (sender)
	{
		sb->Append(",sender='");
		sb->Append(sender);
		sb->AppendChar('\'', 1);
	}
	if (listener->path)
	{
		sb->Append(",path='");
		sb->Append(listener->path);
		sb->AppendChar('\'', 1);
	}
	if (listener->interface)
	{
		sb->Append(",interface='");
		sb->Append(listener->interface);
		sb->AppendChar('\'', 1);
	}
	if (listener->member)
	{
		sb->Append(",member='");
		sb->Append(listener->member);
		sb->AppendChar('\'', 1);
	}
	if (listener->argument)
	{
		sb->Append(",arg0='");
		sb->Append(listener->argument);
		sb->AppendChar('\'', 1);
	}
}

Bool IO::DBusManager::ListenerAddMatch(Listener *listener, void *hdlr)
{
	ClassData *data = (ClassData*)this->clsData;
	DBusError err;
	Text::StringBuilderC sb;

	this->ListenerBuildRule(listener, &sb);
	dbus_error_init(&err);

	dbus_bus_add_match(data->conn, sb.ToString(), &err);
	if (dbus_error_is_set(&err))
	{
		printf("Adding match rule \"%s\" failed: %s", sb.ToString(), err.message);
		dbus_error_free(&err);
		return FALSE;
	}

	///////////////////////////////////
	//listener->handleFunc = hdlr;
	listener->registered = true;

	return TRUE;
}

Bool IO::DBusManager::ListenerRemoveMatch(Listener *listener)
{
	ClassData *data = (ClassData*)this->clsData;
	DBusError err;
	Text::StringBuilderC sb;

	this->ListenerBuildRule(listener, &sb);
	dbus_error_init(&err);
	dbus_bus_remove_match(data->conn, sb.ToString(), &err);
	if (dbus_error_is_set(&err))
	{
		printf("Removing owner match rule for %s failed: %s\r\n", sb.ToString(), err.message);
		dbus_error_free(&err);
		return false;
	}
	return true;
}

IO::DBusManager::ListenerCallbacks *IO::DBusManager::ListenerFindCallback(Listener *listener, UOSInt id)
{
	ListenerCallbacks *cb;
	UOSInt i;
	if (listener->callbacks)
	{
		i = listener->callbacks->GetCount();
		while (i-- > 0)
		{
			cb = listener->callbacks->GetItem(i);
			if (cb->id == id)
			{
				return cb;
			}
		}
	}
	if (listener->processed)
	{
		i = listener->processed->GetCount();
		while (i-- > 0)
		{
			cb = listener->processed->GetItem(i);
			if (cb->id == id)
			{
				return cb;
			}
		}
	}
	return 0;
}

Bool IO::DBusManager::ListenerRemoveCallback(Listener *listener, ListenerCallbacks *cb)
{
	ClassData *data = (ClassData*)this->clsData;
	if (listener->callbacks) listener->callbacks->Remove(cb);
	if (listener->processed) listener->processed->Remove(cb);

	/* Cancel pending operations */
	if (cb->data)
	{
		if (cb->data->call)
			dbus_pending_call_cancel((DBusPendingCall*)cb->data->call);
		this->ServiceDataFree(cb->data);
	}

	if (cb->destroyFunc)
		cb->destroyFunc(cb->userData);

	MemFree(cb);

	/* Don't remove the filter if other callbacks exist or data is lock
	 * processing callbacks */
	if (listener->callbacks->GetCount() > 0 || listener->lock)
		return false;

	if (listener->registered && !this->ListenerRemoveMatch(listener))
		return false;

	data->listeners->Remove(listener);
	this->ListenerFree(listener);

	return TRUE;
}

void IO::DBusManager::ListenerFree(Listener *listener)
{
	ClassData *data = (ClassData*)this->clsData;
	if (data->listeners->GetCount() == 0)
		dbus_connection_remove_filter(data->conn, WatchMessageFilter, 0);

	UOSInt i = listener->callbacks->GetCount();
	while (i-- > 0)
	{
		MemFree(listener->callbacks->GetItem(i));
	}
	DEL_CLASS(listener->callbacks);
	this->RemoveWatch(listener->nameWatch);
	Text::StrDelNew(listener->name);
	Text::StrDelNew(listener->owner);
	Text::StrDelNew(listener->path);
	Text::StrDelNew(listener->interface);
	Text::StrDelNew(listener->member);
	Text::StrDelNew(listener->argument);
	MemFree(listener);
}

Bool IO::DBusManager::RemoveWatch(UOSInt id)
{
	ClassData *data = (ClassData*)this->clsData;
	if (id == 0)
		return false;

	if (data->listeners == 0)
	{
		return false;
	}

	UOSInt i = data->listeners->GetCount();
	while (i-- > 0)
	{
		Listener *listener = data->listeners->GetItem(i);
		ListenerCallbacks *cb = this->ListenerFindCallback(listener, id);
		if (cb)
		{
			this->ListenerRemoveCallback(listener, cb);
			return TRUE;
		}
	}

	return FALSE;
}

void IO::DBusManager::RemoveAllWatches()
{
	ClassData *data = (ClassData*)this->clsData;
	Listener *listener;
	ListenerCallbacks *cb;
	if (data->listeners)
	{
		UOSInt i = data->listeners->GetCount();
		UOSInt j;
		while (i-- > 0)
		{
			listener = data->listeners->RemoveAt(i);
			j = listener->callbacks->GetCount();
			while (j-- > 0)
			{
				cb = listener->callbacks->GetItem(j);
				if (cb->discFunc) cb->discFunc(this, cb->userData);
				if (cb->destroyFunc) cb->destroyFunc(cb->userData);
			}
			this->ListenerFree(listener);
		}
	}
}
