#pragma once
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <queue>
#include <thread>

namespace RWLogger
{
	/**
	 * Implementation of mutex with guaranteed FIFO unlock order. */
	class FIFOMutex
	{
	public:
		FIFOMutex();
		~FIFOMutex();

		/**
		 * Locks the mutex, blocks if the mutex is not available.
		 * The called thread is placed in the queue according to which it will be unblocked. */
		void lock();

		/**
		 * Unlocks the mutex. */
		void unlock();

		/**
		 * Tries to lock the mutex, returns if the mutex is not available. */
		bool try_lock();

	private:
		std::mutex m_mtx;
		std::condition_variable m_cv;
		std::atomic_bool m_locked{ false };
		std::queue<std::thread::id> m_pendingThreads;
	};
}
