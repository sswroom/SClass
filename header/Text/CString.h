#ifndef _SM_TEXT_CSTRING
#define _SM_TEXT_CSTRING
#include "Text/StringBase.h"
namespace Text
{
	struct CString : public StringBase<const UTF8Char>
	{
		CString()
		{
		}

		CString(const UTF8Char *v, UOSInt leng)
		{
			this->v = v;
			this->leng = leng;
		}
	};
}
#endif
