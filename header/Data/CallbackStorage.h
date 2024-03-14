#ifndef _SM_DATA_CALLBACKSTORAGE
#define _SM_DATA_CALLBACKSTORAGE
#include "AnyType.h"

namespace Data
{
	template <class T> struct CallbackStorage
	{
	public:
		T func;
		AnyType userObj;

		CallbackStorage()
		{
			this->func = 0;
			this->userObj = nullptr;
		}

		CallbackStorage(nullptr_t)
		{
			this->func = 0;
			this->userObj = nullptr;
		}

		CallbackStorage(T func, AnyType userObj)
		{
			this->func = func;
			this->userObj = userObj;
		}

		Bool operator==(const CallbackStorage &obj) const
		{
			if (this->func == 0)
			{
				return obj.func == 0;
			}
			return this->func == obj.func && this->userObj.p == obj.userObj.p;
		}
	};
}
#endif
