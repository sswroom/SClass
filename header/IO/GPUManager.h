#ifndef _SM_IO_GPUMANAGER
#define _SM_IO_GPUMANAGER
#include "IO/IGPUControl.h"

namespace IO
{
	class GPUManager
	{
	private:
		void *classObj;

	public:
		GPUManager();
		~GPUManager();

		UOSInt GetGPUCount();
		IO::IGPUControl *GetGPUControl(UOSInt index);
	};
}
#endif
