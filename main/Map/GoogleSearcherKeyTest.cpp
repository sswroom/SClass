#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Data/RandomOS.h"
#include "IO/ConsoleWriter.h"
#include "Map/GoogleMap/GoogleSearcher.h"
#include "Net/OSSocketFactory.h"
#include "Net/SSLEngineFactory.h"

Text::CStringNN gooKey = CSTR("");

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	NN<Net::SocketFactory> sockf;
	NN<Net::TCPClientFactory> clif;
	Optional<Net::SSLEngine> ssl;
	IO::ConsoleWriter console;
	Data::Random *rand;
	Int32 i;
	UTF8Char buff[1024];

	NEW_CLASSNN(sockf, Net::OSSocketFactory(false));
	NEW_CLASSNN(clif, Net::TCPClientFactory(sockf));
	ssl = Net::SSLEngineFactory::Create(clif, false);
	NEW_CLASS(rand, Data::RandomOS());

	Map::GoogleMap::GoogleSearcher *goo;
	NEW_CLASS(goo, Map::GoogleMap::GoogleSearcher(clif, ssl, gooKey, nullptr, nullptr, console));

	i = 1;
	while (i-- > 0)
	{
		Double lat = 22.15 + rand->NextDouble() / 2;
		Double lon = 113.95 + rand->NextDouble() / 2;
		printf("%d: %lf, %lf -> ", i, lat, lon);
		goo->SearchName(buff, 1024, Math::Coord2DDbl(lon, lat), CSTR("en-us"));
		printf("%s\n", buff);
	}

	DEL_CLASS(goo);


	DEL_CLASS(rand);
	ssl.Delete();
	clif.Delete();
	sockf.Delete();
	return 0;
}
