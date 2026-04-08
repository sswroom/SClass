#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Crypto/Hash/CRC16.h"

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	UInt16 crcVal;
	UInt8 buff[] = ">UAVDCzUAAAAAAQEAAAAAAAAAAAAAAAAAAA==*3E9C";
	Crypto::Hash::CRC16 crc(Crypto::Hash::CRC16::GetPolynomialCCITT());
	crcVal = crc.CalcDirect(buff, 37);
	printf("CRC16-CCITT: %04X\r\n", crcVal);
	return 0;
}
