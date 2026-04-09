#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Math/BigIntLSB.h"

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	UTF8Char buff[4096];
	Math::BigIntLSB bi(1024);
	Math::BigIntLSB bi2(512);
	bi = -1213546541;
	bi2 = -14121244;
	bi.ToString(buff);
	printf("bi = %s\n", buff);
	bi2.ToString(buff);
	printf("bi2 = %s\n", buff);
	bi += bi2;
	bi.ToString(buff);
	printf("bi + bi2 = %s\n", buff);

	Math::BigIntLSB bi3(16);
	return 0;
}