#ifndef _SM_DATA_SORT_ARTIFICIALQUICKSORTFUNC
#define _SM_DATA_SORT_ARTIFICIALQUICKSORTFUNC
#include "MyMemory.h"
#include "Data/ArrayList.h"
//http://www.critticall.com/ArtificialSort.html

namespace Data
{
	namespace Sort
	{
		template <class T> class ArtificialQuickSortFunc
		{
		public:
			typedef OSInt (__stdcall *CompareFunc)(T obj1, T obj2);
		private:
			static void PreSort(T *arr, CompareFunc func, OSInt left, OSInt right)
			{
				T temp;

				while (left < right)
				{
					if (func(arr[left], arr[right]) > 0)
					{
						temp = arr[left];
						arr[left] = arr[right];
						arr[right] = temp;
					}
					left++;
					right--;
				}
			}

			static void PreSortInv(T *arr, CompareFunc func, OSInt left, OSInt right)
			{
				T temp;

				while (left < right)
				{
					if (func(arr[left], arr[right]) < 0)
					{
						temp = arr[left];
						arr[left] = arr[right];
						arr[right] = temp;
					}
					left++;
					right--;
				}
			}

			static void ISortB(T *arr, CompareFunc func, OSInt left, OSInt right)
			{
				OSInt i;
				OSInt j;
				OSInt k;
				OSInt l;
				T temp;
				T temp1;
				T temp2;
				temp1 = arr[left];
				i = left + 1;
				while (i <= right)
				{
					temp2 = arr[i];
					if ( func(temp1, temp2) > 0)
					{
						j = left;
						k = i - 1;
						while (j <= k)
						{
							l = (j + k) >> 1;
							temp = arr[l];
							if (func(temp, temp2) > 0)
							{
								k = l - 1;
							}
							else
							{
								j = l + 1;
							}
						}
						MemCopyO(&arr[j + 1], &arr[j], (UOSInt)(i - j) * sizeof(arr[0]));
						arr[j] = temp2;
					}
					else
					{
						temp1 = temp2;
					}
					i++;
				}
			}

			static void ISort(T *arr, CompareFunc func, OSInt left, OSInt right)
			{
				OSInt i;
				OSInt j;
				T temp;
				T temp1;
				T temp2;
				temp1 = arr[left];
				i = left + 1;
				while (i <= right)
				{
					temp2 = arr[i];
					if (func(temp1, temp2) > 0)
					{
						arr[i] = temp1;
						j = i - 1;
						while (j > left)
						{
							temp = arr[j-1];
							if (func(temp, temp2) > 0)
								arr[j--] = temp;
							else
								break;
						}
						arr[j] = temp2;
					}
					else
					{
						temp1 = temp2;
					}
					i++;
				}
			}

			static void ISortInv(T *arr, CompareFunc func, OSInt left, OSInt right)
			{
				OSInt i;
				OSInt j;
				T temp;
				T temp1;
				T temp2;
				temp1 = arr[left];
				i = left + 1;
				while (i <= right)
				{
					temp2 = arr[i];
					if (func(temp1, temp2) < 0)
					{
						arr[i] = temp1;
						j = i - 1;
						while (j > left)
						{
							temp = arr[j-1];
							if (func(temp, temp2) < 0)
								arr[j--] = temp;
							else
								break;
						}
						arr[j] = temp2;
					}
					else
					{
						temp1 = temp2;
					}
					i++;
				}
			}

		public:
			static void Sort(T *arr, CompareFunc func, OSInt firstIndex, OSInt lastIndex)
			{
#if _OSINT_SIZE == 16
				OSInt levi[256];
				OSInt desni[256];
#else
				OSInt *levi = MemAlloc(OSInt, 65536);
				OSInt *desni = &levi[32768];
#endif
				OSInt index;
				OSInt i;
				OSInt left;
				OSInt right;
				T meja;
				OSInt left1;
				OSInt right1;
				T temp;

				PreSort(arr, func, firstIndex, lastIndex);

				index = 0;
				levi[index] = firstIndex;
				desni[index] = lastIndex;

				while ( index >= 0 )
				{
					left = levi[index];
					right = desni[index];
					i = right - left;
					if (i <= 0)
					{
						index--;
					}
					else if (i <= 64)
					{
						ISortB(arr, func, left, right);
						index--;
					}
					else
					{
						meja = arr[ (left + right) >> 1 ];
						left1 = left;
						right1 = right;
						while (true)
						{
							while ( func(arr[right1], meja) >= 0 )
							{
								if (--right1 < left1)
									break;
							}
							while ( func(arr[left1], meja) < 0 )
							{
								if (++left1 > right1)
									break;
							}
							if (left1 > right1)
								break;

							temp = arr[right1];
							arr[right1--] = arr[left1];
							arr[left1++] = temp;
						}
						if (left1 == left)
						{
							arr[(left + right) >> 1] = arr[left];
							arr[left] = meja;
							levi[index] = left + 1;
							desni[index] = right;
						}
						else
						{
							desni[index] = --left1;
							right1++;
							index++;
							levi[index] = right1;
							desni[index] = right;
						}
					}
				}
#if _OSINT_SIZE != 16
				MemFree(levi);
#endif
			}

			static void SortInv(T *arr, CompareFunc func, OSInt firstIndex, OSInt lastIndex)
			{
				OSInt levi[256];
				OSInt desni[256];

				OSInt index;
				OSInt i;
				OSInt j;
				OSInt left;
				OSInt right;
				void *meja;
				OSInt left1;
				OSInt right1;
				void *temp1;
				void *temp2;
				void *temp;

				PreSortInv(arr, func, firstIndex, lastIndex);

				index = 0;
				levi[index] = firstIndex;
				desni[index] = lastIndex;

				while ( index >= 0 )
				{
					left = levi[index];
					right = desni[index];
					i = right - left;
					if (i <= 0)
					{
					}
					else if (i <= 15)
					{
						ISortInv(arr, func, left, right);
					}
					else
					{
						temp1 = arr[left];
						j = left + 1;
						while (j <= right)
						{
							temp2 = arr[j];
							if ( func(temp2, temp1) > 0)
							{
								meja = temp1;
								if ( func(arr[ (left + right) >> 1 ], meja) < 0 )
								{
									meja = arr[ (left + right) >> 1 ];
								}
								left1 = left;
								right1 = right;
								while (true)
								{
									while ( func(arr[right1], meja) <= 0 ) right1--;
									while ( func(arr[left1], meja) > 0 ) left1++;
									if (left1 > right1)
										break;

									temp = arr[right1];
									arr[right1--] = arr[left1];
									arr[left1++] = temp;
								}
								desni[index] = --left1;
								levi[++index] = ++right1;
								desni[index] = right;
								goto labelcritticall5;
							}
							temp1 = temp2;
							j++;
						}
					}
					index--;
			labelcritticall5:;
				}
			}

			static void Sort(NotNullPtr<Data::ArrayList<T>> arr, CompareFunc func)
			{
				UOSInt j;
				T *a = arr->GetPtr(j);
				Sort(a, func, 0, (OSInt)j - 1);
			}
		};
	}
}
#endif
