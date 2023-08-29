#if defined(WITH_THREADS)

#include "Thread.h"

#if !defined(DEATH_TARGET_WINDOWS)

#include "../../Common.h"

#include <unistd.h> // for sysconf()
#include <sched.h> // for sched_yield()
#include <cstring>
#include <utility>

#if defined(DEATH_TARGET_APPLE)
#	include <mach/thread_act.h>
#	include <mach/thread_policy.h>
#endif

#if defined(__FreeBSD__)
#	include <pthread_np.h>
#endif

#if defined(DEATH_TARGET_SWITCH)
#	include <switch.h>
#endif

#if defined(WITH_TRACY)
#	include "common/TracySystem.hpp"
#endif

namespace nCine
{
	namespace
	{
		const unsigned int MaxThreadNameLength = 16;
	}

#if !defined(DEATH_TARGET_ANDROID) && !defined(DEATH_TARGET_EMSCRIPTEN) && !defined(DEATH_TARGET_SWITCH)

	void ThreadAffinityMask::Zero()
	{
#if defined(DEATH_TARGET_APPLE)
		affinityTag_ = THREAD_AFFINITY_TAG_NULL;
#else
		CPU_ZERO(&cpuSet_);
#endif
	}

	void ThreadAffinityMask::Set(int cpuNum)
	{
#if defined(DEATH_TARGET_APPLE)
		affinityTag_ |= 1 << cpuNum;
#else
		CPU_SET(cpuNum, &cpuSet_);
#endif
	}

	void ThreadAffinityMask::Clear(int cpuNum)
	{
#if defined(DEATH_TARGET_APPLE)
		affinityTag_ &= ~(1 << cpuNum);
#else
		CPU_CLR(cpuNum, &cpuSet_);
#endif
	}

	bool ThreadAffinityMask::IsSet(int cpuNum)
	{
#if defined(DEATH_TARGET_APPLE)
		return ((affinityTag_ >> cpuNum) & 1) != 0;
#else
		return CPU_ISSET(cpuNum, &cpuSet_) != 0;
#endif
	}

#endif

	Thread::Thread()
		: tid_(0)
	{
	}

	Thread::Thread(ThreadFunctionPtr startFunction, void* arg)
		: tid_(0)
	{
		Run(startFunction, arg);
	}

	unsigned int Thread::GetProcessorCount()
	{
#if defined(DEATH_TARGET_SWITCH)
		return svcGetCurrentProcessorNumber();
#else
		unsigned int numProcs = 0;
		long int confRet = -1;
#if defined(_SC_NPROCESSORS_ONLN)
		confRet = sysconf(_SC_NPROCESSORS_ONLN);
#elif defined(_SC_NPROC_ONLN)
		confRet = sysconf(_SC_NPROC_ONLN);
#endif

		if (confRet > 0) {
			numProcs = static_cast<unsigned int>(confRet);
		}
		return numProcs;
#endif
	}

	void Thread::Run(ThreadFunctionPtr startFunction, void* arg)
	{
		if (tid_ == 0) {
			threadInfo_.startFunction = startFunction;
			threadInfo_.threadArg = arg;
			const int error = pthread_create(&tid_, nullptr, WrapperFunction, &threadInfo_);
			FATAL_ASSERT_MSG(!error, "Error in pthread_create(): %d", error);
		} else {
			LOGW("Thread %u is already running", tid_);
		}
	}

	void* Thread::Join()
	{
		void* pRetVal = nullptr;
		if (tid_ != 0) {
			if (pthread_join(tid_, &pRetVal) == 0) {
				tid_ = 0;
			}
		}
		return pRetVal;
	}

#if !defined(DEATH_TARGET_EMSCRIPTEN) && !defined(DEATH_TARGET_SWITCH)
#if !defined(DEATH_TARGET_APPLE)
	void Thread::SetName(const char* name)
	{
		if (tid_ == 0) return;

		const auto nameLength = strnlen(name, MaxThreadNameLength);
		if (nameLength <= MaxThreadNameLength - 1) {
			pthread_setname_np(tid_, name);
		} else {
			char buffer[MaxThreadNameLength];
			memcpy(buffer, name, MaxThreadNameLength - 1);
			buffer[MaxThreadNameLength - 1] = '\0';
			pthread_setname_np(tid_, name);
		}
	}
#endif

	void Thread::SetSelfName(const char* name)
	{
#if defined(WITH_TRACY)
		tracy::SetThreadName(name);
#else
		const auto nameLength = strnlen(name, MaxThreadNameLength);
		if (nameLength <= MaxThreadNameLength - 1) {
#if !defined(DEATH_TARGET_APPLE)
			pthread_setname_np(pthread_self(), name);
#else
			pthread_setname_np(name);
#endif
		} else {
			char buffer[MaxThreadNameLength];
			memcpy(buffer, name, MaxThreadNameLength - 1);
			buffer[MaxThreadNameLength - 1] = '\0';
#if !defined(DEATH_TARGET_APPLE)
			pthread_setname_np(pthread_self(), name);
#else
			pthread_setname_np(name);
#endif
		}
#endif
	}
#endif

#if !defined(DEATH_TARGET_SWITCH)
	int Thread::GetPriority() const
	{
		if (tid_ == 0) return 0;

		int policy;
		struct sched_param param;
		pthread_getschedparam(tid_, &policy, &param);
		return param.sched_priority;
	}

	void Thread::SetPriority(int priority)
	{
		if (tid_ != 0) {
			int policy;
			struct sched_param param;
			pthread_getschedparam(tid_, &policy, &param);

			param.sched_priority = priority;
			pthread_setschedparam(tid_, policy, &param);
		}
	}
#endif

	long int Thread::Self()
	{
#if defined(DEATH_TARGET_APPLE) || defined(DEATH_TARGET_SWITCH) || defined(__FreeBSD__)
		return reinterpret_cast<long int>(pthread_self());
#else
		return static_cast<long int>(pthread_self());
#endif
	}

	[[noreturn]] void Thread::Exit(void* retVal)
	{
		pthread_exit(retVal);
	}

	void Thread::YieldExecution()
	{
		sched_yield();
	}

#if !defined(DEATH_TARGET_ANDROID)
	void Thread::Abort()
	{
		if (pthread_cancel(tid_) == 0) {
			tid_ = 0;
		}
	}

#if !defined(DEATH_TARGET_EMSCRIPTEN) && !defined(DEATH_TARGET_SWITCH)
	ThreadAffinityMask Thread::GetAffinityMask() const
	{
		ThreadAffinityMask affinityMask;

		if (tid_ != 0) {
#if defined(DEATH_TARGET_APPLE)
			thread_affinity_policy_data_t threadAffinityPolicy;
			thread_port_t threadPort = pthread_mach_thread_np(tid_);
			mach_msg_type_number_t policyCount = THREAD_AFFINITY_POLICY_COUNT;
			boolean_t getDefault = FALSE;
			thread_policy_get(threadPort, THREAD_AFFINITY_POLICY, reinterpret_cast<thread_policy_t>(&threadAffinityPolicy), &policyCount, &getDefault);
			affinityMask.affinityTag_ = threadAffinityPolicy.affinity_tag;
#else
			pthread_getaffinity_np(tid_, sizeof(cpu_set_t), &affinityMask.cpuSet_);
#endif
		} else {
			LOGW("Cannot get the affinity for a thread that has not been created yet");
		}

		return affinityMask;
	}

	void Thread::SetAffinityMask(ThreadAffinityMask affinityMask)
	{
		if (tid_ != 0) {
#if defined(DEATH_TARGET_APPLE)
			thread_affinity_policy_data_t threadAffinityPolicy = { affinityMask.affinityTag_ };
			thread_port_t threadPort = pthread_mach_thread_np(tid_);
			thread_policy_set(threadPort, THREAD_AFFINITY_POLICY, reinterpret_cast<thread_policy_t>(&threadAffinityPolicy), THREAD_AFFINITY_POLICY_COUNT);
#else
			pthread_setaffinity_np(tid_, sizeof(cpu_set_t), &affinityMask.cpuSet_);
#endif
		} else {
			LOGW("Cannot set the affinity mask for a not yet created thread");
		}
	}
#endif
#endif

	void* Thread::WrapperFunction(void* arg)
	{
		const ThreadInfo* pThreadInfo = static_cast<ThreadInfo*>(arg);
		pThreadInfo->startFunction(pThreadInfo->threadArg);

		return nullptr;
	}
}

#endif

#endif