#include "Stdafx.h"
#include <malloc.h>
#include <stdlib.h>
#include <unwind.h>

extern "C"
{
void *operator new(size_t size)
{
	return malloc(size);
}

void operator delete(void *ptr)
{
	free(ptr);
}

void __cxa_pure_virtual (void)
{
	exit(-1);
}


/*void operator delete(void *ptr, unsigned int sz)
{
	free(ptr);
}*/

/*void _Unwind_SetGR (struct _Unwind_Context *, int, _Unwind_Word)
{
}

_Unwind_Ptr _Unwind_GetIPInfo (struct _Unwind_Context *, int *)
{
	return 0;
}

void _Unwind_SetIP (struct _Unwind_Context *, _Unwind_Ptr)
{

}

void *_Unwind_GetLanguageSpecificData (struct _Unwind_Context *)
{
	return 0;
}

_Unwind_Ptr _Unwind_GetRegionStart (struct _Unwind_Context *)
{
	return 0;
}

_Unwind_Reason_Code LIBGCC2_UNWIND_ATTRIBUTE _Unwind_RaiseException (struct _Unwind_Exception *)
{
	return _URC_NO_REASON;
}

void _Unwind_DeleteException (struct _Unwind_Exception *)
{

}

void LIBGCC2_UNWIND_ATTRIBUTE _Unwind_Resume (struct _Unwind_Exception *)
{

}

_Unwind_Reason_Code LIBGCC2_UNWIND_ATTRIBUTE _Unwind_Resume_or_Rethrow (struct _Unwind_Exception *)
{
	return _URC_NO_REASON;
}

_Unwind_Ptr _Unwind_GetDataRelBase (struct _Unwind_Context *)
{
	return 0;
}

_Unwind_Ptr _Unwind_GetTextRelBase (struct _Unwind_Context *)
{
	return 0;
}
*/
/*void abort()
{
	exit(-1);
}

void *__tls_get_addr(size_t *v)
{
	return 0;
}*/

}
