#ifndef _SM_SSWR_ORGANWEB_ORGANWEBHANDLER
#define _SM_SSWR_ORGANWEB_ORGANWEBHANDLER
#include "Data/RandomOS.h"
#include "IO/MemoryStream.h"
#include "Media/LRGBLimiter.h"
#include "Media/CS/CSConverter.h"
#include "Media/Resizer/LanczosResizerLR_C32.h"
#include "Net/WebServer/MemoryWebSessionManager.h"
#include "Net/WebServer/WebControllerHandler.h"
#include "SSWR/OrganWeb/OrganWebCommon.h"
#include "Sync/RWMutexUsage.h"

namespace SSWR
{
	namespace OrganWeb
	{
		class OrganWebEnv;

		class OrganWebHandler : public Net::WebServer::WebControllerHandler
		{
		private:
			NN<Net::WebServer::MemoryWebSessionManager> sessMgr;
			NN<SSWR::OrganWeb::OrganWebEnv> env;
			UInt32 scnSize;

			static Bool __stdcall OnSessionDel(NN<Net::WebServer::WebSession> sess, AnyType userObj);
			static Bool __stdcall OnSessionCheck(NN<Net::WebServer::WebSession> sess, AnyType userObj);

		public:
			OrganWebHandler(NN<OrganWebEnv> env, UInt32 scnSize, Text::CStringNN rootDir);
			virtual ~OrganWebHandler();
		};
	}
}
#endif
