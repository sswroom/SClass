#ifndef _SM_MYMEMORY
#define _SM_MYMEMORY
#include "MemTool.h"

/*
void NEW_CLASS(variable, className);
void DEL_CLASS(variable);

T *MemAlloc(T, cnt);
T *MemAllocA(T, cnt);
T *MemAllocA64(T, cnt);
void MemFree(ptr);
void MemFreeA(ptr);
void MemFreeA64(ptr);

void MemInit();
void MemDeinit();
void MemPtrChk(void *ptr);
void MemSetBreakPoint(OSInt address);
void MemSetBreakPoint(OSInt address, UOSInt size);
void MemSetLogFile(const UTF8Char *logFile);
void MemLock();
void MemUnlock();
void MemClear(void *buff, OSInt size);
Int32 MemCheckError();
Int32 MemCountBlks();
void MemIncCounter(void *ptr);
void MemDecCounter(void *ptr);
*/

#define NEW_CLASS_D(className) MemNewClass(new className)
#if  0
#define NEW_CLASS(variable, className) {variable = new className;MemPtrChk(variable);MemIncCounter(variable);}
#define NEW_CLASSNN(variable, className) {variable.SetPtr(new className);MemPtrChk(variable.Ptr());MemIncCounter(variable);}
#define DEL_CLASS(variable) {delete variable;MemDecCounter(variable);}
#else
#define NEW_CLASS(variable, className) {variable = new className;MemPtrChk(variable);}
#define NEW_CLASSNN(variable, className) {variable.SetPtr(new className);MemPtrChk(variable.Ptr());}
#define DEL_CLASS(variable) {delete variable;}
#endif
#define SDEL_CLASS(variable) if (variable) {DEL_CLASS(variable); variable = 0;}

#define MemAlloc(T, cnt) (T*)MAlloc(sizeof(T) * (cnt))
#define MemAllocNN(T, cnt) NotNullPtr<T>::FromPtr((T*)MAlloc(sizeof(T) * (cnt)))
#define MemAllocA(T, cnt) (T*)MAllocA64(sizeof(T) * (cnt))
#define MemAllocANN(T, cnt) NotNullPtr<T>::FromPtr((T*)MAllocA64(sizeof(T) * (cnt)))
#define MemAllocA64(T, cnt) (T*)MAllocA64(sizeof(T) * (cnt))
#define MemFreeA64(ptr) MemFreeA(ptr)
#define MemFreeNN(ptr) MemFree(ptr.Ptr())

void MemPtrChk(void *ptr);
void MemInit();
void MemSetBreakPoint(OSInt address);
void MemSetBreakPoint(OSInt address, UOSInt size);
void MemSetLogFile(const UTF8Char *logFile, UOSInt nameLen);
void MemLock();
void MemUnlock();
void *MAlloc(UOSInt size);
void *MAllocA(UOSInt size);
void *MAllocA64(UOSInt size);
void MemFree(void *ptr);
void MemFreeA(void *ptr);
void MemDeinit();
Int32 MemCheckError();
Int32 MemCountBlks();
void MemIncCounter(void *ptr);
void MemDecCounter(void *ptr);

template <class T> T MemNewClass(T cls)
{
	MemPtrChk(cls);
	//MemIncCounter(cls);
	return cls;
}

#if 0 && defined(__cplusplus) && defined(THREADSAFE)
#include <stdlib.h>
FORCEINLINE void *operator new(size_t size)
{
	MemLock();
	void *ret = MAllocA(UInt8, (UOSInt)size);
	MemUnlock();
	return ret;
}

FORCEINLINE void operator delete(void *p)
{
	MemLock();
	MemFreeA(p);
	MemUnlock();
}
#endif

#define SMEMFREE(ptr) {if (ptr) {MemFree(ptr); ptr = 0;} }

#endif
