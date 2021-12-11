#ifndef _SM_NET_HKOWEATHER
#define _SM_NET_HKOWEATHER
#include "Net/RSSReader.h"
#include "Net/SocketFactory.h"
#include "Net/SSLEngine.h"
#include "Text/EncodingFactory.h"

namespace Net
{
	class HKOWeather : public Net::RSSHandler
	{
	public:
		typedef enum
		{
			WS_NONE = 0,
			WS_TYPHOON_1 = 1,
			WS_TYPHOON_3 = 2,
			WS_TYPHOON_8NE = 3,
			WS_TYPHOON_8NW = 4,
			WS_TYPHOON_8SE = 5,
			WS_TYPHOON_8SW = 6,
			WS_TYPHOON_9 = 7,
			WS_TYPHOON_10 = 8,
			WS_TYPHOON_MASK = 15,
			WS_RAIN_YELLOW = 16,
			WS_RAIN_RED = 32,
			WS_RAIN_BLACK = 48,
			WS_RAIN_MASK = 0x70,
			WS_FIRE_YELLOW = 0x80,
			WS_FIRE_RED = 0x100,
			WS_FIRE_MASK = 0x180,
			WS_THUNDERSTORM = 0x200,
			WS_FLOODING = 0x400,
			WS_LANDSLIP = 0x800,
			WS_STRONGMONSOON = 0x1000,
			WS_FROST = 0x2000,
			WS_COLD = 0x4000,
			WS_VERYHOT = 0x8000,
			WS_TSUNAMI = 0x10000,

			WS_UNKNOWN = 0xffffffff
		} WeatherSignal;

		typedef void (__stdcall *UpdateHandler)(WeatherSignal updatedSignal);
	private:
		UpdateHandler hdlr;
		Net::SocketFactory *sockf;
		Net::SSLEngine *ssl;
		Text::EncodingFactory *encFact;
		Net::RSSReader *rss;
		WeatherSignal currSignal;

		static WeatherSignal String2Signal(Text::String *textMessage);
	public:
		static WeatherSignal GetSignalSummary(Net::SocketFactory *sockf, Net::SSLEngine *ssl, Text::EncodingFactory *encFact);

		HKOWeather(Net::SocketFactory *sockf, Net::SSLEngine *ssl, Text::EncodingFactory *encFact, UpdateHandler hdlr);
		virtual ~HKOWeather();

		virtual void ItemAdded(Net::RSSItem *item);
		virtual void ItemRemoved(Net::RSSItem *item);
		WeatherSignal GetCurrentSignal();
	};
}
#endif
