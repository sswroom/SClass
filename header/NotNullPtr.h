#ifndef _SM_NOTNULLPTR
#define _SM_NOTNULLPTR
#include <stdio.h>

struct NotNullPtrAny
{
protected:
	const void *p;

	NotNullPtrAny() = default;
public:
	const void *Ptr()
	{
		return this->p;
	}

	Bool Set(const void *p)
	{
		if (p)
		{
			this->p = p;
			return true;
		}
		return false;
	}
};

template <typename T> struct NotNullPtr : public NotNullPtrAny
{
private:
	NotNullPtr(T *p)
	{
		this->SetPtr(p);
	}
public:
	NotNullPtr() = default;

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
		return (T*)this->p;
	}

	T* operator->() const
	{
        return (T*)this->p;
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

	static NotNullPtr<T> FromPtr(T *p)
	{
		return NotNullPtr<T>(p);
	}

	static NotNullPtr<T> ConvertFrom(NotNullPtrAny ptr)
	{
		NotNullPtr<T> ret;
		ret.p = (T*)ptr.Ptr();
		return ret;
	}

	template <typename V> static NotNullPtr<T> ConvertFrom(NotNullPtr<V> ptr)
	{
		NotNullPtr<T> ret;
		ret.p = (T*)ptr.Ptr();
		return ret;
	}
};
#endif
