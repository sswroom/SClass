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
			Net::WebServer::MemoryWebSessionManager *sessMgr;
			SSWR::OrganWeb::OrganWebEnv *env;
			UInt32 scnSize;

			static Bool __stdcall OnSessionDel(Net::WebServer::IWebSession* sess, void *userObj);
			static Bool __stdcall OnSessionCheck(Net::WebServer::IWebSession* sess, void *userObj);

		public:
			OrganWebHandler(OrganWebEnv *env, UInt32 scnSize);
			virtual ~OrganWebHandler();
		};
	}
}
#endif