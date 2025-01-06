#ifndef _SM_SSWR_ORGANWEB_ORGANWEBSESSION
#define _SM_SSWR_ORGANWEB_ORGANWEBSESSION
#include "Net/WebServer/IWebSession.h"

namespace SSWR
{
	namespace OrganWeb
	{
		class OrganWebSession
		{
		private:
			Optional<Net::WebServer::IWebSession> sess;
		public:
			OrganWebSession();
			OrganWebSession(Optional<Net::WebServer::IWebSession> sess);
			~OrganWebSession();

			void Use(Optional<Net::WebServer::IWebSession> sess);
			void EndUse();
			Optional<Net::WebServer::IWebSession> GetSess();
			void SetPickObjType(Int32 pickObjType);
		};
	}
}
#endif
