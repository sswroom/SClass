#include "Stdafx.h"
#include "Crypto/Encrypt/RC2Cipher.h"
#include "Core/ByteTool_C.h"

UInt8 Crypto::Encrypt::RC2Cipher::permute[256] = {
	217,120,249,196, 25,221,181,237, 40,233,253,121, 74,160,216,157,
	198,126, 55,131, 43,118, 83,142, 98, 76,100,136, 68,139,251,162,
	 23,154, 89,245,135,179, 79, 19, 97, 69,109,141,  9,129,125, 50,
	189,143, 64,235,134,183,123, 11,240,149, 33, 34, 92,107, 78,130,
	 84,214,101,147,206, 96,178, 28,115, 86,192, 20,167,140,241,220,
	 18,117,202, 31, 59,190,228,209, 66, 61,212, 48,163, 60,182, 38,
	111,191, 14,218, 70,105,  7, 87, 39,242, 29,155,188,148, 67,  3,
	248, 17,199,246,144,239, 62,231,  6,195,213, 47,200,102, 30,215,
	  8,232,234,222,128, 82,238,247,132,170,114,172, 53, 77,106, 42,
	150, 26,210,113, 90, 21, 73,116, 75,159,208, 94,  4, 24,164,236,
	194,224, 65,110, 15, 81,203,204, 36,145,175, 80,161,244,112, 57,
	153,124, 58,133, 35,184,180,122,252,  2, 54, 91, 37, 85,151, 49,
	 45, 93,250,152,227,138,146,174,  5,223, 41, 16,103,108,186,201,
	211,  0,230,207,225,158,168, 44, 99, 22,  1, 63, 88,226,137,169,
	 13, 56, 52, 27,171, 51,255,176,187, 72, 12, 95,185,177,205, 46,
	197,243,219, 71,229,165,156,119, 10,166, 32,104,254,127,193,173
};

Crypto::Encrypt::RC2Cipher::RC2Cipher(UnsafeArray<const UInt8> key, UIntOS keyLen) : BlockCipher(8)
{
	this->SetKey(key, keyLen);
}

Crypto::Encrypt::RC2Cipher::~RC2Cipher()
{

}


Bool Crypto::Encrypt::RC2Cipher::SetKey(UnsafeArray<const UInt8> key, UIntOS keyLen)
{
	return this->SetKey(key, keyLen, keyLen << 3);
}

Bool Crypto::Encrypt::RC2Cipher::SetKey(UnsafeArray<const UInt8> key, UIntOS keyLen, UIntOS effectiveBits)
{
	UInt8 tmp[128];
	UIntOS T8;
	UIntOS TM;
	UIntOS i;

	if (keyLen == 0 || keyLen > 128)
	{
		return false;
	}

	i = 0;
	while (i < keyLen)
	{
		tmp[i] = key[i];
		i++;
	}

	while (i < 128)
	{
		tmp[i] = permute[(tmp[i - 1] + tmp[i - keyLen]) & 255];
		i++;
	}

	T8   = (UInt16)((effectiveBits + 7) >> 3);
	TM   = ((UIntOS)255 >> (IntOS)(7 & -(IntOS)effectiveBits));
	tmp[128 - T8] = permute[tmp[128 - T8] & TM];
	if (T8 < 128)
	{
		i = 127 - T8;
		while (true)
		{
			tmp[i] = permute[tmp[i + 1] ^ tmp[i + T8]];

			if (i == 0)
			{
				break;
			}
			i--;
		}
	}

	i = 0;
	while (i < 64)
	{
		this->xkeys[i] = ReadUInt16(&tmp[2 * i]);
		i++;
	}
	return true;
}

UIntOS Crypto::Encrypt::RC2Cipher::EncryptBlock(UnsafeArray<const UInt8> inBlock, UnsafeArray<UInt8> outBlock) const
{
	const UInt16 *xkey = this->xkeys;
	UInt16 x76;
	UInt16 x54;
	UInt16 x32;
	UInt16 x10;
	IntOS i;

    x10 = ReadUInt16(&inBlock[0]);
    x32 = ReadUInt16(&inBlock[2]);
    x54 = ReadUInt16(&inBlock[4]);
    x76 = ReadUInt16(&inBlock[6]);

	i = 0;
	while (i < 16)
	{
        x10 = (UInt16)((x10 + (x32 & ~x76) + (x54 & x76) + xkey[4*i+0]) & 0xFFFF);
        x10 = (UInt16)(((x10 << 1) | (x10 >> 15)));

        x32 = (UInt16)((x32 + (x54 & ~x10) + (x76 & x10) + xkey[4*i+1]) & 0xFFFF);
        x32 = (UInt16)(((x32 << 2) | (x32 >> 14)));

        x54 = (UInt16)((x54 + (x76 & ~x32) + (x10 & x32) + xkey[4*i+2]) & 0xFFFF);
        x54 = (UInt16)(((x54 << 3) | (x54 >> 13)));

        x76 = (UInt16)((x76 + (x10 & ~x54) + (x32 & x54) + xkey[4*i+3]) & 0xFFFF);
        x76 =(UInt16)( ((x76 << 5) | (x76 >> 11)));

        if (i == 4 || i == 10)
		{
            x10 = (UInt16)((x10 + xkey[x76 & 63]) & 0xFFFF);
            x32 = (UInt16)((x32 + xkey[x10 & 63]) & 0xFFFF);
            x54 = (UInt16)((x54 + xkey[x32 & 63]) & 0xFFFF);
            x76 = (UInt16)((x76 + xkey[x54 & 63]) & 0xFFFF);
        }
		i++;
    }

	WriteUInt16(&outBlock[0], x10);
	WriteUInt16(&outBlock[2], x32);
	WriteUInt16(&outBlock[4], x54);
	WriteUInt16(&outBlock[6], x76);

    return 8;
}

UIntOS Crypto::Encrypt::RC2Cipher::DecryptBlock(UnsafeArray<const UInt8> inBlock, UnsafeArray<UInt8> outBlock) const
{
    UInt16 x76;
	UInt16 x54;
	UInt16 x32;
	UInt16 x10;
    const UInt16 *xkey = this->xkeys;
    IntOS i;

    x10 = ReadUInt16(&inBlock[0]);
    x32 = ReadUInt16(&inBlock[2]);
    x54 = ReadUInt16(&inBlock[4]);
    x76 = ReadUInt16(&inBlock[6]);

	i = 15;
	while (i >= 0)
	{
        if (i == 4 || i == 10) {
            x76 = (UInt16)((x76 - xkey[x54 & 63]) & 0xFFFF);
            x54 = (UInt16)((x54 - xkey[x32 & 63]) & 0xFFFF);
            x32 = (UInt16)((x32 - xkey[x10 & 63]) & 0xFFFF);
            x10 = (UInt16)((x10 - xkey[x76 & 63]) & 0xFFFF);
        }

        x76 = (UInt16)(((x76 << 11) | (x76 >> 5)));
        x76 = (UInt16)((x76 - ((x10 & ~x54) + (x32 & x54) + xkey[4*i+3])) & 0xFFFF);

        x54 = (UInt16)(((x54 << 13) | (x54 >> 3)));
        x54 = (UInt16)((x54 - ((x76 & ~x32) + (x10 & x32) + xkey[4*i+2])) & 0xFFFF);

        x32 = (UInt16)(((x32 << 14) | (x32 >> 2)));
        x32 = (UInt16)((x32 - ((x54 & ~x10) + (x76 & x10) + xkey[4*i+1])) & 0xFFFF);

        x10 = (UInt16)(((x10 << 15) | (x10 >> 1)));
        x10 = (UInt16)((x10 - ((x32 & ~x76) + (x54 & x76) + xkey[4*i+0])) & 0xFFFF);

		i--;
	}
	WriteUInt16(&outBlock[0], x10);
	WriteUInt16(&outBlock[2], x32);
	WriteUInt16(&outBlock[4], x54);
	WriteUInt16(&outBlock[6], x76);

    return 8;
}
