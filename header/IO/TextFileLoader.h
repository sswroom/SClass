#ifndef _SM_IO_TEXTFILELOADER
#define _SM_IO_TEXTFILELOADER
#include "Data/ArrayListDbl.h"
#include "Text/CString.h"

namespace IO
{
	class TextFileLoader
	{
	public:
		static Bool LoadDoubleList(Text::CStringNN fileName, NN<Data::ArrayListDbl> dblList);
	};
}

#endif
