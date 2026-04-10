#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "Map/GoogleMap/GoogleWSSearcherJSON.h"
#include "Net/OSSocketFactory.h"
#include "Net/SSLEngineFactory.h"
#include "Text/EncodingFactory.h"

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	UTF8Char sbuff[256];
	Map::ReverseGeocoder *revGeo;
	NN<Net::SocketFactory> sockf;
	NN<Net::TCPClientFactory> clif;
	Optional<Net::SSLEngine> ssl;
	IO::ConsoleWriter console;
	NN<Text::EncodingFactory> encFact;

	NEW_CLASSNN(encFact, Text::EncodingFactory());
	NEW_CLASSNN(sockf, Net::OSSocketFactory(false));
	NEW_CLASSNN(clif, Net::TCPClientFactory(sockf));
	ssl = Net::SSLEngineFactory::Create(clif, false);
	NEW_CLASS(revGeo, Map::GoogleMap::GoogleWSSearcherJSON(clif, ssl, console, encFact));

	IntOS i = 5;
	while (i-- > 0)
	{
		revGeo->SearchName(sbuff, sizeof(sbuff), Math::Coord2DDbl(151.2204210, -33.82967410), 0x0C04);//L"zh-HK");
	}

	DEL_CLASS(revGeo);
	ssl.Delete();
	clif.Delete();
	sockf.Delete();
	encFact.Delete();
	return 0;
}

