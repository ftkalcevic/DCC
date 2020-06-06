#pragma once

template <class T, int BUFFER_SIZE>
	class CircularBuffer
	{
		T items[BUFFER_SIZE];
		int start;
		int end;

	public:
		CircularBuffer()
		{
			start = 0;
			end = 0;
		}

		const T& operator[](int index) const
		{
			if (index > Count())
				assert(false && "Index out of range");
			int n = (start + index) % BUFFER_SIZE;
			return items[n];
		}

		void Add(T item)
		{
			items[end] = item;
			end = (end + 1) % BUFFER_SIZE;
			if (start == end)
				start = (start + 1) % BUFFER_SIZE;
		}

		int Count() const
		{
			int n = end - start;
			if (n < 0)
				n += BUFFER_SIZE;
			return n;
		}

		void Clear()
		{
			start = 0;
			end = 0;
		}

	};
