#include "pch.h"
#include <random>
#include "AIPlayer.h"
#include "UserManager.h"
#include "PlayerStateController.h"
#include "PlayerState.h"
#include "GameWorld.h"
jh::AIPlayer::AIPlayer(GameWorld* worldPtr) : Player(worldPtr, EntityType::AIPlayer, posUpdateInterval), m_fMovementUpdateInterval(0)
{
}

jh::AIPlayer::~AIPlayer()
{
}

void jh::AIPlayer::Update(float delta)
{
	if (IsDead())
		return;

	m_fMovementUpdateInterval -= delta;

	if (m_fMovementUpdateInterval <= 0)
	{
		m_fMovementUpdateInterval = static_cast<float>(GetRandDouble(1.0, 6.0, 3));

		UpdateAIMovement();
	}

	Player::Update(delta);
}

void jh::AIPlayer::UpdateAIMovement()
{
	if (CheckChance(70))
	{
		m_transform.SetRandomDirection();
		m_pStateController->ChangeState(&jh::PlayerMoveState::GetInstance());
	}
	else
	{
		m_pStateController->ChangeState(&jh::PlayerIdleState::GetInstance());
	}

	BroadcastMoveState();
}

