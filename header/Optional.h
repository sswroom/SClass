#ifndef _SM_OPTIONAL
#define _SM_OPTIONAL
#include "NotNullPtr.h"

template <typename T> struct Optional
{
private:
	T *p;
public:
	Optional() = default;

	Optional(T *p)
	{
		this->p = p;
	}

	Optional(T &p)
	{
		this->p = &p;
	}

	Optional(NotNullPtr<T> p)
	{
		this->p = p.Ptr();
	}

	T *OrNull() const
	{
		return this->p;
	}

	Bool SetTo(NotNullPtr<T> &p) const
	{
		return p.Set(this->p);
	}

	Bool IsNull() const
	{
		return this->p == 0;
	}

	template <typename V> static Optional<T> ConvertFrom(Optional<V> ptr)
	{
		NotNullPtr<V> p;
		if (ptr.SetTo(p))
			return Optional<T>((T*)p.Ptr());
		return Optional<T>(0);
	}
};
#endif