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
		enum class TimeUnit
		{
			DAY,
			QUARTER_OF_AN_HOUR,
			HOUR,
			WEEK,
			MONTH,
			YEAR
		};

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

		struct TimedValue
		{
			Data::Timestamp ts;
			Double value;

			TimedValue() = default;
			TimedValue(Int32 *)
			{
				this->ts = 0;
				this->value = 0;
			}

			TimedValue(const Data::Timestamp &ts, Double value)
			{
				this->ts = ts;
				this->value = value;
			}

			Bool operator==(const TimedValue &val)
			{
				return this->ts == val.ts && this->value == val.value;
			}
		};
	private:
		NotNullPtr<Net::SocketFactory> sockf;
		Net::SSLEngine *ssl;
		NotNullPtr<Text::String> apikey;

		void BuildURL(Text::StringBuilderUTF8 *sb, Text::CString path);
		Text::JSONBase *GetJSON(Text::CString url);
	public:
		SolarEdgeAPI(NotNullPtr<Net::SocketFactory> sockf, Net::SSLEngine *ssl, Text::CString apikey);
		~SolarEdgeAPI();

		Text::String *GetCurrentVersion();
		Bool GetSupportedVersions(Data::ArrayListNN<Text::String> *versions);
		Bool GetSiteList(Data::ArrayList<Site*> *versions, UOSInt maxCount, UOSInt startOfst, UOSInt *totalCount);
		void FreeSiteList(Data::ArrayList<Site*> *versions);
		Bool GetSiteOverview(Int32 siteId, SiteOverview *overview);
		Bool GetSiteEnergy(Int32 siteId, Data::Timestamp startTime, Data::Timestamp endTime, TimeUnit timeUnit, Data::ArrayList<TimedValue> *values);
		Bool GetSitePower(Int32 siteId, Data::Timestamp startTime, Data::Timestamp endTime, Data::ArrayList<TimedValue> *values);

		static void AppendFormDate(Text::StringBuilderUTF8 *sb, Data::Timestamp ts, Bool hasTime);
		static Text::CString TimeUnitGetName(TimeUnit timeUnit);
	};
}
#endif
