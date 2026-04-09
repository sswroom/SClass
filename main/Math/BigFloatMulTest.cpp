#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Math/BigFloat.h"
#include "Text/MyString.h"

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	NN<Math::BigFloat> bf;
	NN<Math::BigFloat> bf2;
	UTF8Char sbuff[4096];
	NEW_CLASSNN(bf, Math::BigFloat(64));
	NEW_CLASSNN(bf2, Math::BigFloat(64));

	*bf.Ptr() = CSTR("2");
	*bf2.Ptr() = CSTR("16");
	bf->ToString(sbuff);
	printf("%s\n", sbuff);

	*bf2.Ptr() *= bf2;
	*bf.Ptr() += bf2;
	*bf2.Ptr() *= bf2;
	*bf2.Ptr() *= bf2;
	*bf2.Ptr() *= bf2;
	bf->ToString(sbuff);
	printf("%s\n", sbuff);

	*bf.Ptr() -= bf2;

	bf2->ToString(sbuff);
	printf("%s\n", sbuff);

	bf->ToString(sbuff);
	printf("%s\n", sbuff);

	bf.Delete();
	bf2.Delete();
	return 0;
}
