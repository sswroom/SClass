#ifndef _SM_NOTNULLPTR
#define _SM_NOTNULLPTR
#include <stdio.h>

template <typename T> struct NotNullPtr
{
private:
	T* p;

	NotNullPtr(T *p)
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
#endif
		}
	}
public:
	NotNullPtr() = default;
	NotNullPtr(Int32) = delete;

	NotNullPtr(T &p)
	{
		this->p = &p;
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

	static NotNullPtr<T> FromPtr(T *p)
	{
		return NotNullPtr<T>(p);
	}
};
#endif
