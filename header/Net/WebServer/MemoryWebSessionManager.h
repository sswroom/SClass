#ifndef _SM_NET_WEBSERVER_MEMORYWEBSESSIONMANAGER
#define _SM_NET_WEBSERVER_MEMORYWEBSESSIONMANAGER
#include "Data/ArrayListInt64.h"
#include "Net/WebServer/IWebSessionManager.h"
#include "Net/WebServer/MemoryWebSession.h"
#include "Sync/Mutex.h"

namespace Net
{
	namespace WebServer
	{
		class MemoryWebSessionManager : public Net::WebServer::IWebSessionManager
		{
		private:
			Data::ArrayListInt64 *sessIds;
			Data::ArrayList<Net::WebServer::MemoryWebSession*> *sesses;
			Sync::Mutex *mut;
			const UTF8Char *path;

			Int32 chkInterval;
			SessionHandler chkHdlr;
			void *chkHdlrObj;
			Bool chkRunning;
			Bool chkToStop;
			Sync::Event *chkEvt;

			static UInt32 __stdcall CheckThread(void *userObj);
			Int64 GetSessId(Net::WebServer::IWebRequest *req);
		public:
			MemoryWebSessionManager(const UTF8Char *path, SessionHandler delHdlr, void *delHdlrObj, Int32 chkInterval, SessionHandler chkHdlr, void *chkHdlrObj);
			virtual ~MemoryWebSessionManager();

			virtual IWebSession *GetSession(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp);
			virtual IWebSession *CreateSession(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp);
			virtual void DeleteSession(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp);

			Int64 GenSessId(Net::WebServer::IWebRequest *req);
			IWebSession *CreateSession(Int64 sessId);
			IWebSession *GetSession(Int64 sessId);
			void DeleteSession(Int64 sessId);

			void GetSessionIds(Data::ArrayList<Int64> *sessIds);
		};
	};
};
#endif
