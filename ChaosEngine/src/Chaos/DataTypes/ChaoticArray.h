/* date = May 19th 2021 8:44 am */
/*
* Fixed size array type that keeps track of empty slots when something is removed or added to 
* it, adding an item to it does not mean it'll be added to the end of it, so only use when
* order isn't important. (hence the name "chaotic"). Will ensure a pointer will always be in 
* the same position so long as it exists on this array.
* Does however mean that there is a possibility the data you're accessing could be null
* Need to check before.
* NOTE: will mean itteration is more costly, but it will be scalable
*/
#ifndef _CHAOTIC_ARRAY_H
#define _CHAOTIC_ARRAY_H

namespace Chaos
{
	template<typename T> class ChaoticArray
	{
		public:
		ChaoticArray(size_t size) 
		{
			Data = (T*)malloc(size * sizeof(T));
			memset(Data, 0, size * sizeof(T));
			
			FreeSlots = (int*)malloc(size * sizeof(int));
			memset(FreeSlots, 0, size * sizeof(int));
			
			FreeSlotsSize = size;
			Size = size;
		}
		
		~ChaoticArray() 
		{
			free(Data);
			free(FreeSlots);
		}
		
		
		bool Push(T value)
		{
			if (!FreeSlotsSize)
			{
				LOGCORE_ERROR("Chaotic Array: no more free slots. Array size: {0}", Size);
				return false;
			}
			
			--FreeSlotsSize;
			Data[FreeSlotsSize] = value;
			return true;
		}
		
		
		bool Remove(T value)
		{
			for (int i = 0; i < Size; ++i)
			{
				if (Data[i] == value)
				{
					Data[i] = nullptr;
					FreeSlots[FreeSlotsSize] = i;
					++FreeSlotsSize;
					return true;
				}
			}
			return false;
		}
		
		
		// removes from the array, doesn't free or destroy pointer
		bool Remove(size_t index)
		{
			if (index >= Size)
			{
				LOGCORE_ERROR("Chaotic Array index out of range: Trying to remove element {0} of array size {1}", index, Size);
				return false;
			}
			Data[index] = nullptr;
			FreeSlots[FreeSlotsSize] = index;
			++FreeSlotsSize;
			return true;
		}
		
		
		// frees and removes the data from the array.
		void Destroy(size_t index)
		{
			if (Data[index])
			{
				free(Data[index]);
				Remove(index);
			}
		}
		
		
		bool IsValid(size_t index)
		{
			return index < Size && Data[index];
		}
		
		
		T& operator[](size_t index) const 
		{
			if (index >= Size)
			{
				LOGCORE_ERROR("Chaotic Array index out of range: Trying to access element {0} of array size {1}", index, Size);
			}
			return Data[index];
		}
		
		
		size_t Size = 0;
		size_t FreeSlotsSize = 0;
		
		T* Data = nullptr;
		int* FreeSlots = nullptr;
	};
}

#endif //_CHAOTIC_ARRAY_H
