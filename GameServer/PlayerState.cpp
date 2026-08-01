#include "pch.h"
#include "PlayerStateBase.h"
#include "PlayerState.h"
#include "Player.h"
/// +-------------------+
/// |  PlayerIdleState	|
/// +-------------------+
/// 

jh::PlayerIdleState::PlayerIdleState() : PlayerMoveStateBase(MoveState::Idle) {}

void jh::PlayerIdleState::OnStayState(jh::Player* player, float delta)
{
}


/// +-------------------+
/// |  PlayerMoveState	|
/// +-------------------+
/// 
jh::PlayerMoveState::PlayerMoveState() : PlayerMoveStateBase(MoveState::Move) {}

void jh::PlayerMoveState::OnStayState(jh::Player* player, float delta)
{
	player->Move(delta);

}

/// +-----------------------+
/// |	PlayerAttackState	|
/// +-----------------------+
/// 
jh::PlayerAttackState::PlayerAttackState() : PlayerActionStateBase(ActionState::Attack) {}

void jh::PlayerAttackState::OnEnterState(jh::Player* player)
{
}

void jh::PlayerAttackState::OnStayState(jh::Player* player, float delta)
{
}

void jh::PlayerAttackState::OnExitState(jh::Player* player)
{
}

/// +-----------------------+
/// |  PlayerAttackedState	|
/// +-----------------------+
/// 
jh::PlayerAttackedState::PlayerAttackedState() : PlayerActionStateBase(ActionState::Attacked) {}

void jh::PlayerAttackedState::OnEnterState(jh::Player* player)
{
}

void jh::PlayerAttackedState::OnStayState(jh::Player* player, float delta)
{
}

void jh::PlayerAttackedState::OnExitState(jh::Player* player)
{
}

/// +-------------------+
/// |  PlayerDeadState	|
/// +-------------------+
jh::PlayerDeadState::PlayerDeadState() : PlayerActionStateBase(ActionState::Dead) {}

void jh::PlayerDeadState::OnEnterState(jh::Player* player)
{
}

void jh::PlayerDeadState::OnStayState(jh::Player* player, float delta)
{
}

void jh::PlayerDeadState::OnExitState(jh::Player* player)
{
}

/// +-------------------+
/// |  NoneActionState	|
/// +-------------------+}

void jh::NoneActionState::OnEnterState(jh::Player* player) {}

void jh::NoneActionState::OnStayState(jh::Player* player, float delta) {}

void jh::NoneActionState::OnExitState(jh::Player* player) {}

jh::NoneActionState::NoneActionState() : PlayerActionStateBase(ActionState::None) {}

