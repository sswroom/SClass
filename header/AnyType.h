#ifndef _SM_ANYTYPE
#define _SM_ANYTYPE
#include "NotNullPtr.h"
#include "UnsafeArray.h"

struct AnyType
{
	void *p;

	AnyType() = default;
	
	AnyType(void *p)
	{
		this->p = p;
	}

	template<typename T> AnyType(NN<T> p)
	{
		this->p = p.Ptr();
	}

	template<typename T> NN<T> GetNN() const
	{
		return NN<T>::FromPtr((T*)this->p);
	}

	template<typename T> Optional<T> GetOpt() const
	{
		return Optional<T>((T*)this->p);
	}

	template<typename T> UnsafeArray<T> GetArray() const
	{
		return UnsafeArray<T>((T*)this->p);
	}

	OSInt GetOSInt() const
	{
		return (OSInt)this->p;
	}

	UOSInt GetUOSInt() const
	{
		return (UOSInt)this->p;
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
