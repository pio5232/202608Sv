#pragma once

#include "PlayerStateBase.h"

namespace jh
{
	class Player;

	/// +-----------------------+
	///	| PlayerStateController |
	/// +-----------------------+
	class PlayerStateController
	{
	public:
		PlayerStateController(Player* player);

		void ChangeState(PlayerMoveStateBase* to);
		void ChangeState(PlayerActionStateBase* to);

		void Update(float delta);
		PlayerMoveStateBase::MoveState GetMoveType();
		PlayerActionStateBase::ActionState GetActionType();
	private:
		// 동적할당하지않는 포인터.
		jh::Player*					m_pPlayer;

		jh::PlayerActionStateBase*	m_pActionStateBasePtr;
		jh::PlayerMoveStateBase*	m_pMoveStateBasePtr;

		float								m_fActionElapsedTime;
	};
}

