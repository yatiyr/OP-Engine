#pragma once

#include <JobSystem/ThreadSafeQueue.h>


#include <functional>
#include <thread>
#include <condition_variable>
#include <atomic>

namespace OP
{

	struct JobDispatchArgs
	{
		uint32_t jobIndex;
		uint32_t groupIndex;
	};

	struct JobTracker
	{

		JobTracker(uint32_t val = 1) : isFinished(val) { }

		bool IsFinished()
		{
			return (isFinished == 0);
		}
		uint32_t isFinished;

		friend class JobSystem;
	};

	class JobSystem
	{
	public:

		static void Initialize();
		static void Execute(const std::function<void()>& job, JobTracker& tracker = JobTracker());

		// static JobTracker ExecuteAsync(const std::function<void()>& job);

		// static void Dispatch(uint32_t jobCount, uint32_t groupSize, const std::function<void(JobDispatchArgs)>& job);

		static bool IsBusy();

		static void Wait();
	private:
		static inline void Poll();
	private:


	};
}