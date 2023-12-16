#ifndef _SM_NET_WEBSERVER_MEMORYWEBSESSIONMANAGER
#define _SM_NET_WEBSERVER_MEMORYWEBSESSIONMANAGER
#include "Data/ArrayListInt64.h"
#include "Data/ArrayListNN.h"
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
			Data::ArrayListInt64 sessIds;
			Data::ArrayListNN<Net::WebServer::MemoryWebSession> sesses;
			Sync::Mutex mut;
			NotNullPtr<Text::String> path;
			NotNullPtr<Text::String> cookieName;

			Int32 chkInterval;
			SessionHandler chkHdlr;
			void *chkHdlrObj;
			Bool chkRunning;
			Bool chkToStop;
			Sync::Event chkEvt;

			static UInt32 __stdcall CheckThread(void *userObj);
			Int64 GetSessId(NotNullPtr<Net::WebServer::IWebRequest> req);
		public:
			MemoryWebSessionManager(Text::CString path, SessionHandler delHdlr, void *delHdlrObj, Int32 chkInterval, SessionHandler chkHdlr, void *chkHdlrObj, Text::CString cookieName);
			virtual ~MemoryWebSessionManager();

			virtual Optional<IWebSession> GetSession(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp);
			virtual NotNullPtr<IWebSession> CreateSession(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp);
			virtual void DeleteSession(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp);

			Int64 GenSessId(NotNullPtr<Net::WebServer::IWebRequest> req);
			NotNullPtr<IWebSession> CreateSession(Int64 sessId);
			Optional<IWebSession> GetSession(Int64 sessId);
			void DeleteSession(Int64 sessId);

			void GetSessionIds(Data::ArrayList<Int64> *sessIds);
		};
	}
}
#endif
