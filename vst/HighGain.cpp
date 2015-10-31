#include "HighGain.h"

v8f_t :: v8f_t()
{
	m_vf1 = _mm_setzero_ps();
	m_vf2 = _mm_setzero_ps();
}

v8f_t
v8f_t :: operator+(const v8f_t& rhs)
{
	v8f_t obj;
	obj.m_vf1 = _mm_add_ps(m_vf1, rhs.m_vf1);
	obj.m_vf2 = _mm_add_ps(m_vf2, rhs.m_vf2);

	return obj;
}

void
v8f_t :: operator+=(const v8f_t& rhs)
{
	m_vf1 = _mm_add_ps(m_vf1, rhs.m_vf1);
	m_vf2 = _mm_add_ps(m_vf2, rhs.m_vf2);
}

v8f_t
v8f_t :: operator*(const v8f_t& rhs)
{
	v8f_t obj;
	obj.m_vf1 = _mm_mul_ps(m_vf1, rhs.m_vf1);
	obj.m_vf2 = _mm_mul_ps(m_vf2, rhs.m_vf2);

	return obj;
}

float&
v8f_t :: operator[](const int uiIndex)
{
	if (uiIndex >= 4)
		return ((float*)&m_vf2)[uiIndex - 4];
	else
		return ((float*)&m_vf1)[uiIndex];
}