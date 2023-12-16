#ifndef _SM_IO_WRITER
#define _SM_IO_WRITER
#include "Data/ArrayIterator.h"
#include "Text/CString.h"
#include "Text/String.h"

namespace IO
{
	class Writer
	{
	public:
		virtual ~Writer() {};

		virtual Bool WriteStrC(const UTF8Char *str, UOSInt nChar) = 0;
		virtual Bool WriteLineC(const UTF8Char *str, UOSInt nChar) = 0;
		virtual Bool WriteLine() = 0;

		Bool WriteStr(Text::CString str)
		{
			return WriteStrC(str.v, str.leng);
		}

		Bool WriteLineCStr(Text::CString str)
		{
			return WriteLineC(str.v, str.leng);
		}

		Bool WriteLines(Data::ArrayIterator<NotNullPtr<Text::String>> it)
		{
			NotNullPtr<Text::String> s;
			while (it.HasNext())
			{
				s = it.Next();
				if (!WriteLineC(s->v, s->leng))
					return false;
			}
			return true;
		}
	};
}
#endif
