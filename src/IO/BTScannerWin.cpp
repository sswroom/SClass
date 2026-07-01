#include "Stdafx.h"
#include <winrt/Windows.Devices.Bluetooth.Advertisement.h>
#include <windows.h>

#include "IO/BTScanner.h"
#include "Win32/WindowsBTScanner.h"

Optional<IO::BTScanner> IO::BTScanner::CreateScanner()
{
	NN<Win32::WindowsBTScanner> bt;
	NEW_CLASSNN(bt, Win32::WindowsBTScanner());
	return bt;
}