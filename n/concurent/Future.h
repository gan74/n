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

#ifndef N_CONCURENTFUTURE_H
#define N_CONCURENTFUTURE_H

#include "MultiThreadPtr.h"
#include <n/core/Option.h>

namespace n {
namespace concurent {

template<typename T>
class Promise;


template<typename T>
class SharedFuture
{
	public:
		enum State
		{
			Waiting,
			Failed,
			Succeded
		};

		bool isWaiting() const {
			return shared->state == Waiting;
		}

		core::Option<T> get() {
			wait();
			Internal i = getInternal();
			if(i.state != Succeded) {
				return core::Option<T>();
			}
			return i.val;
		}

		core::Option<T> tryGet() {
			Internal i = getInternal();
			if(i.state != Succeded) {
				return core::Option<T>();
			}
			return i.val;
		}

		void wait() {
			Mutex *m = getMutex();
			m->lock();
			while(isWaiting()) {
				m->wait();
			}
			m->unlock();
		}

	private:
		friend class Promise<T>;

		struct Internal
		{
			Internal() : state(Waiting) {
			}

			~Internal() {
				if(state == Succeded) {
					val.~T();
				}
			}

			State state;
			union
			{
				T val;
			};
		};

		SharedFuture() : shared(new Internal()){
		}

		void complete(T e) {
			Mutex *m = getMutex();
			m->lock();
			if(!isWaiting()) {
				m->unlock();
				nError("Invalid future state");
				m->notifyAll();
			}
			shared->state = Succeded;
			new(&(shared->val)) T(e);
			m->unlock();
			m->notifyAll();
		}

		void fail() {
			Mutex *m = getMutex();
			m->lock();
			if(!isWaiting()) {
				m->unlock();
				nError("Invalid future state");
				m->notifyAll();
			}
			shared->state = Failed;
			m->unlock();
			m->notifyAll();
		}

		Mutex *getMutex() {
			return shared.getLockingPolicy().getMutex();
		}

		Internal getInternal() {
			Mutex *m = getMutex();
			m->lock();
			Internal i = *shared;
			m->unlock();
			return i;
		}

		core::SmartPtr<Internal, typename core::NoProxy<Internal>, PtrLock<true>> shared;
};

}
}

#endif // FUTURE_H