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

		virtual Bool Write(Text::CStringNN str) = 0;
		virtual Bool WriteLine(Text::CStringNN str) = 0;
		virtual Bool WriteLine() = 0;

		Bool WriteLines(Data::ArrayIterator<NN<Text::String>> it)
		{
			NN<Text::String> s;
			while (it.HasNext())
			{
				s = it.Next();
				if (!WriteLine(s->ToCString()))
					return false;
			}
			return true;
		}
	};
}
#endif
