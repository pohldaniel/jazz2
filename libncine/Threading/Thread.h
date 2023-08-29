#pragma once

#include <CommonWindows.h>

#if defined(DEATH_TARGET_WINDOWS)
#	include <process.h>
#	include <processthreadsapi.h>
#else
#	include <pthread.h>
#endif

#if defined(DEATH_TARGET_APPLE)
#	include <mach/mach_init.h>
#endif

namespace nCine
{
#if !defined(DEATH_TARGET_ANDROID) && !defined(DEATH_TARGET_EMSCRIPTEN) && !defined(DEATH_TARGET_SWITCH)

	/// A class representing the CPU affinity mask for a thread
	class ThreadAffinityMask
	{
	public:
		ThreadAffinityMask()
		{
			Zero();
		}

		ThreadAffinityMask(int cpuNum)
		{
			Zero();
			Set(cpuNum);
		}

		/// Clears the CPU set
		void Zero();
		/// Sets the specified CPU number to be included in the set
		void Set(int cpuNum);
		/// Sets the specified CPU number to be excluded by the set
		void Clear(int cpuNum);
		/// Returns true if the specified CPU number belongs to the set
		bool IsSet(int cpuNum);

	private:
#	if defined(DEATH_TARGET_WINDOWS)
		DWORD_PTR affinityMask_;
#	elif defined(DEATH_TARGET_APPLE)
		integer_t affinityTag_;
#	else
		cpu_set_t cpuSet_;
#	endif

		friend class Thread;
	};

#endif

	/// Thread class
	class Thread
	{
	public:
		using ThreadFunctionPtr = void (*)(void*);

		/// A default constructor for an object without the associated function
		Thread();
		/// Creates a thread around a function and runs it
		Thread(ThreadFunctionPtr startFunction, void* arg);

		/// Returns the number of processors in the machine
		static unsigned int GetProcessorCount();

		/// Spawns a new thread if the object hasn't one already associated
		void Run(ThreadFunctionPtr startFunction, void* arg);
		/// Joins the thread
		void* Join();

#if !defined(DEATH_TARGET_EMSCRIPTEN) && !defined(DEATH_TARGET_SWITCH)
#	if !defined(DEATH_TARGET_APPLE)
		/// Sets the thread name
		void SetName(const char* name);
#	endif

		/// Sets the calling thread name
		static void SetSelfName(const char* name);
#endif

#if !defined(DEATH_TARGET_SWITCH)
		/// Gets the thread priority
		int GetPriority() const;
		/// Sets the thread priority
		void SetPriority(int priority);
#endif

		/// Returns the calling thread id
		static long int Self();
		/// Terminates the calling thread
		[[noreturn]] static void Exit(void* retVal);
		/// Yields the calling thread in favour of another one with the same priority
		static void YieldExecution();

#if !defined(DEATH_TARGET_ANDROID)
		/// Asks the thread for termination
		void Abort();

#	if !defined(DEATH_TARGET_EMSCRIPTEN) && !defined(DEATH_TARGET_SWITCH)
		/// Gets the thread affinity mask
		ThreadAffinityMask GetAffinityMask() const;
		/// Sets the thread affinity mask
		void SetAffinityMask(ThreadAffinityMask affinityMask);
#	endif
#endif

	private:
		/// The structure wrapping the information for thread creation
		struct ThreadInfo
		{
			ThreadInfo()
				: startFunction(nullptr), threadArg(nullptr) {}
			ThreadFunctionPtr startFunction;
			void* threadArg;
		};

#if defined(DEATH_TARGET_WINDOWS)
		HANDLE handle_;
#else
		pthread_t tid_;
#endif

		ThreadInfo threadInfo_;
		/// The wrapper start function for thread creation
#if defined(DEATH_TARGET_WINDOWS)
#	if defined(DEATH_TARGET_MINGW)
		static unsigned int(__attribute__((__stdcall__)) WrapperFunction)(void* arg);
#	else
		static unsigned int __stdcall WrapperFunction(void* arg);
#	endif
#else
		static void* WrapperFunction(void* arg);
#endif
	};
}