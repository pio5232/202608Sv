#pragma once

#include "Player.h"

namespace jh
{
	class GameWorld;
	class AIPlayer : public Player
	{
	public:
		AIPlayer(GameWorld* worldPtr);
		~AIPlayer();

		virtual void Update(float delta);

		void UpdateAIMovement();

	private:
		float m_fMovementUpdateInterval;
	};
}