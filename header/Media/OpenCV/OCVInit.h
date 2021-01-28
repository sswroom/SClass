#ifndef _SM_MEDIA_OPENCV_OCVINIT
#define _SM_MEDIA_OPENCV_OCVINIT
#include "Sync/Mutex.h"

#if defined(__MINGW32__)
namespace std
{
	// Common base class for std::recursive_mutex and std::recursive_timed_mutex
	class __recursive_mutex_base
	{
	protected:
		typedef Sync::Mutex *		__native_type;

		__recursive_mutex_base(const __recursive_mutex_base&) = delete;
		__recursive_mutex_base& operator=(const __recursive_mutex_base&) = delete;

		__native_type  _M_mutex;

		__recursive_mutex_base()
		{
			NEW_CLASS(_M_mutex, Sync::Mutex());
		}

		~__recursive_mutex_base()
		{
			DEL_CLASS(_M_mutex);
	 	}
	};

	class recursive_mutex : private __recursive_mutex_base
	{
	public:
		typedef __native_type* 			native_handle_type;

		recursive_mutex() = default;
		~recursive_mutex() = default;

		recursive_mutex(const recursive_mutex&) = delete;
		recursive_mutex& operator=(const recursive_mutex&) = delete;

		void lock()
		{
			_M_mutex->Lock();
		}

		bool try_lock() noexcept
		{
			return _M_mutex->TryLock();
		}

		void unlock()
		{
			_M_mutex->Unlock();
		}

		native_handle_type native_handle() noexcept
		{
			return &_M_mutex;
		}
	};
}
#endif

#endif
