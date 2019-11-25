#include "WINAPITransport.h"



WINAPITransport::WINAPITransport()
{
}


WINAPITransport::~WINAPITransport()
{
}

bool WINAPITransport::Intialize(NetworkMode mode, int ip, int port)
{
	return false;
}

void WINAPITransport::SendData(char* data, int size)
{
}

void WINAPITransport::RecieveData(char* data, int size)
{
}

bool WINAPITransport::HasDataAvailable()
{
	return false;
}

bool WINAPITransport::GetNextData(char*& data, int& size)
{
	return false;
}
