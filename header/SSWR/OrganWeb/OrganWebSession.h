#ifndef _SM_SSWR_ORGANWEB_ORGANWEBSESSION
#define _SM_SSWR_ORGANWEB_ORGANWEBSESSION
#include "Net/WebServer/WebSession.h"

namespace SSWR
{
	namespace OrganWeb
	{
		class OrganWebSession
		{
		private:
			Optional<Net::WebServer::WebSession> sess;
		public:
			OrganWebSession();
			OrganWebSession(Optional<Net::WebServer::WebSession> sess);
			~OrganWebSession();

			void Use(Optional<Net::WebServer::WebSession> sess);
			void EndUse();
			Optional<Net::WebServer::WebSession> GetSess();
			void SetPickObjType(Int32 pickObjType);
		};
	}
}
#endif
