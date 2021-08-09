/* date = July 3rd 2021 10:23 am */
// Size -  refers to the index of the last item + 1 (the number of used indcies)
// Capacity - the maximum amount this array can store


#ifndef _ARRAY_H
#define _ARRAY_H

namespace Chaos
{
	template <typename T> class Array
	{
		public:
		Array(size_t size)
		{
			Data = (T*)malloc(sizeof(T) * size);
			memset(Data, 0, sizeof(T) * size);
			
			m_capacity = size;
			m_size = 0;
		}
		
		
		~Array()
		{
			free(Data);
		}
		
		
		bool Push(T item)
		{
			if (m_size + 1 == m_capacity)
			{
				LOGCORE_ERROR("ARRAY: Could not add item to array. Array is full!");
				return false;
			}
			
			Data[m_size] = item;
			++m_size;
			return true;
		}
		
		
		bool Remove(T item)
		{
			for (int i = 0; i < m_size; ++i)
			{
				if (Data[i] == item)
				{
					for (int j = i; j < m_size - 1; ++j)
					{
						T temp = Data[i];
						Data[i] = Data[i + 1];
						Data[i + 1] = temp;
					}
					Data[m_size - 1] = nullptr;
					--m_size;
					return true;
				}
			}
			return false;
		}
		
		
		bool Remove(size_t index)
		{
			if (index >= m_size)
			{
				LOGCORE_ERROR("ARRAY: Trying to remove at index {0}, which is greater than the array's size!", index);
				return false;
			}
			
			for (int i = index; i < m_size - 1; ++i)
			{
				T temp = Data[i];
				Data[i] = Data[i + 1];
				Data[i + 1] = temp;
			}
			
			Data[m_size - 1] = nullptr;
			--m_size;
			return true;
		}
		
		
		// removes and deletes the item from memory
		bool Destroy(T item)
		{
			Remove(item);
			delete item;
			return true;
		}
		
		
		bool Destroy(int index)
		{
			delete Data[index];
			Data[index] = nullptr;
			Remove(index);
			return true;
		}
		
		
		void Clear()
		{
			memset(Data, 0, sizeof(T) * size);
			m_size = 0;
		}
		
		
		T& operator[](size_t index)const {return Data[index];}
		
		
		size_t Capacity() { return m_capacity; }
		size_t Size() { return m_size; }
		
		private:
		size_t m_capacity;
		size_t m_size;
		
		public:
		T* Data = nullptr;
	};
}

#endif //_ARRAY_H
