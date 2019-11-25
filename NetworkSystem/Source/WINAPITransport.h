#pragma once
#include "TransportLayer.h"

class WINAPITransport :
	public TransportLayer
{
public:
	WINAPITransport();
	~WINAPITransport();

	virtual bool Intialize(NetworkMode mode, int ip, int port) override;
	virtual void SendData(char* data, int size) override;
	virtual void RecieveData(char* data, int size) override;
	virtual bool HasDataAvailable() override;
	virtual bool GetNextData(char*& data, int& size) override;
};

