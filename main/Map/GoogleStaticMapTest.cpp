#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/FileStream.h"
#include "Map/GoogleMap/GoogleStaticMap.h"
#include "Net/OSSocketFactory.h"
#include "Net/SSLEngineFactory.h"

Text::CStringNN gooKey = CSTR("");

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	NN<Net::SocketFactory> sockf;
	NN<Net::TCPClientFactory> clif;
	Optional<Net::SSLEngine> ssl;
	UInt8 *imgBuff;
	Map::GoogleMap::GoogleStaticMap *map;
	UIntOS retSize;
	IO::FileStream *stm;

	NEW_CLASSNN(sockf, Net::OSSocketFactory(false));
	NEW_CLASSNN(clif, Net::TCPClientFactory(sockf));
	ssl = Net::SSLEngineFactory::Create(clif, false);

	imgBuff = MemAlloc(UInt8, 10485760);
	NEW_CLASS(map, Map::GoogleMap::GoogleStaticMap(clif, ssl, gooKey, nullptr, nullptr));
	retSize = map->GetMap(imgBuff, 22.4, 114.2, 100000, Math::Size2D<UIntOS>(1024, 768), CSTR("en-us"), 0, 0, 0);
	if (retSize)
	{
		NEW_CLASS(stm, IO::FileStream(CSTR("Test.png"), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
		stm->Write(Data::ByteArray(imgBuff, retSize));
		DEL_CLASS(stm);
	}
	DEL_CLASS(map);
	MemFree(imgBuff);
	ssl.Delete();
	clif.Delete();
	sockf.Delete();
	return 0;
}