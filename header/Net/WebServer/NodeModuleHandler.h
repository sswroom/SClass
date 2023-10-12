#ifndef _SM_NET_WEBSERVER_NODEMODULEHANDLER
#define _SM_NET_WEBSERVER_NODEMODULEHANDLER
#include "Net/WebServer/HTTPDirectoryHandler.h"

namespace Net
{
	namespace WebServer
	{
		class NodeModuleHandler : public Net::WebServer::HTTPDirectoryHandler
		{
		public:
			NodeModuleHandler(Text::CStringNN rootDir, UInt64 fileCacheSize);
			virtual ~NodeModuleHandler();
			virtual Bool FileValid(Text::CStringNN subReq);
		};
	}
}
#endif
