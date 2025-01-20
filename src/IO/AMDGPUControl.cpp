#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/AMDGPUControl.h"
#include "IO/AMDGPUManager.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "adl_sdk.h"

#define AMDVENDORID             (1002)
#define ADL_WARNING_NO_DATA      -100

IO::AMDGPUControl::AMDGPUControl(NN<IO::AMDGPUManager> gpuMgr, AnyType adapter)
{
	NN<AdapterInfo> adapt = adapter.GetNN<AdapterInfo>();
	this->gpuMgr = gpuMgr;
	this->adapter = adapter;
	this->odrivever = 0;
	Text::StringBuilderUTF8 sb;
	sb.AppendSlow((const UTF8Char*)adapt->strAdapterName);
	this->name = Text::String::New(sb.ToCString());

	int iOverdriveSupported;
	int iOverdriveEnabled;
	int iOverdriveVersion;
	if (gpuMgr->Overdrive_Caps(adapt->iAdapterIndex, &iOverdriveSupported, &iOverdriveEnabled, &iOverdriveVersion) == ADL_OK)
	{
		if (iOverdriveSupported)
		{
			this->odrivever = iOverdriveVersion;
		}
	}
}

IO::AMDGPUControl::~AMDGPUControl()
{
	this->name->Release();
}

NN<Text::String> IO::AMDGPUControl::GetName()
{
	return this->name;
}

Bool IO::AMDGPUControl::GetTemperature(OutParam<Double> temp)
{
	NN<AdapterInfo> adapter = this->adapter.GetNN<AdapterInfo>();
	if (this->odrivever == 5)
	{
		ADLThermalControllerInfo termalControllerInfo;
		termalControllerInfo.iSize = sizeof(ADLThermalControllerInfo);
		int i = 0;
		int err;
		while (i < 10)
		{
			err = this->gpuMgr->Overdrive5_ThermalDevices_Enum(adapter->iAdapterIndex, i, &termalControllerInfo);
			if (err == ADL_WARNING_NO_DATA)
			{
				break;
			}
			
			if (termalControllerInfo.iThermalDomain == ADL_DL_THERMAL_DOMAIN_GPU)
			{
				ADLTemperature adlTemperature;
				adlTemperature.iSize = sizeof (ADLTemperature);
				if (this->gpuMgr->Overdrive5_Temperature_Get(adapter->iAdapterIndex, i, &adlTemperature) != ADL_OK)
				{
					return false;
				}

				temp.Set(adlTemperature.iTemperature * 0.001);
				return true;
			}
			i++;
		}
		return false;
	}
	else if (this->odrivever == 6)
	{
		ADLOD6ThermalControllerCaps thermalControllerCaps;
		if (this->gpuMgr->Overdrive6_ThermalController_Caps(adapter->iAdapterIndex, &thermalControllerCaps) != ADL_OK)
			return false;
		if ((thermalControllerCaps.iCapabilities & ADL_OD6_TCCAPS_THERMAL_CONTROLLER) == 0)
			return false;

		int iTemp;
		if (this->gpuMgr->Overdrive6_Temperature_Get(adapter->iAdapterIndex, &iTemp) != ADL_OK)
			return false;
		temp.Set(iTemp * 0.001);
		return true;
	}
	return false;
}

Bool IO::AMDGPUControl::GetCoreClock(OutParam<Double> mhz)
{
	NN<AdapterInfo> adapter = this->adapter.GetNN<AdapterInfo>();
	if (this->odrivever == 5)
	{
		ADLPMActivity activity;
		activity.iSize = sizeof(ADLPMActivity);
		if (this->gpuMgr->Overdrive5_CurrentActivity_Get(adapter->iAdapterIndex, &activity) == ADL_OK)
		{
			mhz.Set(activity.iEngineClock * 0.01);
			return true;
		}
	}
	else if (this->odrivever == 6)
	{
		ADLOD6CurrentStatus currentStatus;
		if (this->gpuMgr->Overdrive6_CurrentStatus_Get(adapter->iAdapterIndex, &currentStatus) == ADL_OK)
		{
			mhz.Set(currentStatus.iEngineClock * 0.01);
			return true;
		}
	}
	return false;
}

Bool IO::AMDGPUControl::GetMemoryClock(OutParam<Double> mhz)
{
	NN<AdapterInfo> adapter = this->adapter.GetNN<AdapterInfo>();
	if (this->odrivever == 5)
	{
		ADLPMActivity activity;
		activity.iSize = sizeof(ADLPMActivity);
		if (this->gpuMgr->Overdrive5_CurrentActivity_Get(adapter->iAdapterIndex, &activity) == ADL_OK)
		{
			mhz.Set(activity.iMemoryClock * 0.01);
			return true;
		}
	}
	else if (this->odrivever == 6)
	{
		ADLOD6CurrentStatus currentStatus;
		if (this->gpuMgr->Overdrive6_CurrentStatus_Get(adapter->iAdapterIndex, &currentStatus) == ADL_OK)
		{
			mhz.Set(currentStatus.iMemoryClock * 0.01);
			return true;
		}
	}
	return false;
}

Bool IO::AMDGPUControl::GetVoltage(OutParam<Double> volt)
{
	NN<AdapterInfo> adapter = this->adapter.GetNN<AdapterInfo>();
	if (this->odrivever == 5)
	{
		ADLPMActivity activity;
		activity.iSize = sizeof(ADLPMActivity);
		if (this->gpuMgr->Overdrive5_CurrentActivity_Get(adapter->iAdapterIndex, &activity) == ADL_OK)
		{
			volt.Set(activity.iVddc * 0.001);
			return true;
		}
	}
	else if (this->odrivever == 6)
	{
	}
	return false;
}
