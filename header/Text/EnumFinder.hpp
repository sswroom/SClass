#ifndef _SM_TEXT_ENUMFINDER
#define _SM_TEXT_ENUMFINDER
#include "Text/MyString.h"

namespace Text
{
	template<class T> class EnumFinder
	{
	private:
		UnsafeArray<const Char> searchStr;
		T val;
		Bool found;
	public:
		EnumFinder(UnsafeArray<const Char> searchStr, T defVal)
		{
			this->searchStr = searchStr;
			this->val = defVal;
			this->found = false;
		}

		~EnumFinder()
		{

		}

		void Entry(const Char *enumName, T e)
		{
			if (!this->found)
			{
				if (Text::StrEqualsCh(this->searchStr, enumName))
				{
					this->val = e;
					this->found = true;
				}
			}
		}

		T GetResult()
		{
			return this->val;
		}
	};
}
#endif
