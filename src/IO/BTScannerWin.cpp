#include "Stdafx.h"
#include "IO/BTScanner.h"
#include "Win32/WindowsBTScanner.h"

IO::BTScanner *IO::BTScanner::CreateScanner()
{
	Win32::WindowsBTScanner *bt;
	NEW_CLASS(bt, Win32::WindowsBTScanner());
	return bt;
}