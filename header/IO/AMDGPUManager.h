#ifndef _SM_IO_AMDGPUMANAGER
#define _SM_IO_AMDGPUMANAGER
#include "Data/ArrayListArr.hpp"
#include "IO/GPUControl.h"
#include "IO/Library.h"

namespace IO
{
	class AMDGPUManager
	{
	private:
		Optional<IO::Library> lib;
		void *funcs;
		UInt8 *adapterInfos;
		Optional<Data::ArrayListArr<UInt8>> adapterList;

	public:
		AMDGPUManager();
		~AMDGPUManager();

		UIntOS GetGPUCount();
		Optional<IO::GPUControl> CreateGPUControl(UIntOS index);

		int Overdrive_Caps(int iAdapterIndex, int *iSupported, int *iEnabled, int *iVersion);
		int Overdrive5_ThermalDevices_Enum(int iAdapterIndex, int iThermalControllerIndex, void *lpThermalControllerInfo);
		int Overdrive5_Temperature_Get(int iAdapterIndex, int iThermalControllerIndex, void *lpTemperature);
		int Overdrive5_CurrentActivity_Get(int iAdapterIndex, void *lpActivity);
		int Overdrive6_ThermalController_Caps(int iAdapterIndex, void *lpThermalControllerCaps);
		int Overdrive6_Temperature_Get(int iAdapterIndex, int *lpTemperature);
		int	Overdrive6_CurrentStatus_Get(int iAdapterIndex, void *lpCurrentStatus);
	};
}
#endif
