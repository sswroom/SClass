#ifndef _SM_IO_DBUSMANAGER
#define _SM_IO_DBUSMANAGER
#include "AnyType.h"
#include "Data/ArrayListNN.hpp"
#include "Text/StringBuilderUTF8.h"

namespace IO
{
	class DBusManager
	{
	public:
		typedef enum
		{
			DBT_SESSION,
			DBT_SYSTEM,
			DBT_STARTER
		} DBusType;

		typedef enum
		{
			MT_INVALID,
			MT_METHOD_CALL,
			MT_METHOD_RETURN,
			MT_ERROR,
			MT_SIGNAL
		} MessageType;

		typedef enum
		{
			HR_HANDLED,
			HR_NOT_YET_HANDLED,
			HR_NEED_MEMORY
		} HandlerResult;

		typedef enum
		{
			ET_SUCCESS,
			ET_NOMEM,
			ET_INVAL,
			ET_IO_ERROR
		} ErrorType;

		typedef HandlerResult (* RawMessageFunction)(void *connection, void *message, void *userData);

		class Message
		{
		private:
			void *message;

		public:
			Message(void *message);
			~Message();

			MessageType GetType();
			const Char *GetSender();
			const Char *GetPath();
			const Char *GetInterface();
			const Char *GetMember();
			const Char *GetArguments();
			const Char *GetSignature();
			Bool GetNoReply();

			void *GetHandle();
		};

		struct GenericData;
		struct InterfaceData;
		struct SecurityData;
		struct BuiltinSecurityData;
		struct AuthorizationData;
		struct ListenerCallbacks;
		struct PropertyTable;
		struct MethodTable;
		struct SignalTable;
		struct SecurityTable;
		struct WatchInfo;
		struct ArgInfo;

		typedef Bool (*PropertyGetter)(NN<const PropertyTable> property, void *itera, AnyType data);
		typedef void (*PropertySetter)(const PropertyTable *property, void *value, UInt32 id, void *data);
		typedef Bool (*PropertyExists)(NN<const PropertyTable> property, AnyType data);
		typedef void (CALLBACKFUNC MessageFunction) (NN<DBusManager> dbusManager, NN<Message> message, AnyType userData);
		typedef Bool (CALLBACKFUNC SignalFunction) (NN<DBusManager> dbusManager, NN<Message> message, AnyType userData);
		typedef void (CALLBACKFUNC WatchFunction) (NN<DBusManager> dbusManager, AnyType userData);
		typedef void (CALLBACKFUNC DestroyFunction) (AnyType userData);
		typedef void *(CALLBACKFUNC MethodFunction) (DBusManager *dbusManager, NN<Message> message, AnyType userData);
		typedef void (CALLBACKFUNC SecurityFunction) (DBusManager *dbusManager, const Char *action, Bool interaction, UInt32 pending);
		typedef void (CALLBACKFUNC PolkitFunction) (Bool authorized, AnyType userData);

		enum PropertyFlags
		{
			PF_DEPRECATED   = (1 << 0),
			PF_EXPERIMENTAL = (1 << 1),
		};

		enum MethodFlags
		{
			MF_DEPRECATED   = (1 << 0),
			MF_NOREPLY      = (1 << 1),
			MF_ASYNC        = (1 << 2),
			MF_EXPERIMENTAL = (1 << 3),
		};

		enum SignalFlags
		{
			SF_DEPRECATED   = (1 << 0),
			SF_EXPERIMENTAL = (1 << 1),
		};

		enum SecurityFlags
		{
			SECF_DEPRECATED        = (1 << 0),
			SECF_BUILTIN           = (1 << 1),
			SECF_ALLOW_INTERACTION = (1 << 2),
		};

		typedef struct
		{
			NN<IO::DBusManager> me;
			void *call; //DBusPendingCall *call;
			UnsafeArray<const UTF8Char> name;
			UnsafeArrayOpt<const UTF8Char> owner;
			UInt32 id;
			NN<ListenerCallbacks> callback;
		} ServiceData;

		struct ListenerCallbacks
		{
			IO::DBusManager::WatchFunction connFunc;
			IO::DBusManager::WatchFunction discFunc;
			IO::DBusManager::SignalFunction signalFunc;
			IO::DBusManager::DestroyFunction destroyFunc;
			Optional<ServiceData> data;
			AnyType userData;
			UIntOS id;
		};

		typedef struct
		{
			NN<IO::DBusManager> me;
			RawMessageFunction handleFunc;
			UnsafeArrayOpt<const UTF8Char> name;
			UnsafeArrayOpt<const UTF8Char> owner;
			UnsafeArrayOpt<const UTF8Char> path;
			UnsafeArrayOpt<const UTF8Char> interface;
			UnsafeArrayOpt<const UTF8Char> member;
			UnsafeArrayOpt<const UTF8Char> argument;
			NN<Data::ArrayListNN<ListenerCallbacks>> callbacks;
			NN<Data::ArrayListNN<ListenerCallbacks>> processed;
			UIntOS nameWatch;
			Bool lock;
			Bool registered;
		} Listener;


	private:
		struct ClassData;
		NN<ClassData> clsData;
		static const struct MethodTable managerMethods[];
		static const struct SignalTable managerSignals[];
		static const struct MethodTable introspectMethods[];

		void SetupDbusWithMainLoop();
		Bool SetupBus(const Char *name);
	public:
		DBusManager(DBusType dbType, const Char *name);
		~DBusManager();

		Bool IsError();
		NN<DBusManager> Ref();
		void Unref();
		void *GetHandle();
		void QueueDispatch(Int32 status);

		//Object
		static void *GetObjects(IO::DBusManager *dbusManager, Message *message, void *userData);
		static void *Introspect(IO::DBusManager *dbusManager, Message *message, void *userData);
		void PrintArguments(NN<Text::StringBuilderUTF8> sb, const ArgInfo *args, const Char *direction);
		void GenerateInterfaceXml(NN<Text::StringBuilderUTF8> sb, NN<InterfaceData> iface);
		void GenerateIntrospectionXml(NN<GenericData> data, const Char *path);
		void AppendInterfaces(NN<GenericData> data, void *itera);
		void AppendObject(NN<GenericData> data, void *itera);
		void PendingSuccess(UInt32 pending);
		void *CreateError(void *message, const Char *name, const Char *errMsg);
		Bool SendError(void *message, const Char *name, const Char *errMsg);
		void PendingError(UInt32 pending, const Char *name, const Char *errMsg);

		Optional<GenericData> ObjectPathRef(UnsafeArray<const UTF8Char> path);
		Bool AttachObjectManager();
		void GenericDataFree(NN<GenericData> data);
		void GenericUnregister(NN<GenericData> data);
		HandlerResult GenericMessage(NN<GenericData> data, NN<Message> message);
		HandlerResult ProcessMessage(NN<Message> message, const MethodTable *method, AnyType ifaceUserData);
		Optional<GenericData> InvalidateParentData(UnsafeArray<const UTF8Char> childPath);
		void AddPending(NN<GenericData> data);
		Optional<InterfaceData> FindInterface(NN<GenericData> data, UnsafeArray<const UTF8Char> name);
		Bool AddInterface(NN<GenericData> data, UnsafeArray<const UTF8Char> name, const MethodTable *methods, UnsafeArrayOpt<const SignalTable> signals, UnsafeArrayOpt<const PropertyTable> properties, AnyType userData, DestroyFunction destroy);
		Bool ArgsHaveSignature(const ArgInfo *args, NN<Message> message);

		static void BuiltinSecurityResult(Bool authorized, AnyType userData);
		void BuiltinSecurityFunction(const Char *action, Bool interaction, UInt32 pending);
		Bool CheckPrivilege(NN<Message> message, const MethodTable *method, AnyType ifaceUserData);

		Bool CheckSignal(UnsafeArray<const UTF8Char> path, UnsafeArray<const UTF8Char> interface, UnsafeArray<const UTF8Char> name, OutParam<const ArgInfo*> args);
		Bool CheckExperimental(Int32 flags, Int32 flag);
		void AppendName(UnsafeArray<const UTF8Char> name, void *itera);
		void AppendProperty(NN<InterfaceData> iface, NN<const PropertyTable> p, void *itera);
		void AppendProperties(NN<InterfaceData> iface, void *itera);
		void AppendInterface(NN<InterfaceData> iface, void *itera);
		void ProcessPropertyChanges(NN<GenericData> data);
		void ProcessPropertiesFromInterface(NN<GenericData> data, NN<InterfaceData> iface);
		void EmitInterfacesAdded(NN<GenericData> data);
		void EmitInterfacesRemoved(NN<GenericData> data);

		void Flush();
		void RemovePending(NN<GenericData> data);
		static Bool ProcessChanges(void *userData);
		Bool SendMessageWithReply(void *message, void **call, Int32 timeout);
		Bool SendMessage(void *message);

		//Watch
	private:
		void ServiceDataFree(NN<ServiceData> data);
		static Bool UpdateService(void *userObj);
		static void ServiceReply(void *pending, void *userObj);
		static HandlerResult ServiceFilter(void *connection, void *message, void *userData);
		static HandlerResult SignalFilter(void *connection, void *message, void *userData);

		static Bool ListenerUpdateService(void *userObj);
		Optional<Listener> ListenerFind();
		void ListenerBuildRule(NN<Listener> listener, NN<Text::StringBuilderUTF8> sb);
		Bool ListenerAddMatch(NN<Listener> listener, RawMessageFunction hdlr);
		Bool ListenerRemoveMatch(NN<Listener> listener);
		Optional<Listener> ListenerFindMatch(UnsafeArrayOpt<const UTF8Char> name, UnsafeArrayOpt<const UTF8Char> owner, UnsafeArrayOpt<const UTF8Char> path, UnsafeArrayOpt<const UTF8Char> interface, UnsafeArrayOpt<const UTF8Char> member, UnsafeArrayOpt<const UTF8Char> argument);
		Optional<Listener> ListenerGet(RawMessageFunction filter, UnsafeArrayOpt<const UTF8Char> sender, UnsafeArrayOpt<const UTF8Char> path, UnsafeArrayOpt<const UTF8Char> interface, UnsafeArrayOpt<const UTF8Char> member, UnsafeArrayOpt<const UTF8Char> argument);
		NN<ListenerCallbacks> ListenerAddCallback(NN<Listener> listener, WatchFunction connect, WatchFunction disconnect, SignalFunction signal, DestroyFunction destroy, AnyType userData);
		Optional<ListenerCallbacks> ListenerFindCallback(NN<Listener> listener, UIntOS id);
		Bool ListenerRemoveCallback(NN<Listener> listener, NN<ListenerCallbacks> cb);
		void ListenerFree(NN<Listener> listener);
		void ListenerCheckService(UnsafeArray<const UTF8Char> name, NN<ListenerCallbacks> callback);
		UnsafeArrayOpt<const UTF8Char> ListenerCheckNameCache(UnsafeArray<const UTF8Char> name);
		void ListenerUpdateNameCache(UnsafeArrayOpt<const UTF8Char> name, UnsafeArrayOpt<const UTF8Char> owner);
		
	public:
		HandlerResult WatchMessageFilter(Message *message);
		UIntOS AddServiceWatch(UnsafeArray<const UTF8Char> name, WatchFunction connect, WatchFunction disconnect, void *userData, DestroyFunction destroy);
		UIntOS AddSignalWatch(UnsafeArrayOpt<const UTF8Char> sender, UnsafeArrayOpt<const UTF8Char> path, UnsafeArrayOpt<const UTF8Char> interface, UnsafeArrayOpt<const UTF8Char> member, SignalFunction function, void *userData, DestroyFunction destroy);
		UIntOS AddPropertiesWatch(UnsafeArrayOpt<const UTF8Char> sender, UnsafeArrayOpt<const UTF8Char> path, UnsafeArrayOpt<const UTF8Char> interface, SignalFunction function, void *userData, DestroyFunction destroy);
		Bool RemoveWatch(UIntOS id);
		void RemoveAllWatches();

		//polkit
	private:
		void AddDictWithStringValue(void *itera, const Char *key, const Char *str);
		void AddEmptyStringDict(void *itera);
		void AddArguments(void *itera, const Char *action, UInt32 flags);
		static Bool PolkitParseResult(void *itera);
		static void AuthorizationReply(void *call, void *userData);
		ErrorType PolkitCheckAuthorization(const Char *action, Bool interaction, PolkitFunction function, AnyType userData, Int32 timeout);
	};
}
#endif
