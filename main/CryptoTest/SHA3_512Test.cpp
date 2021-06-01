#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Crypto/Hash/SHA512.h"
#include "IO/ConsoleWriter.h"
#include "Text/MyString.h"

Int32 MyMain(Core::IProgControl *progCtrl)
{
	UInt8 hashVal[64];
	UTF8Char sbuff[129];
	Crypto::Hash::IHash *hash;
	IO::ConsoleWriter console;

	NEW_CLASS(hash, Crypto::Hash::SHA512());

	hash->GetValue(hashVal);
	Text::StrHexBytes(sbuff, hashVal, 64, 0);
	console.WriteLine(sbuff);
	console.WriteLine((const UTF8Char*)"a69f73cca23a9ac5c8b567dc185a756e97c982164fe25859e0d1dcc1475c80a615b2123af1f5f94c11e3e9402c3ac558f500199d95b6d3e301758586281dcd26");
	console.WriteLine();

	const UInt8 testBlock[] = "abc";
	hash->Calc(testBlock, sizeof(testBlock) - 1);
	hash->GetValue(hashVal);
	Text::StrHexBytes(sbuff, hashVal, 64, 0);
	console.WriteLine(sbuff);
	console.WriteLine((const UTF8Char*)"b751850b1a57168a5693cd924b6b096e08f621827444f70d884f5d0240d2712e10e116e9192af3c91a7ec57647e3934057340b4cf408d5a56592f8274eec53f0");
	console.WriteLine();

	const UInt8 testBlock2[] = "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";
	hash->Clear();
	hash->Calc(testBlock2, sizeof(testBlock2) - 1);
	hash->GetValue(hashVal);
	Text::StrHexBytes(sbuff, hashVal, 64, 0);
	console.WriteLine(sbuff);
	console.WriteLine((const UTF8Char*)"04a371e84ecfb5b8b77cb48610fca8182dd457ce6f326a0fd3d7ec2f1e91636dee691fbe0c985302ba1b0d8dc78c086346b533b49c030d99a27daf1139d6e75e");

	DEL_CLASS(hash);
	return 0;
}
