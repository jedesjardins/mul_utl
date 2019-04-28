

#include <atomic>

namespace utl
{

template <typename T, size_t N>
class mpsc_s_queue
{
public:
	void push(T const& element)
	{
		data[(++tail) % N] = element;
	}

	T* peek()
	{
		if (tail < head)
		{
			return nullptr;
		}

		return &data[head%N];
	}

	void pop()
	{
		size_t old_tail = tail;
		size_t old_head = head;

		if (old_tail >= old_head)
		{
			head++;
		}
	}

	size_t size()
	{
		return (tail-head)+1;
	}

public:
	std::atomic<size_t> head{1};
	std::atomic<size_t> tail{0};

	T data[N]{};

}; // class MPNCQueue

//
// steal_queue Implementation
//

class steal_queue
{
public:
	steal_queue(size_t n_capacity)
	:m_tasks{new TaskId[n_capacity]},
	 capacity{n_capacity}
	{
	}

	~steal_queue()
	{
		delete[] m_tasks;
	}

	void Push(TaskId task)
	{
		int32_t b = m_bottom;
		m_tasks[b & (capacity-1)] = task;
		m_bottom++;
	}

	TaskId Pop()
	{
		int32_t b = --m_bottom;
		int32_t t = m_top;

		if (t <= b)
		{
			// non-empty queue
			TaskId task = m_tasks[b & (capacity-1)];
			if (t != b)
			{
				// there's still more than one item left in the queue
				return task;
			}
	 
			// this is the last item in the queue
			if (std::atomic_compare_exchange_strong(&m_top, &t, t+1))
			{

				m_bottom = t+1;
				return task;
			}
			// failed race against steal operation
			return NULL_TASK;
		}
		else
		{
			// deque was already empty
			m_bottom = 0;
			m_top = 0;
			return NULL_TASK;
		}
	}

	TaskId Steal()
	{
		int32_t t = m_top;
		int32_t b = m_bottom;
		if (t < b)
		{
			// non-empty queue
			TaskId task = m_tasks[t & (capacity-1)];
			// if m_top = t then it is valid to return task, replace m_top with t+1
			if (std::atomic_compare_exchange_strong(&m_top, &t, t+1))
			{
				return task;
			}

			return NULL_TASK;
		}
		else
		{
			// empty queue
			return NULL_TASK;
		}
	}

	size_t size()
	{
		return m_bottom - m_top;
	}

public:
	std::atomic<int32_t> m_top{0};
	std::atomic<int32_t> m_bottom{0};
	TaskId* m_tasks{nullptr};
	size_t capacity{0};
};

} // namespace utl
