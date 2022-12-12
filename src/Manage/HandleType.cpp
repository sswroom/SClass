#include "Stdafx.h"
#include "Manage/HandleType.h"

Text::CString Manage::HandleTypeGetName(HandleType handleType)
{
	switch (handleType)
	{
	case HandleType::File:
		return CSTR("File");
	case HandleType::Device:
		return CSTR("Device");
	case HandleType::EPoll:
		return CSTR("EPoll");
	case HandleType::Event:
		return CSTR("Event");
	case HandleType::INotify:
		return CSTR("INotify");
	case HandleType::Signal:
		return CSTR("Signal");
	case HandleType::Timer:
		return CSTR("Timer");
	case HandleType::Socket:
		return CSTR("Socket");
	case HandleType::Pipe:
		return CSTR("Pipe");
	case HandleType::Memory:
		return CSTR("Memory");
	default:
		return CSTR("Unknown");
	}
}