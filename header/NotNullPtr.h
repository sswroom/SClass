#ifndef _SM_NOTNULLPTR
#define _SM_NOTNULLPTR
#include <stdio.h>

template <typename T> struct NotNullPtr
{
private:
	T* p;

	NotNullPtr(T *p)
	{
		this->SetPtr(p);
	}
public:
	NotNullPtr() = default;
	NotNullPtr(Int32) = delete;

	NotNullPtr(T &p)
	{
		this->p = &p;
	}

	template <typename V> NotNullPtr(NotNullPtr<V> p)
	{
		this->p = p.Ptr();
	}

/*	NotNullPtr(const NotNullPtr<T> &p)
	{
		this->p = p.p;
	}*/

	T *Ptr() const
	{
		return this->p;
	}

	T* operator->() const
	{
        return this->p;
    }

	Bool operator==(const NotNullPtr<T> &p)
	{
		return this->p == p.p;
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

	void Set(const T &v)
	{
		*this->p = v;
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
		delete this->p;
	}

	static NotNullPtr<T> FromPtr(T *p)
	{
		return NotNullPtr<T>(p);
	}
	template <typename V> static NotNullPtr<T> ConvertFrom(NotNullPtr<V> ptr)
	{
		NotNullPtr<T> ret;
		ret.p = (T*)ptr.Ptr();
		return ret;
	}
};
#endif
