#ifndef _SM_NET_SOLAREDGEAPI
#define _SM_NET_SOLAREDGEAPI
#include "Net/SSLEngine.h"
#include "Text/JSON.h"
#include "Text/StringBuilderUTF8.h"

namespace Net
{
	class SolarEdgeAPI
	{
	public:
		struct Site
		{
			Int32 id;
			Text::String *name;
			Int32 accountId;
			Text::String *status;
			Double peakPower_kWp;
			Data::Timestamp lastUpdateTime;
			Text::String *currency;
			Data::Timestamp installationDate;
			Data::Timestamp ptoDate;
			Text::String *notes;
			Text::String *type;
			Text::String *country;
			Text::String *city;
			Text::String *address;
			Text::String *address2;
			Text::String *zip;
			Text::String *timeZone;
			Text::String *countryCode;
			Bool isPublic;
			Text::String *publicName;
		};

		struct SiteOverview
		{
			Data::Timestamp lastUpdateTime;
			Double lifeTimeEnergy_Wh;
			Double lifeTimeRevenue;
			Double yearlyEnergy_Wh;
			Double monthlyEnergy_Wh;
			Double dailyEnergy_Wh;
			Double currentPower_W;
		};
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
		Bool GetSiteList(Data::ArrayList<Site*> *versions, UOSInt maxCount, UOSInt startOfst, UOSInt *totalCount);
		void FreeSiteList(Data::ArrayList<Site*> *versions);
		Bool GetSiteOverview(Int32 siteId, SiteOverview *overview);
	};
}
#endif
