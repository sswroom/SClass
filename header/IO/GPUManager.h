#ifndef _SM_IO_GPUMANAGER
#define _SM_IO_GPUMANAGER
#include "IO/GPUControl.h"

namespace IO
{
	class GPUManager
	{
	private:
		struct ClassData;
		
		ClassData *clsData;

	public:
		GPUManager();
		~GPUManager();

		UOSInt GetGPUCount();
		Optional<IO::GPUControl> GetGPUControl(UOSInt index);
	};
}
#endif
