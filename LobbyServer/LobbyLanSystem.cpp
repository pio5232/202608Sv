#include "pch.h"
#include "LobbyLanSystem.h"
#include "LobbySystem.h"
#include "Memory.h"


jh::LobbyLanSystem::LobbyLanSystem(jh::IocpServer* owner) : m_pOwner{ owner }, m_pLobbySystem{ nullptr } {}

jh::LobbyLanSystem::~LobbyLanSystem()
{

}

void jh::LobbyLanSystem::ProcessPacket(ULONGLONG sessionId, USHORT packetType, PacketBufferRef& packet)
{
	if (m_packetFuncDic.find(packetType) == m_packetFuncDic.end())
		return;

	(this->*m_packetFuncDic[packetType])(sessionId, packet);
}


void jh::LobbyLanSystem::Init()
{
	m_packetFuncDic.clear();

	m_packetFuncDic[jh::GAME_SERVER_LAN_INFO_PACKET] = &LobbyLanSystem::HandleLanInfoNotifyPacket; // ip Port
	m_packetFuncDic[jh::GAME_SERVER_SETTING_REQUEST_PACKET] = &LobbyLanSystem::HandleGameSettingRequestPacket; // completePacket
}

void jh::LobbyLanSystem::HandleLanInfoNotifyPacket(ULONGLONG sessionId, PacketBufferRef& packet)
{
	m_pLobbySystem->HandleLanInfoNotify(sessionId, packet, m_pOwner);
}

void jh::LobbyLanSystem::HandleGameSettingRequestPacket(ULONGLONG sessionId, PacketBufferRef& packet)
{
	m_pLobbySystem->HandleGameSettingRequest(sessionId, packet, m_pOwner);
}
