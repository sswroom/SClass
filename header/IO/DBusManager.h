#ifndef _SM_IO_DBUSMANAGER
#define _SM_IO_DBUSMANAGER
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

		typedef Bool (__stdcall* SignalFunction) (DBusManager *dbusManager, void *message, void *userData);
		typedef void (__stdcall* WatchFunction) (DBusManager *dbusManager, void *userData);
		typedef void (__stdcall* DestroyFunction) (void *userData);

		struct ListenerCallbacks;

		typedef struct
		{
			void *call; //DBusPendingCall *call;
			const Char *name;
			const Char *owner;
			UOSInt id;
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
			DBusHandleMessageFunction handleFunc;
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
	public:
		DBusManager(DBusType dbType, const Char *name);
		~DBusManager();

		Bool IsError();
		DBusManager *Ref();
		void Unref();
		void *GetHandle();

		//Watch
	private:
		void ServiceDataFree(ServiceData *data);

		void ListenerBuildRule(Listener *listener, Text::StringBuilderC *sb);
		Bool ListenerAddMatch(Listener *listener, void *hdlr);
		Bool ListenerRemoveMatch(Listener *listener);
		ListenerCallbacks *ListenerFindCallback(Listener *listener, UOSInt id);
		Bool ListenerRemoveCallback(Listener *listener, ListenerCallbacks *cb);
		void ListenerFree(Listener *listener);
		
	public:
		Bool RemoveWatch(UOSInt id);
		void RemoveAllWatches();
	};
}
#endif
