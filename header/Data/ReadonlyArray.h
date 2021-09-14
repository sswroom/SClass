#ifndef _SM_DATA_READONLYARRAY
#define _SM_DATA_READONLYARRAY
namespace Data
{
	template <class T> class ReadonlyArray
	{
	protected:
		T *arr;
		UOSInt cnt;

	public:
		ReadonlyArray(const T *arr, UOSInt cnt);
		~ReadonlyArray();

		UOSInt GetCount();
		T GetItem(UOSInt index);
		T operator [](UOSInt index);
	};

	template <class T> ReadonlyArray<T>::ReadonlyArray(const T *arr, UOSInt cnt)
	{
		this->arr = MemAlloc(T, cnt);
		MemCopyNO(this->arr, arr, cnt * sizeof(T));
		this->cnt = cnt;
	}

	template <class T> ReadonlyArray<T>::~ReadonlyArray()
	{
		MemFree(this->arr);
	}

	template <class T> UOSInt ReadonlyArray<T>::GetCount()
	{
		return cnt;
	}

	template <class T> T ReadonlyArray<T>::GetItem(UOSInt index)
	{
		return (*this)[index];
	}
	
	template <class T> T ReadonlyArray<T>::operator [](UOSInt index)
	{
		if (index >= this->cnt)
		{
			return 0;
		}
		return this->arr[index];
	}
}
#endif
