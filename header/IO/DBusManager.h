#ifndef _SM_IO_DBUSMANAGER
#define _SM_IO_DBUSMANAGER
#include "Data/ArrayList.h"
#include "Text/StringBuilderC.h"

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

			void *GetHandle();
		};

		struct GenericData;
		struct InterfaceData;
		struct ListenerCallbacks;
		struct PropertyTable;
		struct MethodTable;
		struct SignalTable;
		struct WatchInfo;
		struct ArgInfo;

		typedef Bool (*PropertyGetter)(const PropertyTable *property, void *itera, void *data);
		typedef void (*PropertySetter)(const PropertyTable *property, void *value, UInt32 id, void *data);
		typedef Bool (*PropertyExists)(const PropertyTable *property, void *data);
		typedef void (__stdcall* MessageFunction) (DBusManager *dbusManager, Message *message, void *userData);
		typedef Bool (__stdcall* SignalFunction) (DBusManager *dbusManager, Message *message, void *userData);
		typedef void (__stdcall* WatchFunction) (DBusManager *dbusManager, void *userData);
		typedef void (__stdcall* DestroyFunction) (void *userData);
		typedef Message *(__stdcall* MethodFunction) (DBusManager *dbusManager, Message *message, void *userData);

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

		typedef struct
		{
			IO::DBusManager *me;
			void *call; //DBusPendingCall *call;
			const Char *name;
			const Char *owner;
			UInt32 id;
			ListenerCallbacks *callback;
		} ServiceData;

		struct ListenerCallbacks
		{
			IO::DBusManager::WatchFunction connFunc;
			IO::DBusManager::WatchFunction discFunc;
			IO::DBusManager::SignalFunction signalFunc;
			IO::DBusManager::DestroyFunction destroyFunc;
			ServiceData *data;
			void *userData;
			UOSInt id;
		};

		typedef struct
		{
			IO::DBusManager *me;
			RawMessageFunction handleFunc;
			const Char *name;
			const Char *owner;
			const Char *path;
			const Char *interface;
			const Char *member;
			const Char *argument;
			Data::ArrayList<ListenerCallbacks*> *callbacks;
			Data::ArrayList<ListenerCallbacks*> *processed;
			UOSInt nameWatch;
			Bool lock;
			Bool registered;
		} Listener;


	private:
		void *clsData;

		void SetupDbusWithMainLoop();
		Bool SetupBus(const Char *name);
	public:
		DBusManager(DBusType dbType, const Char *name);
		~DBusManager();

		Bool IsError();
		DBusManager *Ref();
		void Unref();
		void *GetHandle();
		void QueueDispatch(Int32 status);

		//Object
		GenericData *ObjectPathRef(const Char *path);
		Bool AttachObjectManager();
		void GenericDataFree(GenericData *data);
		void GenericUnregister(GenericData *data);
		HandlerResult GenericMessage(GenericData *data, Message *message);
		GenericData *InvalidateParentData(const Char *childPath);
		void AddPending(GenericData *data);
		InterfaceData *FindInterface(GenericData *data, const Char *name);

		Bool AddInterface(GenericData *data, const Char *name, const MethodTable *methods, const SignalTable *signals, const PropertyTable *properties, void *userData, DestroyFunction destroy);

		Bool CheckExperimental(Int32 flags, Int32 flag);
		void AppendName(const Char *name, void *itera);
		void AppendProperty(InterfaceData *iface, const PropertyTable *p, void *itera);
		void AppendProperties(InterfaceData *iface, void *itera);
		void AppendInterface(InterfaceData *iface, void *itera);
		void ProcessPropertyChanges(GenericData *data);
		void ProcessPropertiesFromInterface(GenericData *data, InterfaceData *iface);
		void EmitInterfacesAdded(GenericData *data);
		void EmitInterfacesRemoved(GenericData *data);

		void Flush();
		void RemovePending(GenericData *data);
		static Bool ProcessChanges(void *userData);
		Bool SendMessageWithReply(void *message, void **call, Int32 timeout);

		//Watch
	private:
		void ServiceDataFree(ServiceData *data);
		static Bool UpdateService(void *userObj);
		static void ServiceReply(void *pending, void *userObj);
		static HandlerResult ServiceFilter(void *connection, void *message, void *userData);
		static HandlerResult SignalFilter(void *connection, void *message, void *userData);

		static Bool ListenerUpdateService(void *userObj);
		Listener *ListenerFind();
		void ListenerBuildRule(Listener *listener, Text::StringBuilderC *sb);
		Bool ListenerAddMatch(Listener *listener, RawMessageFunction hdlr);
		Bool ListenerRemoveMatch(Listener *listener);
		Listener *ListenerFindMatch(const Char *name, const Char *owner, const Char *path, const Char *interface, const Char *member, const Char *argument);
		Listener *ListenerGet(RawMessageFunction filter, const Char *sender, const Char *path, const Char *interface, const Char *member, const Char *argument);
		ListenerCallbacks *ListenerAddCallback(Listener *listener, WatchFunction connect, WatchFunction disconnect, SignalFunction signal, DestroyFunction destroy, void *userData);
		ListenerCallbacks *ListenerFindCallback(Listener *listener, UOSInt id);
		Bool ListenerRemoveCallback(Listener *listener, ListenerCallbacks *cb);
		void ListenerFree(Listener *listener);
		void ListenerCheckService(const Char *name, ListenerCallbacks *callback);
		const Char *ListenerCheckNameCache(const Char *name);
		void ListenerUpdateNameCache(const Char *name, const Char *owner);
		
	public:
		HandlerResult WatchMessageFilter(Message *message);
		UOSInt AddServiceWatch(const Char *name, WatchFunction connect, WatchFunction disconnect, void *userData, DestroyFunction destroy);
		UOSInt AddSignalWatch(const Char *sender, const Char *path, const Char *interface, const Char *member, SignalFunction function, void *userData, DestroyFunction destroy);
		UOSInt AddPropertiesWatch(const Char *sender, const Char *path, const Char *interface, SignalFunction function, void *userData, DestroyFunction destroy);
		Bool RemoveWatch(UOSInt id);
		void RemoveAllWatches();
	};
}
#endif
