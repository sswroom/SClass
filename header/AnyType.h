#ifndef _SM_ANYTYPE
#define _SM_ANYTYPE
#include "NotNullPtr.h"

struct AnyType
{
	void *p;

	AnyType() = default;
	
	AnyType(void *p)
	{
		this->p = p;
	}

	template<typename T> AnyType(NotNullPtr<T> p)
	{
		this->p = p.Ptr();
	}

	template<typename T> NotNullPtr<T> GetNN() const
	{
		return NotNullPtr<T>::FromPtr((T*)this->p);
	}

	template<typename T> Optional<T> GetOpt() const
	{
		return Optional<T>((T*)this->p);
	}

	Bool IsNull() const
	{
		return this->p == 0;
	}

	Bool NotNull() const
	{
		return this->p != 0;
	}

	Bool operator==(AnyType p)
	{
		return this->p == p.p;
	}
};

#endif
