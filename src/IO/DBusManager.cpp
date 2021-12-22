#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayList.h"
#include "IO/DBusManager.h"
#include "Sync/Interlocked.h"
#include "Text/MyString.h"
#include "Text/XML.h"
#include <dbus/dbus.h>
#include <glib.h>
#include <stdio.h>

#define AUTHORITY_DBUS	"org.freedesktop.PolicyKit1"
#define AUTHORITY_INTF	"org.freedesktop.PolicyKit1.Authority"
#define AUTHORITY_PATH	"/org/freedesktop/PolicyKit1/Authority"
#define DBUS_ANNOTATE(name_, value_)				\
	"<annotation name=\"org.freedesktop.DBus." name_ "\" "	\
	"value=\"" value_ "\"/>"

#define DBUS_ANNOTATE_DEPRECATED \
	DBUS_ANNOTATE("Deprecated", "true")

#define DBUS_ANNOTATE_NOREPLY \
	DBUS_ANNOTATE("Method.NoReply", "true")

#ifndef DBUS_INTERFACE_OBJECT_MANAGER
#define DBUS_INTERFACE_OBJECT_MANAGER DBUS_INTERFACE_DBUS ".ObjectManager"
#endif

#define GDBUS_ARGS(args...) (const ArgInfo[]) { args, { } }
#define GDBUS_METHOD(_name, _in_args, _out_args, _function) _name, _function, (IO::DBusManager::MethodFlags)0, 0, _in_args, _out_args
#define GDBUS_SIGNAL(_name, _args) _name, (IO::DBusManager::SignalFlags)0, _args

struct DBusManagerTimeoutHandler {
	guint id;
	DBusTimeout *timeout;
};

struct IO::DBusManager::WatchInfo
{
	UInt32 id;
	DBusWatch *watch;
	IO::DBusManager *me;
	DBusConnection *conn;
};

struct DBusManagerDisconnectData {
	IO::DBusManager::WatchFunction function;
	void *userData;
};

struct IO::DBusManager::InterfaceData
{
	const Char *name;
	const MethodTable *methods;
	const SignalTable *signals;
	const PropertyTable *properties;
	Data::ArrayList<PropertyTable*> *pendingProp;
	void *userData;
	DestroyFunction destroy;	
};

struct IO::DBusManager::SecurityData
{
	UInt32 pending;
	DBusMessage *message;
	const MethodTable *method;
	void *ifaceUserData;
};

struct IO::DBusManager::BuiltinSecurityData
{
	IO::DBusManager *me;
	UInt32 pending;
};

struct IO::DBusManager::AuthorizationData
{
	PolkitFunction function;
	void *userData;
};

struct IO::DBusManager::ArgInfo
{
	const Char *name;
	const Char *signature;
};

struct IO::DBusManager::GenericData
{
	UOSInt refcount;
	IO::DBusManager *me;
	const Char *path;
	Data::ArrayList<InterfaceData*> *interfaces;
	Data::ArrayList<GenericData*> *objects;
	Data::ArrayList<InterfaceData*> *added;
	Data::ArrayList<const Char*> *removed;
	UInt32 processId;
	Bool pendingProp;
	const Char *introspect;
	IO::DBusManager::GenericData *parent;
};

struct IO::DBusManager::PropertyTable
{
	const Char *name;
	const Char *type;
	PropertyGetter get;
	PropertySetter set;
	PropertyExists exists;
	PropertyFlags flags;
};

struct IO::DBusManager::MethodTable
{
	const Char *name;
	MethodFunction function;
	MethodFlags flags;
	unsigned int privilege;
	const ArgInfo *inArgs;
	const ArgInfo *outArgs;
};

struct IO::DBusManager::SignalTable
{
	const Char *name;
	SignalFlags flags;
	const ArgInfo *args;
};

struct IO::DBusManager::SecurityTable
{
	UInt32 privilege;
	const Char *action;
	SecurityFlags flags;
	SecurityFunction function;
};

typedef struct
{
	Int32 refCount;
	DBusConnection *conn;
	IO::DBusManager::GenericData *root;
	Data::ArrayList<IO::DBusManager::Listener*> *listeners;
	UOSInt listenerId;
	Data::ArrayList<IO::DBusManager::GenericData*> *pending;
	IO::DBusManager::PropertyFlags globalFlags;
	UInt32 nextPending;
	Data::ArrayList<IO::DBusManager::SecurityData*> *pendingSecurity;
	const IO::DBusManager::SecurityTable *securityTable;
} ClassData;

const IO::DBusManager::MethodTable IO::DBusManager::managerMethods[] = {
	{ GDBUS_METHOD("GetManagedObjects", NULL,
		GDBUS_ARGS({ "objects", "a{oa{sa{sv}}}" }), GetObjects) },
	{ }
};

const IO::DBusManager::SignalTable IO::DBusManager::managerSignals[] = {
	{ GDBUS_SIGNAL("InterfacesAdded",
		GDBUS_ARGS({ "object", "o" },
				{ "interfaces", "a{sa{sv}}" })) },
	{ GDBUS_SIGNAL("InterfacesRemoved",
		GDBUS_ARGS({ "object", "o" }, { "interfaces", "as" })) },
	{ }
};

const IO::DBusManager::MethodTable IO::DBusManager::introspectMethods[] = {
	{ GDBUS_METHOD("Introspect", NULL,
			GDBUS_ARGS({ "xml", "s" }), Introspect) },
	{ }
};

static void DBusManager_GenericUnregister(DBusConnection *connection, void *user_data)
{
	IO::DBusManager::GenericData *data = (IO::DBusManager::GenericData*)user_data;
	data->me->GenericUnregister(data);
}

DBusHandlerResult DBusManager_GenericMessage(DBusConnection *connection, DBusMessage *message, void *user_data)
{
	IO::DBusManager::GenericData *data = (IO::DBusManager::GenericData*)user_data;
	IO::DBusManager::Message msg(message);
	return (DBusHandlerResult)data->me->GenericMessage(data, &msg);
}

static DBusObjectPathVTable DBusManager_genericTable = {
	.unregister_function	= DBusManager_GenericUnregister,
	.message_function	= DBusManager_GenericMessage,
};


/*static gboolean DBusManager_DisconnectedSignal(DBusConnection *conn, DBusMessage *msg, void *data)
{
	struct DBusManagerDisconnectData *dc_data = (DBusManagerDisconnectData*)data;

	//error("Got disconnected from the system message bus");

	dc_data->function(conn, dc_data->userData);

	dbus_connection_unref(conn);

	return TRUE;
}*/

static gboolean DBusManager_MessageDispatch(void *data)
{
	DBusConnection *conn = (DBusConnection*)data;

	/* Dispatch messages */
	while (dbus_connection_dispatch(conn) == DBUS_DISPATCH_DATA_REMAINS);

	dbus_connection_unref(conn);

	return FALSE;
}

static gboolean DBusManager_WatchFunc(GIOChannel *chan, GIOCondition cond, gpointer data)
{
	IO::DBusManager::WatchInfo *info = (IO::DBusManager::WatchInfo*)data;
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
	info->me->QueueDispatch(status);

	dbus_connection_unref(conn);

	return TRUE;
}

static void DBusManager_WatchInfoFree(void *data)
{
	IO::DBusManager::WatchInfo *info = (IO::DBusManager::WatchInfo*)data;

	if (info->id > 0) {
		g_source_remove(info->id);
		info->id = 0;
	}

	dbus_connection_unref(info->conn);

	MemFree(info);
}

static dbus_bool_t DBusManager_OnAddWatch(DBusWatch *watch, void *data)
{
	IO::DBusManager *me = (IO::DBusManager*)data;
	DBusConnection *conn = (DBusConnection*)me->GetHandle();
	GIOCondition cond = (GIOCondition)(G_IO_HUP | G_IO_ERR);
	GIOChannel *chan;
	IO::DBusManager::WatchInfo *info;
	unsigned int flags;
	int fd;

	if (!dbus_watch_get_enabled(watch))
		return TRUE;

	info = MemAlloc(IO::DBusManager::WatchInfo, 1);
	info->me = me;

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
	handler->id = g_timeout_add((guint)interval, DBusManager_TimeoutHandlerDispatch,	handler);

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
	IO::DBusManager *me = (IO::DBusManager*)data;
	if (!dbus_connection_get_is_connected(conn))
		return;

	me->QueueDispatch(status);
}

/*
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

gboolean DBusManager_detach_object_manager(ClassData *clsData)
{
	if (!g_dbus_unregister_interface(connection, "/", DBUS_INTERFACE_OBJECT_MANAGER))
		return FALSE;

	clsData->root = 0;
	return TRUE;
}*/

DBusHandlerResult DBusManager_WatchMessageFilter(DBusConnection *connection, DBusMessage *message, void *userData)
{
	IO::DBusManager *me = (IO::DBusManager*)userData;
	IO::DBusManager::Message msg(message);
	return (DBusHandlerResult)me->WatchMessageFilter(&msg);
}

IO::DBusManager::Message::Message(void *message)
{
	this->message = message;
	dbus_message_ref((DBusMessage*)this->message);
}

IO::DBusManager::Message::~Message()
{
	dbus_message_unref((DBusMessage*)this->message);
}

IO::DBusManager::MessageType IO::DBusManager::Message::GetType()
{
	switch (dbus_message_get_type((DBusMessage*)message))
	{
	default:
	case DBUS_MESSAGE_TYPE_INVALID:
		return MT_INVALID;
	case DBUS_MESSAGE_TYPE_METHOD_CALL:
		return MT_METHOD_CALL;
	case DBUS_MESSAGE_TYPE_METHOD_RETURN:
		return MT_METHOD_RETURN;
	case DBUS_MESSAGE_TYPE_ERROR:
		return MT_ERROR;
	case DBUS_MESSAGE_TYPE_SIGNAL:
		return MT_SIGNAL;
	}
}

const Char *IO::DBusManager::Message::GetSender()
{
	return dbus_message_get_sender((DBusMessage*)this->message);
}

const Char *IO::DBusManager::Message::GetPath()
{
	return dbus_message_get_path((DBusMessage*)this->message);

}

const Char *IO::DBusManager::Message::GetInterface()
{
	return dbus_message_get_interface((DBusMessage*)this->message);

}

const Char *IO::DBusManager::Message::GetMember()
{
	return dbus_message_get_member((DBusMessage*)this->message);

}

const Char *IO::DBusManager::Message::GetArguments()
{
	const Char *arg = 0;
	dbus_message_get_args((DBusMessage*)this->message, NULL, DBUS_TYPE_STRING, &arg, DBUS_TYPE_INVALID);
	return arg;
}

const Char *IO::DBusManager::Message::GetSignature()
{
	return dbus_message_get_signature((DBusMessage*)this->message);
}


Bool IO::DBusManager::Message::GetNoReply()
{
	return dbus_message_get_no_reply((DBusMessage*)this->message);
}

void *IO::DBusManager::Message::GetHandle()
{
	return this->message;
}

void IO::DBusManager::SetupDbusWithMainLoop()
{
	ClassData *data = (ClassData*)this->clsData;
	dbus_connection_set_watch_functions(data->conn, DBusManager_OnAddWatch, DBusManager_OnRemoveWatch, DBusManager_OnWatchToggled, this, 0);
	dbus_connection_set_timeout_functions(data->conn, DBusManager_OnAddTimeout, DBusManager_OnRemoveTimeout, DBusManager_OnTimeoutToggled, this, 0);
	dbus_connection_set_dispatch_status_function(data->conn, DBusManager_OnDispatchStatus, this, 0);
}

Bool IO::DBusManager::SetupBus(const Char *name)
{
	ClassData *data = (ClassData*)this->clsData;

	if (name)
	{
		int result = dbus_bus_request_name(data->conn, name, DBUS_NAME_FLAG_DO_NOT_QUEUE, 0);
		if (result != DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER)
		{
			dbus_connection_unref(data->conn);
			data->conn = 0;
			return false;
		}
	}

	DBusDispatchStatus status;
	status = dbus_connection_get_dispatch_status(data->conn);
	this->QueueDispatch(status);

	return true;
}

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
	NEW_CLASS(data->pending, Data::ArrayList<GenericData*>());
	data->nextPending = 1;
	NEW_CLASS(data->pendingSecurity, Data::ArrayList<IO::DBusManager::SecurityData*>());
	data->securityTable = 0;

	if (data->conn == 0)
	{
		return;
	}

	if (!this->SetupBus(name))
	{
		dbus_connection_unref(data->conn);
		data->conn = 0;
		return;
	}
}

IO::DBusManager::~DBusManager()
{
	ClassData *data = (ClassData*)this->clsData;
	if (data->conn)
	{
		dbus_connection_unref(data->conn);
	}
	DEL_CLASS(data->pending);
	DEL_CLASS(data->pendingSecurity);
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
	return this;
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

void IO::DBusManager::QueueDispatch(Int32 status)
{
	ClassData *data = (ClassData*)this->clsData;

	if (status == DBUS_DISPATCH_DATA_REMAINS)
		g_idle_add(DBusManager_MessageDispatch, dbus_connection_ref(data->conn));
}

void *IO::DBusManager::GetObjects(IO::DBusManager *dbusManager, Message *message, void *userData)
{
	GenericData *data = (GenericData*)userData;
	DBusMessage *reply;
	DBusMessageIter iter;
	DBusMessageIter array;

	reply = dbus_message_new_method_return((DBusMessage*)message->GetHandle());
	if (reply == NULL)
		return NULL;

	dbus_message_iter_init_append(reply, &iter);

	dbus_message_iter_open_container(&iter, DBUS_TYPE_ARRAY,
					DBUS_DICT_ENTRY_BEGIN_CHAR_AS_STRING
					DBUS_TYPE_OBJECT_PATH_AS_STRING
					DBUS_TYPE_ARRAY_AS_STRING
					DBUS_DICT_ENTRY_BEGIN_CHAR_AS_STRING
					DBUS_TYPE_STRING_AS_STRING
					DBUS_TYPE_ARRAY_AS_STRING
					DBUS_DICT_ENTRY_BEGIN_CHAR_AS_STRING
					DBUS_TYPE_STRING_AS_STRING
					DBUS_TYPE_VARIANT_AS_STRING
					DBUS_DICT_ENTRY_END_CHAR_AS_STRING
					DBUS_DICT_ENTRY_END_CHAR_AS_STRING
					DBUS_DICT_ENTRY_END_CHAR_AS_STRING,
					&array);

	UOSInt i = 0;
	UOSInt j = data->objects->GetCount();
	while (i < j)
	{
		dbusManager->AppendObject(data->objects->GetItem(i), &array);
		i++;
	}

	dbus_message_iter_close_container(&iter, &array);

	return reply;
}

void *IO::DBusManager::Introspect(IO::DBusManager *dbusManager, Message *message, void *userData)
{
	GenericData *data = (GenericData*)userData;
	DBusMessage *reply;

	if (data->introspect == NULL)
		dbusManager->GenerateIntrospectionXml(data, message->GetPath());

	reply = dbus_message_new_method_return((DBusMessage*)message->GetHandle());
	if (reply == NULL)
		return NULL;

	dbus_message_append_args(reply, DBUS_TYPE_STRING, &data->introspect, DBUS_TYPE_INVALID);
	return reply;
}

void IO::DBusManager::PrintArguments(Text::StringBuilderC *sb, const ArgInfo *args, const Char *direction)
{
	Text::String *s;
	while (args && args->name)
	{
		sb->Append("<arg name=");
		s = Text::XML::ToNewAttrText((const UTF8Char*)args->name);
		sb->Append((const Char*)s->v);
		s->Release();
		sb->Append(" type=");
		s = Text::XML::ToNewAttrText((const UTF8Char*)args->signature);
		sb->Append((const Char*)s->v);
		s->Release();

		if (direction)
		{
			sb->Append(" direction=");
			s = Text::XML::ToNewAttrText((const UTF8Char*)direction);
			sb->Append((const Char*)s->v);
			s->Release();
			sb->Append("/>\n");
		}
		else
		{
			sb->Append("/>\n");
		}
		args++;
	}
}

void IO::DBusManager::GenerateInterfaceXml(Text::StringBuilderC *sb, InterfaceData *iface)
{
	Text::String *s;
	const MethodTable *method;
	const SignalTable *signal;
	const PropertyTable *property;

	method = iface->methods;
	while (method && method->name)
	{
		if (!this->CheckExperimental(method->flags, MF_EXPERIMENTAL))
		{
			sb->Append("<method name=");
			s = Text::XML::ToNewAttrText((const UTF8Char*)method->name);
			sb->Append((const Char*)s->v);
			s->Release();
			sb->Append(">");
			this->PrintArguments(sb, method->inArgs, "in");
			this->PrintArguments(sb, method->outArgs, "out");

			if (method->flags & MF_DEPRECATED)
				sb->Append(DBUS_ANNOTATE_DEPRECATED);

			if (method->flags & MF_NOREPLY)
				sb->Append(DBUS_ANNOTATE_NOREPLY);

			sb->Append("</method>");
		}
		method++;
	}

	signal = iface->signals;
	while (signal && signal->name)
	{
		if (!this->CheckExperimental(signal->flags, SF_EXPERIMENTAL))
		{
			sb->Append("<signal name=");
			s = Text::XML::ToNewAttrText((const UTF8Char*)signal->name);
			sb->Append((const Char*)s->v);
			s->Release();
			sb->Append(">");
			this->PrintArguments(sb, signal->args, NULL);

			if (signal->flags & SF_DEPRECATED)
				sb->Append(DBUS_ANNOTATE_DEPRECATED);

			sb->Append("</signal>\n");
		}
		signal++;
	}

	property = iface->properties;
	while (property && property->name)
	{
		if (!this->CheckExperimental(property->flags, PF_EXPERIMENTAL))
		{
			sb->Append("<property name=");
			s = Text::XML::ToNewAttrText((const UTF8Char*)property->name);
			sb->Append((const Char*)s->v);
			s->Release();
			sb->Append(" type=");
			s = Text::XML::ToNewAttrText((const UTF8Char*)property->type);
			sb->Append((const Char*)s->v);
			s->Release();
			sb->Append(" access=\"");
			if (property->get) sb->Append("read");
			if (property->set) sb->Append("write");
			sb->Append("\">");

			if (property->flags & PF_DEPRECATED)
				sb->Append(DBUS_ANNOTATE_DEPRECATED);

			sb->Append("</property>");
		}
		property++;
	}
}

void IO::DBusManager::GenerateIntrospectionXml(GenericData *data, const Char *path)
{
	ClassData *clsData = (ClassData*)this->clsData;
	Text::String *s;
	Text::StringBuilderC sb;
	Char **children;

	SDEL_TEXT(data->introspect);

	sb.Append(DBUS_INTROSPECT_1_0_XML_DOCTYPE_DECL_NODE);
	sb.Append("<node>");

	UOSInt i = 0;
	UOSInt j = data->interfaces->GetCount();
	while (i < j)
	{
		InterfaceData *iface = data->interfaces->GetItem(i);

		sb.Append("<interface name=");
		s = Text::XML::ToNewAttrText((const UTF8Char*)iface->name);
		sb.Append((const Char*)s->v);
		s->Release();
		sb.Append(">");

		this->GenerateInterfaceXml(&sb, iface);

		sb.Append("</interface>");
		i++;
	}

	if (dbus_connection_list_registered(clsData->conn, path, &children))
	{
		i = 0;
		while (children[i])
		{
			sb.Append("<node name=");
			s = Text::XML::ToNewAttrText((const UTF8Char*)children[i]);
			sb.Append((const Char*)s->v);
			s->Release();
			sb.Append("/>");
			i++;
		}

		dbus_free_string_array(children);
	}
	sb.Append("</node>");
	data->introspect = Text::StrCopyNew(sb.ToString());
}

void IO::DBusManager::AppendInterfaces(GenericData *data, void *itera)
{
	DBusMessageIter *iter = (DBusMessageIter*)itera;
	DBusMessageIter array;

	dbus_message_iter_open_container(iter, DBUS_TYPE_ARRAY,
				DBUS_DICT_ENTRY_BEGIN_CHAR_AS_STRING
				DBUS_TYPE_STRING_AS_STRING
				DBUS_TYPE_ARRAY_AS_STRING
				DBUS_DICT_ENTRY_BEGIN_CHAR_AS_STRING
				DBUS_TYPE_STRING_AS_STRING
				DBUS_TYPE_VARIANT_AS_STRING
				DBUS_DICT_ENTRY_END_CHAR_AS_STRING
				DBUS_DICT_ENTRY_END_CHAR_AS_STRING, &array);

	UOSInt i = 0;
	UOSInt j = data->interfaces->GetCount();
	while (i < j)
	{
		this->AppendInterface(data->interfaces->GetItem(i), &array);
		i++;
	}

	dbus_message_iter_close_container(iter, &array);
}

void IO::DBusManager::AppendObject(GenericData *child, void *itera)
{
	DBusMessageIter *array = (DBusMessageIter*)itera;
	DBusMessageIter entry;

	dbus_message_iter_open_container(array, DBUS_TYPE_DICT_ENTRY, NULL, &entry);
	dbus_message_iter_append_basic(&entry, DBUS_TYPE_OBJECT_PATH, &child->path);
	this->AppendInterfaces(child, &entry);
	dbus_message_iter_close_container(array, &entry);

	UOSInt i = 0;
	UOSInt j = child->objects->GetCount();
	while (i < j)
	{
		this->AppendObject(child->objects->GetItem(i), array);
		i++;
	}
}

void IO::DBusManager::PendingSuccess(UInt32 pending)
{
	ClassData *clsData = (ClassData*)this->clsData;
	SecurityData *secdata;
	UOSInt i = 0;
	UOSInt j = clsData->pendingSecurity->GetCount();
	while (i < j)
	{
		secdata = clsData->pendingSecurity->GetItem(i);
		if (secdata->pending == pending)
		{
			clsData->pendingSecurity->RemoveAt(i);
			{
				IO::DBusManager::Message message(secdata->message);
				this->ProcessMessage(&message, secdata->method, secdata->ifaceUserData);
			}


			dbus_message_unref(secdata->message);
			MemFree(secdata);
		}
		i++;
	}
}

void *IO::DBusManager::CreateError(void *message, const Char *name, const Char *errMsg)
{
	if (dbus_message_get_no_reply((DBusMessage*)message))
		return 0;

	return dbus_message_new_error((DBusMessage*)message, name, errMsg?errMsg:"");
}

Bool IO::DBusManager::SendError(void *message, const Char *name, const Char *errMsg)
{
	void *error;
	error = this->CreateError(message, name, errMsg);
	if (error == NULL)
		return FALSE;

	return this->SendMessage(error);
}

void IO::DBusManager::PendingError(UInt32 pending, const Char *name, const Char *errMsg)
{
	ClassData *clsData = (ClassData*)this->clsData;
	SecurityData *secdata;
	UOSInt i = 0;
	UOSInt j = clsData->pendingSecurity->GetCount();
	while (i < j)
	{
		secdata = clsData->pendingSecurity->GetItem(i);
		if (secdata->pending == pending)
		{
			clsData->pendingSecurity->RemoveAt(i);
			this->SendError(secdata->message, name, errMsg);

			dbus_message_unref(secdata->message);
			MemFree(secdata);
		}
		i++;
	}
}

IO::DBusManager::GenericData *IO::DBusManager::ObjectPathRef(const Char *path)
{
	ClassData *clsData = (ClassData*)this->clsData;
	GenericData *data;
	if (dbus_connection_get_object_path_data(clsData->conn, path, (void **) &data) == TRUE)
	{
		if (data != NULL)
		{
			data->refcount++;
			return data;
		}
	}

	data = MemAlloc(GenericData, 1);
	MemClear(data, sizeof(GenericData));
	data->me = this;
	data->path = Text::StrCopyNew(path);
	data->refcount = 1;
	data->introspect = Text::StrCopyNew(DBUS_INTROSPECT_1_0_XML_DOCTYPE_DECL_NODE "<node></node>");

	if (!dbus_connection_register_object_path(clsData->conn, path, &DBusManager_genericTable, data))
	{
		GenericDataFree(data);
		return NULL;
	}
	NEW_CLASS(data->objects, Data::ArrayList<GenericData*>());
	NEW_CLASS(data->added, Data::ArrayList<InterfaceData*>());
	NEW_CLASS(data->interfaces, Data::ArrayList<InterfaceData*>());

	this->InvalidateParentData(path);

	this->AddInterface(data, DBUS_INTERFACE_INTROSPECTABLE, introspectMethods, NULL, NULL, data, NULL);

	return data;
}

Bool IO::DBusManager::AttachObjectManager()
{
	ClassData *clsData = (ClassData*)this->clsData;
	GenericData *data;

	data = this->ObjectPathRef("/");
	if (data == NULL)
		return FALSE;

	this->AddInterface(data, DBUS_INTERFACE_OBJECT_MANAGER, managerMethods, managerSignals, NULL, data, NULL);
	clsData->root = data;

	return TRUE;
}

void IO::DBusManager::GenericDataFree(GenericData *data)
{
	SDEL_CLASS(data->objects);
	SDEL_CLASS(data->interfaces);
	SDEL_CLASS(data->added);
	Text::StrDelNew(data->path);
	Text::StrDelNew(data->introspect);
	MemFree(data);
}

void IO::DBusManager::GenericUnregister(GenericData *data)
{
	GenericData *parent = data->parent;

	if (parent != NULL)
		parent->objects->Remove(data);

	if (data->processId > 0)
	{
		g_source_remove(data->processId);
		data->processId = 0;
		this->ProcessChanges(data);
	}

	UOSInt i = data->objects->GetCount();
	while (i-- > 0)
	{
		data->objects->GetItem(i)->parent = parent;
	}

	GenericDataFree(data);
}

IO::DBusManager::HandlerResult IO::DBusManager::GenericMessage(GenericData *data, Message *message)
{
	InterfaceData *iface;
	const MethodTable *method;
	const Char *interface;

	interface = dbus_message_get_interface((DBusMessage*)message->GetHandle());

	iface = this->FindInterface(data, interface);
	if (iface == NULL)
		return HR_NOT_YET_HANDLED;

	method = iface->methods;
	while (method && method->name && method->function)
	{

		if (dbus_message_is_method_call((DBusMessage*)message->GetHandle(), iface->name, method->name) == FALSE)
		{
			method++;
			continue;
		}

		if (this->CheckExperimental(method->flags, MF_EXPERIMENTAL))
			return HR_NOT_YET_HANDLED;

		if (!this->ArgsHaveSignature(method->inArgs, message))
		{
			method++;
			continue;
		}

		if (this->CheckPrivilege(message, method, iface->userData))
			return HR_HANDLED;

		return this->ProcessMessage(message, method, iface->userData);
	}

	return HR_NOT_YET_HANDLED;
}

IO::DBusManager::HandlerResult IO::DBusManager::ProcessMessage(Message *message, const MethodTable *method, void *ifaceUserData)
{
	void *reply;

	reply = method->function(this, message, ifaceUserData);

	if (method->flags & MF_NOREPLY || message->GetNoReply())
	{
		dbus_message_unref((DBusMessage*)reply);
		return HR_HANDLED;
	}

	if (method->flags & MF_ASYNC)
	{
		if (reply == NULL)
			return HR_HANDLED;
	}

	if (reply == NULL)
		return HR_NEED_MEMORY;

	this->SendMessage(reply);

	return HR_HANDLED;
}

IO::DBusManager::GenericData *IO::DBusManager::InvalidateParentData(const Char *childPath)
{
	ClassData *clsData = (ClassData*)this->clsData;
	GenericData *data = NULL;
	GenericData *child = NULL;
	GenericData *parent = NULL;
	Text::StringBuilderC sbParentPath;
	UOSInt i;

	sbParentPath.Append(childPath);
	i = sbParentPath.LastIndexOf('/');
	if (i == INVALID_INDEX)
	{
		return data;
	}

	if (i == 0 && sbParentPath.ToString()[1] != 0)
		sbParentPath.TrimToLength(1);
	else
		sbParentPath.TrimToLength((UOSInt)i);

	if (sbParentPath.GetLength() == 0)
	{
		return data;
	}

	if (dbus_connection_get_object_path_data(clsData->conn, sbParentPath.ToString(), (void **)&data) == FALSE)
	{
		return data;
	}

	parent = this->InvalidateParentData(sbParentPath.ToString());

	if (data == NULL)
	{
		data = parent;
		if (data == NULL)
		{
			return data;
		}
	}

	SDEL_TEXT(data->introspect);

	if (!dbus_connection_get_object_path_data(clsData->conn, childPath, (void **) &child))
	{
		return data;
	}

	if (child == NULL || data->objects->IndexOf(child) != INVALID_INDEX)
	{
		return data;
	}

	data->objects->Add(child);
	child->parent = data;
	return data;
}

void IO::DBusManager::AddPending(GenericData *data)
{
	ClassData *clsData = (ClassData*)this->clsData;
	UInt32 oldId = data->processId;

	data->processId = g_idle_add((GSourceFunc)ProcessChanges, data);

	if (oldId > 0)
	{
		g_source_remove(oldId);
		return;
	}

	clsData->pending->Add(data);
}

IO::DBusManager::InterfaceData *IO::DBusManager::FindInterface(GenericData *data, const Char *name)
{
	if (name == 0)
		return 0;

	UOSInt i = data->interfaces->GetCount();
	while (i-- > 0)
	{
		InterfaceData *iface = data->interfaces->GetItem(i);
		if (Text::StrEquals(name, iface->name)) //!strcmp ?
		{
			return iface;
		}
	}
	return 0;
}

Bool IO::DBusManager::AddInterface(GenericData *data, const Char *name, const MethodTable *methods, const SignalTable *signals, const PropertyTable *properties, void *userData, DestroyFunction destroy)
{
	InterfaceData *iface;
	const MethodTable *method;
	const SignalTable *signal;
	const PropertyTable *property;
	Bool found = false;
	method = methods;
	if (!found && method)
	{
		while (method->name)
		{
			if (!this->CheckExperimental(method->flags, MF_EXPERIMENTAL))
			{
				found = true;
				break;
			}
			method++;
		}
	}

	signal = signals;
	if (!found && signal)
	{
		while (signal->name)
		{
			if (!this->CheckExperimental(signal->flags, SF_EXPERIMENTAL))
			{
				found = true;
				break;
			}
			signal++;
		}
	}

	property = properties;
	if (!found && property)
	{
		while (property->name)
		{
			if (!this->CheckExperimental(property->flags, PF_EXPERIMENTAL))
			{
				found = true;
				break;
			}
			property++;
		}
	}

	if (!found)
	{
		return false;
	}

	iface = MemAlloc(InterfaceData, 1);
	MemClear(iface, sizeof(InterfaceData));
	iface->name = Text::StrCopyNew(name);
	iface->methods = methods;
	iface->signals = signals;
	iface->properties = properties;
	iface->userData = userData;
	iface->destroy = destroy;

	data->interfaces->Add(iface);
	if (data->parent == NULL)
		return true;

	data->added->Add(iface);

	this->AddPending(data);

	return true;
}

Bool IO::DBusManager::ArgsHaveSignature(const ArgInfo *args, Message *message)
{
	const Char *sig = message->GetSignature();
	const Char *p = NULL;

	while (args && args->signature && *sig)
	{
		p = args->signature;

		while (*sig && *p)
		{
			if (*p != *sig)
				return FALSE;
			sig++;
			p++;
		}
		args++;
	}

	if (*sig || (p && *p) || (args && args->signature))
		return FALSE;

	return TRUE;
}

void IO::DBusManager::BuiltinSecurityResult(Bool authorized, void *userData)
{
	BuiltinSecurityData *data = (BuiltinSecurityData*)userData;

	if (authorized == true)
		data->me->PendingSuccess(data->pending);
	else
		data->me->PendingError(data->pending, DBUS_ERROR_AUTH_FAILED, NULL);

	MemFree(data);
}

void IO::DBusManager::BuiltinSecurityFunction(const Char *action, Bool interaction, UInt32 pending)
{
	BuiltinSecurityData *data;

	data = MemAlloc(BuiltinSecurityData, 1);
	data->me = this;
	data->pending = pending;

	if (this->PolkitCheckAuthorization(action, interaction, BuiltinSecurityResult, data, 30000) != ET_SUCCESS)
		this->PendingError(pending, NULL, NULL);
}

Bool IO::DBusManager::CheckPrivilege(Message *message, const MethodTable *method, void *ifaceUserData)
{
	ClassData *clsData = (ClassData*)this->clsData;
	const SecurityTable *security;

	security = clsData->securityTable;
	while (security && security->privilege)
	{
		SecurityData *secdata;
		Bool interaction;

		if (security->privilege != method->privilege)
		{
			security++;
			continue;
		}

		secdata = MemAlloc(SecurityData, 1);
		secdata->pending = clsData->nextPending++;
		secdata->message = dbus_message_ref((DBusMessage*)message->GetHandle());
		secdata->method = method;
		secdata->ifaceUserData = ifaceUserData;
		clsData->pendingSecurity->Add(secdata);

		if (security->flags & SECF_ALLOW_INTERACTION)
			interaction = true;
		else
			interaction = false;

		if (!(security->flags & SECF_BUILTIN) && security->function)
			security->function(this, security->action, interaction, secdata->pending);
		else
			this->BuiltinSecurityFunction(security->action, interaction, secdata->pending);

		return true;
	}

	return false;
}

Bool IO::DBusManager::CheckSignal(const Char *path, const Char *interface, const Char *name, const ArgInfo **args)
{
	ClassData *clsData = (ClassData*)this->clsData;
	GenericData *data = NULL;
	InterfaceData *iface;
	const SignalTable *signal;

	*args = NULL;
	if (!dbus_connection_get_object_path_data(clsData->conn, path, (void **) &data) || data == NULL)
	{
		printf("dbus_connection_emit_signal: path %s isn't registered\r\n", path);
		return false;
	}

	iface = this->FindInterface(data, interface);
	if (iface == NULL)
	{
		printf("dbus_connection_emit_signal: %s does not implement %s\r\n", path, interface);
		return false;
	}

	signal = iface->signals;
	while (signal && signal->name)
	{
		if (!Text::StrEquals(signal->name, name))
		{
			signal++;
			continue;			
		}

		if (signal->flags & SF_EXPERIMENTAL)
		{
			const Char *env = g_getenv("GDBUS_EXPERIMENTAL");
			if (!Text::StrEquals(env, "1"))
				break;
		}

		*args = signal->args;
		return true;
	}

	printf("No signal named %s on interface %s\r\n", name, interface);
	return false;
}

Bool IO::DBusManager::CheckExperimental(Int32 flags, Int32 flag)
{
	ClassData *data = (ClassData*)this->clsData;
	if (!(flags & flag))
		return false;
	
	return !(data->globalFlags & PF_EXPERIMENTAL);
}

void IO::DBusManager::AppendName(const Char *name, void *itera)
{
	DBusMessageIter *iter = (DBusMessageIter*)itera;

	dbus_message_iter_append_basic(iter, DBUS_TYPE_STRING, &name);
}

void IO::DBusManager::AppendProperty(InterfaceData *iface, const PropertyTable *p, void *itera)
{
	DBusMessageIter *dict = (DBusMessageIter*)itera;
	DBusMessageIter entry;
	DBusMessageIter value;

	dbus_message_iter_open_container(dict, DBUS_TYPE_DICT_ENTRY, NULL, &entry);
	dbus_message_iter_append_basic(&entry, DBUS_TYPE_STRING, &p->name);
	dbus_message_iter_open_container(&entry, DBUS_TYPE_VARIANT, p->type, &value);

	p->get(p, &value, iface->userData);

	dbus_message_iter_close_container(&entry, &value);
	dbus_message_iter_close_container(dict, &entry);
}

void IO::DBusManager::AppendProperties(InterfaceData *iface, void *itera)
{
	DBusMessageIter *iter = (DBusMessageIter*)itera;
	DBusMessageIter dict;
	const PropertyTable *p;

	dbus_message_iter_open_container(iter, DBUS_TYPE_ARRAY,
				DBUS_DICT_ENTRY_BEGIN_CHAR_AS_STRING
				DBUS_TYPE_STRING_AS_STRING
				DBUS_TYPE_VARIANT_AS_STRING
				DBUS_DICT_ENTRY_END_CHAR_AS_STRING, &dict);

	p = iface->properties;
	while (p && p->name)
	{
		if (this->CheckExperimental(p->flags, PF_EXPERIMENTAL))
			continue;

		if (p->get == NULL)
			continue;

		if (p->exists != NULL && !p->exists(p, iface->userData))
			continue;

		this->AppendProperty(iface, p, &dict);

		p++;
	}

	dbus_message_iter_close_container(iter, &dict);
}

void IO::DBusManager::AppendInterface(InterfaceData *iface, void *itera)
{
	DBusMessageIter *array = (DBusMessageIter*)itera;
	DBusMessageIter entry;

	dbus_message_iter_open_container(array, DBUS_TYPE_DICT_ENTRY, NULL, &entry);
	dbus_message_iter_append_basic(&entry, DBUS_TYPE_STRING, &iface->name);
	this->AppendProperties(iface, &entry);
	dbus_message_iter_close_container(array, &entry);
}

void IO::DBusManager::ProcessPropertyChanges(GenericData *data)
{
	InterfaceData *iface;
	data->pendingProp = FALSE;
	if (data->interfaces)
	{
		UOSInt i = data->interfaces->GetCount();
		while (i-- > 0)
		{
			iface = data->interfaces->GetItem(i);
			this->ProcessPropertiesFromInterface(data, iface);
		}
	}
}

void IO::DBusManager::ProcessPropertiesFromInterface(GenericData *data, InterfaceData *iface)
{
	ClassData *clsData = (ClassData*)this->clsData;
	DBusMessage *signal;
	DBusMessageIter iter;
	DBusMessageIter dict;
	DBusMessageIter array;
	Data::ArrayList<PropertyTable*> invalidated;

	if (iface->pendingProp == NULL)
		return;

	signal = dbus_message_new_signal(data->path, DBUS_INTERFACE_PROPERTIES, "PropertiesChanged");
	if (signal == NULL)
	{
		printf("Unable to allocate new " DBUS_INTERFACE_PROPERTIES ".PropertiesChanged signal\r\n");
		return;
	}

	dbus_message_iter_init_append(signal, &iter);
	dbus_message_iter_append_basic(&iter, DBUS_TYPE_STRING,	&iface->name);
	dbus_message_iter_open_container(&iter, DBUS_TYPE_ARRAY,
			DBUS_DICT_ENTRY_BEGIN_CHAR_AS_STRING
			DBUS_TYPE_STRING_AS_STRING DBUS_TYPE_VARIANT_AS_STRING
			DBUS_DICT_ENTRY_END_CHAR_AS_STRING, &dict);

	UOSInt i = iface->pendingProp->GetCount();
	while (i-- > 0)
	{
		PropertyTable *p = iface->pendingProp->GetItem(i);

		if (p->get == NULL)
			continue;

		if (p->exists != NULL && !p->exists(p, iface->userData))
		{
			invalidated.Add(p);
			continue;
		}

		this->AppendProperty(iface, p, &dict);
	}

	dbus_message_iter_close_container(&iter, &dict);

	dbus_message_iter_open_container(&iter, DBUS_TYPE_ARRAY, DBUS_TYPE_STRING_AS_STRING, &array);
	i = invalidated.GetCount();
	while (i-- > 0)
	{
		PropertyTable *p = invalidated.GetItem(i);

		dbus_message_iter_append_basic(&array, DBUS_TYPE_STRING, &p->name);
	}
	dbus_message_iter_close_container(&iter, &array);
	SDEL_CLASS(iface->pendingProp);

	dbus_connection_send(clsData->conn, signal, NULL);
	dbus_message_unref(signal);
}

void IO::DBusManager::EmitInterfacesAdded(GenericData *data)
{
	ClassData *clsData = (ClassData*)this->clsData;
	DBusMessage *signal;
	DBusMessageIter iter, array;

	if (clsData->root == NULL || data == clsData->root)
		return;

	signal = dbus_message_new_signal(clsData->root->path, DBUS_INTERFACE_OBJECT_MANAGER, "InterfacesAdded");
	if (signal == NULL)
		return;

	dbus_message_iter_init_append(signal, &iter);
	dbus_message_iter_append_basic(&iter, DBUS_TYPE_OBJECT_PATH, &data->path);

	dbus_message_iter_open_container(&iter, DBUS_TYPE_ARRAY,
				DBUS_DICT_ENTRY_BEGIN_CHAR_AS_STRING
				DBUS_TYPE_STRING_AS_STRING
				DBUS_TYPE_ARRAY_AS_STRING
				DBUS_DICT_ENTRY_BEGIN_CHAR_AS_STRING
				DBUS_TYPE_STRING_AS_STRING
				DBUS_TYPE_VARIANT_AS_STRING
				DBUS_DICT_ENTRY_END_CHAR_AS_STRING
				DBUS_DICT_ENTRY_END_CHAR_AS_STRING, &array);

	UOSInt i = data->added->GetCount();
	while (i-- > 0)
	{
		this->AppendInterface(data->added->GetItem(i), &array);
	}
	SDEL_CLASS(data->added);

	dbus_message_iter_close_container(&iter, &array);

	dbus_connection_send(clsData->conn, signal, NULL);
	dbus_message_unref(signal);	
}

void IO::DBusManager::EmitInterfacesRemoved(GenericData *data)
{
	ClassData *clsData = (ClassData*)this->clsData;
	DBusMessage *signal;
	DBusMessageIter iter;
	DBusMessageIter array;

	if (clsData->root == NULL || data == clsData->root)
		return;

	signal = dbus_message_new_signal(clsData->root->path, DBUS_INTERFACE_OBJECT_MANAGER, "InterfacesRemoved");
	if (signal == NULL)
		return;

	dbus_message_iter_init_append(signal, &iter);
	dbus_message_iter_append_basic(&iter, DBUS_TYPE_OBJECT_PATH, &data->path);
	dbus_message_iter_open_container(&iter, DBUS_TYPE_ARRAY, DBUS_TYPE_STRING_AS_STRING, &array);

	UOSInt i = data->removed->GetCount();
	while (i-- > 0)
	{
		this->AppendName(data->removed->GetItem(i), &array);		
	}
	LIST_FREE_FUNC(data->removed, Text::StrDelNew);
	DEL_CLASS(data->removed);

	dbus_message_iter_close_container(&iter, &array);

	dbus_connection_send(clsData->conn, signal, NULL);
	dbus_message_unref(signal);
}

void IO::DBusManager::Flush()
{
	ClassData *clsData = (ClassData*)this->clsData;
	GenericData *data;
	UOSInt i = clsData->pending->GetCount();
	while (i-- > 0)
	{
		data = clsData->pending->GetItem(i);
		ProcessChanges(data);
	}
}

void IO::DBusManager::RemovePending(GenericData *data)
{
	if (data->processId > 0)
	{
		g_source_remove(data->processId);
		data->processId = 0;
	}

	ClassData *clsData = (ClassData*)this->clsData;
	clsData->pending->Remove(data);
}

Bool IO::DBusManager::ProcessChanges(void *userData)
{
	GenericData *data = (GenericData*)userData;
	IO::DBusManager *me = data->me;

	me->RemovePending(data);

	if (data->added != NULL)
		me->EmitInterfacesAdded(data);

	if (data->pendingProp)
		me->ProcessPropertyChanges(data);

	if (data->removed != NULL)
		me->EmitInterfacesRemoved(data);

	data->processId = 0;

	return FALSE;

}

Bool IO::DBusManager::SendMessageWithReply(void *message, void **call, Int32 timeout)
{
	ClassData *data = (ClassData*)this->clsData;
	Bool ret;

	this->Flush();

	ret = dbus_connection_send_with_reply(data->conn, (DBusMessage*)message, (DBusPendingCall**)call, timeout);

	if (ret == TRUE && call != NULL && *call == NULL)
	{
		printf("Unable to send message (passing fd blocked?)\r\n");
		return false;
	}

	return ret;
}

Bool IO::DBusManager::SendMessage(void *msg)
{
	ClassData *data = (ClassData*)this->clsData;
	DBusMessage *message = (DBusMessage*)msg;
	Bool result = false;
	Bool skip = false;

	if (message == 0)
		return false;

	if (dbus_message_get_type(message) == DBUS_MESSAGE_TYPE_METHOD_CALL)
		dbus_message_set_no_reply(message, TRUE);
	else if (dbus_message_get_type(message) == DBUS_MESSAGE_TYPE_SIGNAL)
	{
		const char *path = dbus_message_get_path(message);
		const char *interface = dbus_message_get_interface(message);
		const char *name = dbus_message_get_member(message);
		const ArgInfo *args;

		if (!this->CheckSignal(path, interface, name, &args))
		{
			skip = true;
		}
	}

	if (!skip)
	{
		this->Flush();

		result = dbus_connection_send(data->conn, message, NULL);
	}
	dbus_message_unref(message);

	return result;
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

Bool IO::DBusManager::UpdateService(void *userObj)
{
	ServiceData *data = (ServiceData*)userObj;
	IO::DBusManager *me = data->me;
	ListenerCallbacks *cb = data->callback;
		
	me->ServiceDataFree(data);

	if (cb->connFunc)
		cb->connFunc(me, cb->userData);

	return false;
}

void IO::DBusManager::ServiceReply(void *pending, void *userObj)
{
	ServiceData *data = (ServiceData*)userObj;
	IO::DBusManager *me = data->me;
	DBusMessage *reply;
	DBusError err;

	reply = dbus_pending_call_steal_reply((DBusPendingCall*)pending);
	if (reply == NULL)
		return;

	dbus_error_init(&err);

	if (dbus_set_error_from_message(&err, reply) ||
		dbus_message_get_args(reply, &err, DBUS_TYPE_STRING, &data->owner, DBUS_TYPE_INVALID) == FALSE)
	{
		printf("%s\n", err.message);
		dbus_error_free(&err);
		me->ServiceDataFree(data);
	}
	else
	{
		me->UpdateService(data);
	}
	dbus_message_unref(reply);
}

IO::DBusManager::HandlerResult IO::DBusManager::ServiceFilter(void *connection, void *message, void *userData)
{
	Listener *listener = (Listener*)userData;
	IO::DBusManager *me = listener->me;
	ListenerCallbacks *cb;
	const Char *name;
	const Char *oldName;
	const Char *newName;

	if (!dbus_message_get_args((DBusMessage*)message, NULL, DBUS_TYPE_STRING, &name, DBUS_TYPE_STRING, &oldName, DBUS_TYPE_STRING, &newName, DBUS_TYPE_INVALID))
	{
		printf("Invalid arguments for NameOwnerChanged signal\r\n");
		return HR_NOT_YET_HANDLED;
	}

	me->ListenerUpdateNameCache(name, newName);

	UOSInt i = listener->callbacks->GetCount();
	while (i-- > 0)
	{
		cb = listener->callbacks->GetItem(i);

		if (*newName == '\0')
		{
			if (cb->discFunc)
				cb->discFunc(me, cb->userData);
		} else {
			if (cb->connFunc)
				cb->connFunc(me, cb->userData);
		}

		if (listener->callbacks->IndexOf(cb) == INVALID_INDEX)
			continue;

		/* Only auto remove if it is a bus name watch */
		if (listener->argument[0] == ':' && (cb->connFunc == NULL || cb->discFunc == NULL))
		{
			if (me->ListenerRemoveCallback(listener, cb))
				break;

			continue;
		}

		listener->callbacks->Remove(cb);
		listener->processed->Remove(cb);
	}

	return HR_NOT_YET_HANDLED;
}

IO::DBusManager::HandlerResult IO::DBusManager::SignalFilter(void *connection, void *message, void *userData)
{
	Listener *listener = (Listener*)userData;
	IO::DBusManager *me = listener->me;
	ListenerCallbacks *cb;

	UOSInt i = listener->callbacks->GetCount();
	while (i-- > 0)
	{
		cb = listener->callbacks->GetItem(i);
		IO::DBusManager::Message msg(message);

		if (cb->signalFunc && !cb->signalFunc(listener->me, &msg, cb->userData))
		{
			if (me->ListenerRemoveCallback(listener, cb))
				break;

			continue;
		}

		if (listener->callbacks->IndexOf(cb) == INVALID_INDEX)
			continue;

		listener->callbacks->Remove(cb);
		listener->processed->Remove(cb);
	}

	return HR_NOT_YET_HANDLED;
}

Bool IO::DBusManager::ListenerUpdateService(void *userObj)
{
	ServiceData *data = (ServiceData*)userObj;
	ListenerCallbacks *cb = data->callback;
	IO::DBusManager *me = data->me;

	me->ServiceDataFree(data);

	if (cb->connFunc)
		cb->connFunc(me, cb->userData);

	return FALSE;
}

IO::DBusManager::Listener *IO::DBusManager::ListenerFind()
{
	ClassData *data = (ClassData*)this->clsData;
	return data->listeners->GetItem(0);
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

Bool IO::DBusManager::ListenerAddMatch(Listener *listener, RawMessageFunction hdlr)
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
		return false;
	}

	listener->handleFunc = hdlr;
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

IO::DBusManager::Listener *IO::DBusManager::ListenerFindMatch(const Char *name, const Char *owner, const Char *path, const Char *interface, const Char *member, const Char *argument)
{
	ClassData *data = (ClassData *)this->clsData;
	Listener *listener;
	UOSInt i = data->listeners->GetCount();
	while (i-- > 0)
	{
		listener = data->listeners->GetItem(i);

		if (!Text::StrEqualsN(name, listener->name))
			continue;

		if (!Text::StrEqualsN(owner, listener->owner))
			continue;

		if (!Text::StrEqualsN(path, listener->path))
			continue;

		if (!Text::StrEqualsN(interface, listener->interface))
			continue;

		if (!Text::StrEqualsN(member, listener->member))
			continue;

		if (!Text::StrEqualsN(argument, listener->argument))
			continue;

	}

	return NULL;
}

IO::DBusManager::Listener *IO::DBusManager::ListenerGet(RawMessageFunction filter, const Char *sender, const Char *path, const Char *interface, const Char *member, const Char *argument)
{
	ClassData *data = (ClassData*)this->clsData;
	Listener *listener;
	const Char *name = 0;
	const Char *owner = 0;

	if (this->ListenerFind() == 0)
	{
		if (!dbus_connection_add_filter(data->conn, DBusManager_WatchMessageFilter, NULL, NULL))
		{
			printf("dbus_connection_add_filter() failed\r\n");
			return NULL;
		}
	}

	if (sender != NULL)
	{
		if (sender[0] == ':')
			owner = sender;
		else
			name = sender;
	}

	listener = this->ListenerFindMatch(name, owner, path, interface, member, argument);
	if (listener)
		return listener;

	listener = MemAlloc(Listener, 1);
	MemClear(listener, sizeof(Listener));
	listener->me = this;
	listener->name = SCOPY_TEXT(name);
	listener->owner = SCOPY_TEXT(owner);
	listener->path = SCOPY_TEXT(path);
	listener->interface = SCOPY_TEXT(interface);
	listener->member = SCOPY_TEXT(member);
	listener->argument = SCOPY_TEXT(argument);
	NEW_CLASS(listener->callbacks, Data::ArrayList<ListenerCallbacks*>());
	NEW_CLASS(listener->processed, Data::ArrayList<ListenerCallbacks*>());

	if (!this->ListenerAddMatch(listener, filter))
	{
		this->ListenerFree(listener);
		return 0;
	}

	data->listeners->Add(listener);

	return listener;
}

IO::DBusManager::ListenerCallbacks *IO::DBusManager::ListenerAddCallback(Listener *listener, WatchFunction connect, WatchFunction disconnect, SignalFunction signal, DestroyFunction destroy, void *userData)
{
	ClassData *data = (ClassData*)this->clsData;
	ListenerCallbacks *cb = MemAlloc(ListenerCallbacks, 1);
	MemClear(cb, sizeof(ListenerCallbacks));

	cb->connFunc = connect;
	cb->discFunc = disconnect;
	cb->signalFunc = signal;
	cb->destroyFunc = destroy;
	cb->userData = userData;
	cb->id = ++(data->listenerId);

	if (listener->lock)
		listener->processed->Add(cb);
	else
		listener->callbacks->Add(cb);

	return cb;
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
		dbus_connection_remove_filter(data->conn, DBusManager_WatchMessageFilter, this);

	UOSInt i = listener->callbacks->GetCount();
	while (i-- > 0)
	{
		MemFree(listener->callbacks->GetItem(i));
	}
	DEL_CLASS(listener->callbacks);
	DEL_CLASS(listener->processed);
	this->RemoveWatch(listener->nameWatch);
	Text::StrDelNew(listener->name);
	Text::StrDelNew(listener->owner);
	Text::StrDelNew(listener->path);
	Text::StrDelNew(listener->interface);
	Text::StrDelNew(listener->member);
	Text::StrDelNew(listener->argument);
	MemFree(listener);
}

void IO::DBusManager::ListenerCheckService(const Char *name, ListenerCallbacks *callback)
{
	ClassData *clsData = (ClassData*)this->clsData;
	DBusMessage *message;
	ServiceData *data;

	data = MemAlloc(ServiceData, 1);
	if (data == 0)
	{
		printf("Can't allocate data structure\r\n");
		return;
	}
	MemClear(data, sizeof(ServiceData));
	data->me = this;
	data->name = Text::StrCopyNew(name);
	data->callback = callback;
	callback->data = data;

	data->owner = this->ListenerCheckNameCache(name);
	if (data->owner != NULL)
	{
		data->id = g_idle_add((GSourceFunc)ListenerUpdateService, data);
		return;
	}

	message = dbus_message_new_method_call(DBUS_SERVICE_DBUS, DBUS_PATH_DBUS, DBUS_INTERFACE_DBUS, "GetNameOwner");
	if (message == NULL)
	{
		printf("Can't allocate new message\r\n");
		MemFree(data);
		return;
	}

	dbus_message_append_args(message, DBUS_TYPE_STRING, &name, DBUS_TYPE_INVALID);

	if (dbus_connection_send_with_reply(clsData->conn, message, (DBusPendingCall**)&data->call, -1) == FALSE)
	{
		printf("Failed to execute method call\r\n");
		MemFree(data);
	}
	else if (data->call == 0)
	{
		printf("D-Bus connection not available\r\n");
		g_free(data);
	}
	else
	{
		dbus_pending_call_set_notify((DBusPendingCall*)data->call, (DBusPendingCallNotifyFunction)ServiceReply, data, 0);
	}

	dbus_message_unref(message);
}

const Char *IO::DBusManager::ListenerCheckNameCache(const Char *name)
{
	ClassData *clsData = (ClassData*)this->clsData;
	Listener *listener;
	UOSInt i = clsData->listeners->GetCount();
	while (i-- > 0)
	{
		listener = clsData->listeners->GetItem(i);
		if (Text::StrEqualsN(listener->name, name))
		{
			return listener->owner;
		}
	}
	return 0;
}

void IO::DBusManager::ListenerUpdateNameCache(const Char *name, const Char *owner)
{
	ClassData *clsData = (ClassData*)this->clsData;
	Listener *listener;
	UOSInt i = clsData->listeners->GetCount();
	while (i-- > 0)
	{
		listener = clsData->listeners->GetItem(i);
		if (Text::StrEqualsN(listener->name, name))
		{
			SDEL_TEXT(listener->owner);
			listener->owner = SCOPY_TEXT(owner);
		}
	}
}

IO::DBusManager::HandlerResult IO::DBusManager::WatchMessageFilter(Message *message)
{
	ClassData *clsData = (ClassData*)this->clsData;
	IO::DBusManager::Listener *listener;
	const Char *sender;
	const Char *path;
	const Char *iface;
	const Char *member;
	const Char *arg = 0;
	Data::ArrayList<Listener*> deleteListener;

	if (message->GetType() != MT_SIGNAL)
		return HR_NOT_YET_HANDLED;

	sender = message->GetSender();
	path = message->GetPath();
	iface = message->GetInterface();
	member = message->GetMember();
	arg = message->GetArguments();

	UOSInt i = clsData->listeners->GetCount();
	while (i-- > 0)
	{
		listener = clsData->listeners->GetItem(i);	

		if (!sender && listener->owner)
			continue;

		if (listener->owner && !Text::StrEquals(sender, listener->owner))
			continue;

		if (listener->path && !Text::StrEquals(path, listener->path))
			continue;

		if (listener->interface && !Text::StrEquals(iface, listener->interface))
			continue;

		if (listener->member && !Text::StrEquals(member, listener->member))
			continue;

		if (listener->argument && !Text::StrEquals(arg, listener->argument))
			continue;

		if (listener->handleFunc)
		{
			listener->lock = TRUE;

			listener->handleFunc(clsData->conn, (DBusMessage*)message->GetHandle(), listener);

			listener->callbacks->Clear();
			listener->callbacks->AddAll(listener->processed);
			listener->processed->Clear();
			listener->lock = false;
		}

		if (listener->callbacks == 0 || listener->callbacks->GetCount() == 0)
		{
			deleteListener.Add(listener);
		}
	}

	if (deleteListener.GetCount() == 0)
		return HR_NOT_YET_HANDLED;

	i = deleteListener.GetCount();
	while (i-- > 0)
	{
		listener = deleteListener.GetItem(i);		
		if (listener->callbacks != 0)
			continue;

		this->ListenerRemoveMatch(listener);
		clsData->listeners->Remove(listener);
		this->ListenerFree(listener);
	}
	return HR_NOT_YET_HANDLED;
}

UOSInt IO::DBusManager::AddServiceWatch(const Char *name, WatchFunction connect, WatchFunction disconnect, void *userData, DestroyFunction destroy)
{
	Listener *listener;
	ListenerCallbacks *cb;

	if (name == NULL)
		return 0;

	listener = this->ListenerGet(ServiceFilter, DBUS_SERVICE_DBUS, DBUS_PATH_DBUS, DBUS_INTERFACE_DBUS, "NameOwnerChanged", name);
	if (listener == NULL)
		return 0;

	cb = this->ListenerAddCallback(listener, connect, disconnect, NULL, destroy, userData);
	if (cb == NULL)
		return 0;

	if (connect)
		this->ListenerCheckService(name, cb);

	return cb->id;
}

UOSInt IO::DBusManager::AddSignalWatch(const Char *sender, const Char *path, const Char *interface, const Char *member, SignalFunction function, void *userData, DestroyFunction destroy)
{
	Listener *listener;
	ListenerCallbacks *cb;

	listener = this->ListenerGet(SignalFilter, sender, path, interface, member, NULL);
	if (listener == NULL)
		return 0;

	cb = this->ListenerAddCallback(listener, NULL, NULL, function, destroy, userData);
	if (cb == NULL)
		return 0;

	if (listener->name != NULL && listener->nameWatch == 0)
		listener->nameWatch = this->AddServiceWatch(listener->name, NULL, NULL, NULL, NULL);

	return cb->id;
}

UOSInt IO::DBusManager::AddPropertiesWatch(const Char *sender, const Char *path, const Char *interface, SignalFunction function, void *userData, DestroyFunction destroy)
{
	Listener *listener;
	ListenerCallbacks *cb;

	listener = this->ListenerGet(SignalFilter, sender, path, DBUS_INTERFACE_PROPERTIES, "PropertiesChanged", interface);
	if (listener == NULL)
		return 0;

	cb = this->ListenerAddCallback(listener, NULL, NULL, function, destroy, userData);
	if (cb == NULL)
		return 0;

	if (listener->name != NULL && listener->nameWatch == 0)
		listener->nameWatch = this->AddServiceWatch(listener->name, NULL, NULL, NULL, NULL);

	return cb->id;
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

void IO::DBusManager::AddDictWithStringValue(void *itera, const Char *key, const Char *str)
{
	DBusMessageIter *iter = (DBusMessageIter*)itera;
	DBusMessageIter dict;
	DBusMessageIter entry;
	DBusMessageIter value;

	dbus_message_iter_open_container(iter, DBUS_TYPE_ARRAY,
			DBUS_DICT_ENTRY_BEGIN_CHAR_AS_STRING
			DBUS_TYPE_STRING_AS_STRING DBUS_TYPE_VARIANT_AS_STRING
			DBUS_DICT_ENTRY_END_CHAR_AS_STRING, &dict);
	dbus_message_iter_open_container(&dict, DBUS_TYPE_DICT_ENTRY, NULL, &entry);
	dbus_message_iter_append_basic(&entry, DBUS_TYPE_STRING, &key);

	dbus_message_iter_open_container(&entry, DBUS_TYPE_VARIANT, DBUS_TYPE_STRING_AS_STRING, &value);
	dbus_message_iter_append_basic(&value, DBUS_TYPE_STRING, &str);
	dbus_message_iter_close_container(&entry, &value);

	dbus_message_iter_close_container(&dict, &entry);
	dbus_message_iter_close_container(iter, &dict);
}

void IO::DBusManager::AddEmptyStringDict(void *itera)
{
	DBusMessageIter *iter = (DBusMessageIter*)itera;
	DBusMessageIter dict;

	dbus_message_iter_open_container(iter, DBUS_TYPE_ARRAY,
			DBUS_DICT_ENTRY_BEGIN_CHAR_AS_STRING
			DBUS_TYPE_STRING_AS_STRING DBUS_TYPE_STRING_AS_STRING
			DBUS_DICT_ENTRY_END_CHAR_AS_STRING, &dict);

	dbus_message_iter_close_container(iter, &dict);
}

void IO::DBusManager::AddArguments(void *itera, const Char *action, UInt32 flags)
{
	ClassData *clsData = (ClassData*)this->clsData;
	DBusMessageIter *iter = (DBusMessageIter*)itera;
	const Char *busname = dbus_bus_get_unique_name(clsData->conn);
	const Char *kind = "system-bus-name";
	const Char *cancel = "";
	DBusMessageIter subject;

	dbus_message_iter_open_container(iter, DBUS_TYPE_STRUCT, NULL, &subject);
	dbus_message_iter_append_basic(&subject, DBUS_TYPE_STRING, &kind);
	this->AddDictWithStringValue(&subject, "name", busname);
	dbus_message_iter_close_container(iter, &subject);

	dbus_message_iter_append_basic(iter, DBUS_TYPE_STRING, &action);
	this->AddEmptyStringDict(iter);
	dbus_message_iter_append_basic(iter, DBUS_TYPE_UINT32, &flags);
	dbus_message_iter_append_basic(iter, DBUS_TYPE_STRING, &cancel);
}

Bool IO::DBusManager::PolkitParseResult(void *itera)
{
	DBusMessageIter *iter = (DBusMessageIter*)itera;
	DBusMessageIter result;
	Bool authorized;
	Bool challenge;

	dbus_message_iter_recurse(iter, &result);
	dbus_message_iter_get_basic(&result, &authorized);
	dbus_message_iter_get_basic(&result, &challenge);
	return authorized;
}

void IO::DBusManager::AuthorizationReply(void *pcall, void *userData)
{
	DBusPendingCall *call = (DBusPendingCall*)pcall;
	AuthorizationData *data = (AuthorizationData*)userData;
	DBusMessage *reply;
	DBusMessageIter iter;
	Bool authorized = false;

	reply = dbus_pending_call_steal_reply(call);
	if ((dbus_message_get_type(reply) == DBUS_MESSAGE_TYPE_ERROR) ||
		(dbus_message_has_signature(reply, "(bba{ss})") == FALSE))
	{

	}
	else
	{
		dbus_message_iter_init(reply, &iter);
		authorized = PolkitParseResult(&iter);
	}

	if (data->function != NULL)
		data->function(authorized, data->userData);

	dbus_message_unref(reply);
	dbus_pending_call_unref(call);
}

IO::DBusManager::ErrorType IO::DBusManager::PolkitCheckAuthorization(const Char *action, Bool interaction, PolkitFunction function, void *userData, Int32 timeout)
{
	ClassData *clsData = (ClassData*)this->clsData;
	AuthorizationData *data;
	DBusMessage *msg;
	DBusMessageIter iter;
	DBusPendingCall *call;
	UInt32 flags = 0x00000000;

	if (clsData->conn == NULL)
		return ET_INVAL;

	data = MemAlloc(AuthorizationData, 1);
	if (data == NULL)
		return ET_NOMEM;

	msg = dbus_message_new_method_call(AUTHORITY_DBUS, AUTHORITY_PATH, AUTHORITY_INTF, "CheckAuthorization");
	if (msg == NULL)
	{
		MemFree(data);
		return ET_NOMEM;
	}

	if (interaction == TRUE)
		flags |= 0x00000001;

	if (action == NULL)
		action = "org.freedesktop.policykit.exec";

	dbus_message_iter_init_append(msg, &iter);
	this->AddArguments(&iter, action, flags);

	if (dbus_connection_send_with_reply(clsData->conn, msg, &call, timeout) == FALSE)
	{
		dbus_message_unref(msg);
		MemFree(data);
		return ET_IO_ERROR;
	}

	if (call == NULL)
	{
		dbus_message_unref(msg);
		MemFree(data);
		return ET_IO_ERROR;
	}

	data->function = function;
	data->userData = userData;

	dbus_pending_call_set_notify(call, (DBusPendingCallNotifyFunction)AuthorizationReply, data, MemFree);
	dbus_message_unref(msg);

	return ET_SUCCESS;
}
