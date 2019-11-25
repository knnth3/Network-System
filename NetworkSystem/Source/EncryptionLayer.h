#pragma once
#include "Packet.h"

class EncryptionLayer
{
public:
	EncryptionLayer();
	~EncryptionLayer();

	virtual void DecryptPacket(Packet p) = 0;
	virtual void EncryptPacket(Packet& p) = 0;
	virtual const char* GetSignature() = 0;
};

