#ifndef _SM_MANAGE_COMPUTERUSER
#define _SM_MANAGE_COMPUTERUSER

namespace Manage
{
	class ComputerUser
	{
	public:
		static UnsafeArrayOpt<UTF8Char> GetProcessUser(UnsafeArray<UTF8Char> sbuff);
	};
}

#endif
