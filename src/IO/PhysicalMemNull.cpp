#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/PhysicalMem.h"

IO::PhysicalMem::PhysicalMem(IntOS addr, IntOS size)
{
	this->clsData = (void*)addr;
}

IO::PhysicalMem::~PhysicalMem()
{
}

UInt8 *IO::PhysicalMem::GetPointer()
{
	return (UInt8*)this->clsData;
}

Bool IO::PhysicalMem::IsError()
{
	return false;
}
