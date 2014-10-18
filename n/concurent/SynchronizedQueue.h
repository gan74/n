/*******************************
Copyright (C) 2013-2014 gr�goire ANGERAND

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
**********************************/

#ifndef N_CONCURENT_SYNCHRONIZEDQUEUE_H
#define N_CONCURENT_SYNCHRONIZEDQUEUE_H

#include <n/core/List.h>
#include <n/core/Option.h>
#include "Mutex.h"

namespace n {
namespace concurent {

template<typename T>
class SynchronizedQueue
{
	public:
		SynchronizedQueue() {
		}

		void queue(const T &e) {
			mutex.lock();
			list.append(e);
			mutex.unlock();
			mutex.notify();
		}

		T dequeue() {
			mutex.lock();
			while(list.isEmpty()) {
				mutex.wait();
			}
			T e = list.first();
			list.popFront();
			mutex.unlock();
			return e;
		}

		core::Option<T> tryDequeue() {
			if(!mutex.trylock() ) {
				return core::Option<T>();
			}
			if(list.isEmpty()) {
				mutex.unlock();
				return core::Option<T>();
			}
			T e = list.first();
			list.popFront();
			mutex.unlock();
			return e;
		}

		core::List<T> getDetachedList() const {
			core::List<T> l;
			mutex.lock();
			l = list;
			mutex.unlock();
			return l;
		}

		uint size() const {
			return list.size();
		}

	private:
		mutable Mutex mutex;
		core::List<T> list;
};

}
}

#endif // N_CONCURENT_SYNCHRONIZEDQUEUE_H