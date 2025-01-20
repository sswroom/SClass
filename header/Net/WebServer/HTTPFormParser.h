#ifndef _SM_NET_WEBSERVER_HTTPFORMPARSER
#define _SM_NET_WEBSERVER_HTTPFORMPARSER
#include "Data/ArrayListStrUTF8.h"
#include "Net/WebServer/WebRequest.h"

namespace Net
{
	namespace WebServer
	{
		class HTTPFormParser
		{
		private:
			Data::ArrayListStrUTF8 *strNames;
			Data::ArrayListStrUTF8 *strValues;
		public:
			HTTPFormParser(Net::WebServer::WebRequest *req, Int32 codePage);
			~HTTPFormParser();

			UOSInt GetStrCount() const;
			UnsafeArrayOpt<const UTF8Char> GetStrName(UOSInt index);
			UnsafeArrayOpt<const UTF8Char> GetStrValue(const UTF8Char *strName);
		};
	}
}
#endif
