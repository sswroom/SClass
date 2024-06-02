#ifndef _SM_MANAGE_COMPUTER
#define _SM_MANAGE_COMPUTER

namespace Manage
{
	class Computer
	{
	public:
		static UnsafeArrayOpt<UTF8Char> GetHostName(UnsafeArray<UTF8Char> sbuff);
	};
}

#endif
