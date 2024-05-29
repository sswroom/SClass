#ifndef _SM_UNSAFEARRAYOPT
#define _SM_UNSAFEARRAYOPT
#include "MyMemory.h"
#include "UnsafeArray.h"

template <typename T> struct UnsafeArrayOpt
{
private:
	T *p;

	virtual void SetPtr(T *p)
	{
		this->p = p;
	}

public:
	UnsafeArrayOpt() = default;

	UnsafeArrayOpt(T *p)
	{
		this->SetPtr(p);
	}

	UnsafeArrayOpt(UnsafeArray<T> p)
	{
		this->SetPtr(p.Ptr());
	}

	template<typename V> UnsafeArrayOpt(UnsafeArrayOpt<V> p)
	{
		this->p = p.Ptr();
	}

	Bool IsNull() const
	{
		return this->p == 0;
	}

	Bool NotNull() const
	{
		return this->p != 0;
	}

	T *Ptr() const
	{
		return this->p;
	}

	Bool SetTo(UnsafeArray<T> &val)
	{
		return val.Set(this->p);
	}

	template <typename V> static UnsafeArrayOpt<T> ConvertFrom(UnsafeArrayOpt<V> ptr)
	{
		UnsafeArrayOpt<T> ret;
		ret.p = (T*)ptr.Ptr();
		return ret;
	}
};
#endif
