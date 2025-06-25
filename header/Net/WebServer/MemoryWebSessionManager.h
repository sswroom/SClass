#ifndef _SM_NET_WEBSERVER_MEMORYWEBSESSIONMANAGER
#define _SM_NET_WEBSERVER_MEMORYWEBSESSIONMANAGER
#include "AnyType.h"
#include "Data/ArrayListInt64.h"
#include "Data/ArrayListNN.h"
#include "Net/WebServer/WebSessionManager.h"
#include "Net/WebServer/MemoryWebSession.h"
#include "Sync/Mutex.h"

namespace Net
{
	namespace WebServer
	{
		class MemoryWebSessionManager : public Net::WebServer::WebSessionManager
		{
		private:
			Data::ArrayListInt64 sessIds;
			Data::ArrayListNN<Net::WebServer::MemoryWebSession> sesses;
			Sync::Mutex mut;
			NN<Text::String> path;
			NN<Text::String> cookieName;
			Bool forceSecure;
			Bool checkReferer;

			Int32 chkInterval;
			SessionHandler chkHdlr;
			AnyType chkHdlrObj;
			Bool chkRunning;
			Bool chkToStop;
			Sync::Event chkEvt;

			static UInt32 __stdcall CheckThread(AnyType userObj);
			Int64 GetSessId(NN<Net::WebServer::WebRequest> req);
		public:
			MemoryWebSessionManager(Text::CStringNN path, SessionHandler delHdlr, AnyType delHdlrObj, Int32 chkInterval, SessionHandler chkHdlr, AnyType chkHdlrObj, Text::CString cookieName);
			virtual ~MemoryWebSessionManager();

			virtual Optional<WebSession> GetSession(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp);
			virtual NN<WebSession> CreateSession(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp);
			virtual void DeleteSession(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp);

			void SetForceSecure(Bool forceSecure);
			void SetCheckReferer(Bool checkReferer);
			Int64 GenSessId(NN<Net::WebServer::WebRequest> req);
			NN<WebSession> CreateSession(Int64 sessId, Text::CStringNN origin);
			Optional<WebSession> GetSession(Int64 sessId);
			void DeleteSession(Int64 sessId);

			void GetSessionIds(NN<Data::ArrayList<Int64>> sessIds);
		};
	}
}
#endif
