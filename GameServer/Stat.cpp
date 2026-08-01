#include "pch.h"
#include "Stat.h"

Stat::Stat() : m_usCurHp(defaultMaxHp), m_usMaxHp(defaultMaxHp), m_fAttackRange(defaultAttackRange), m_usAttackDamage(defaultAttackDamage)
{

}

Stat::~Stat()
{
}

void Stat::TakeDamage(USHORT damage)
{
	if (m_usCurHp <= damage)
		m_usCurHp = 0;
	else
		m_usCurHp -= damage;
}
