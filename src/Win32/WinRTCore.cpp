#include "Stdafx.h"
#include "Win32/WinRTCore.h"
#include <winrt/Windows.Devices.h>

Bool Win32::WinRTCore::inited = false;

void Win32::WinRTCore::Init()
{
	if (!inited)
	{
		inited = true;
		winrt::init_apartment();
	}
}