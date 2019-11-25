#include "NetworkSystem.h"
#include <type_traits>

NetworkSystem::NetworkSystem()
{
	m_EncryptionLayer = nullptr;
	m_MaxPacketSize = 1024;
	m_PacketSize = 0;
	m_bThreadRunning = false;
}

NetworkSystem::~NetworkSystem()
{
	if (m_EncryptionLayer)
		delete m_EncryptionLayer;
}

void NetworkSystem::ShutDown()
{
	m_bThreadRunning = false;
}

void NetworkSystem::SendData(int ID, char* data, int size)
{
	Packet p;
	p.ID = ID;
	p.DataPtr = new char[size];
	p.Size = size;

	memcpy(p.DataPtr, data, size);

	DepositPacket(p);
}

void NetworkSystem::SetEncryptionLayer(EncryptionLayer* layer)
{
	if (!layer)
		return;

	m_EncryptionLayer = layer;
}

void NetworkSystem::DepositPacket(const Packet& p)
{
	m_OutboundPackets.push(p);
	m_PacketSize += p.Size;

	if (m_PacketSize >= m_MaxPacketSize)
	{
		m_PacketSize = 0;
		GenerateSendPacket();
	}
}

void NetworkSystem::AddRecievedPacket(const Packet& p)
{
	std::lock_guard<std::mutex> lock(m_RecvMutex);

	auto found = m_RecievedPackets.find(p.ID);
	if (found == m_RecievedPackets.end())
	{
		m_RecievedPackets[p.ID] = std::queue<Packet>();
	}

	m_RecievedPackets[p.ID].push(p);
}

void NetworkSystem::GenerateSendPacket()
{
	Packet newPacket;
	std::vector<char> bytes = std::vector<char>();
	char* header = new char[sizeof(int) * 2];
	while (!m_OutboundPackets.empty())
	{
		const Packet& p = m_OutboundPackets.front();

		memcpy(header, &p.ID, sizeof(int));
		memcpy(header + sizeof(int), &p.Size, sizeof(int));

		auto data = std::vector<char>(p.DataPtr, p.DataPtr + p.Size);

		bytes.insert(bytes.end(), header, header + sizeof(int) * 2);
		bytes.insert(bytes.end(), data.begin(), data.end());

		m_OutboundPackets.pop();
	}

	delete[] header;
	newPacket.Size = (int)bytes.size();
	newPacket.DataPtr = new char[newPacket.Size];
	memcpy(newPacket.DataPtr, bytes.data(), bytes.size());

	if (m_EncryptionLayer)
		m_EncryptionLayer->EncryptPacket(newPacket);

	std::lock_guard<std::mutex> lock(m_sendMutex);
	m_SendBuffer.push(newPacket);
}

Packet* NetworkSystem::GetOutgoingPacket()
{
	std::lock_guard<std::mutex> lock(m_sendMutex);
	if (!m_SendBuffer.empty())
	{
		Packet* result = new Packet(m_SendBuffer.front());
		m_SendBuffer.pop();
		return result;
	}
	return nullptr;
}

void NetworkSystem::UnpackIncomingPacket(const Packet& p)
{
	if (m_EncryptionLayer)
		m_EncryptionLayer->DecryptPacket(p);

	int offset = 0;
	while (p.Size > 0)
	{
		Packet extracted;
		memcpy(&extracted.ID, p.DataPtr + offset, sizeof(int));
		offset += sizeof(int);

		memcpy(&extracted.Size, p.DataPtr + offset, sizeof(int));
		offset += sizeof(int);

		extracted.DataPtr = new char[extracted.Size];
		memcpy(extracted.DataPtr, p.DataPtr + offset, extracted.Size);
		offset += extracted.Size;

		AddRecievedPacket(extracted);
	}
}