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

#ifndef N_CORE_ARRAY_H
#define N_CORE_ARRAY_H

#include <n/types.h>
#include <n/utils.h>
#include "AsCollection.h"

namespace n {
namespace core {

class DefaultArrayResizePolicy
{
	public:
		static uint standardSize(uint size) {
			static const uint offset = 12;
			uint l = log2ui(size + offset);
			return size < 2 ? 8 : (1 << (l + 1)) - offset;
		}

	protected:
		uint size(uint size) const {
			return standardSize(size);
		}

		bool shrink(uint, uint) const {
			return false;
		}
};

class CompactArrayResizePolicy
{
	protected:
		uint size(uint size) const {
			static const uint linearThreshold = 11; // 2048
			if(size > (1 << linearThreshold)) {
				uint s = DefaultArrayResizePolicy::standardSize(2048);
				return s + ((1 << linearThreshold) * (((size - s) >> linearThreshold) + 1));
			}
			return DefaultArrayResizePolicy::standardSize(size);
		}

		bool shrink(uint s, uint cap) const {
			return s + 2048 < cap;
		}
};

class OptimalArrayResizePolicy
{
	protected:
		uint size(uint size) const {
			return size;
		}

		bool shrink(uint s, uint cap) const {
			return s < cap;
		}
};

template<typename T, typename ResizePolicy = DefaultArrayResizePolicy>
class Array : public ResizePolicy// Be SUPER careful when adding collections directly, will use the lazyest cast possible, can be wrong !
{
	typedef typename TypeInfo<T>::nonConst TT;
	public:
		typedef T * iterator;
		typedef T const * const_iterator;

		/*class Size
		{
			public:
				Size(uint s) : size(s) {
				}

			private:
				friend class Array<T, ResizePolicy>;
				const uint size;
		};*/

		typedef uint Size;
		typedef T Element;

		Array() : ResizePolicy(), data(0), dataEnd(0), allocEnd(0) {
		}

		~Array() {
			makeEmpty();
			free(data);
		}

		Array(const Array<T, ResizePolicy> &a) : Array(Size(a.size())) {
			copy(data, a.data, a.size());
			dataEnd = data + a.size();
		}

		template<typename R>
		Array(const Array<T, R> &a) : Array(Size(a.size())) {
			copy(data, a.data, a.size());
			dataEnd = data + a.size();
		}

		Array(Array<T, ResizePolicy> &&a) : Array() {
			swap(std::move(a));
		}

		template<typename C, typename R>
		Array(const Array<C, R> &a) : Array(Size(a.size())) {
			append(a);
		}

		template<typename C>
		Array(std::initializer_list<C> l) : Array(Size(l.size())) {
			for(auto x : l) {
				append(x);
			}
		}

		Array(uint s) : Array() {
			setCapacityUnsafe(0, s);
		}

		template<typename C>
		Array<T, ResizePolicy> &append(const C &c) {
			appendDispatch(c, BoolToType<!ShouldInsertAsCollection<C, T>::value>()); // <------------------------ This is the line you are looking for
			return *this;
		}

		template<typename C>
		Array<T, ResizePolicy> &append(std::initializer_list<C> l) {
			for(auto x : l) {
				append(x);
			}
			return *this;
		}

		iterator remove(const T &e) {
			return erase(find(e));
		}

		iterator erase(const_iterator b) {
			return erase(b, b + 1);
		}

		iterator erase(const_iterator b, const_iterator e) {
			if(e < b) {
				std::swap(b, e);
			}
			if(b >= dataEnd) {
				return dataEnd;
			}
			uint diff = e - b;
			if(e == dataEnd) {
				clear((T *)b, diff);
				dataEnd -= diff;
				shrinkIfNeeded();
				return dataEnd;
			}
			moveBack((T *)b, dataEnd - b, diff);
			dataEnd -= diff;
			shrinkIfNeeded();
			return (T *)b;
		}

		template<typename I>
		iterator insert(I b, I e, iterator position) {
			if(position == dataEnd) {
				for(; b != e; ++b) {
					append(*b);
				}
				return end();
			}
			Array<T, ResizePolicy> buffer;
			buffer.assign(begin(), position);
			buffer.insert(b, e, buffer.end());
			uint pos = buffer.size();
			buffer.insert(position, end(), buffer.end());
			swap(buffer);
			return data + pos;
		}

		template<typename C>
		iterator insert(const C &e, iterator position) {
			return insertDispatch(e, position, BoolToType<ShouldInsertAsCollection<C, T>::value>());
		}

		template<typename C>
		iterator insert(const C &e) {
			append(e);
			return end() - 1;
		}

		template<typename C>
		void assign(const C &c) {
			makeEmpty();
			append(c);
		}

		template<typename I>
		void assign(I b, I e) {
			makeEmpty();
			for(; b != e; ++b) {
				append(*b);
			}
			shrinkIfNeeded();
		}

		template<typename RP>
		void swap(Array<T, RP> &arr) {
			T *e = arr.dataEnd;
			T *a = arr.allocEnd;
			T *d = arr.data;
			arr.dataEnd = dataEnd;
			arr.allocEnd = allocEnd;
			arr.data = data;
			dataEnd = e;
			allocEnd = a;
			data = d;
		}

		template<typename RP>
		void swap(Array<T, RP> &&arr) {
			T *e = arr.dataEnd;
			T *a = arr.allocEnd;
			T *d = arr.data;
			arr.dataEnd = dataEnd;
			arr.allocEnd = allocEnd;
			arr.data = data;
			dataEnd = e;
			allocEnd = a;
			data = d;
		}

		void pop() {
			dataEnd--;
			dataEnd->~T();
			shrinkIfNeeded();
		}

		uint size() const {
			return dataEnd - data;
		}

		void makeEmpty() {
			clear(data, size());
			dataEnd = data;
		}

		void clear() {
			clear(data, size());
			free(data);
			data = dataEnd = allocEnd = 0;
		}

		const T &operator[](uint i) const {
			return data[i];
		}

		T &operator[](uint i) {
			return data[i];
		}

		template<typename C>
		bool operator==(const C &l) const {
			if(size() == l.size()) {
				const_iterator a = begin();
				auto b = l.begin();
				while(a != end()) {
					if(*a++ != *b++) {
						return false;
					}
				}
				return true;
			}
			return false;
		}

		template<typename C>
		bool operator!=(const C &c) const {
			return !(*this == c);
		}

		template<typename C>
		Array<T, ResizePolicy> &operator+=(const C &e) {
			append(e);
			return *this;
		}

		template<typename C>
		Array<T, ResizePolicy> operator+(const C &e) {
			Array<T, ResizePolicy> x(*this);
			x.append(e);
			return x;
		}

		template<typename C>
		Array<T, ResizePolicy> &operator<<(const C &e) {
			append(e);
			return *this;
		}

		Array<T, ResizePolicy> &operator=(const Array<T, ResizePolicy> &e) {
			assign(e);
			return *this;
		}

		template<typename C>
		Array<T, ResizePolicy> &operator=(const C &e) {
			assign(e);
			return *this;
		}

		template<typename P>
		Array<T, ResizePolicy> &operator=(Array<T, P> &&arr) {
			swap(arr);
			return *this;
		}

		void setCapacity(uint cap) {
			uint s = size();
			if(cap < s) {
				clear(data + cap, s - cap);
			}
			setCapacityUnsafe(s, cap);
		}

		void setMinCapacity(uint s) {
			setCapacity(ResizePolicy::size(s));
		}

		void reserve(uint s) {
			setMinCapacity(s + size());
		}

		uint getCapacity() const {
			return allocEnd - data;
		}

		void squeeze() {
			uint s = size();
			setCapacityUnsafe(s, s);
		}

		iterator getIterator(uint i) {
			return data + i;
		}

		const_iterator getIterator(uint i) const {
			return (const_iterator)(data + i);
		}

		const_iterator begin() const {
			return (const_iterator)data;
		}

		const_iterator end() const {
			return (const_iterator)dataEnd;
		}

		const_iterator cbegin() const {
			return (const_iterator)data;
		}

		const_iterator cend() const {
			return (const_iterator)dataEnd;
		}

		iterator begin() {
			return data;
		}

		iterator end() {
			return dataEnd;
		}

		bool isEmpty() const {
			return data == dataEnd;
		}

		T &first() {
			return *data;
		}

		T &last() {
			return *(dataEnd - 1);
		}

		const T &first() const {
			return *data;
		}

		const T &last() const {
			return *(dataEnd - 1);
		}

		bool isValid(const_iterator i) const {
			return i >= data && i < dataEnd;
		}

		template<typename C = std::less<T>>
		void sort(const C &c = C()) {
			std::sort(begin(), end(), c);
		}

		bool isSorted() const {
			const_iterator l = begin();
				for(const_iterator it = begin() + 1; it != end(); it++) {
				if(*it < *l) {
					return false;
				}
				l = it;
			}
			return true;
		}

		template<typename U>
		void foreach(const U &f) {
			std::for_each(begin(), end(), f);
		}

		template<typename U>
		void foreach(const U &f) const {
			std::for_each(begin(), end(), f);
		}

		template<typename V, typename C = Array<typename std::result_of<V(const T &)>::type, ResizePolicy>>
		C mapped(const V &f) const {
			C a;
			AsCollection(a).setMinCapacity(size());
			foreach([&](const T &e) { a.insert(f(e)); });
			return a;
		}

		template<typename U, typename C = Array<T, ResizePolicy>>
		C filtered(const U &f) const {
			C a;
			AsCollection(a).setMinCapacity(size());
			foreach([&](const T &e) {
				if(f(e)) {
					a.insert(e);
				}
			});
			return a;
		}

		template<typename C = Array<T, ResizePolicy>>
		C shuffled() const {
			C sh(*this);
			sh.shuffle();
			return sh;
		}

		template<typename C = Array<T, ResizePolicy>>
		C reversed() const {
			C re(*this);
			re.reverse();
			return re;
		}

		template<typename U>
		bool forall(const U &f) const {
			for(const T &t : *this) {
				if(!f(t)) {
					return false;
				}
			}
			return true;
		}

		template<typename V>
		void map(const V &f) {
			foreach([&](T &e) { e = f(e); });
		}

		template<typename U>
		void filter(const U &f) {
			Array<T, ResizePolicy>::iterator it = begin();
			foreach([&](T &e) {
				if(f((const T &)e)) {
					*it = std::move(e);
					++it;
				}
			});
			dataEnd = it;
			shrinkIfNeeded();
		}

		void shuffle() {
			uint s = size();
			for(uint i = 0; i != s; i++) {
				uint a = random(s);
				if(a != i) {
					std::swap(data[a], data[i]);
				}
			}
		}

		void reverse() {
			uint s = size() / 2;
			for(uint i = 0; i != s; i++) {
				std::swap(data[i], dataEnd[-(i + 1)]);
			}
		}

		Array<T, ResizePolicy> range(const_iterator from, const_iterator to = 0) const {
			to = to ? to : end();
			Array<T, ResizePolicy> a(to - from);
			a.assign(from, to);
			return a;
		}

		template<typename U>
		iterator findOne(const U &f, const_iterator from = 0, const_iterator to = 0) {
			from = from ? from : begin();
			to = to ? to : end();
			for(iterator i = const_cast<iterator>(from); i != to; i++) {
				if(f(*i)) {
					return i;
				}
			}
			return end();
		}

		template<typename U>
		const_iterator findOne(const U &f, const_iterator from = 0, const_iterator to = 0) const {
			from = from ? from : begin();
			to = to ? to : end();
			for(const_iterator i = from; i != to; i++) {
				if(f(*i)) {
					return i;
				}
			}
			return end();
		}

		template<typename U>
		uint countAll(const U &f, const_iterator from = 0, const_iterator to = 0) const {
			from = from ? from : begin();
			to = to ? to : end();
			uint c = 0;
			for(const_iterator i = from; i != to; i++) {
				if(f(*i)) {
					c++;
				}
			}
			return c;
		}

		template<typename V>
		bool existsOne(const V &f, const_iterator from = 0, const_iterator to = 0) const {
			from = from ? from : begin();
			to = to ? to : end();
			for(const_iterator i = from; i != to; i++) {
				if(f(*i)) {
					return true;
				}
			}
			return false;
		}

		iterator find(const T &e, const_iterator from = 0, const_iterator to = 0) {
			return findOne([&](const T &t) { return t == e; }, from, to);
		}

		const_iterator find(const T &e, const_iterator from = 0, const_iterator to = 0) const {
			return findOne([&](const T &t) { return t == e; }, from, to);
		}

		uint count(const T &e, const_iterator from = 0, const_iterator to = 0) const {
			return countAll([&](const T &t) { return t == e; }, from, to);
		}

		bool exists(const T &e, const_iterator from = 0, const_iterator to = 0) const {
			return existsOne([&](const T &t) { return t == e; }, from, to);
		}

	private:
		template<typename U, typename R>
		friend class Array;

		void copy(TT *dst, const TT *src, uint n) {
			if(TypeInfo<T>::isPod) {
				memcpy(dst, src, sizeof(T) * n);
			} else {
				for(; n; n--) {
					new(dst++) TT(*(src++));
				}
			}
		}

		void move(TT *dst, const TT *src, uint n) {
			if(TypeInfo<T>::isPod) {
				memmove(dst, src, sizeof(T) * n);
			} else {
				for(; n; n--) {
					new(dst++) TT(std::move(*(src++)));
				}
			}
		}

		void moveBack(TT *dst, uint n, uint interval = 1) {
			n -= interval;
			if(TypeInfo<T>::isPod) {
				memmove(dst, dst + interval, sizeof(T) * n);
			} else {
				for(T *e = dst + n; dst != e; dst++) {
					std::swap(*dst, *(dst + interval));
				}
				for(; interval; interval--) {
					dst->~T();
					dst++;
				}
			}
		}

		void clear(TT *src, uint n) {
			if(!TypeInfo<T>::isPod) {
				for(; n; n--) {
					(src++)->~TT();
				}
			}
		}

		void expend() {
			uint s = size();
			uint ns = ResizePolicy::size(s);
			setCapacityUnsafe(s, ns);
		}

		void shrinkIfNeeded() {
			uint cc = getCapacity();
			uint s = size();
			if(this->shrink(s, cc)) {
				uint tc = ResizePolicy::size(s);
				if(cc != tc) {
					setCapacityUnsafe(s, tc);
				}
			}
		}

		template<typename C>
		iterator insertDispatch(const C &e, iterator position, FalseType) {
			if(getCapacity() - size()) {
				move(position + 1, position, dataEnd - position);
				position->~T();
				new(position) T(e);
				return position + 1;
			}
			return insert(&e, (&e) + 1, position);
		}

		template<typename C>
		iterator insertDispatch(const C &e, iterator position, TrueType) {
			return insert(e.begin(), e.end(), position);
		}

		template<typename C>
		void appendDispatch(const C &e, TrueType) {
			if(dataEnd == allocEnd) {
				expend();
			}
			new(dataEnd++) TT(e);
		}


		template<typename C>
		void appendDispatch(const C &c, FalseType) {
			static_assert(Collection<C>::isCollection, "Can not build n::core::Array<T> from given type. (type is not convertible to T and is not a collection of T)");
			setMinCapacity(size() + AsCollection(c).sizeOption().get(0));
			for(const auto &e : c) {
				append(e);
			}
		}

		void append() {
		}

		void setCapacityUnsafe(uint s, uint ns) {
			if(TypeInfo<T>::isPod) {
				data = (TT *)safeRealloc(data, ns * sizeof(T));
			} else {
				TT *n = (TT *)malloc(ns * sizeof(T));
				move(n, data, s);
				clear(data, s);
				free(data);
				data = n;
			}
			dataEnd = data + s;
			allocEnd = data + ns;
		}

		TT *data;
		TT *dataEnd;
		TT *allocEnd;
};


} //core
} //n


template<typename T>
n::core::Array<T> operator+(const T &i, const n::core::Array<T> &a) {
	return n::core::Array<T>({i}) + a;
}



#endif // N_CORE_ARRAY_H
