#pragma once

enum NetworkMode
{
	CLIENT,
	SERVER,
	HYBRID
};

class TransportLayer
{
public:
	TransportLayer();
	~TransportLayer();

	virtual bool Intialize(NetworkMode mode, int ip, int port) = 0;
	virtual void SendData(char* data, int size) = 0;
	virtual void RecieveData(char* data, int size) = 0;
	virtual bool HasDataAvailable() = 0;
	virtual bool GetNextData(char*& data, int& size) = 0;
};