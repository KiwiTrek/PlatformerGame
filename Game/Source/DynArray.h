#ifndef __DYNARRAY_H__
#define __DYNARRAY_H__

#include "Defs.h"

#define DYN_ARRAY_BLOCK_SIZE 16

template<class tdata>
class DynArray
{
private:

	tdata* data;
	uint memCapacity;
	uint numElements;

public:

	// Constructors
	DynArray() : memCapacity(0), numElements(0), data(NULL)
	{
		Alloc(DYN_ARRAY_BLOCK_SIZE);
	}

	DynArray(uint capacity) : memCapacity(0), numElements(0), data(NULL)
	{
		Alloc(capacity);
	}

	// Destructor
	~DynArray()
	{
		delete[] data;
	}

	// Operators
	tdata& operator[](uint index)
	{
		ASSERT(index < numElements);
		return data[index];
	}

	void Create(uint capacity)
	{
		memCapacity = 0;
		numElements = 0;
		data = NULL;
		Alloc(capacity);
	}

	const tdata& operator[](uint index) const
	{
		assert(index < numElements);
		return data[index];
	}

	const DynArray<tdata>& operator+= (const DynArray<tdata>& array)
	{
		if(numElements + array.numElements > memCapacity)
			Alloc(numElements + array.numElements);

		for(uint i = 0; i < array.numElements; ++i)
			data[numElements++] = array.data[i];

		return(*this);
	}

	// Data Management
	void PushBack(const tdata& element)
	{
		if(numElements >= memCapacity)
		{
			Alloc(memCapacity + DYN_ARRAY_BLOCK_SIZE);
		}

		data[numElements++] = element;
	}

	bool Pop(tdata& result)
	{
		if(numElements > 0)
		{
			result = data[--numElements];
			return true;
		}
		return false;
	}

	void Clear()
	{
		numElements = 0;
	}

	bool Insert(const tdata& element, uint position)
	{
		if(position > numElements)
			return false;

		if(position == numElements)
		{
			PushBack(element);
			return true;
		}

		if(numElements + 1 > memCapacity)
			Alloc(memCapacity + DYN_ARRAY_BLOCK_SIZE);

		for(uint i = numElements; i > position; --i)
		{
			data[i] = data[i - 1];
		}

		data[position] = element;
		++numElements;

		return true;
	}

	bool Insert(const DynArray<tdata>& array, uint position)
	{
		if(position > numElements)
			return false;

		if(numElements + array.numElements > memCapacity)
			Alloc(numElements + array.numElements + 1);

		for(uint i = position; i < position + array.numElements; ++i)
		{
			data[i + array.numElements] = data[i];
			data[i] = array[i - position];
			++numElements;
		}

		return true;
	}

	tdata* At(uint index)
	{
		tdata* result = NULL;

		if(index < numElements)
			return result = &data[index];

		return result;
	}

	const tdata* At(uint index) const
	{
		tdata* result = NULL;

		if(index < numElements)
			return result = &data[index];

		return result;
	}

	// Utils
	uint GetCapacity() const
	{
		return memCapacity;
	}

	uint Count() const
	{
		return numElements;
	}

	// Sort
	int BubbleSort()
	{
		int ret = 0;
		bool swapped = true;

		while(swapped)
		{
			swapped = false;
			for(uint i = 0; i < numElements - 2; ++i)
			{
				++ret;
				if(data[i] > data[i + 1])
				{
					SWAP(data[i], data[i + 1]);
					swapped = true;
				}
			}
		}

		return ret;
	}


	int BubbleSortOptimized()
	{
		int ret = 0;
		uint count;
		uint last = numElements - 2;

		while(last > 0)
		{
			count = last;
			last = 0;
			for(uint i = 0; i < count; ++i)
			{
				++ret;
				if(data[i] > data[i + 1])
				{
					SWAP(data[i], data[i + 1]);
					last = i;
				}
			}
		}
		return ret;
	}


	int CombSort()
	{
		int ret = 0;
		bool swapped = true;
		int gap = numElements - 1;
		float shrink = 1.3f;

		while(swapped || gap > 1)
		{
			gap = MAX(1, gap / shrink);

			swapped = false;
			for(uint i = 0; i + gap < numElements - 1; ++i)
			{
				++ret;
				if(data[i] > data[i + gap])
				{
					SWAP(data[i], data[i + gap]);
					swapped = true;
				}
			}
		}

		return ret;
	}

	void Flip()
	{
		tdata* start = &data[0];
		tdata* end = &data[numElements-1];

		while(start < end) SWAP(*start++, *end--);
	}

private:

	// Private Utils
	void Alloc(uint mem)
	{
		tdata* tmp = data;

		memCapacity = mem;
		data = new tdata[memCapacity];

		numElements = MIN(memCapacity, numElements);

		if(tmp != NULL)
		{
			for(uint i = 0; i < numElements; ++i) data[i] = tmp[i];

			delete[] tmp;
		}
	}
};

#endif // __DYNARRAY_H__