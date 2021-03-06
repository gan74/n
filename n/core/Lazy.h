/*******************************
Copyright (C) 2013-2015 gregoire ANGERAND

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

#ifndef N_CORE_LAZY_H
#define N_CORE_LAZY_H

#include <type_traits>
#include "Option.h"
#include <n/types.h>

namespace n {
namespace core {

template<typename T>
class LazyVal : NonCopyable
{
	public:
		typedef typename std::result_of<T()>::type resultType;

		LazyVal(T &&f) : func(f) {
		}

		LazyVal(T &f) : func(f) {
		}

		LazyVal(LazyVal &&l) : func(l.func), value(l.value) {
		}

		resultType get() const {
			if(!value) {
				value.set(func());
			}
			return value;
		}

		operator resultType() {
			return get();
		}

		resultType operator()() {
			return get();
		}

		bool hasValue() const {
			return value;
		}

		void invalidate() {
			value.clear();
		}

		const Option<resultType> &getOption() const {
			return value;
		}

		const T &getFunction() const {
			return func;
		}

	private:
		T func;
		mutable Option<resultType> value;
};

template<typename T>
LazyVal<typename TypeInfo<T>::nonRef> Lazy(T&& t) {
	return LazyVal<typename TypeInfo<T>::nonRef>(std::forward<T>(t));
}

} //core
} //n

#endif // N_CORE_LAZY_H
