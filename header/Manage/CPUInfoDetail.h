#ifndef _SM_MANAGE_CPUINFODETAIL
#define _SM_MANAGE_CPUINFODETAIL
#include "Manage/CPUInfo.h"
#include "Text/CString.h"

namespace Manage
{
	class CPUInfoDetail : public Manage::CPUInfo
	{
	private:
		Text::CString cpuModel;

	public:
		CPUInfoDetail();
		virtual ~CPUInfoDetail();

		Text::CString GetCPUModel();
		Int32 GetTCC(); //Thermal Control Circuit value
		Bool GetCPUTemp(UOSInt index, Double *temp);
	};
}

#endif
