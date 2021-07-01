#include "chaospch.h"

#include "Binary.h"

namespace Chaos
{
	Binary::Binary(size_t size)
	{
		Data = (char*)malloc(size);
		memset((void*)Data, 0, size);
		m_capacity = size;
	}
	
	
	Binary::Binary(void* data, size_t size)
	{
		Data = (char*)malloc(size);
		memset((void*)Data, 0, size);
		m_capacity = size;
		
		Write(data, size);
	}
	
	
	Binary::Binary(const Binary& other)
	{
		m_capacity = other.m_capacity;
		WriteLocation = other.WriteLocation;
		
		Data = (char*)malloc(m_capacity);
		memcpy(Data, other.Data, m_capacity);
	}
	
	
	Binary::~Binary()
	{
		if (Data)
			free(Data);
	}
	
	
	bool Binary::Write(void* inData, size_t size)
	{
		if (WriteLocation + size > m_capacity)
		{
			LOGCORE_ERROR("BINARY: writing data would cause binary to overflow, aborting write to binary!");
			return false;
		}
		
		memcpy((void*)&Data[WriteLocation], inData, size);
		WriteLocation += size;
		
		return true;
	}
	
	
	char* Binary::Read(size_t readLocation, size_t size)
	{
		char* outData = (char*)malloc(size);
		memset((void*)outData, 0, size);
		
		if (readLocation + size > m_capacity)
		{
			LOGCORE_ERROR("BINARY: reading specified size of data would read past allocated memory for this file, aborting read");
			return outData;
		}
		
		memcpy((void*)outData, (void*)&Data[readLocation], size);
		
		return outData;
	}
	
	
	size_t Binary::Capacity()
	{
		return m_capacity;
	}
}