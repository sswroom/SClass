#ifndef _SM_UNSAFEARRAYOPT
#define _SM_UNSAFEARRAYOPT
#include "MyMemory.h"

template <typename T> struct UnsafeArrayOpt
{
protected:
	T *p;
private:
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

	Bool operator<(const UnsafeArrayOpt<T> p)
	{
		return this->p < p.p;
	}

	Bool operator>(const UnsafeArrayOpt<T> p)
	{
		return this->p > p.p;
	}

	Bool operator<=(const UnsafeArrayOpt<T> p)
	{
		return this->p <= p.p;
	}

	Bool operator>=(const UnsafeArrayOpt<T> p)
	{
		return this->p >= p.p;
	}

	Bool operator==(const UnsafeArrayOpt<T> ptr)
	{
		return this->p == ptr.p;
	}

	UnsafeArrayOpt<T> operator++(int)
	{
		T *tmp = this->p;
		this->p++;
		return tmp;
	}

	UnsafeArrayOpt<T> operator++()
	{
		this->p++;
		return this->p;
	}

	UnsafeArrayOpt<T> operator--(int)
	{
		T *tmp = this->p;
		this->p--;
		return tmp;
	}

	UnsafeArrayOpt<T> operator--()
	{
		this->p--;
		return this->p;
	}

	T &operator[] (UOSInt index) const
	{
		return this->p[index];
	}

	T &operator[] (OSInt index) const
	{
		return this->p[index];
	}

	virtual T *operator+(OSInt val) const
	{
		return this->p + val;
	}

	virtual T *operator+(UOSInt val) const
	{
		return this->p + val;
	}

	UnsafeArrayOpt<T> &operator+=(OSInt val)
	{
		this->p += val;
		return *this;
	}

	UnsafeArrayOpt<T> &operator+=(UOSInt val)
	{
		this->p += val;
		return *this;
	}

#if _OSINT_SIZE == 64
	T &operator[] (UInt32 index) const
	{
		return this->p[index];
	}

	T &operator[] (Int32 index) const
	{
		return this->p[index];
	}

	T *operator+(Int32 val) const
	{
		return this->p + val;
	}

	T *operator+(UInt32 val) const
	{
		return this->p + val;
	}

	UnsafeArrayOpt<T> &operator+=(Int32 val)
	{
		this->p += val;
		return *this;
	}

	UnsafeArrayOpt<T> &operator+=(UInt32 val)
	{
		this->p += val;
		return *this;
	}
#endif

	OSInt operator-(UnsafeArrayOpt<T> p)
	{
		return this->p - p.p;
	}

	T &operator*() const
	{
		return this->p[0];
	}

	void CopyFromNO(UnsafeArrayOpt<const T> srcArr, UOSInt arrSize)
	{
		MemCopyNO(this->p, srcArr.Ptr(), arrSize * sizeof(T));
	}

	template <typename V> static UnsafeArrayOpt<T> ConvertFrom(UnsafeArrayOpt<V> ptr)
	{
		UnsafeArrayOpt<T> ret;
		ret.p = (T*)ptr.Ptr();
		return ret;
	}
};

template<typename V> OSInt operator-(V *p1, UnsafeArrayOpt<V> p2)
{
	return p1 - p2.Ptr();
}

template<typename V> Bool operator>(V *p1, UnsafeArrayOpt<V> p2)
{
	return p1 > p2.Ptr();
}

template<typename V> Bool operator<(V *p1, UnsafeArrayOpt<V> p2)
{
	return p1 < p2.Ptr();
}

template<typename V> Bool operator>=(V *p1, UnsafeArrayOpt<V> p2)
{
	return p1 >= p2.Ptr();
}

template<typename V> Bool operator<=(V *p1, UnsafeArrayOpt<V> p2)
{
	return p1 <= p2.Ptr();
}
#endif
