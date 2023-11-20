#ifndef _SM_INOUTPARAM
#define _SM_INOUTPARAM
#include "OutParam.h"

template <typename T> struct InOutParam : public OutParam<T>
{
public:
	InOutParam() = delete;

	InOutParam(T &p) : OutParam<T>(p)
	{
	}

	InOutParam(NotNullPtr<T> p) : OutParam<T>(p)
	{
	}

	T Get() const
	{
		return *this->p;
	}
};
#endif
