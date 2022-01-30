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
	OSInt size;
};

static PhysicalMemInfo *PhysicalMem_status = 0;

IO::PhysicalMem::PhysicalMem(OSInt addr, OSInt size)
{
	if (PhysicalMem_status == 0)
	{
		PhysicalMem_status = MemAlloc(PhysicalMemInfo, 1);
		PhysicalMem_status->fd = open("/dev/mem", O_RDWR|O_SYNC);
		PhysicalMem_status->useCnt = 1;
	}
	else
	{
		Sync::Interlocked::Increment(&PhysicalMem_status->useCnt);
	}

	ClassData *clsData = 0;
	if (PhysicalMem_status->fd >= 0)
	{
		OSInt addrRet = addr & (size - 1);
		void *m = mmap(0, size, PROT_READ|PROT_WRITE, MAP_SHARED, PhysicalMem_status->fd, addr - addrRet);
		if (m != MAP_FAILED)
		{
			clsData = MemAlloc(ClassData, 1);
			clsData->map = m;
			clsData->ptr = addrRet + (UInt8*)m;
			clsData->size = size;
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
	
	this->clsData = clsData;
}

IO::PhysicalMem::~PhysicalMem()
{
	if (this->clsData)
	{
		munmap(this->clsData->map, this->clsData->size);
		MemFree(this->clsData);
		this->clsData = 0;
	}
	if (Sync::Interlocked::Decrement(&PhysicalMem_status->useCnt) == 0)
	{
		close(PhysicalMem_status->fd);
		MemFree(PhysicalMem_status);
		PhysicalMem_status = 0;
	}
}

UInt8 *IO::PhysicalMem::GetPointer()
{
	if (this->clsData == 0)
		return 0;
	return (UInt8*)this->clsData->ptr;
}

Bool IO::PhysicalMem::IsError()
{
	return this->clsData == 0;
}
