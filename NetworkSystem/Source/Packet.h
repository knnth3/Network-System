#pragma once
#include <vector>

struct Packet
{
	int ID = 0;
	int Size = 0;
	char* DataPtr = 0;

	~Packet()
	{
		if(DataPtr)
			delete[] DataPtr;
	}

	Packet() = default;

	Packet(const Packet& other)
	{
		ID = other.ID;
		Size = other.Size;

		if (other.DataPtr)
		{
			DataPtr = new char[Size];
			memcpy(DataPtr, other.DataPtr, Size);
		}
	}

	void operator=(const Packet& other)
	{
		ID = other.ID;
		Size = other.Size;

		if (other.DataPtr)
		{
			DataPtr = new char[Size];
			memcpy(DataPtr, other.DataPtr, Size);
		}
	}
};

