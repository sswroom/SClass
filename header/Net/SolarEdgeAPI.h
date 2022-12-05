#ifndef _SM_NET_SOLAREDGEAPI
#define _SM_NET_SOLAREDGEAPI
#include "Net/SSLEngine.h"
#include "Text/JSON.h"
#include "Text/StringBuilderUTF8.h"

namespace Net
{
	class SolarEdgeAPI
	{
	private:
		Net::SocketFactory *sockf;
		Net::SSLEngine *ssl;
		Text::String *apikey;

		void BuildURL(Text::StringBuilderUTF8 *sb, Text::CString path);
		Text::JSONBase *GetJSON(Text::CString url);
	public:
		SolarEdgeAPI(Net::SocketFactory *sockf, Net::SSLEngine *ssl, Text::CString apikey);
		~SolarEdgeAPI();

		Text::String *GetCurrentVersion();
		Bool GetSupportedVersions(Data::ArrayList<Text::String*> *versions);
	};
}
#endif
