
#define NETWORK_LAYER_API _declspec(dllexport)
#include "NetworkSystem.h"
#include"WINAPITransport.h"
#include <vector>

extern "C"
{
	static NetworkSystem* m_Manager;

	NETWORK_LAYER_API bool CreateManager(int mode, int ip, int port)
	{
		m_Manager = new NetworkSystem();
		m_Manager->Startup<WINAPITransport>(mode, ip, port);
		return true;
	}

	NETWORK_LAYER_API void RegisterOnSendData(void(*action)(int value))
	{
		if (!m_Manager)
			return;

		m_Manager->OnSendData = action;
	}

	NETWORK_LAYER_API void SendData(int ID, char* data, int size)
	{
		if (!m_Manager)
			return;

		m_Manager->SendData(ID, data, size);
	}

	NETWORK_LAYER_API bool Shutdown()
	{
		if (m_Manager)
		{
			m_Manager->ShutDown();
			delete m_Manager;
			return true;
		}

		return false;
	}
}

int main()
{
	std::vector<char> data = std::vector<char>();
	for (int x = 0; x < 1024; x++)
	{
		data.push_back('a');
	}

	NetworkSystem system;
	system.Startup<WINAPITransport>(0, 0, 0);

	system.SendData(0, data.data(), (int)data.size());

	system.ShutDown();
	return 0;
}