#ifndef _SM_NET_WEBSERVER_MEMORYWEBSESSIONMANAGER
#define _SM_NET_WEBSERVER_MEMORYWEBSESSIONMANAGER
#include "AnyType.h"
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
			NN<Text::String> path;
			NN<Text::String> cookieName;

			Int32 chkInterval;
			SessionHandler chkHdlr;
			AnyType chkHdlrObj;
			Bool chkRunning;
			Bool chkToStop;
			Sync::Event chkEvt;

			static UInt32 __stdcall CheckThread(AnyType userObj);
			Int64 GetSessId(NN<Net::WebServer::IWebRequest> req);
		public:
			MemoryWebSessionManager(Text::CStringNN path, SessionHandler delHdlr, AnyType delHdlrObj, Int32 chkInterval, SessionHandler chkHdlr, AnyType chkHdlrObj, Text::CString cookieName);
			virtual ~MemoryWebSessionManager();

			virtual Optional<IWebSession> GetSession(NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp);
			virtual NN<IWebSession> CreateSession(NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp);
			virtual void DeleteSession(NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp);

			Int64 GenSessId(NN<Net::WebServer::IWebRequest> req);
			NN<IWebSession> CreateSession(Int64 sessId);
			Optional<IWebSession> GetSession(Int64 sessId);
			void DeleteSession(Int64 sessId);

			void GetSessionIds(NN<Data::ArrayList<Int64>> sessIds);
		};
	}
}
#endif
