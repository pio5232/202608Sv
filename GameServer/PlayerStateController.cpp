#include "pch.h"
#include "Player.h"
#include "PlayerStateController.h"
#include "PlayerState.h"
/// +-----------------------+
///	| PlayerStateController |
/// +-----------------------+
/// 
jh::PlayerStateController::PlayerStateController(jh::Player* player) : m_pPlayer(player), m_pActionStateBasePtr(&NoneActionState::GetInstance()),
m_pMoveStateBasePtr(nullptr), m_fActionElapsedTime(0)
{
}

void jh::PlayerStateController::ChangeState(PlayerMoveStateBase* to)
{
	if (to == nullptr || to == m_pMoveStateBasePtr)
		return;

	m_pMoveStateBasePtr = to;
}

void jh::PlayerStateController::ChangeState(PlayerActionStateBase* to)
{
	if (to == nullptr || to == m_pActionStateBasePtr)
		return;

	m_fActionElapsedTime = 0;

	m_pActionStateBasePtr->OnExitState(m_pPlayer);
	m_pActionStateBasePtr = to;
	m_pActionStateBasePtr->OnEnterState(m_pPlayer);
}

void jh::PlayerStateController::Update(float delta)
{
	if (m_pMoveStateBasePtr != nullptr)
		m_pMoveStateBasePtr->OnStayState(m_pPlayer, delta);

	if (m_pActionStateBasePtr != nullptr)
	{
		if (m_pActionStateBasePtr->GetType() == jh::PlayerActionStateBase::ActionState::None)
			return;

		m_pActionStateBasePtr->OnStayState(m_pPlayer, delta);


		m_fActionElapsedTime += delta;

		if (m_fActionElapsedTime >= m_pActionStateBasePtr->GetDuration())
			ChangeState(&jh::NoneActionState::GetInstance());
	}

}

jh::PlayerMoveStateBase::MoveState jh::PlayerStateController::GetMoveType()
{
	if (m_pMoveStateBasePtr == nullptr)
		return jh::PlayerMoveStateBase::MoveState::None;

	return m_pMoveStateBasePtr->GetType();
}

jh::PlayerActionStateBase::ActionState jh::PlayerStateController::GetActionType()
{
	if (m_pActionStateBasePtr == nullptr)
		return PlayerActionStateBase::ActionState::None;

	return m_pActionStateBasePtr->GetType();
}
