#ifndef _SM_OUTPARAM
#define _SM_OUTPARAM

template <typename T> struct OutParam
{
private:
	T* p;

	OutParam(T *p)
	{
		this->p = p;
	}
public:
	OutParam() = delete;

	OutParam(T &p)
	{
		this->p = &p;
	}

	OutParam(NotNullPtr<T> p)
	{
		this->p = p.Ptr();
	}

	void Set(T p)
	{
		*this->p = p;
	}

	T *Ptr()
	{
		return this->p;
	}

	template <typename V> static OutParam<T> ConvertFrom(OutParam<V> p)
	{
		return OutParam<T>((T*)p.Ptr());
	}
};
#endif