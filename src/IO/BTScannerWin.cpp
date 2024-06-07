#include "Stdafx.h"
#include <winrt/Windows.Devices.Bluetooth.Advertisement.h>
#include <windows.h>

#include "IO/BTScanner.h"
#include "Win32/WindowsBTScanner.h"

Optional<IO::BTScanner> IO::BTScanner::CreateScanner()
{
	Win32::WindowsBTScanner *bt;
	NEW_CLASS(bt, Win32::WindowsBTScanner());
	return bt;
}