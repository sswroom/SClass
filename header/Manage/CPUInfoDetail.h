#ifndef _SM_MANAGE_CPUINFODETAIL
#define _SM_MANAGE_CPUINFODETAIL
#include "Manage/CPUInfo.h"

namespace Manage
{
	class CPUInfoDetail : public Manage::CPUInfo
	{
	private:
		const UTF8Char *cpuModel;

	public:
		CPUInfoDetail();
		virtual ~CPUInfoDetail();

		const UTF8Char *GetCPUModel();
		Int32 GetTCC(); //Thermal Control Circuit value
		Bool GetCPUTemp(UOSInt index, Double *temp);
	};
}

#endif
