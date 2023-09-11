#include "Stdafx.h"
#include "MyMemory.h"
#include "Manage/ThreadContext.h"

Text::CStringNN Manage::ThreadContext::ContextTypeGetName(ContextType contextType)
{
	switch (contextType)
	{
	case ContextType::X86_32:
		return CSTR("x86");
	case ContextType::X86_64:
		return CSTR("AMD64");
	case ContextType::ARM:
		return CSTR("ARM");
	case ContextType::MIPS:
		return CSTR("MIPS");
	case ContextType::ARM64:
		return CSTR("ARM64");
	case ContextType::MIPS64:
		return CSTR("MIPS64");
	case ContextType::AVR:
		return CSTR("AVR");
	case ContextType::ARM64EC:
		return CSTR("ARM64EC");
	case ContextType::Unknown:
	default:
		return CSTR("Unknown");
	}
}
