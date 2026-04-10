#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "Map/GoogleMap/GoogleSearcher.h"
#include "Net/OSSocketFactory.h"
#include "Net/SSLEngineFactory.h"

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	UTF8Char sbuff[256];
	NN<Net::SocketFactory> sockf;
	NN<Net::TCPClientFactory> clif;
	Optional<Net::SSLEngine> ssl;
	IO::ConsoleWriter console;
	Map::GoogleMap::GoogleSearcher *gooSrch;

	NEW_CLASSNN(sockf, Net::OSSocketFactory(false));
	NEW_CLASSNN(clif, Net::TCPClientFactory(sockf));
	ssl = Net::SSLEngineFactory::Create(clif, false);
	NEW_CLASS(gooSrch, Map::GoogleMap::GoogleSearcher(clif, ssl, Text::CString(nullptr), nullptr, nullptr, console));
	gooSrch->SearchName(sbuff, sizeof(sbuff), Math::Coord2DDbl(114.2, 22.4), 0x0409);

	DEL_CLASS(gooSrch);
	ssl.Delete();
	clif.Delete();
	sockf.Delete();
	return 0;
}
