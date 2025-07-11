#ifndef _SM_OPTOUT
#define _SM_OPTOUT
#include "OutParam.h"
#include <cstddef>

template <typename T> struct OptOut
{
private:
	T* p;

	OptOut(T *p, int)
	{
		this->p = p;
	}
public:
	OptOut()
	{
		this->p = 0;
	}

	OptOut(std::nullptr_t)
	{
		this->p = 0;
	}

	OptOut(T &p)
	{
		this->p = &p;
	}

	OptOut(NN<T> p)
	{
		this->p = p.Ptr();
	}

	OptOut(OutParam<T> p)
	{
		this->p = p.Ptr();
	}

	Bool IsNull() const
	{
		return this->p == 0;
	}

	Bool IsNotNull() const
	{
		return this->p != 0;
	}

	void Set(T p)
	{
		if (this->p)
			*this->p = p;
	}

	void SetNoCheck(T p)
	{
		*this->p = p;
	}

	OutParam<T> Or(T &p)
	{
		if (this->p)
			return OutParam<T>(*this->p);
		else
			return OutParam<T>(p);
	}

	T *Ptr()
	{
		return this->p;
	}

	T operator=(T) = delete;

	template <typename V> static OptOut<T> ConvertFrom(OptOut<V> p)
	{
		return OptOut<T>((T*)p.Ptr(), 0);
	}
};
#endif
