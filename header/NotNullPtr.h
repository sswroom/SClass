#ifndef _SM_NOTNULLPTR
#define _SM_NOTNULLPTR
#include "MemTool.h"
#include <stdio.h>

template <typename T> struct NN
{
private:
	T *p;
private:
	NN(T *p)
	{
		this->SetPtr(p);
	}
public:
	NN() = default;

	NN(T &p)
	{
		this->p = &p;
	}

	template <typename V> NN(NN<V> p)
	{
		this->p = p.Ptr();
	}

/*	NN(const NN<T> &p)
	{
		this->p = p.p;
	}*/

	T *Ptr() const
	{
		return (T*)this->p;
	}

	T* operator->() const
	{
        return (T*)this->p;
    }

	Bool operator==(const NN<T> &p)
	{
		return this->p == p.p;
	}

	Bool operator!=(const NN<T> &p)
	{
		return this->p != p.p;
	}

	Bool Set(T *p)
	{
		if (p)
		{
			this->p = p;
			return true;
		}
		return false;
	}

	void SetVal(const T &v)
	{
		*(T*)this->p = v;
	}

	void SetPtr(T *p)
	{
		if (p == 0)
		{
#if defined(__PRETTY_FUNCTION__)
			printf("Null found in not null variable: " __PRETTY_FUNCTION__ "\r\n");
#elif defined(__FUNCSIG__)
			printf("Null found in not null pointer: " __FUNCSIG__ "\r\n");
#elif defined(__FUNCTION__)
			printf("Null found in not null pointer: " __FUNCTION__ "\r\n");
#elif defined(__func__)
			printf("Null found in not null pointer: " __func__ "\r\n");
#else
			printf("Null found in not null pointer\r\n");
#endif
		}
		this->p = p;
	}

	void Delete()
	{
		T *p = (T*)this->p;
		delete p;
	}

	void CopyFrom(NN<const T> p)
	{
		MemCopyNO(this->p, p.Ptr(), sizeof(T));
	}

	void ZeroContent()
	{
		MemClear(this->p, sizeof(T));
	}

	static NN<T> FromPtr(T *p)
	{
		return NN<T>(p);
	}

	template <typename V> static NN<T> ConvertFrom(NN<V> ptr)
	{
		NN<T> ret;
		ret.p = (T*)ptr.Ptr();
		return ret;
	}
};

#define NotNullPtr NN
#endif
