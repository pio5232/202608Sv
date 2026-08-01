#include "pch.h"
#include "Entity.h"
#include <random>
jh::Entity::Entity(EntityType type) : m_entityType(type), m_transform()
{
	static std::mt19937_64 gen(1);
	static ULONGLONG generator = gen();

	m_ullEntityId = InterlockedIncrement64((LONGLONG*)&generator);

	_LOG(L"Entity", LOG_LEVEL_DEBUG, L"[Entity(type)] My ID : [%llu]", m_ullEntityId);

	const Vector3& position = m_transform.GetPosConst();
	m_curSector.m_iX = position.x / sectorCriteriaSize + 1;
	m_curSector.m_iZ = position.z / sectorCriteriaSize + 1;

	m_prevSector = m_curSector;
}

jh::Entity::Entity(EntityType type, const Vector3& startPos) : m_entityType(type), m_transform(startPos)
{
	static std::mt19937_64 gen(1);
	static ULONGLONG generator = gen();

	m_ullEntityId = InterlockedIncrement64((LONGLONG*)&generator);

	_LOG(L"Entity", LOG_LEVEL_DEBUG, L"[Entity(type, pos)] My ID : [%llu]", m_ullEntityId);

	const Vector3& position = m_transform.GetPosConst();
	m_curSector.m_iX = position.x / sectorCriteriaSize + 1;
	m_curSector.m_iZ = position.z / sectorCriteriaSize + 1;

	m_prevSector = m_curSector;
}

jh::Entity::~Entity()
{
}

bool jh::Entity::IsSectorUpdated()
{
	m_prevSector = m_curSector;

	const Vector3& position = m_transform.GetPosConst();

	m_curSector.m_iX = position.x / sectorCriteriaSize + 1;
	m_curSector.m_iZ = position.z / sectorCriteriaSize + 1;

	return m_prevSector != m_curSector;
}
