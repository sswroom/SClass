#ifndef _SM_NINT32
#define _SM_NINT32
#include "NotNullPtr.h"
#include <cstddef>

#define NINT32_NULL ((Int32)0x80000000)

struct NInt32
{
	Int32 val;
	NInt32() = default;
	NInt32(std::nullptr_t)
	{
		this->val = NINT32_NULL;
	}

	NInt32(Int32 val)
	{
		this->val = val;
	}

	Bool IsNull() const
	{
		return this->val == NINT32_NULL;
	}

	Int32 IntVal() const
	{
		return this->val;
	}

	Bool operator==(NInt32 val) const
	{
		return this->val == val.val;
	}
};
#endif
