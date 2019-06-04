#ifndef __LEAF_THREADEX_H__
#define __LEAF_THREADEX_H__

typedef enum {
	PRIORITY_IDLE = -1,
	PRIORITY_NORMAL = 0,
	PRIORITY_ABOVE_NORMAL = 1,
	PRIORITY_HIGH = 2,
}THREAD_PRIORITY_T;

namespace Leaf
{
	namespace System
	{
		class CThreadEx
		{
		public:
			CThreadEx();
			~CThreadEx();
			
		public:
			bool			CreateThread();
			virtual bool	CloseThread();
			
			virtual int		Run() = 0;
			bool			IsRunning();
			
			unsigned int	ResumeThread();
			unsigned int	SuspendThread();
			
			bool			SetThreadPriority(int nPriority);
			int				GetThreadPriority();			
			
		protected:
			static void*	_ThreadEntry(LPVOID pParam);
			
		protected:
			void*			m_StartEvent;
			void*			m_EndEvent;
			
		private:
			mutable volatile bool m_bExit;
			void* 			m_thread_t;
		};
	}
}

#endif