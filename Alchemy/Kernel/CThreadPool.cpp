//	CThreadPool.cpp
//
//	CThreadPool class
//	Copyright (c) 2015 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

void CThreadPool::AddTask (IThreadPoolTask *pTask)

//	AddTask
//
//	Adds a task to the thread pool. We take ownership of the object.
//
//	NOTE: This should only be called from the same thread as Run. That is, we 
//	should not call this method while the worker threads are running.

	{
	m_Tasks.Enqueue(pTask);
	m_iTasksRemaining++;
	}

bool CThreadPool::Boot (int iThreadCount)

//	Boot
//
//	Start the thread pool.

	{
	int i;

	ASSERT(iThreadCount > 0);

	//	Create all our events

	m_WorkAvail.Create();

	m_WorkCompleted.Create();
	m_WorkCompleted.Set();

	m_Quit.Create();

	//	Start all the threads

	m_Threads.InsertEmpty(iThreadCount - 1);
	for (i = 0; i < m_Threads.GetCount(); i++)
		m_Threads[i].hThread = ::kernelCreateThread(WorkerThreadStub, this);

	m_iTasksRemaining = 0;

	//	Done

	return true;
	}

void CThreadPool::CleanUp (void)

//	CleanUp
//
//	Free up resources. We assume that this is only called from the same thread
//	that calls Run.

	{
	int i;

	//	Ask all the threads to quit

	m_Quit.Set();

	//	Wait for all

	if (m_Threads.GetCount() > 0)
		{
		HANDLE *pThreads = new HANDLE [m_Threads.GetCount()];
		for (i = 0; i < m_Threads.GetCount(); i++)
			pThreads[i] = m_Threads[i].hThread;

		::WaitForMultipleObjects(m_Threads.GetCount(), pThreads, TRUE, 5000);

		delete [] pThreads;

		//	Close all the handles

		for (i = 0; i < m_Threads.GetCount(); i++)
			::CloseHandle(m_Threads[i].hThread);

		m_Threads.DeleteAll();
		}

	//	Free up any tasks

	for (i = 0; i < m_Tasks.GetCount(); i++)
		delete m_Tasks[i];

	m_Tasks.DeleteAll();

	for (i = 0; i < m_Completed.GetCount(); i++)
		delete m_Completed[i];

	m_Completed.DeleteAll();
	m_iTasksRemaining = 0;
	}

IThreadPoolTask *CThreadPool::GetTaskToRun (void)

//	GetTaskToRun
//
//	Pull task off the queueu

	{
	IThreadPoolTask *pTask;

	m_cs.Lock();

	if (m_Tasks.GetCount() > 0)
		{
		pTask = m_Tasks.Head();
		m_Tasks.Dequeue();
		if (m_Tasks.GetCount() == 0)
			m_WorkAvail.Reset();
		}
	else
		pTask = NULL;

	m_cs.Unlock();

	return pTask;
	}

void CThreadPool::Run (void)

//	Run
//
//	Runs until all the tasks are completed. This should only ever be run from
//	a single thread.

	{
	int i;
	IThreadPoolTask *pTask = NULL;

	//	If we have no threads then we just run all the tasks

	if (m_Threads.GetCount() == 0)
		{
		while (m_Tasks.GetCount() > 0)
			{
			pTask = m_Tasks.Head();
			m_Tasks.Dequeue();

			RunTask(pTask);
			}
		}

	//	Otherwise, we let the threads do stuff

	else
		{
		bool bWorkLeft = false;

		//	Signal the threads to start

		m_cs.Lock();
		if (m_Tasks.GetCount() == 0)
			{
			m_cs.Unlock();
			return;
			}

		//	Pull a task for us to run

		pTask = m_Tasks.Head();
		m_Tasks.Dequeue();
		bWorkLeft = (m_Tasks.GetCount() > 0);
		m_WorkCompleted.Reset();

		m_cs.Unlock();

		//	Let the other threads go

		if (bWorkLeft)
			m_WorkAvail.Set();

		//	Do our own task

		if (pTask)
			RunTask(pTask);

		//	Wait until we're done with all tasks

		::WaitForSingleObject(m_WorkCompleted.GetWaitObject(), INFINITE);
		}

	//	Clean up all tasks

	for (i = 0; i < m_Completed.GetCount(); i++)
		delete m_Completed[i];

	m_Completed.DeleteAll();
	}

void CThreadPool::RunTask (IThreadPoolTask *pTask)

//	RunTask
//
//	Get a task off the queue and run it.

	{
	//	Do the task

	try
		{
		pTask->Run();
		}
	catch (...)
		{
		}

	//	Done with task

	m_cs.Lock();
	m_Completed.Insert(pTask);
	m_iTasksRemaining--;
	if (m_iTasksRemaining <= 0)
		m_WorkCompleted.Set();
	m_cs.Unlock();
	}

void CThreadPool::WorkerThread (void)

//	WorkerThread
//
//	Do the work.

	{
	HANDLE Events[2];
	Events[0] = m_WorkAvail.GetWaitObject();
	Events[1] = m_Quit.GetWaitObject();

	while (true)
		{
		DWORD dwWait = ::WaitForMultipleObjects(2, Events, FALSE, INFINITE);

		if (dwWait == WAIT_OBJECT_0 + 0)
			{
			IThreadPoolTask *pTask = GetTaskToRun();
			if (pTask)
				RunTask(pTask);
			}

		else if (dwWait == WAIT_OBJECT_0 + 1)
			return;
		}
	}
