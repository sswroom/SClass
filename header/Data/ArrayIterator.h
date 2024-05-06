#ifndef _SM_DATA_ARRAYITERATOR
#define _SM_DATA_ARRAYITERATOR

namespace Data
{
	template <class T> class ArrayIterator
	{
	private:
		UnsafeArray<const T> arr;
		UOSInt cnt;
	public:
		ArrayIterator() = default;
		ArrayIterator(UnsafeArray<const T> arr, UOSInt cnt)
		{
			this->arr = arr;
			this->cnt = cnt;
		}

		Bool HasNext() const
		{
			return this->cnt > 0;
		}

		T Next()
		{
			this->cnt--;
			return *(arr)++;
		}
	};
}
#endif
