/* date = June 30th 2021 8:11 am */

#ifndef _BINARY_H
#define _BINARY_H

namespace Chaos
{
	class Binary
	{
		public:
		// initialises binary data pointer to size provided
		Binary(size_t size);
		// will immediately copy in data provided for the size provided
		Binary(void* data, size_t size); 
		Binary(const Binary& other);
		~Binary();
		bool Write(void* inData, size_t size);
		char* Read(size_t readLocation, size_t size);
		
		size_t Capacity();
		
		
		inline Binary operator + (const Binary& other) 
		{
			Binary result(this->m_capacity + other.m_capacity);
			
			memcpy((void*)result.Data, (void*)Data, m_capacity);
			memcpy((void*)result.Data[m_capacity], (void*)other.Data, other.m_capacity);
			
			return result;
		}
		
		inline Binary operator = (const Binary& other)
		{
			free(Data);
			
			m_capacity = other.m_capacity;
			Data = (char*)malloc(m_capacity);
			
			memcpy((void*)Data, (void*)other.Data, m_capacity);
			WriteLocation = other.WriteLocation;
			
			return *this;
		}
		
		public:
		size_t WriteLocation = 0;
		char* Data;
		
		private:
		size_t m_capacity;
	};
}

#endif //_BINARY_H
