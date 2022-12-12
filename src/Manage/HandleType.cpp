#include "Stdafx.h"
#include "Manage/HandleType.h"

Text::CString Manage::HandleTypeGetName(HandleType handleType)
{
	switch (handleType)
	{
	case HandleType::File:
		return CSTR("File");
	case HandleType::Directory:
		return CSTR("Directory");
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
	case HandleType::Key:
		return CSTR("Key");
	case HandleType::WaitCompletionPacket:
		return CSTR("WaitCompletionPacket");
	case HandleType::IoCompletion:
		return CSTR("IoCompletion");
	case HandleType::Mutant:
		return CSTR("Mutant");
	case HandleType::TpWorkerFactory:
		return CSTR("TpWorkerFactory");
	case HandleType::Section:
		return CSTR("Section");
	case HandleType::IRTimer:
		return CSTR("IRTimer");
	case HandleType::ALPC_Port:
		return CSTR("ALPC Port");
	case HandleType::Semaphore:
		return CSTR("Semaphore");
	case HandleType::Thread:
		return CSTR("Thread");
	case HandleType::IoCompletionReserve:
		return CSTR("IoCompletionReserve");
	case HandleType::WindowStation:
		return CSTR("WindowStation");
	case HandleType::Desktop:
		return CSTR("Desktop");
	case HandleType::Token:
		return CSTR("Token");
	case HandleType::Process:
		return CSTR("Process");
	case HandleType::DxgkCompositionObject:
		return CSTR("DxgkCompositionObject");
	case HandleType::EtwRegistration:
		return CSTR("EtwRegistration");
	case HandleType::Unknown:
	default:
		return CSTR("Unknown");
	}
}