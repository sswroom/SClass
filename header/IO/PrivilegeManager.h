#ifndef _SM_IO_PRIVILEGEMANAGER
#define _SM_IO_PRIVILEGEMANAGER

namespace IO
{
	class PrivilegeManager
	{
	private:
		struct ClassData;
		NN<ClassData> clsData;

	public:
		PrivilegeManager();
		~PrivilegeManager();

		Bool EnableSystemTime();
	};
}

#endif
