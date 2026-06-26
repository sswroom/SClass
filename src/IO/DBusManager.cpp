#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayListArr.hpp"
#include "Data/ArrayListNN.hpp"
#include "Data/ArrayListUtil.hpp"
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
	UnsafeArray<const UTF8Char> name;
	const MethodTable *methods;
	UnsafeArrayOpt<const SignalTable> signals;
	UnsafeArrayOpt<const PropertyTable> properties;
	Data::ArrayListNN<PropertyTable> *pendingProp;
	AnyType userData;
	DestroyFunction destroy;	
};

struct IO::DBusManager::SecurityData
{
	UInt32 pending;
	DBusMessage *message;
	const MethodTable *method;
	AnyType ifaceUserData;
};

struct IO::DBusManager::BuiltinSecurityData
{
	IO::DBusManager *me;
	UInt32 pending;
};

struct IO::DBusManager::AuthorizationData
{
	PolkitFunction function;
	AnyType userData;
};

struct IO::DBusManager::ArgInfo
{
	const Char *name;
	const Char *signature;
};

struct IO::DBusManager::GenericData
{
	UIntOS refcount;
	NN<IO::DBusManager> me;
	UnsafeArray<const UTF8Char> path;
	Data::ArrayListNN<InterfaceData> *interfaces;
	Data::ArrayListNN<GenericData> *objects;
	Data::ArrayListNN<InterfaceData> *added;
	Data::ArrayListArr<const UTF8Char> *removed;
	UInt32 processId;
	Bool pendingProp;
	UnsafeArrayOpt<const UTF8Char> introspect;
	Optional<IO::DBusManager::GenericData> parent;
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
	UnsafeArrayOpt<const UTF8Char> name;
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

struct IO::DBusManager::ClassData
{
	Int32 refCount;
	DBusConnection *conn;
	Optional<IO::DBusManager::GenericData> root;
	Data::ArrayListNN<IO::DBusManager::Listener> *listeners;
	UIntOS listenerId;
	Data::ArrayListNN<IO::DBusManager::GenericData> *pending;
	IO::DBusManager::PropertyFlags globalFlags;
	UInt32 nextPending;
	Data::ArrayListNN<IO::DBusManager::SecurityData> *pendingSecurity;
	const IO::DBusManager::SecurityTable *securityTable;
};

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
	NN<IO::DBusManager::GenericData> data;
	if (data.Set((IO::DBusManager::GenericData*)user_data))
		data->me->GenericUnregister(data);
}

DBusHandlerResult DBusManager_GenericMessage(DBusConnection *connection, DBusMessage *message, void *user_data)
{
	NN<IO::DBusManager::GenericData> data;
	if (!data.Set((IO::DBusManager::GenericData*)user_data))
		return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
	IO::DBusManager::Message msg(message);
	return (DBusHandlerResult)data->me->GenericMessage(data, msg);
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
	NN<IO::DBusManager::WatchInfo> info;
	unsigned int flags;
	int fd;

	if (!dbus_watch_get_enabled(watch))
		return TRUE;

	info = MemAllocNN(IO::DBusManager::WatchInfo);
	info->me = me;

	fd = dbus_watch_get_unix_fd(watch);
	chan = g_io_channel_unix_new(fd);

	info->watch = watch;
	info->conn = dbus_connection_ref(conn);

	dbus_watch_set_data(watch, info.Ptr(), DBusManager_WatchInfoFree);

	flags = dbus_watch_get_flags(watch);

	if (flags & DBUS_WATCH_READABLE) cond = (GIOCondition)(cond | G_IO_IN);
	if (flags & DBUS_WATCH_WRITABLE) cond = (GIOCondition)(cond | G_IO_OUT);

	info->id = g_io_add_watch(chan, cond, DBusManager_WatchFunc, info.Ptr());

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
	NN<ClassData> data = this->clsData;
	dbus_connection_set_watch_functions(data->conn, DBusManager_OnAddWatch, DBusManager_OnRemoveWatch, DBusManager_OnWatchToggled, this, 0);
	dbus_connection_set_timeout_functions(data->conn, DBusManager_OnAddTimeout, DBusManager_OnRemoveTimeout, DBusManager_OnTimeoutToggled, this, 0);
	dbus_connection_set_dispatch_status_function(data->conn, DBusManager_OnDispatchStatus, this, 0);
}

Bool IO::DBusManager::SetupBus(const Char *name)
{
	NN<ClassData> data = this->clsData;

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
	NN<ClassData> data = MemAllocNN(ClassData);
	this->clsData = data;
	data->refCount = 1;
	data->root = nullptr;
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
	NEW_CLASS(data->pending, Data::ArrayListNN<GenericData>());
	data->nextPending = 1;
	NEW_CLASS(data->pendingSecurity, Data::ArrayListNN<IO::DBusManager::SecurityData>());
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
	NN<ClassData> data = this->clsData;
	if (data->conn)
	{
		dbus_connection_unref(data->conn);
	}
	DEL_CLASS(data->pending);
	DEL_CLASS(data->pendingSecurity);
	MemFreeNN(data);
}

Bool IO::DBusManager::IsError()
{
	NN<ClassData> data = this->clsData;
	return data->conn == 0;
}

NN<IO::DBusManager> IO::DBusManager::Ref()
{
	NN<ClassData> data = this->clsData;
	Sync::Interlocked::IncrementI32(data->refCount);
	return *this;
}

void IO::DBusManager::Unref()
{
	NN<ClassData> data = this->clsData;
	if (Sync::Interlocked::DecrementI32(data->refCount) <= 0)
	{
		DEL_CLASS(this);
	}
}

void *IO::DBusManager::GetHandle()
{
	NN<ClassData> data = this->clsData;
	return data->conn;
}

void IO::DBusManager::QueueDispatch(Int32 status)
{
	NN<ClassData> data = this->clsData;

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

	UIntOS i = 0;
	UIntOS j = data->objects->GetCount();
	while (i < j)
	{
		dbusManager->AppendObject(data->objects->GetItemNoCheck(i), &array);
		i++;
	}

	dbus_message_iter_close_container(&iter, &array);

	return reply;
}

void *IO::DBusManager::Introspect(IO::DBusManager *dbusManager, Message *message, void *userData)
{
	NN<GenericData> data;
	if (!data.Set((GenericData*)userData))
		return NULL;
	DBusMessage *reply;

	if (data->introspect.IsNull())
		dbusManager->GenerateIntrospectionXml(data, message->GetPath());

	reply = dbus_message_new_method_return((DBusMessage*)message->GetHandle());
	if (reply == NULL)
		return NULL;

	dbus_message_append_args(reply, DBUS_TYPE_STRING, &data->introspect, DBUS_TYPE_INVALID);
	return reply;
}

void IO::DBusManager::PrintArguments(NN<Text::StringBuilderUTF8> sb, const ArgInfo *args, const Char *direction)
{
	NN<Text::String> s;
	while (args && args->name)
	{
		sb->Append(CSTR("<arg name="));
		s = Text::XML::ToNewAttrText((const UTF8Char*)args->name);
		sb->Append(s);
		s->Release();
		sb->Append(CSTR(" type="));
		s = Text::XML::ToNewAttrText((const UTF8Char*)args->signature);
		sb->Append(s);
		s->Release();

		if (direction)
		{
			sb->Append(CSTR(" direction="));
			s = Text::XML::ToNewAttrText((const UTF8Char*)direction);
			sb->Append(s);
			s->Release();
			sb->Append(CSTR("/>\n"));
		}
		else
		{
			sb->Append(CSTR("/>\n"));
		}
		args++;
	}
}

void IO::DBusManager::GenerateInterfaceXml(NN<Text::StringBuilderUTF8> sb, NN<InterfaceData> iface)
{
	NN<Text::String> s;
	const MethodTable *method;
	UnsafeArray<const SignalTable> signal;
	UnsafeArray<const PropertyTable> property;

	method = iface->methods;
	while (method && method->name)
	{
		if (!this->CheckExperimental(method->flags, MF_EXPERIMENTAL))
		{
			sb->Append(CSTR("<method name="));
			s = Text::XML::ToNewAttrText((const UTF8Char*)method->name);
			sb->Append(s);
			s->Release();
			sb->Append(CSTR(">"));
			this->PrintArguments(sb, method->inArgs, "in");
			this->PrintArguments(sb, method->outArgs, "out");

			if (method->flags & MF_DEPRECATED)
				sb->Append(CSTR(DBUS_ANNOTATE_DEPRECATED));

			if (method->flags & MF_NOREPLY)
				sb->Append(CSTR(DBUS_ANNOTATE_NOREPLY));

			sb->Append(CSTR("</method>"));
		}
		method++;
	}

	if (iface->signals.SetTo(signal))
	{
		UnsafeArray<const UInt8> name;
		while (signal->name.SetTo(name))
		{
			if (!this->CheckExperimental(signal->flags, SF_EXPERIMENTAL))
			{
				sb->Append(CSTR("<signal name="));
				s = Text::XML::ToNewAttrText(name);
				sb->Append(s);
				s->Release();
				sb->Append(CSTR(">"));
				this->PrintArguments(sb, signal->args, NULL);

				if (signal->flags & SF_DEPRECATED)
					sb->Append(CSTR(DBUS_ANNOTATE_DEPRECATED));

				sb->Append(CSTR("</signal>\n"));
			}
			signal++;
		}
	}

	if (iface->properties.SetTo(property))
	{
		while (property->name)
		{
			if (!this->CheckExperimental(property->flags, PF_EXPERIMENTAL))
			{
				sb->Append(CSTR("<property name="));
				s = Text::XML::ToNewAttrText((const UTF8Char*)property->name);
				sb->Append(s);
				s->Release();
				sb->Append(CSTR(" type="));
				s = Text::XML::ToNewAttrText((const UTF8Char*)property->type);
				sb->Append(s);
				s->Release();
				sb->Append(CSTR(" access=\""));
				if (property->get) sb->Append(CSTR("read"));
				if (property->set) sb->Append(CSTR("write"));
				sb->Append(CSTR("\">"));

				if (property->flags & PF_DEPRECATED)
					sb->Append(CSTR(DBUS_ANNOTATE_DEPRECATED));

				sb->Append(CSTR("</property>"));
			}
			property++;
		}
	}
}

void IO::DBusManager::GenerateIntrospectionXml(NN<GenericData> data, const Char *path)
{
	NN<ClassData> clsData = this->clsData;
	NN<Text::String> s;
	Text::StringBuilderUTF8 sb;
	Char **children;

	SDEL_TEXT(data->introspect);

	sb.Append(CSTR(DBUS_INTROSPECT_1_0_XML_DOCTYPE_DECL_NODE));
	sb.Append(CSTR("<node>"));

	UIntOS i = 0;
	UIntOS j = data->interfaces->GetCount();
	while (i < j)
	{
		NN<InterfaceData> iface = data->interfaces->GetItemNoCheck(i);

		sb.Append(CSTR("<interface name="));
		s = Text::XML::ToNewAttrText(iface->name);
		sb.Append(s);
		s->Release();
		sb.Append(CSTR(">"));

		this->GenerateInterfaceXml(sb, iface);

		sb.Append(CSTR("</interface>"));
		i++;
	}

	if (dbus_connection_list_registered(clsData->conn, path, &children))
	{
		i = 0;
		while (children[i])
		{
			sb.Append(CSTR("<node name="));
			s = Text::XML::ToNewAttrText((const UTF8Char*)children[i]);
			sb.Append(s);
			s->Release();
			sb.Append(CSTR("/>"));
			i++;
		}

		dbus_free_string_array(children);
	}
	sb.Append(CSTR("</node>"));
	data->introspect = Text::StrCopyNew(sb.ToString());
}

void IO::DBusManager::AppendInterfaces(NN<GenericData> data, void *itera)
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

	UIntOS i = 0;
	UIntOS j = data->interfaces->GetCount();
	while (i < j)
	{
		this->AppendInterface(data->interfaces->GetItemNoCheck(i), &array);
		i++;
	}

	dbus_message_iter_close_container(iter, &array);
}

void IO::DBusManager::AppendObject(NN<GenericData> child, void *itera)
{
	DBusMessageIter *array = (DBusMessageIter*)itera;
	DBusMessageIter entry;

	dbus_message_iter_open_container(array, DBUS_TYPE_DICT_ENTRY, NULL, &entry);
	dbus_message_iter_append_basic(&entry, DBUS_TYPE_OBJECT_PATH, &child->path);
	this->AppendInterfaces(child, &entry);
	dbus_message_iter_close_container(array, &entry);

	UIntOS i = 0;
	UIntOS j = child->objects->GetCount();
	while (i < j)
	{
		this->AppendObject(child->objects->GetItemNoCheck(i), array);
		i++;
	}
}

void IO::DBusManager::PendingSuccess(UInt32 pending)
{
	NN<ClassData> clsData = this->clsData;
	NN<SecurityData> secdata;
	UIntOS i = 0;
	UIntOS j = clsData->pendingSecurity->GetCount();
	while (i < j)
	{
		secdata = clsData->pendingSecurity->GetItemNoCheck(i);
		if (secdata->pending == pending)
		{
			clsData->pendingSecurity->RemoveAt(i);
			{
				IO::DBusManager::Message message(secdata->message);
				this->ProcessMessage(message, secdata->method, secdata->ifaceUserData);
			}


			dbus_message_unref(secdata->message);
			MemFreeNN(secdata);
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
	NN<ClassData> clsData = this->clsData;
	NN<SecurityData> secdata;
	UIntOS i = 0;
	UIntOS j = clsData->pendingSecurity->GetCount();
	while (i < j)
	{
		secdata = clsData->pendingSecurity->GetItemNoCheck(i);
		if (secdata->pending == pending)
		{
			clsData->pendingSecurity->RemoveAt(i);
			this->SendError(secdata->message, name, errMsg);

			dbus_message_unref(secdata->message);
			MemFreeNN(secdata);
		}
		i++;
	}
}

Optional<IO::DBusManager::GenericData> IO::DBusManager::ObjectPathRef(UnsafeArray<const UTF8Char> path)
{
	NN<ClassData> clsData = this->clsData;
	NN<GenericData> data;
	GenericData *tmpData;
	if (dbus_connection_get_object_path_data(clsData->conn, (const Char*)path.Ptr(), (void **) &tmpData) == TRUE)
	{
		if (data.Set(tmpData))
		{
			data->refcount++;
			return data;
		}
	}

	data = MemAllocNN(GenericData);
	data.ZeroContent();
	data->me = *this;
	data->path = Text::StrCopyNew(path);
	data->refcount = 1;
	data->introspect = Text::StrCopyNew(CSTR(DBUS_INTROSPECT_1_0_XML_DOCTYPE_DECL_NODE "<node></node>").v);

	if (!dbus_connection_register_object_path(clsData->conn, (const Char*)path.Ptr(), &DBusManager_genericTable, data.Ptr()))
	{
		GenericDataFree(data);
		return nullptr;
	}
	NEW_CLASS(data->objects, Data::ArrayListNN<GenericData>());
	NEW_CLASS(data->added, Data::ArrayListNN<InterfaceData>());
	NEW_CLASS(data->interfaces, Data::ArrayListNN<InterfaceData>());

	this->InvalidateParentData(path);

	this->AddInterface(data, CSTR(DBUS_INTERFACE_INTROSPECTABLE).v, introspectMethods, nullptr, nullptr, data, NULL);

	return data;
}

Bool IO::DBusManager::AttachObjectManager()
{
	NN<ClassData> clsData = this->clsData;
	NN<GenericData> data;

	if (!this->ObjectPathRef(U8STR("/")).SetTo(data))
		return FALSE;

	this->AddInterface(data, CSTR(DBUS_INTERFACE_OBJECT_MANAGER).v, managerMethods, managerSignals, nullptr, data, NULL);
	clsData->root = data;

	return TRUE;
}

void IO::DBusManager::GenericDataFree(NN<GenericData> data)
{
	SDEL_CLASS(data->objects);
	SDEL_CLASS(data->interfaces);
	SDEL_CLASS(data->added);
	Text::StrDelNew(data->path);
	SDEL_TEXT(data->introspect);
	MemFreeNN(data);
}

void IO::DBusManager::GenericUnregister(NN<GenericData> data)
{
	NN<GenericData> parent;

	if (data->parent.SetTo(parent))
		parent->objects->Remove(data);

	if (data->processId > 0)
	{
		g_source_remove(data->processId);
		data->processId = 0;
		this->ProcessChanges(data.Ptr());
	}

	UIntOS i = data->objects->GetCount();
	while (i-- > 0)
	{
		data->objects->GetItemNoCheck(i)->parent = parent;
	}

	GenericDataFree(data);
}

IO::DBusManager::HandlerResult IO::DBusManager::GenericMessage(NN<GenericData> data, NN<Message> message)
{
	NN<InterfaceData> iface;
	const MethodTable *method;
	const Char *interface;

	interface = dbus_message_get_interface((DBusMessage*)message->GetHandle());
	if (interface == NULL)
		return HR_NOT_YET_HANDLED;
	if (!this->FindInterface(data, (const UTF8Char*)interface).SetTo(iface))
		return HR_NOT_YET_HANDLED;

	method = iface->methods;
	while (method && method->name && method->function)
	{

		if (dbus_message_is_method_call((DBusMessage*)message->GetHandle(), (const Char*)iface->name.Ptr(), method->name) == FALSE)
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

IO::DBusManager::HandlerResult IO::DBusManager::ProcessMessage(NN<Message> message, const MethodTable *method, AnyType ifaceUserData)
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

Optional<IO::DBusManager::GenericData> IO::DBusManager::InvalidateParentData(UnsafeArray<const UTF8Char> childPath)
{
	NN<ClassData> clsData = this->clsData;
	GenericData *data = NULL;
	GenericData *optchild = NULL;
	NN<GenericData> child;
	Optional<GenericData> parent = nullptr;
	Text::StringBuilderUTF8 sbParentPath;
	UIntOS i;

	sbParentPath.AppendSlow(childPath);
	i = sbParentPath.LastIndexOf('/');
	if (i == INVALID_INDEX)
	{
		return data;
	}

	if (i == 0 && sbParentPath.ToString()[1] != 0)
		sbParentPath.TrimToLength(1);
	else
		sbParentPath.TrimToLength((UIntOS)i);

	if (sbParentPath.GetLength() == 0)
	{
		return data;
	}

	if (dbus_connection_get_object_path_data(clsData->conn, (const Char*)sbParentPath.v.Ptr(), (void **)&data) == FALSE)
	{
		return data;
	}

	parent = this->InvalidateParentData(sbParentPath.ToString());

	if (data == NULL)
	{
		data = parent.OrNull();
		if (data == NULL)
		{
			return data;
		}
	}

	SDEL_TEXT(data->introspect);

	if (!dbus_connection_get_object_path_data(clsData->conn, (const Char*)childPath.Ptr(), (void **) &optchild))
	{
		return data;
	}

	if (!child.Set(optchild) || data->objects->IndexOf(child) != INVALID_INDEX)
	{
		return data;
	}

	data->objects->Add(child);
	child->parent = data;
	return data;
}

void IO::DBusManager::AddPending(NN<GenericData> data)
{
	NN<ClassData> clsData = this->clsData;
	UInt32 oldId = data->processId;

	data->processId = g_idle_add((GSourceFunc)ProcessChanges, data.Ptr());

	if (oldId > 0)
	{
		g_source_remove(oldId);
		return;
	}

	clsData->pending->Add(data);
}

Optional<IO::DBusManager::InterfaceData> IO::DBusManager::FindInterface(NN<GenericData> data, UnsafeArray<const UTF8Char> name)
{
	UIntOS i = data->interfaces->GetCount();
	while (i-- > 0)
	{
		NN<InterfaceData> iface = data->interfaces->GetItemNoCheck(i);
		if (Text::StrEquals(name, iface->name)) //!strcmp ?
		{
			return iface;
		}
	}
	return nullptr;
}

Bool IO::DBusManager::AddInterface(NN<GenericData> data, UnsafeArray<const UTF8Char> name, const MethodTable *methods, UnsafeArrayOpt<const SignalTable> signals, UnsafeArrayOpt<const PropertyTable> properties, AnyType userData, DestroyFunction destroy)
{
	NN<InterfaceData> iface;
	const MethodTable *method;
	UnsafeArray<const SignalTable> signal;
	UnsafeArray<const PropertyTable> property;
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

	if (!found && signals.SetTo(signal))
	{
		while (signal->name.NotNull())
		{
			if (!this->CheckExperimental(signal->flags, SF_EXPERIMENTAL))
			{
				found = true;
				break;
			}
			signal++;
		}
	}

	if (!found && properties.SetTo(property))
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

	iface = MemAllocNN(InterfaceData);
	iface.ZeroContent();
	iface->name = Text::StrCopyNew(name);
	iface->methods = methods;
	iface->signals = signals;
	iface->properties = properties;
	iface->userData = userData;
	iface->destroy = destroy;

	data->interfaces->Add(iface);
	if (data->parent.IsNull())
		return true;

	data->added->Add(iface);

	this->AddPending(data);

	return true;
}

Bool IO::DBusManager::ArgsHaveSignature(const ArgInfo *args, NN<Message> message)
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

void IO::DBusManager::BuiltinSecurityResult(Bool authorized, AnyType userData)
{
	NN<BuiltinSecurityData> data = userData.GetNN<BuiltinSecurityData>();

	if (authorized == true)
		data->me->PendingSuccess(data->pending);
	else
		data->me->PendingError(data->pending, DBUS_ERROR_AUTH_FAILED, NULL);

	MemFreeNN(data);
}

void IO::DBusManager::BuiltinSecurityFunction(const Char *action, Bool interaction, UInt32 pending)
{
	NN<BuiltinSecurityData> data;

	data = MemAllocNN(BuiltinSecurityData);
	data->me = this;
	data->pending = pending;

	if (this->PolkitCheckAuthorization(action, interaction, BuiltinSecurityResult, data, 30000) != ET_SUCCESS)
		this->PendingError(pending, NULL, NULL);
}

Bool IO::DBusManager::CheckPrivilege(NN<Message> message, const MethodTable *method, AnyType ifaceUserData)
{
	NN<ClassData> clsData = this->clsData;
	const SecurityTable *security;

	security = clsData->securityTable;
	while (security && security->privilege)
	{
		NN<SecurityData> secdata;
		Bool interaction;

		if (security->privilege != method->privilege)
		{
			security++;
			continue;
		}

		secdata = MemAllocNN(SecurityData);
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

Bool IO::DBusManager::CheckSignal(UnsafeArray<const UTF8Char> path, UnsafeArray<const UTF8Char> interface, UnsafeArray<const UTF8Char> name, OutParam<const ArgInfo*> args)
{
	NN<ClassData> clsData = this->clsData;
	GenericData *optdata = NULL;
	NN<GenericData> data;
	NN<InterfaceData> iface;
	UnsafeArray<const SignalTable> signal;

	args.Set(NULL);
	if (!dbus_connection_get_object_path_data(clsData->conn, (const Char*)path.Ptr(), (void **) &optdata) || !data.Set(optdata))
	{
		printf("dbus_connection_emit_signal: path %s isn't registered\r\n", path.Ptr());
		return false;
	}

	if (!this->FindInterface(data, interface).SetTo(iface))
	{
		printf("dbus_connection_emit_signal: %s does not implement %s\r\n", path.Ptr(), interface.Ptr());
		return false;
	}

	if (iface->signals.SetTo(signal))
	{
		UnsafeArray<const UInt8> signalName;
		while (signal->name.SetTo(signalName))
		{
			if (!Text::StrEquals(signalName, name))
			{
				signal++;
				continue;			
			}

			if (signal->flags & SF_EXPERIMENTAL)
			{
				const Char *env = g_getenv("GDBUS_EXPERIMENTAL");
				if (!Text::StrEqualsCh(env, "1"))
					break;
			}

			args.Set(signal->args);
			return true;
		}
	}

	printf("No signal named %s on interface %s\r\n", name, interface);
	return false;
}

Bool IO::DBusManager::CheckExperimental(Int32 flags, Int32 flag)
{
	NN<ClassData> data = this->clsData;
	if (!(flags & flag))
		return false;
	
	return !(data->globalFlags & PF_EXPERIMENTAL);
}

void IO::DBusManager::AppendName(UnsafeArray<const UTF8Char> name, void *itera)
{
	DBusMessageIter *iter = (DBusMessageIter*)itera;
	const Char *cname = (const Char*)name.Ptr();

	dbus_message_iter_append_basic(iter, DBUS_TYPE_STRING, &cname);
}

void IO::DBusManager::AppendProperty(NN<InterfaceData> iface, NN<const PropertyTable> p, void *itera)
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

void IO::DBusManager::AppendProperties(NN<InterfaceData> iface, void *itera)
{
	DBusMessageIter *iter = (DBusMessageIter*)itera;
	DBusMessageIter dict;
	UnsafeArray<const PropertyTable> p;

	dbus_message_iter_open_container(iter, DBUS_TYPE_ARRAY,
				DBUS_DICT_ENTRY_BEGIN_CHAR_AS_STRING
				DBUS_TYPE_STRING_AS_STRING
				DBUS_TYPE_VARIANT_AS_STRING
				DBUS_DICT_ENTRY_END_CHAR_AS_STRING, &dict);

	if (iface->properties.SetTo(p))
	{
		while (p->name)
		{
			if (this->CheckExperimental(p->flags, PF_EXPERIMENTAL))
				continue;

			if (p->get == NULL)
				continue;

			if (p->exists != NULL && !p->exists(p[0], iface->userData))
				continue;

			this->AppendProperty(iface, p[0], &dict);

			p++;
		}
	}

	dbus_message_iter_close_container(iter, &dict);
}

void IO::DBusManager::AppendInterface(NN<InterfaceData> iface, void *itera)
{
	DBusMessageIter *array = (DBusMessageIter*)itera;
	DBusMessageIter entry;

	dbus_message_iter_open_container(array, DBUS_TYPE_DICT_ENTRY, NULL, &entry);
	dbus_message_iter_append_basic(&entry, DBUS_TYPE_STRING, &iface->name);
	this->AppendProperties(iface, &entry);
	dbus_message_iter_close_container(array, &entry);
}

void IO::DBusManager::ProcessPropertyChanges(NN<GenericData> data)
{
	NN<InterfaceData> iface;
	data->pendingProp = FALSE;
	if (data->interfaces)
	{
		UIntOS i = data->interfaces->GetCount();
		while (i-- > 0)
		{
			iface = data->interfaces->GetItemNoCheck(i);
			this->ProcessPropertiesFromInterface(data, iface);
		}
	}
}

void IO::DBusManager::ProcessPropertiesFromInterface(NN<GenericData> data, NN<InterfaceData> iface)
{
	NN<ClassData> clsData = this->clsData;
	DBusMessage *signal;
	DBusMessageIter iter;
	DBusMessageIter dict;
	DBusMessageIter array;
	Data::ArrayListNN<PropertyTable> invalidated;

	if (iface->pendingProp == NULL)
		return;

	signal = dbus_message_new_signal((const Char*)data->path.Ptr(), DBUS_INTERFACE_PROPERTIES, "PropertiesChanged");
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

	UIntOS i = iface->pendingProp->GetCount();
	while (i-- > 0)
	{
		NN<PropertyTable> p = iface->pendingProp->GetItemNoCheck(i);

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
		NN<PropertyTable> p = invalidated.GetItemNoCheck(i);

		dbus_message_iter_append_basic(&array, DBUS_TYPE_STRING, &p->name);
	}
	dbus_message_iter_close_container(&iter, &array);
	SDEL_CLASS(iface->pendingProp);

	dbus_connection_send(clsData->conn, signal, NULL);
	dbus_message_unref(signal);
}

void IO::DBusManager::EmitInterfacesAdded(NN<GenericData> data)
{
	NN<ClassData> clsData = this->clsData;
	NN<GenericData> root;
	DBusMessage *signal;
	DBusMessageIter iter, array;

	if (!clsData->root.SetTo(root) || data == root)
		return;

	signal = dbus_message_new_signal((const Char*)root->path.Ptr(), DBUS_INTERFACE_OBJECT_MANAGER, "InterfacesAdded");
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

	UIntOS i = data->added->GetCount();
	while (i-- > 0)
	{
		this->AppendInterface(data->added->GetItemNoCheck(i), &array);
	}
	SDEL_CLASS(data->added);

	dbus_message_iter_close_container(&iter, &array);

	dbus_connection_send(clsData->conn, signal, NULL);
	dbus_message_unref(signal);	
}

void IO::DBusManager::EmitInterfacesRemoved(NN<GenericData> data)
{
	NN<ClassData> clsData = this->clsData;
	NN<GenericData> root;
	DBusMessage *signal;
	DBusMessageIter iter;
	DBusMessageIter array;

	if (!clsData->root.SetTo(root) || data == root)
		return;

	signal = dbus_message_new_signal((const Char*)root->path.Ptr(), DBUS_INTERFACE_OBJECT_MANAGER, "InterfacesRemoved");
	if (signal == NULL)
		return;

	dbus_message_iter_init_append(signal, &iter);
	dbus_message_iter_append_basic(&iter, DBUS_TYPE_OBJECT_PATH, &data->path);
	dbus_message_iter_open_container(&iter, DBUS_TYPE_ARRAY, DBUS_TYPE_STRING_AS_STRING, &array);

	UIntOS i = data->removed->GetCount();
	while (i-- > 0)
	{
		this->AppendName(data->removed->GetItemNoCheck(i), &array);		
	}
	NNLIST_CALL_FUNC(data->removed, Text::StrDelNew);
	DEL_CLASS(data->removed);

	dbus_message_iter_close_container(&iter, &array);

	dbus_connection_send(clsData->conn, signal, NULL);
	dbus_message_unref(signal);
}

void IO::DBusManager::Flush()
{
	NN<ClassData> clsData = this->clsData;
	NN<GenericData> data;
	UIntOS i = clsData->pending->GetCount();
	while (i-- > 0)
	{
		data = clsData->pending->GetItemNoCheck(i);
		ProcessChanges(data.Ptr());
	}
}

void IO::DBusManager::RemovePending(NN<GenericData> data)
{
	if (data->processId > 0)
	{
		g_source_remove(data->processId);
		data->processId = 0;
	}

	NN<ClassData> clsData = this->clsData;
	clsData->pending->Remove(data);
}

Bool IO::DBusManager::ProcessChanges(void *userData)
{
	NN<GenericData> data;
	if (!data.Set((GenericData*)userData))
		return FALSE;

	NN<IO::DBusManager> me = data->me;
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
	NN<ClassData> data = this->clsData;
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
	NN<ClassData> data = this->clsData;
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

		if (path && interface && name && !this->CheckSignal((const UTF8Char*)path, (const UTF8Char*)interface, (const UTF8Char*)name, args))
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

void IO::DBusManager::ServiceDataFree(NN<ServiceData> data)
{
	NN<ListenerCallbacks> callback = data->callback;

	if (data->call)
		dbus_pending_call_unref((DBusPendingCall*)data->call);

	if (data->id)
		g_source_remove(data->id); ////////////////////////

	Text::StrDelNew(data->name);
	MemFreeNN(data);

	callback->data = nullptr;
}

Bool IO::DBusManager::UpdateService(void *userObj)
{
	NN<ServiceData> data;
	if (!data.Set((ServiceData*)userObj))
	{
		return FALSE;
	}
	NN<IO::DBusManager> me = data->me;
	NN<ListenerCallbacks> cb = data->callback;
		
	me->ServiceDataFree(data);

	if (cb->connFunc)
		cb->connFunc(me, cb->userData);

	return false;
}

void IO::DBusManager::ServiceReply(void *pending, void *userObj)
{
	NN<ServiceData> data;
	if (!data.Set((ServiceData*)userObj))
	{
		return;
	}
	NN<IO::DBusManager> me = data->me;
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
		me->UpdateService(data.Ptr());
	}
	dbus_message_unref(reply);
}

IO::DBusManager::HandlerResult IO::DBusManager::ServiceFilter(void *connection, void *message, void *userData)
{
	NN<Listener> listener;
	if (!listener.Set((Listener*)userData))
	{
		return HR_NOT_YET_HANDLED;
	}
	NN<IO::DBusManager> me = listener->me;
	NN<ListenerCallbacks> cb;
	const Char *name;
	const Char *oldName;
	const Char *newName;

	if (!dbus_message_get_args((DBusMessage*)message, NULL, DBUS_TYPE_STRING, &name, DBUS_TYPE_STRING, &oldName, DBUS_TYPE_STRING, &newName, DBUS_TYPE_INVALID))
	{
		printf("Invalid arguments for NameOwnerChanged signal\r\n");
		return HR_NOT_YET_HANDLED;
	}

	me->ListenerUpdateNameCache((const UTF8Char*)name, (const UTF8Char*)newName);

	UIntOS i = listener->callbacks->GetCount();
	while (i-- > 0)
	{
		cb = listener->callbacks->GetItemNoCheck(i);

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

		UnsafeArray<const UTF8Char> argument;
		/* Only auto remove if it is a bus name watch */
		if (listener->argument.SetTo(argument) && argument[0] == ':' && (cb->connFunc == NULL || cb->discFunc == NULL))
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
	NN<Listener> listener;
	if (!listener.Set((Listener*)userData))
	{
		return HR_NOT_YET_HANDLED;
	}
	NN<IO::DBusManager> me = listener->me;
	NN<ListenerCallbacks> cb;

	UIntOS i = listener->callbacks->GetCount();
	while (i-- > 0)
	{
		cb = listener->callbacks->GetItemNoCheck(i);
		IO::DBusManager::Message msg(message);

		if (cb->signalFunc && !cb->signalFunc(listener->me, msg, cb->userData))
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
	NN<ServiceData> data;
	if (!data.Set((ServiceData*)userObj))
	{
		return FALSE;
	}
	NN<ListenerCallbacks> cb = data->callback;
	NN<IO::DBusManager> me = data->me;

	me->ServiceDataFree(data);

	if (cb->connFunc)
		cb->connFunc(me, cb->userData);

	return FALSE;
}

Optional<IO::DBusManager::Listener> IO::DBusManager::ListenerFind()
{
	NN<ClassData> data = this->clsData;
	return data->listeners->GetItem(0);
}

void IO::DBusManager::ListenerBuildRule(NN<Listener> listener, NN<Text::StringBuilderUTF8> sb)
{
	sb->Append(CSTR("type='signal'"));
	UnsafeArrayOpt<const UTF8Char> sender = listener->name.NotNull()?listener->name:listener->owner;

	UnsafeArray<const UTF8Char> csptr;
	if (sender.SetTo(csptr))
	{
		sb->Append(CSTR(",sender='"));
		sb->AppendSlow(csptr);
		sb->AppendUTF8Char('\'');
	}
	if (listener->path.SetTo(csptr))
	{
		sb->Append(CSTR(",path='"));
		sb->AppendSlow(csptr);
		sb->AppendUTF8Char('\'');
	}
	if (listener->interface.SetTo(csptr))
	{
		sb->Append(CSTR(",interface='"));
		sb->AppendSlow(csptr);
		sb->AppendUTF8Char('\'');
	}
	if (listener->member.SetTo(csptr))
	{
		sb->Append(CSTR(",member='"));
		sb->AppendSlow(csptr);
		sb->AppendUTF8Char('\'');
	}
	if (listener->argument.SetTo(csptr))
	{
		sb->Append(CSTR(",arg0='"));
		sb->AppendSlow(csptr);
		sb->AppendUTF8Char('\'');
	}
}

Bool IO::DBusManager::ListenerAddMatch(NN<Listener> listener, RawMessageFunction hdlr)
{
	NN<ClassData> data = this->clsData;
	DBusError err;
	Text::StringBuilderUTF8 sb;

	this->ListenerBuildRule(listener, sb);
	dbus_error_init(&err);

	dbus_bus_add_match(data->conn, (const Char*)sb.v.Ptr(), &err);
	if (dbus_error_is_set(&err))
	{
		printf("Adding match rule \"%s\" failed: %s", sb.ToString().Ptr(), err.message);
		dbus_error_free(&err);
		return false;
	}

	listener->handleFunc = hdlr;
	listener->registered = true;

	return TRUE;
}

Bool IO::DBusManager::ListenerRemoveMatch(NN<Listener> listener)
{
	NN<ClassData> data = this->clsData;
	DBusError err;
	Text::StringBuilderUTF8 sb;

	this->ListenerBuildRule(listener, sb);
	dbus_error_init(&err);
	dbus_bus_remove_match(data->conn, (const Char*)sb.ToString().Ptr(), &err);
	if (dbus_error_is_set(&err))
	{
		printf("Removing owner match rule for %s failed: %s\r\n", sb.ToString(), err.message);
		dbus_error_free(&err);
		return false;
	}
	return true;
}

Optional<IO::DBusManager::Listener> IO::DBusManager::ListenerFindMatch(UnsafeArrayOpt<const UTF8Char> name, UnsafeArrayOpt<const UTF8Char> owner, UnsafeArrayOpt<const UTF8Char> path, UnsafeArrayOpt<const UTF8Char> interface, UnsafeArrayOpt<const UTF8Char> member, UnsafeArrayOpt<const UTF8Char> argument)
{
	NN<ClassData> data = this->clsData;
	NN<Listener> listener;
	UIntOS i = data->listeners->GetCount();
	while (i-- > 0)
	{
		listener = data->listeners->GetItemNoCheck(i);

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

		return listener;
	}

	return nullptr;
}

Optional<IO::DBusManager::Listener> IO::DBusManager::ListenerGet(RawMessageFunction filter, UnsafeArrayOpt<const UTF8Char> sender, UnsafeArrayOpt<const UTF8Char> path, UnsafeArrayOpt<const UTF8Char> interface, UnsafeArrayOpt<const UTF8Char> member, UnsafeArrayOpt<const UTF8Char> argument)
{
	NN<ClassData> data = this->clsData;
	NN<Listener> listener;
	UnsafeArrayOpt<const UTF8Char> name = nullptr;
	UnsafeArrayOpt<const UTF8Char> owner = nullptr;
	UnsafeArray<const UTF8Char> csptr;

	if (this->ListenerFind().IsNull())
	{
		if (!dbus_connection_add_filter(data->conn, DBusManager_WatchMessageFilter, NULL, NULL))
		{
			printf("dbus_connection_add_filter() failed\r\n");
			return nullptr;
		}
	}

	if (sender.SetTo(csptr))
	{
		if (csptr[0] == ':')
			owner = sender;
		else
			name = sender;
	}

	if (this->ListenerFindMatch(name, owner, path, interface, member, argument).SetTo(listener))
		return listener;

	listener = MemAllocNN(Listener);
	listener.ZeroContent();
	listener->me = *this;
	listener->name = Text::StrSCopyNew(name);
	listener->owner = Text::StrSCopyNew(owner);
	listener->path = Text::StrSCopyNew(path);
	listener->interface = Text::StrSCopyNew(interface);
	listener->member = Text::StrSCopyNew(member);
	listener->argument = Text::StrSCopyNew(argument);
	NEW_CLASSNN(listener->callbacks, Data::ArrayListNN<ListenerCallbacks>());
	NEW_CLASSNN(listener->processed, Data::ArrayListNN<ListenerCallbacks>());

	if (!this->ListenerAddMatch(listener, filter))
	{
		this->ListenerFree(listener);
		return nullptr;
	}

	data->listeners->Add(listener);

	return listener;
}

NN<IO::DBusManager::ListenerCallbacks> IO::DBusManager::ListenerAddCallback(NN<Listener> listener, WatchFunction connect, WatchFunction disconnect, SignalFunction signal, DestroyFunction destroy, AnyType userData)
{
	NN<ClassData> data = this->clsData;
	NN<ListenerCallbacks> cb = MemAllocNN(ListenerCallbacks);
	cb.ZeroContent();
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

Optional<IO::DBusManager::ListenerCallbacks> IO::DBusManager::ListenerFindCallback(NN<Listener> listener, UIntOS id)
{
	NN<ListenerCallbacks> cb;
	UIntOS i;
	i = listener->callbacks->GetCount();
	while (i-- > 0)
	{
		cb = listener->callbacks->GetItemNoCheck(i);
		if (cb->id == id)
		{
			return cb;
		}
	}
	i = listener->processed->GetCount();
	while (i-- > 0)
	{
		cb = listener->processed->GetItemNoCheck(i);
		if (cb->id == id)
		{
			return cb;
		}
	}
	return nullptr;
}

Bool IO::DBusManager::ListenerRemoveCallback(NN<Listener> listener, NN<ListenerCallbacks> cb)
{
	NN<ClassData> data = this->clsData;
	listener->callbacks->Remove(cb);
	listener->processed->Remove(cb);

	NN<ServiceData> cbdata;
	/* Cancel pending operations */
	if (cb->data.SetTo(cbdata))
	{
		if (cbdata->call)
			dbus_pending_call_cancel((DBusPendingCall*)cbdata->call);
		this->ServiceDataFree(cbdata);
	}

	if (cb->destroyFunc)
		cb->destroyFunc(cb->userData);

	MemFreeNN(cb);

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

void IO::DBusManager::ListenerFree(NN<Listener> listener)
{
	NN<ClassData> data = this->clsData;
	if (data->listeners->GetCount() == 0)
		dbus_connection_remove_filter(data->conn, DBusManager_WatchMessageFilter, this);

	listener->callbacks->MemFreeAll();
	listener->callbacks.Delete();
	listener->processed.Delete();
	this->RemoveWatch(listener->nameWatch);
	SDEL_TEXT(listener->name);
	SDEL_TEXT(listener->owner);
	SDEL_TEXT(listener->path);
	SDEL_TEXT(listener->interface);
	SDEL_TEXT(listener->member);
	SDEL_TEXT(listener->argument);
	MemFreeNN(listener);
}

void IO::DBusManager::ListenerCheckService(UnsafeArray<const UTF8Char> name, NN<ListenerCallbacks> callback)
{
	NN<ClassData> clsData = this->clsData;
	DBusMessage *message;
	NN<ServiceData> data;

	data = MemAllocNN(ServiceData);
	data.ZeroContent();
	data->me = *this;
	data->name = Text::StrCopyNew(name);
	data->callback = callback;

	callback->data = data;

	data->owner = this->ListenerCheckNameCache(name);
	if (data->owner.NotNull())
	{
		data->id = g_idle_add((GSourceFunc)ListenerUpdateService, data.Ptr());
		return;
	}

	message = dbus_message_new_method_call(DBUS_SERVICE_DBUS, DBUS_PATH_DBUS, DBUS_INTERFACE_DBUS, "GetNameOwner");
	if (message == NULL)
	{
		printf("Can't allocate new message\r\n");
		MemFreeNN(data);
		return;
	}

	dbus_message_append_args(message, DBUS_TYPE_STRING, &name, DBUS_TYPE_INVALID);

	if (dbus_connection_send_with_reply(clsData->conn, message, (DBusPendingCall**)&data->call, -1) == FALSE)
	{
		printf("Failed to execute method call\r\n");
		MemFreeNN(data);
	}
	else if (data->call == 0)
	{
		printf("D-Bus connection not available\r\n");
		MemFreeNN(data);
	}
	else
	{
		dbus_pending_call_set_notify((DBusPendingCall*)data->call, (DBusPendingCallNotifyFunction)ServiceReply, data.Ptr(), 0);
	}

	dbus_message_unref(message);
}

UnsafeArrayOpt<const UTF8Char> IO::DBusManager::ListenerCheckNameCache(UnsafeArray<const UTF8Char> name)
{
	NN<ClassData> clsData = this->clsData;
	NN<Listener> listener;
	UIntOS i = clsData->listeners->GetCount();
	while (i-- > 0)
	{
		listener = clsData->listeners->GetItemNoCheck(i);
		if (Text::StrEqualsN(listener->name, name))
		{
			return listener->owner;
		}
	}
	return nullptr;
}

void IO::DBusManager::ListenerUpdateNameCache(UnsafeArrayOpt<const UTF8Char> name, UnsafeArrayOpt<const UTF8Char> owner)
{
	NN<ClassData> clsData = this->clsData;
	NN<Listener> listener;
	UIntOS i = clsData->listeners->GetCount();
	while (i-- > 0)
	{
		listener = clsData->listeners->GetItemNoCheck(i);
		if (Text::StrEqualsN(listener->name, name))
		{
			SDEL_TEXT(listener->owner);
			listener->owner = Text::StrSCopyNew(owner);
		}
	}
}

IO::DBusManager::HandlerResult IO::DBusManager::WatchMessageFilter(Message *message)
{
	NN<ClassData> clsData = this->clsData;
	NN<Listener> listener;
	UnsafeArrayOpt<const UTF8Char> sender;
	UnsafeArrayOpt<const UTF8Char> path;
	UnsafeArrayOpt<const UTF8Char> iface;
	UnsafeArrayOpt<const UTF8Char> member;
	UnsafeArrayOpt<const UTF8Char> arg = nullptr;
	Data::ArrayListNN<Listener> deleteListener;

	if (message->GetType() != MT_SIGNAL)
		return HR_NOT_YET_HANDLED;

	sender = (const UTF8Char*)message->GetSender();
	path = (const UTF8Char*)message->GetPath();
	iface = (const UTF8Char*)message->GetInterface();
	member = (const UTF8Char*)message->GetMember();
	arg = (const UTF8Char*)message->GetArguments();

	UIntOS i = clsData->listeners->GetCount();
	while (i-- > 0)
	{
		listener = clsData->listeners->GetItemNoCheck(i);	

		if (sender.IsNull() && listener->owner.NotNull())
			continue;

		if (listener->owner.NotNull() && !Text::StrEqualsN(sender, listener->owner))
			continue;

		if (listener->path.NotNull() && !Text::StrEqualsN(path, listener->path))
			continue;

		if (listener->interface.NotNull() && !Text::StrEqualsN(iface, listener->interface))
			continue;

		if (listener->member.NotNull() && !Text::StrEqualsN(member, listener->member))
			continue;

		if (listener->argument.NotNull() && !Text::StrEqualsN(arg, listener->argument))
			continue;

		if (listener->handleFunc)
		{
			listener->lock = TRUE;

			listener->handleFunc(clsData->conn, (DBusMessage*)message->GetHandle(), listener.Ptr());

			listener->callbacks->Clear();
			listener->callbacks->AddAll(listener->processed);
			listener->processed->Clear();
			listener->lock = false;
		}

		if (listener->callbacks->GetCount() == 0)
		{
			deleteListener.Add(listener);
		}
	}

	if (deleteListener.GetCount() == 0)
		return HR_NOT_YET_HANDLED;

	i = deleteListener.GetCount();
	while (i-- > 0)
	{
		listener = deleteListener.GetItemNoCheck(i);		
		if (listener->callbacks->GetCount() != 0)
			continue;

		this->ListenerRemoveMatch(listener);
		clsData->listeners->Remove(listener);
		this->ListenerFree(listener);
	}
	return HR_NOT_YET_HANDLED;
/*	listener->callbacks.Delete();
	listener->processed.Delete();
	this->RemoveWatch(listener->nameWatch);
	SDEL_TEXT(listener->name);*/
}

UIntOS IO::DBusManager::AddServiceWatch(UnsafeArray<const UTF8Char> name, WatchFunction connect, WatchFunction disconnect, void *userData, DestroyFunction destroy)
{
	NN<Listener> listener;
	NN<ListenerCallbacks> cb;

	if (!this->ListenerGet(ServiceFilter, CSTR(DBUS_SERVICE_DBUS).v, CSTR(DBUS_PATH_DBUS).v, CSTR(DBUS_INTERFACE_DBUS).v, U8STR("NameOwnerChanged"), name).SetTo(listener))
		return 0;

	cb = this->ListenerAddCallback(listener, connect, disconnect, NULL, destroy, userData);

	if (connect)
		this->ListenerCheckService(name, cb);

	return cb->id;
}

UIntOS IO::DBusManager::AddSignalWatch(UnsafeArrayOpt<const UTF8Char> sender, UnsafeArrayOpt<const UTF8Char> path, UnsafeArrayOpt<const UTF8Char> interface, UnsafeArrayOpt<const UTF8Char> member, SignalFunction function, void *userData, DestroyFunction destroy)
{
	NN<Listener> listener;
	NN<ListenerCallbacks> cb;
	UnsafeArray<const UTF8Char> name;

	if (!this->ListenerGet(SignalFilter, sender, path, interface, member, nullptr).SetTo(listener))
		return 0;
	cb = this->ListenerAddCallback(listener, NULL, NULL, function, destroy, userData);

	if (listener->name.SetTo(name) && listener->nameWatch == 0)
		listener->nameWatch = this->AddServiceWatch(name, NULL, NULL, NULL, NULL);

	return cb->id;
}

UIntOS IO::DBusManager::AddPropertiesWatch(UnsafeArrayOpt<const UTF8Char> sender, UnsafeArrayOpt<const UTF8Char> path, UnsafeArrayOpt<const UTF8Char> interface, SignalFunction function, void *userData, DestroyFunction destroy)
{
	NN<Listener> listener;
	NN<ListenerCallbacks> cb;

	if (!this->ListenerGet(SignalFilter, sender, path, CSTR(DBUS_INTERFACE_PROPERTIES).v, U8STR("PropertiesChanged"), interface).SetTo(listener))
		return 0;

	cb = this->ListenerAddCallback(listener, NULL, NULL, function, destroy, userData);
	UnsafeArray<const UTF8Char> name;
	if (listener->name.SetTo(name) && listener->nameWatch == 0)
		listener->nameWatch = this->AddServiceWatch(name, NULL, NULL, NULL, NULL);

	return cb->id;
}

Bool IO::DBusManager::RemoveWatch(UIntOS id)
{
	NN<ClassData> data = this->clsData;
	if (id == 0)
		return false;

	if (data->listeners == 0)
	{
		return false;
	}

	UIntOS i = data->listeners->GetCount();
	while (i-- > 0)
	{
		NN<Listener> listener = data->listeners->GetItemNoCheck(i);
		NN<ListenerCallbacks> cb;
		if (this->ListenerFindCallback(listener, id).SetTo(cb))
		{
			this->ListenerRemoveCallback(listener, cb);
			return TRUE;
		}
	}

	return FALSE;
}

void IO::DBusManager::RemoveAllWatches()
{
	NN<ClassData> data = this->clsData;
	NN<Listener> listener;
	NN<ListenerCallbacks> cb;
	if (data->listeners)
	{
		UIntOS i = data->listeners->GetCount();
		UIntOS j;
		while (i-- > 0)
		{
			if (data->listeners->RemoveAt(i).SetTo(listener))
			{
				j = listener->callbacks->GetCount();
				while (j-- > 0)
				{
					cb = listener->callbacks->GetItemNoCheck(j);
					if (cb->discFunc) cb->discFunc(*this, cb->userData);
					if (cb->destroyFunc) cb->destroyFunc(cb->userData);
				}
				this->ListenerFree(listener);
			}
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
	NN<ClassData> clsData = this->clsData;
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

IO::DBusManager::ErrorType IO::DBusManager::PolkitCheckAuthorization(const Char *action, Bool interaction, PolkitFunction function, AnyType userData, Int32 timeout)
{
	NN<ClassData> clsData = this->clsData;
	NN<AuthorizationData> data;
	DBusMessage *msg;
	DBusMessageIter iter;
	DBusPendingCall *call;
	UInt32 flags = 0x00000000;

	if (clsData->conn == NULL)
		return ET_INVAL;

	data = MemAllocNN(AuthorizationData);
	msg = dbus_message_new_method_call(AUTHORITY_DBUS, AUTHORITY_PATH, AUTHORITY_INTF, "CheckAuthorization");
	if (msg == NULL)
	{
		MemFreeNN(data);
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
		MemFreeNN(data);
		return ET_IO_ERROR;
	}

	if (call == NULL)
	{
		dbus_message_unref(msg);
		MemFreeNN(data);
		return ET_IO_ERROR;
	}

	data->function = function;
	data->userData = userData;

	dbus_pending_call_set_notify(call, (DBusPendingCallNotifyFunction)AuthorizationReply, data.Ptr(), MemFree);
	dbus_message_unref(msg);

	return ET_SUCCESS;
}
