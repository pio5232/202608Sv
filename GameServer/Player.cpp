#include "pch.h"
#include "Player.h"
#include "UserManager.h"
#include "PlayerStateController.h"
#include "PacketBuilder.h"
#include "GameWorld.h"
#include "PlayerState.h"
using namespace jh;
jh::Player::Player(GameWorld* worldPtr, EntityType type, float updateInterval) : Entity(type), m_pWorldPtr(worldPtr), m_lastUpdatePos{}, m_fPosUpdateInterval(updateInterval), m_stat()
{
	m_pStateController = jh::MakeUnique<PlayerStateController>(this);

	const Vector3 pos = GetPosition();
}

jh::Player::~Player()
{

}

void jh::Player::Update(float delta)
{

	m_pStateController->Update(delta);
}

void jh::Player::Move(float delta)
{
	m_fPosUpdateInterval -= delta;

	if (m_fPosUpdateInterval <= 0)
	{
		m_fPosUpdateInterval = posUpdateInterval;

		SendPositionUpdate();
	}

	m_transform.Move(delta);
}

bool jh::Player::IsMoving() const
{
	return m_pStateController->GetMoveType() == jh::PlayerMoveStateBase::MoveState::Move;
}

void jh::Player::TakeDamage(USHORT damage)
{
	m_stat.TakeDamage(damage);

	m_pStateController->ChangeState(&jh::PlayerAttackedState::GetInstance());
}


void jh::Player::BroadcastMoveState()
{
	if (m_pStateController->GetMoveType() == jh::PlayerMoveStateBase::MoveState::Move)
	{
		PacketBufferRef buffer = jh::PacketBuilder::BuildMoveStartNotifyPacket(GetEntityId(), m_transform.GetPosConst(), m_transform.GetRotConst().y);

		m_pWorldPtr->SendPacketAroundSectorNSpectators(GetCurrentSector(), buffer);
	}
	else if (m_pStateController->GetMoveType() == jh::PlayerMoveStateBase::MoveState::Idle)
	{
		PacketBufferRef buffer = jh::PacketBuilder::BuildMoveStopNotifyPacket(GetEntityId(), m_transform.GetPosConst(), m_transform.GetRotConst().y);

		m_pWorldPtr->SendPacketAroundSectorNSpectators(GetCurrentSector(), buffer);
	}
}
void jh::Player::SendPositionUpdate()
{
	Vector3 currentPos = m_transform.GetPosConst();
	
	if ((currentPos - m_lastUpdatePos).sqrMagnitude() < 0.01f)
		return;

	PacketBufferRef buffer = jh::PacketBuilder::BuildUpdateTransformPacket(jh_utility::GetTimeStamp(), GetEntityId(), currentPos, m_transform.GetRotConst());

	m_pWorldPtr->SendPacketAroundSectorNSpectators(GetCurrentSector(), buffer);

	m_lastUpdatePos = currentPos;
}
