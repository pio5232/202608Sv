#pragma once
#include "Stat.h"
#include "Entity.h"

namespace jh
{
	class GameWorld;
	class PlayerStateController;

	class Player : public Entity
	{
	public:
		Player(class GameWorld* worldPtr, EntityType type, float updateInterval);
		~Player();

		virtual void Update(float delta) = 0;

		void Move(float delta);

		virtual bool IsMoving() const override;
		virtual void TakeDamage(USHORT damage) override;
		virtual USHORT GetHp() const override				{ return m_stat.GetHp(); }
		virtual bool IsDead() const override				{ return m_stat.IsDead(); }
		
		USHORT GetAttackDamage() const						{ return m_stat.GetAttackDamage(); }
		float GetAttackRange() const						{ return m_stat.GetAttackRange(); }
	protected:
		void BroadcastMoveState();
		void SendPositionUpdate();

	protected:
		Stat									m_stat;
		jh::UniquePtr<PlayerStateController>	m_pStateController;

		float									m_fPosUpdateInterval;
		class GameWorld							* m_pWorldPtr;
	private:
		Vector3									m_lastUpdatePos;

	};
}