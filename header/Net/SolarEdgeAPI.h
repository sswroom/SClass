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
			Optional<Text::String> name;
			Int32 accountId;
			Optional<Text::String> status;
			Double peakPower_kWp;
			Data::Timestamp lastUpdateTime;
			Optional<Text::String> currency;
			Data::Timestamp installationDate;
			Data::Timestamp ptoDate;
			Optional<Text::String> notes;
			Optional<Text::String> type;
			Optional<Text::String> country;
			Optional<Text::String> city;
			Optional<Text::String> address;
			Optional<Text::String> address2;
			Optional<Text::String> zip;
			Optional<Text::String> timeZone;
			Optional<Text::String> countryCode;
			Bool isPublic;
			Optional<Text::String> publicName;
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
		Optional<Net::SSLEngine> ssl;
		NotNullPtr<Text::String> apikey;

		void BuildURL(NotNullPtr<Text::StringBuilderUTF8> sb, Text::CString path);
		Text::JSONBase *GetJSON(Text::CStringNN url);
	public:
		SolarEdgeAPI(NotNullPtr<Net::SocketFactory> sockf, Optional<Net::SSLEngine> ssl, Text::CString apikey);
		~SolarEdgeAPI();

		Optional<Text::String> GetCurrentVersion();
		Bool GetSupportedVersions(Data::ArrayListStringNN *versions);
		Bool GetSiteList(Data::ArrayList<Site*> *versions, UOSInt maxCount, UOSInt startOfst, UOSInt *totalCount);
		void FreeSiteList(Data::ArrayList<Site*> *versions);
		Bool GetSiteOverview(Int32 siteId, SiteOverview *overview);
		Bool GetSiteEnergy(Int32 siteId, Data::Timestamp startTime, Data::Timestamp endTime, TimeUnit timeUnit, Data::ArrayList<TimedValue> *values);
		Bool GetSitePower(Int32 siteId, Data::Timestamp startTime, Data::Timestamp endTime, Data::ArrayList<TimedValue> *values);

		static void AppendFormDate(NotNullPtr<Text::StringBuilderUTF8> sb, Data::Timestamp ts, Bool hasTime);
		static Text::CStringNN TimeUnitGetName(TimeUnit timeUnit);
	};
}
#endif
