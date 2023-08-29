#if defined(WITH_THREADS)

#include "ThreadPool.h"
#include "../../Common.h"

namespace nCine
{
	ThreadPool::ThreadPool()
		: ThreadPool(Thread::GetProcessorCount())
	{
	}

	ThreadPool::ThreadPool(unsigned int numThreads)
		: threads_(numThreads), numThreads_(numThreads)
	{
		threadStruct_.queue = &queue_;
		threadStruct_.queueMutex = &queueMutex_;
		threadStruct_.queueCV = &queueCV_;
		threadStruct_.shouldQuit = false;

		quitMutex_.Lock();

		for (unsigned int i = 0; i < numThreads_; i++) {
			threads_.emplace_back(WorkerFunction, &threadStruct_);
#if !defined(DEATH_TARGET_EMSCRIPTEN) && !defined(DEATH_TARGET_ANDROID) && !defined(DEATH_TARGET_SWITCH)
			threads_.back().SetAffinityMask(ThreadAffinityMask(i));
#endif
		}
	}

	ThreadPool::~ThreadPool()
	{
		threadStruct_.shouldQuit = true;
		queueCV_.Broadcast();

		for (unsigned int i = 0; i < numThreads_; i++) {
			threads_[i].Join();
		}
	}

	void ThreadPool::EnqueueCommand(std::unique_ptr<IThreadCommand> threadCommand)
	{
		ASSERT(threadCommand);

		queueMutex_.Lock();
		queue_.push_back(std::move(threadCommand));
		queueCV_.Broadcast();
		queueMutex_.Unlock();
	}

	void ThreadPool::WorkerFunction(void* arg)
	{
		ThreadStruct* threadStruct = static_cast<ThreadStruct*>(arg);

		LOGD("Worker thread %u is starting", Thread::Self());

		while (true) {
			threadStruct->queueMutex->Lock();
			while (threadStruct->queue->empty() && !threadStruct->shouldQuit) {
				threadStruct->queueCV->Wait(*(threadStruct->queueMutex));
			}

			if (threadStruct->shouldQuit) {
				threadStruct->queueMutex->Unlock();
				break;
			}

			std::unique_ptr<IThreadCommand> threadCommand = std::move(threadStruct->queue->front());
			threadStruct->queue->pop_front();
			threadStruct->queueMutex->Unlock();

			LOGD("Worker thread %u is executing its command", Thread::Self());
			threadCommand->Execute();
		}

		LOGD("Worker thread %u is exiting", Thread::Self());
	}

}

#endif