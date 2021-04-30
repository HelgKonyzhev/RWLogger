#pragma once
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <queue>
#include <thread>

/**
 * Implementation of mutex with guaranteed FIFO unlock order. */
class FIFOMutex
{
public:
	FIFOMutex();
	~FIFOMutex();

	void lock();
	void unlock();
	bool try_lock();

private:
	std::mutex m_mtx;
	std::condition_variable m_cv;
	std::atomic_bool m_locked{ false };
	std::queue<std::thread::id> m_pendingThreads;
};
