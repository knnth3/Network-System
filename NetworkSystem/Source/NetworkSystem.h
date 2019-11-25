#pragma once
#define NETWORK_LAYER_API _declspec(dllexport)

#include <functional>
#include <map>
#include <queue>
#include <thread>
#include <mutex>
#include <atomic>
#include "TransportLayer.h"
#include "EncryptionLayer.h"
#include "Packet.h"

class NetworkSystem
{
public:

	NetworkSystem();
	~NetworkSystem();

	template<typename Transport>
	void Startup(int mode, int ip, int port)
	{
		m_bThreadRunning = true;
		UpdateThread<Transport>((NetworkMode)mode, ip, port);
	}

	void ShutDown();

	void SendData(int ID, char* data, int size);
	void SetEncryptionLayer(EncryptionLayer* layer);

	std::function<void(int)> OnSendData;
	std::function<void(int)> OnDataRecieved;

private:

	void DepositPacket(const Packet& p);
	void AddRecievedPacket(const Packet& p);
	void GenerateSendPacket();
	Packet* GetOutgoingPacket();
	void UnpackIncomingPacket(const Packet& p);

	template<class T>
	void UpdateThread(NetworkMode mode, int ip, int port)
	{
		static_assert(std::is_base_of<TransportLayer, T>::value, "T must inherit from TransportLayer");
		TransportLayer* transport = (TransportLayer*)new T();
		if (!transport->Intialize(mode, ip, port))
			m_bThreadRunning = false;

		while (m_bThreadRunning)
		{
			Packet* outbound = GetOutgoingPacket();
			if (outbound)
			{
				transport->SendData(outbound->DataPtr, outbound->Size);
				delete outbound;
			}

			if (transport->HasDataAvailable())
			{
				Packet p;
				transport->GetNextData(p.DataPtr, p.Size);
				UnpackIncomingPacket(p);
			}
		}

		delete transport;
	}

	int m_MaxPacketSize;
	int m_PacketSize;
	std::mutex m_RecvMutex;
	std::mutex m_sendMutex;
	EncryptionLayer* m_EncryptionLayer;
	std::queue<Packet> m_OutboundPackets;
	std::map<int, std::queue<Packet>> m_RecievedPackets;

	std::atomic<bool> m_bThreadRunning;
	std::queue<Packet> m_SendBuffer;
};

