#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/PhysicalMem.h"
#include "Sync/Interlocked.h"

#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/mman.h>

typedef struct
{
	int fd;
	Int32 useCnt;
} PhysicalMemInfo;

struct IO::PhysicalMem::ClassData
{
	void *map;
	void *ptr;
	IntOS size;
};

static Optional<PhysicalMemInfo> PhysicalMem_status = nullptr;

IO::PhysicalMem::PhysicalMem(IntOS addr, IntOS size)
{
	NN<PhysicalMemInfo> status;
	if (!PhysicalMem_status.SetTo(status))
	{
		PhysicalMem_status = status = MemAllocNN(PhysicalMemInfo);
		status->fd = open("/dev/mem", O_RDWR|O_SYNC);
		status->useCnt = 1;
	}
	else
	{
		Sync::Interlocked::IncrementI32(status->useCnt);
	}

	this->clsData = nullptr;
	NN<ClassData> clsData;
	if (status->fd >= 0)
	{
		IntOS addrRet = addr & (size - 1);
		void *m = mmap(0, (size_t)size, PROT_READ|PROT_WRITE, MAP_SHARED, status->fd, addr - addrRet);
		if (m != MAP_FAILED)
		{
			clsData = MemAllocNN(ClassData);
			clsData->map = m;
			clsData->ptr = addrRet + (UInt8*)m;
			clsData->size = size;
			this->clsData = clsData;
		}
		else
		{
			printf("Error in mapping memory: %d\r\n", errno);
		}		
	}
	else
	{
		printf("Error in opening /dev/mem\r\n");
	}
}

IO::PhysicalMem::~PhysicalMem()
{
	NN<ClassData> clsData;
	if (this->clsData.SetTo(clsData))
	{
		munmap(clsData->map, (size_t)clsData->size);
		MemFreeNN(clsData);
		this->clsData = nullptr;
	}
	NN<PhysicalMemInfo> status;
	if (PhysicalMem_status.SetTo(status) && Sync::Interlocked::DecrementI32(status->useCnt) == 0)
	{
		close(status->fd);
		MemFreeNN(status);
		PhysicalMem_status = nullptr;
	}
}

UnsafeArrayOpt<UInt8> IO::PhysicalMem::GetPointer()
{
	NN<ClassData> clsData;
	if (!this->clsData.SetTo(clsData))
		return nullptr;
	return (UInt8*)clsData->ptr;
}

Bool IO::PhysicalMem::IsError()
{
	return this->clsData.IsNull();
}
