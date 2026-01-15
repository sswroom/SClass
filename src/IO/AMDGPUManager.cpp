#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/AMDGPUControl.h"
#include "IO/AMDGPUManager.h"
#include "adl_sdk.h"

#define AMDVENDORID             (1002)
#define ADL_WARNING_NO_DATA      -100

// Definitions of the used function pointers. Add more if you use other ADL APIs
typedef int ( *ADL_MAIN_CONTROL_CREATE )(ADL_MAIN_MALLOC_CALLBACK, int );
typedef int ( *ADL_MAIN_CONTROL_DESTROY )();
typedef int ( *ADL_ADAPTER_NUMBEROFADAPTERS_GET ) ( int* );
typedef int ( *ADL_ADAPTER_ADAPTERINFO_GET ) ( LPAdapterInfo, int );
typedef int ( *ADL_ADAPTER_ACTIVE_GET ) ( int, int* );
typedef int ( *ADL_OVERDRIVE_CAPS ) (int iAdapterIndex, int *iSupported, int *iEnabled, int *iVersion);

typedef int ( *ADL_OVERDRIVE5_THERMALDEVICES_ENUM ) (int iAdapterIndex, int iThermalControllerIndex, ADLThermalControllerInfo *lpThermalControllerInfo);
typedef int ( *ADL_OVERDRIVE5_ODPARAMETERS_GET ) ( int  iAdapterIndex,  ADLODParameters *  lpOdParameters );
typedef int ( *ADL_OVERDRIVE5_TEMPERATURE_GET ) (int iAdapterIndex, int iThermalControllerIndex, ADLTemperature *lpTemperature);
typedef int ( *ADL_OVERDRIVE5_FANSPEED_GET ) (int iAdapterIndex, int iThermalControllerIndex, ADLFanSpeedValue *lpFanSpeedValue);
typedef int ( *ADL_OVERDRIVE5_FANSPEEDINFO_GET ) (int iAdapterIndex, int iThermalControllerIndex, ADLFanSpeedInfo *lpFanSpeedInfo);
typedef int ( *ADL_OVERDRIVE5_ODPERFORMANCELEVELS_GET ) (int iAdapterIndex, int iDefault, ADLODPerformanceLevels *lpOdPerformanceLevels); 
typedef int ( *ADL_OVERDRIVE5_ODPARAMETERS_GET ) (int iAdapterIndex, ADLODParameters *lpOdParameters);
typedef int ( *ADL_OVERDRIVE5_CURRENTACTIVITY_GET ) (int iAdapterIndex, ADLPMActivity *lpActivity);
typedef int	( *ADL_OVERDRIVE5_FANSPEED_SET )(int iAdapterIndex, int iThermalControllerIndex, ADLFanSpeedValue *lpFanSpeedValue);
typedef int ( *ADL_OVERDRIVE5_ODPERFORMANCELEVELS_SET ) (int iAdapterIndex, ADLODPerformanceLevels *lpOdPerformanceLevels);
typedef int ( *ADL_OVERDRIVE5_POWERCONTROL_CAPS )(int iAdapterIndex,  int *lpSupported);
typedef int ( *ADL_OVERDRIVE5_POWERCONTROLINFO_GET )(int iAdapterIndex, ADLPowerControlInfo *lpPowerControlInfo);
typedef int ( *ADL_OVERDRIVE5_POWERCONTROL_GET )(int iAdapterIndex, int *lpCurrentValue, int *lpDefaultValue);
typedef int ( *ADL_OVERDRIVE5_POWERCONTROL_SET )(int iAdapterIndex, int iValue);
typedef int ( *ADL_OVERDRIVE6_FANSPEED_GET )(int iAdapterIndex, ADLOD6FanSpeedInfo *lpFanSpeedInfo);
typedef int ( *ADL_OVERDRIVE6_THERMALCONTROLLER_CAPS )(int iAdapterIndex, ADLOD6ThermalControllerCaps *lpThermalControllerCaps);
typedef int ( *ADL_OVERDRIVE6_TEMPERATURE_GET )(int iAdapterIndex, int *lpTemperature);
typedef int ( *ADL_OVERDRIVE6_CAPABILITIES_GET ) (int iAdapterIndex, ADLOD6Capabilities *lpODCapabilities);
typedef int ( *ADL_OVERDRIVE6_STATEINFO_GET )(int iAdapterIndex, int iStateType, ADLOD6StateInfo *lpStateInfo);
typedef int	( *ADL_OVERDRIVE6_CURRENTSTATUS_GET )(int iAdapterIndex, ADLOD6CurrentStatus *lpCurrentStatus);
typedef int ( *ADL_OVERDRIVE6_POWERCONTROL_CAPS ) (int iAdapterIndex, int *lpSupported);
typedef int ( *ADL_OVERDRIVE6_POWERCONTROLINFO_GET )(int iAdapterIndex, ADLOD6PowerControlInfo *lpPowerControlInfo);
typedef int ( *ADL_OVERDRIVE6_POWERCONTROL_GET )(int iAdapterIndex, int *lpCurrentValue, int *lpDefaultValue);
typedef int ( *ADL_OVERDRIVE6_FANSPEED_SET )(int iAdapterIndex, ADLOD6FanSpeedValue *lpFanSpeedValue);
typedef int ( *ADL_OVERDRIVE6_STATE_SET )(int iAdapterIndex, int iStateType, ADLOD6StateInfo *lpStateInfo);
typedef int ( *ADL_OVERDRIVE6_POWERCONTROL_SET )(int iAdapterIndex, int iValue);

typedef struct
{
	ADL_MAIN_CONTROL_CREATE					ADL_Main_Control_Create;
	ADL_MAIN_CONTROL_DESTROY				ADL_Main_Control_Destroy;
	ADL_ADAPTER_NUMBEROFADAPTERS_GET		ADL_Adapter_NumberOfAdapters_Get;
	ADL_ADAPTER_ADAPTERINFO_GET				ADL_Adapter_AdapterInfo_Get;
	ADL_ADAPTER_ACTIVE_GET					ADL_Adapter_Active_Get;
	ADL_OVERDRIVE_CAPS						ADL_Overdrive_Caps;

	ADL_OVERDRIVE5_THERMALDEVICES_ENUM		ADL_Overdrive5_ThermalDevices_Enum;
	ADL_OVERDRIVE5_ODPARAMETERS_GET			ADL_Overdrive5_ODParameters_Get;
	ADL_OVERDRIVE5_TEMPERATURE_GET			ADL_Overdrive5_Temperature_Get;
	ADL_OVERDRIVE5_FANSPEED_GET				ADL_Overdrive5_FanSpeed_Get;
	ADL_OVERDRIVE5_FANSPEEDINFO_GET			ADL_Overdrive5_FanSpeedInfo_Get;
	ADL_OVERDRIVE5_ODPERFORMANCELEVELS_GET	ADL_Overdrive5_ODPerformanceLevels_Get;
	ADL_OVERDRIVE5_CURRENTACTIVITY_GET		ADL_Overdrive5_CurrentActivity_Get;
	ADL_OVERDRIVE5_FANSPEED_SET				ADL_Overdrive5_FanSpeed_Set;
	ADL_OVERDRIVE5_ODPERFORMANCELEVELS_SET  ADL_Overdrive5_ODPerformanceLevels_Set;
	ADL_OVERDRIVE5_POWERCONTROL_CAPS		ADL_Overdrive5_PowerControl_Caps;
	ADL_OVERDRIVE5_POWERCONTROLINFO_GET		ADL_Overdrive5_PowerControlInfo_Get;
	ADL_OVERDRIVE5_POWERCONTROL_GET			ADL_Overdrive5_PowerControl_Get;
	ADL_OVERDRIVE5_POWERCONTROL_SET			ADL_Overdrive5_PowerControl_Set;


	ADL_OVERDRIVE6_FANSPEED_GET				ADL_Overdrive6_FanSpeed_Get;
	ADL_OVERDRIVE6_THERMALCONTROLLER_CAPS	ADL_Overdrive6_ThermalController_Caps;
	ADL_OVERDRIVE6_TEMPERATURE_GET			ADL_Overdrive6_Temperature_Get;
	ADL_OVERDRIVE6_CAPABILITIES_GET			ADL_Overdrive6_Capabilities_Get;
	ADL_OVERDRIVE6_STATEINFO_GET			ADL_Overdrive6_StateInfo_Get;
	ADL_OVERDRIVE6_CURRENTSTATUS_GET		ADL_Overdrive6_CurrentStatus_Get;
	ADL_OVERDRIVE6_POWERCONTROL_CAPS		ADL_Overdrive6_PowerControl_Caps;
	ADL_OVERDRIVE6_POWERCONTROLINFO_GET		ADL_Overdrive6_PowerControlInfo_Get;
	ADL_OVERDRIVE6_POWERCONTROL_GET			ADL_Overdrive6_PowerControl_Get;
	ADL_OVERDRIVE6_FANSPEED_SET				ADL_Overdrive6_FanSpeed_Set;
	ADL_OVERDRIVE6_STATE_SET				ADL_Overdrive6_State_Set;
	ADL_OVERDRIVE6_POWERCONTROL_SET			ADL_Overdrive6_PowerControl_Set;
} ADLFuncs;

void* __stdcall ADL_Main_Memory_Alloc(int iSize)
{
	UInt8* lpBuffer = MemAlloc(UInt8, (UIntOS)(UInt32)iSize);
	return lpBuffer;
}

void __stdcall ADL_Main_Memory_Free(void** lpBuffer)
{
	if (*lpBuffer != 0)
	{
		MemFree(*lpBuffer);
		*lpBuffer = 0;
	}
}

IO::AMDGPUManager::AMDGPUManager()
{
	this->adapterInfos = 0;
	this->adapterList = 0;
#if defined (LINUX)
	NEW_CLASS(this->lib, IO::Library((const UTF8Char*)"libatiadlxx.so"));
#else
	NEW_CLASS(this->lib, IO::Library((const UTF8Char*)"atiadlxx.dll"));
	if (this->lib->IsError())
	{
		DEL_CLASS(this->lib);
		NEW_CLASS(this->lib, IO::Library((const UTF8Char*)"atiadlxy.dll"));
	}
#endif
	if (this->lib->IsError())
	{
		DEL_CLASS(this->lib);
		this->lib = 0;
		this->funcs = 0;
	}
	else
	{
		ADLFuncs *adlFuncs = MemAlloc(ADLFuncs, 1);
		MemClear(adlFuncs, sizeof(ADLFuncs));

		adlFuncs->ADL_Main_Control_Create = (ADL_MAIN_CONTROL_CREATE) this->lib->GetFunc("ADL_Main_Control_Create");
		adlFuncs->ADL_Main_Control_Destroy = (ADL_MAIN_CONTROL_DESTROY) this->lib->GetFunc("ADL_Main_Control_Destroy");
		adlFuncs->ADL_Adapter_NumberOfAdapters_Get = (ADL_ADAPTER_NUMBEROFADAPTERS_GET) this->lib->GetFunc("ADL_Adapter_NumberOfAdapters_Get");
		adlFuncs->ADL_Adapter_AdapterInfo_Get = (ADL_ADAPTER_ADAPTERINFO_GET) this->lib->GetFunc("ADL_Adapter_AdapterInfo_Get");
		adlFuncs->ADL_Adapter_Active_Get = (ADL_ADAPTER_ACTIVE_GET) this->lib->GetFunc("ADL_Adapter_Active_Get");
		adlFuncs->ADL_Overdrive_Caps = (ADL_OVERDRIVE_CAPS) this->lib->GetFunc("ADL_Overdrive_Caps");
		
		adlFuncs->ADL_Overdrive5_ThermalDevices_Enum = (ADL_OVERDRIVE5_THERMALDEVICES_ENUM) this->lib->GetFunc("ADL_Overdrive5_ThermalDevices_Enum");
		adlFuncs->ADL_Overdrive5_ODParameters_Get = (ADL_OVERDRIVE5_ODPARAMETERS_GET) this->lib->GetFunc("ADL_Overdrive5_ODParameters_Get");
		adlFuncs->ADL_Overdrive5_Temperature_Get = (ADL_OVERDRIVE5_TEMPERATURE_GET) this->lib->GetFunc("ADL_Overdrive5_Temperature_Get");
		adlFuncs->ADL_Overdrive5_FanSpeed_Get = (ADL_OVERDRIVE5_FANSPEED_GET) this->lib->GetFunc("ADL_Overdrive5_FanSpeed_Get");
		adlFuncs->ADL_Overdrive5_FanSpeedInfo_Get = (ADL_OVERDRIVE5_FANSPEEDINFO_GET ) this->lib->GetFunc("ADL_Overdrive5_FanSpeedInfo_Get");
		adlFuncs->ADL_Overdrive5_ODPerformanceLevels_Get = (ADL_OVERDRIVE5_ODPERFORMANCELEVELS_GET) this->lib->GetFunc("ADL_Overdrive5_ODPerformanceLevels_Get");		
		adlFuncs->ADL_Overdrive5_CurrentActivity_Get = (ADL_OVERDRIVE5_CURRENTACTIVITY_GET) this->lib->GetFunc("ADL_Overdrive5_CurrentActivity_Get");
		adlFuncs->ADL_Overdrive5_FanSpeed_Set = (ADL_OVERDRIVE5_FANSPEED_SET)this->lib->GetFunc("ADL_Overdrive5_FanSpeed_Set");
		adlFuncs->ADL_Overdrive5_ODPerformanceLevels_Set = (ADL_OVERDRIVE5_ODPERFORMANCELEVELS_SET ) this->lib->GetFunc("ADL_Overdrive5_ODPerformanceLevels_Set");
		adlFuncs->ADL_Overdrive5_PowerControl_Caps = (ADL_OVERDRIVE5_POWERCONTROL_CAPS) this->lib->GetFunc("ADL_Overdrive5_PowerControl_Caps");
		adlFuncs->ADL_Overdrive5_PowerControlInfo_Get = (ADL_OVERDRIVE5_POWERCONTROLINFO_GET) this->lib->GetFunc("ADL_Overdrive5_PowerControlInfo_Get");
		adlFuncs->ADL_Overdrive5_PowerControl_Get = (ADL_OVERDRIVE5_POWERCONTROL_GET ) this->lib->GetFunc("ADL_Overdrive5_PowerControl_Get");
		adlFuncs->ADL_Overdrive5_PowerControl_Set = (ADL_OVERDRIVE5_POWERCONTROL_SET) this->lib->GetFunc("ADL_Overdrive5_PowerControl_Set");

		adlFuncs->ADL_Overdrive6_FanSpeed_Get = (ADL_OVERDRIVE6_FANSPEED_GET) this->lib->GetFunc("ADL_Overdrive6_FanSpeed_Get");
		adlFuncs->ADL_Overdrive6_ThermalController_Caps = (ADL_OVERDRIVE6_THERMALCONTROLLER_CAPS) this->lib->GetFunc("ADL_Overdrive6_ThermalController_Caps");
		adlFuncs->ADL_Overdrive6_Temperature_Get = (ADL_OVERDRIVE6_TEMPERATURE_GET) this->lib->GetFunc("ADL_Overdrive6_Temperature_Get");
		adlFuncs->ADL_Overdrive6_Capabilities_Get = (ADL_OVERDRIVE6_CAPABILITIES_GET) this->lib->GetFunc("ADL_Overdrive6_Capabilities_Get");
		adlFuncs->ADL_Overdrive6_StateInfo_Get = (ADL_OVERDRIVE6_STATEINFO_GET) this->lib->GetFunc("ADL_Overdrive6_StateInfo_Get");
		adlFuncs->ADL_Overdrive6_CurrentStatus_Get = (ADL_OVERDRIVE6_CURRENTSTATUS_GET) this->lib->GetFunc("ADL_Overdrive6_CurrentStatus_Get");
		adlFuncs->ADL_Overdrive6_PowerControl_Caps = (ADL_OVERDRIVE6_POWERCONTROL_CAPS) this->lib->GetFunc("ADL_Overdrive6_PowerControl_Caps");
		adlFuncs->ADL_Overdrive6_PowerControlInfo_Get = (ADL_OVERDRIVE6_POWERCONTROLINFO_GET) this->lib->GetFunc("ADL_Overdrive6_PowerControlInfo_Get");
		adlFuncs->ADL_Overdrive6_PowerControl_Get = (ADL_OVERDRIVE6_POWERCONTROL_GET) this->lib->GetFunc("ADL_Overdrive6_PowerControl_Get");
		adlFuncs->ADL_Overdrive6_FanSpeed_Set  = (ADL_OVERDRIVE6_FANSPEED_SET) this->lib->GetFunc("ADL_Overdrive6_FanSpeed_Set");
		adlFuncs->ADL_Overdrive6_State_Set = (ADL_OVERDRIVE6_STATE_SET) this->lib->GetFunc("ADL_Overdrive6_State_Set");
		adlFuncs->ADL_Overdrive6_PowerControl_Set = (ADL_OVERDRIVE6_POWERCONTROL_SET) this->lib->GetFunc("ADL_Overdrive6_PowerControl_Set");

		if (adlFuncs->ADL_Main_Control_Create && adlFuncs->ADL_Main_Control_Create)
		{
			if (adlFuncs->ADL_Main_Control_Create(ADL_Main_Memory_Alloc, 1) == ADL_OK)
			{
				this->funcs = adlFuncs;

				int cnt;
				int i;
				if (adlFuncs->ADL_Adapter_NumberOfAdapters_Get && adlFuncs->ADL_Adapter_NumberOfAdapters_Get(&cnt) == ADL_OK)
				{
				}
				else
				{
					cnt = 0;
				}
				this->adapterInfos = MemAlloc(UInt8, sizeof(AdapterInfo) * (UIntOS)(UInt32)cnt);
				NEW_CLASS(this->adapterList, Data::ArrayList<UInt8*>());
				AdapterInfo *adapter;
				if (cnt > 0)
				{
					MemClear(this->adapterInfos, sizeof(AdapterInfo) * (UIntOS)(UInt32)cnt);
					if (adlFuncs->ADL_Adapter_AdapterInfo_Get)
						adlFuncs->ADL_Adapter_AdapterInfo_Get((LPAdapterInfo)this->adapterInfos, (int)sizeof(AdapterInfo) * cnt);
				}
				i = 0;
				while (i < cnt)
				{
					adapter = (AdapterInfo*)&this->adapterInfos[sizeof(AdapterInfo) * (UInt32)i];
					if (adapter->iVendorID == AMDVENDORID)
					{
						int active = 0;
						if (adlFuncs->ADL_Adapter_Active_Get && adlFuncs->ADL_Adapter_Active_Get(adapter->iAdapterIndex, &active) == ADL_OK)
						{
							if (active)
							{
								this->adapterList->Add((UInt8*)adapter);
							}
						}
					}
					i++;
				}
			}
			else
			{
				MemFree(adlFuncs);
				this->funcs = 0;
				DEL_CLASS(this->lib);
				this->lib = 0;
			}
		}
		else
		{
			MemFree(adlFuncs);
			this->funcs = 0;
			DEL_CLASS(this->lib);
			this->lib = 0;
		}
	}
}

IO::AMDGPUManager::~AMDGPUManager()
{
	if (this->funcs)
	{
		ADLFuncs *adlFuncs = (ADLFuncs*)this->funcs;
		adlFuncs->ADL_Main_Control_Destroy();
		MemFree(adlFuncs);
	}
	if (this->adapterInfos)
	{
		MemFree(this->adapterInfos);
	}
	if (this->adapterList)
	{
		DEL_CLASS(this->adapterList);
	}
	SDEL_CLASS(this->lib);
}

UIntOS IO::AMDGPUManager::GetGPUCount()
{
	if (this->adapterList == 0)
		return 0;
	return this->adapterList->GetCount();
}

Optional<IO::GPUControl> IO::AMDGPUManager::CreateGPUControl(UIntOS index)
{
	IO::AMDGPUControl *gpuCtrl;
	if (this->adapterList == 0)
		return 0;
	AdapterInfo *adapter = (AdapterInfo*)this->adapterList->GetItem(index);
	if (adapter == 0)
		return 0;
	NEW_CLASS(gpuCtrl, IO::AMDGPUControl(*this, adapter));
	return gpuCtrl;
}

int IO::AMDGPUManager::Overdrive_Caps(int iAdapterIndex, int *iSupported, int *iEnabled, int *iVersion)
{
	if (this->funcs == 0)
		return ADL_ERR;

	ADLFuncs *adlFuncs = (ADLFuncs*)this->funcs;
	if (adlFuncs->ADL_Overdrive_Caps == 0)
		return ADL_ERR;
	return adlFuncs->ADL_Overdrive_Caps(iAdapterIndex, iSupported, iEnabled, iVersion);
}

int IO::AMDGPUManager::Overdrive5_ThermalDevices_Enum(int iAdapterIndex, int iThermalControllerIndex, void *lpThermalControllerInfo)
{
	if (this->funcs == 0)
		return ADL_ERR;

	ADLFuncs *adlFuncs = (ADLFuncs*)this->funcs;
	if (adlFuncs->ADL_Overdrive5_ThermalDevices_Enum == 0)
		return ADL_ERR;
	return adlFuncs->ADL_Overdrive5_ThermalDevices_Enum(iAdapterIndex, iThermalControllerIndex, (ADLThermalControllerInfo*)lpThermalControllerInfo);
}

int IO::AMDGPUManager::Overdrive5_Temperature_Get(int iAdapterIndex, int iThermalControllerIndex, void *lpTemperature)
{
	if (this->funcs == 0)
		return ADL_ERR;

	ADLFuncs *adlFuncs = (ADLFuncs*)this->funcs;
	if (adlFuncs->ADL_Overdrive5_Temperature_Get == 0)
		return ADL_ERR;
	return adlFuncs->ADL_Overdrive5_Temperature_Get(iAdapterIndex, iThermalControllerIndex, (ADLTemperature*)lpTemperature);
}

int IO::AMDGPUManager::Overdrive5_CurrentActivity_Get(int iAdapterIndex, void *lpActivity)
{
	if (this->funcs == 0)
		return ADL_ERR;

	ADLFuncs *adlFuncs = (ADLFuncs*)this->funcs;
	if (adlFuncs->ADL_Overdrive5_CurrentActivity_Get == 0)
		return ADL_ERR;
	return adlFuncs->ADL_Overdrive5_CurrentActivity_Get(iAdapterIndex, (ADLPMActivity*)lpActivity);
}

int IO::AMDGPUManager::Overdrive6_ThermalController_Caps(int iAdapterIndex, void *lpThermalControllerCaps)
{
	if (this->funcs == 0)
		return ADL_ERR;

	ADLFuncs *adlFuncs = (ADLFuncs*)this->funcs;
	if (adlFuncs->ADL_Overdrive6_ThermalController_Caps == 0)
		return ADL_ERR;
	return adlFuncs->ADL_Overdrive6_ThermalController_Caps(iAdapterIndex, (ADLOD6ThermalControllerCaps*)lpThermalControllerCaps);
}

int IO::AMDGPUManager::Overdrive6_Temperature_Get(int iAdapterIndex, int *lpTemperature)
{
	if (this->funcs == 0)
		return ADL_ERR;

	ADLFuncs *adlFuncs = (ADLFuncs*)this->funcs;
	if (adlFuncs->ADL_Overdrive6_Temperature_Get == 0)
		return ADL_ERR;
	return adlFuncs->ADL_Overdrive6_Temperature_Get(iAdapterIndex, lpTemperature);
}

int	IO::AMDGPUManager::Overdrive6_CurrentStatus_Get(int iAdapterIndex, void *lpCurrentStatus)
{
	if (this->funcs == 0)
		return ADL_ERR;

	ADLFuncs *adlFuncs = (ADLFuncs*)this->funcs;
	if (adlFuncs->ADL_Overdrive6_CurrentStatus_Get == 0)
		return ADL_ERR;
	return adlFuncs->ADL_Overdrive6_CurrentStatus_Get(iAdapterIndex, (ADLOD6CurrentStatus*)lpCurrentStatus);
}
