#pragma once

#include <cstdint>
#include <mutex>

namespace OP
{
	template<typename T, uint32_t Cap>
	class ThreadSafeQueue
	{
	public:
		inline bool Push(T& element)
		{
			bool result = false;
			m_Lock.lock();
			uint32_t next = (m_Head + 1) % Cap;
			if (next != m_Tail)
			{
				m_Data[m_Head] = element;
				m_Head = next;
				result = true;
			}
			m_Lock.unlock();
			return result;
		}

		inline bool Pop(T& element)
		{
			bool result = false;
			m_Lock.lock();
			if (m_Tail != m_Head)
			{
				element = m_Data[m_Tail];
				m_Tail = (m_Tail + 1) % Cap;
				result = true;
			}
			m_Lock.unlock();
			return result;
		}

	private:
		T m_Data[Cap];
		uint32_t m_Head = 0;
		uint32_t m_Tail = 0;
		std::mutex m_Lock;
	};
}