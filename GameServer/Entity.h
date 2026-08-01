#pragma once
#include <iostream>
#include "Transform.h"
#include "Sector.h"
namespace jh
{
	class Entity
	{
	public:
		enum class EntityType : byte
		{
			GamePlayer = 0,
			AIPlayer,
		};
	public:
		Entity(EntityType type);
		Entity(EntityType type, const Vector3& startPos);
		~Entity();

		virtual void Update(float delta) = 0;
		
		virtual void TakeDamage(USHORT damage) = 0;
		virtual bool IsDead() const = 0;
		virtual USHORT GetHp() const = 0;
		virtual bool IsMoving() const				{ return false; }

		const Vector3& GetPosition() const			{ return m_transform.GetPosConst(); }
		const Vector3& GetRotation() const			{ return m_transform.GetRotConst(); }
		const Vector3& GetNormalizedForward() const { return m_transform.GetNormalizedDir(); }

		ULONGLONG GetEntityId() const				{ return m_ullEntityId; }
		EntityType GetType() const					{ return m_entityType; }

		bool IsSectorUpdated();
		const Sector& GetCurrentSector() const		{ return m_curSector; }
		const Sector& GetPrevSector() const			{ return m_prevSector; }

	private:
		ULONGLONG				m_ullEntityId;
		const EntityType		m_entityType;
	protected:
		Transform		m_transform;
		Sector					m_curSector;
		Sector					m_prevSector;
	};
}