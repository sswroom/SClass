#ifndef _SM_OPTIONAL
#define _SM_OPTIONAL
#include "NotNullPtr.h"

template <typename T> struct Optional
{
public:
	typedef void (__stdcall *FreeFunc)(NN<T> v);
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

	template <typename V> Optional(NN<V> p)
	{
		this->p = p.Ptr();
	}

	template <typename V> Optional(Optional<V> p)
	{
		NN<V> nnp;
		if (p.SetTo(nnp))
			this->p = nnp.Ptr();
		else
			this->p = 0;
	}

	T *OrNull() const
	{
		return this->p;
	}

	Bool SetTo(NN<T> &p) const
	{
		return p.Set(this->p);
	}

	Bool IsNull() const
	{
		return this->p == 0;
	}

	Bool NotNull() const
	{
		return this->p != 0;
	}

	void Delete()
	{
		if (this->p)
		{
			delete this->p;
			this->p = 0;
		}
	}

	void FreeBy(FreeFunc freeFunc)
	{
		NN<T> v;
		if (v.Set(this->p))
		{
			freeFunc(v);
			this->p = 0;
		}
	}

	Bool operator==(Optional<T> v) const
	{
		return this->p == v.p;
	}

	Bool operator!=(Optional<T> v) const
	{
		return this->p != v.p;
	}

	Bool operator==(std::nullptr_t) const = delete;

	template <typename V> static Optional<T> ConvertFrom(Optional<V> ptr)
	{
		NN<V> p;
		if (ptr.SetTo(p))
			return Optional<T>((T*)p.Ptr());
		return Optional<T>(0);
	}
};
#endif
