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

		UIntOS GetGPUCount();
		Optional<IO::GPUControl> GetGPUControl(UIntOS index);
	};
}
#endif
