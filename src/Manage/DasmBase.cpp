#include "Stdafx.h"
#include "MyMemory.h"
#include "Manage/DasmBase.h"

Manage::Dasm::~Dasm()
{
}

Manage::Dasm::RegBitDepth Manage::Dasm16::GetRegBitDepth()
{
	return Manage::Dasm::RBD_16;
}

Manage::Dasm::RegBitDepth Manage::Dasm32::GetRegBitDepth()
{
	return Manage::Dasm::RBD_32;
}

Manage::Dasm::RegBitDepth Manage::Dasm64::GetRegBitDepth()
{
	return Manage::Dasm::RBD_64;
}

