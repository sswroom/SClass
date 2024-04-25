#include "Stdafx.h"
#include "Core/Core.h"
#include "Crypto/Encrypt/RSACipher.h"
#include "Text/MyString.h"
#include <stdio.h>

Int32 MyMain(NN<Core::IProgControl> progCtrl)
{
	UInt8 hashVal[50];
	UTF8Char sbuff[128];
	UTF8Char *sptr;

	Crypto::Encrypt::RSACipher::MGF1(hashVal, UTF8STRC("foo"), 3, Crypto::Hash::HashType::SHA1);
	sptr = Text::StrHexBytes(sbuff, hashVal, 3, 0);
	if (!Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("1AC907")))
	{
		return 1;
	}

	Crypto::Encrypt::RSACipher::MGF1(hashVal, UTF8STRC("foo"), 5, Crypto::Hash::HashType::SHA1);
	sptr = Text::StrHexBytes(sbuff, hashVal, 5, 0);
	if (!Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("1AC9075CD4")))
	{
		return 1;
	}

	Crypto::Encrypt::RSACipher::MGF1(hashVal, UTF8STRC("bar"), 5, Crypto::Hash::HashType::SHA1);
	sptr = Text::StrHexBytes(sbuff, hashVal, 5, 0);
	if (!Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("BC0C655E01")))
	{
		return 1;
	}

	Crypto::Encrypt::RSACipher::MGF1(hashVal, UTF8STRC("bar"), 50, Crypto::Hash::HashType::SHA1);
	sptr = Text::StrHexBytes(sbuff, hashVal, 50, 0);
	if (!Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("BC0C655E016BC2931D85A2E675181ADCEF7F581F76DF2739DA74FAAC41627BE2F7F415C89E983FD0CE80CED9878641CB4876")))
	{
		return 1;
	}

	Crypto::Encrypt::RSACipher::MGF1(hashVal, UTF8STRC("bar"), 50, Crypto::Hash::HashType::SHA256);
	sptr = Text::StrHexBytes(sbuff, hashVal, 50, 0);
	if (!Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("382576A7841021CC28FC4C0948753FB8312090CEA942EA4C4E735D10DC724B155F9F6069F289D61DACA0CB814502EF04EAE1")))
	{
		return 1;
	}

	return 0;
}
