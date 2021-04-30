#include "FIFOMutex.h"

FIFOMutex::FIFOMutex() = default;

FIFOMutex::~FIFOMutex() = default;

void FIFOMutex::lock()
{
	std::unique_lock<std::mutex> lck(m_mtx);
	if(!m_locked && m_pendingThreads.empty())
	{
		m_locked = true;
		return;
	}

	const auto threadId = std::this_thread::get_id();
	m_pendingThreads.push(threadId);
	while(m_locked || m_pendingThreads.front() != threadId)
		m_cv.wait(lck);

	m_pendingThreads.pop();
	m_locked = true;
}

void FIFOMutex::unlock()
{
	std::unique_lock<std::mutex> lck(m_mtx);
	m_locked = false;
	m_cv.notify_all();
}

bool FIFOMutex::try_lock()
{
	std::unique_lock<std::mutex> lck(m_mtx);
	if(!m_locked && m_pendingThreads.empty())
	{
		m_locked = true;
		return true;
	}
	return false;
}
