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
#ifndef N_CORE_LIST_H
#define N_CORE_LIST_H

#include <n/types.h>
#include <n/utils.h>
#include "AsCollection.h"

namespace n {
namespace core {

template<typename T>
class List
{
	class ListElem : NonCopyable
	{
		public:
			ListElem(const T &t, ListElem *n = 0, ListElem *p = 0) : ListElem(n, p) {
				new(&elem) T(t);
			}

			ListElem(ListElem *n = 0, ListElem *p = 0) : next(n ? n : this), prev(p ? p : this) {
			}

			~ListElem() {
				if(next != this) {
					elem.~T();
				}
			}

			union
			{
				T elem;
			};
			ListElem *next;
			ListElem *prev;
	};

	public:
		class iterator
		{
			public:
				iterator &operator++() {
					elem = elem->next;
					return *this;
				}

				iterator &operator--() {
					elem = elem->prev;
					return *this;
				}

				iterator operator++(int) {
					iterator it(elem);
					elem = elem->next;
					return it;
				}

				iterator operator--(int) {
					iterator it(elem);
					elem = elem->prev;
					return it;
				}

				bool operator!=(iterator t) const {
					return t.elem != elem;
				}

				bool operator==(iterator t) const {
					return t.elem == elem;
				}

				T &operator*() {
					return elem->elem;
				}

				const T &operator*() const {
					return elem->elem;
				}

			private:
				friend class List;
				iterator(ListElem *e) : elem(e) {
				}


				template<typename C>
				iterator(const C &i) : iterator(i.elem) {
				}


				ListElem *elem;
		};

		class const_iterator
		{
			public:
				const_iterator &operator++() {
					elem = elem->next;
					return *this;
				}

				const_iterator &operator--() {
					elem = elem->prev;
					return *this;
				}

				const_iterator operator++(int) {
					const_iterator it(elem);
					elem = elem->next;
					return it;
				}

				const_iterator operator--(int) {
					const_iterator it(elem);
					elem = elem->prev;
					return it;
				}

				bool operator!=(const_iterator t) const {
					return t.elem != elem;
				}

				bool operator==(const_iterator t) const {
					return t.elem == elem;
				}

				const T &operator*() const {
					return elem->elem;
				}

				const_iterator(iterator i) : const_iterator(i.elem) {
				}

			private:
				friend class List;
				const_iterator(ListElem *e) : elem(e) {
				}

				iterator nonConst() const {
					return iterator(*this);
				}

				ListElem *elem;
		};

		typedef T Element;

		List() : lSize(0), tail(new ListElem()), head(tail) {
		}

		template<typename C>
		List(std::initializer_list<C> l) : List() {
			for(auto x : l) {
				append(x);
			}
		}

		template<typename U>
		List(const List<U> &l) : List() {
			append(l);
		}

		List(List<T> &&l) : List() {
			swap(l);
		}

		List(const List<T> &l) : List() {
			for(const T &x : l) {
				append(x);
			}
		}

		template<typename A, typename... Args>
		List(const A &a, Args... args) : List() {
			append(a);
			append(args...);
		}

		~List() {
			while(true) {
				ListElem *d = head;
				head = head->next;
				delete d;
				if(d == tail) {
					break;
				}
			}
		}

		void swap(List<T> &l) {
			ListElem *h = head;
			ListElem *t = tail;
			uint s = lSize;
			head = l.head;
			tail = l.tail;
			lSize = l.lSize;
			l.head = h;
			l.tail = t;
			l.lSize = s;
		}

		template<typename C>
		void append(const C &c) {
			appendDispatch(c, BoolToType<!ShouldInsertAsCollection<C, T>::value>());
		}

		template<typename A, typename B, typename... Args>
		void append(const A &a, const B &b, Args... args) {
			append(a);
			append(b);
			append(args...);
		}

		template<typename A, typename... Args>
		void append(const A &a, Args... args) {
			append(a);
			append(args...);
		}

		template<typename C>
		void prepend(const C &c) {
			prependDispatch(c, BoolToType<!ShouldInsertAsCollection<C, T>::value>());
		}

		template<typename C>
		void insert(const C &c) {
			append(c);
		}

		void popFront() {
			ListElem *e = head->next;
			delete head;
			head = e;
			lSize--;
		}

		void pop() {
			remove(--end());
		}

		void move(iterator from, iterator to) {
			if(from == to) {
				return;
			}
			from.elem->prev->next = from.elem->next;
			from.elem->next->prev = from.elem->prev;

			from.elem->next = to.elem;
			from.elem->prev = to.elem->prev;
			to.elem->prev->next = from.elem;
			to.elem->prev = from.elem;
		}

		iterator remove(iterator t) {
			if(t == begin()) {
				popFront();
				return begin();
			}
			lSize--;
			t.elem->next->prev = t.elem->prev;
			t.elem->prev->next = t.elem->next;
			ListElem *e = t.elem->next;
			delete t.elem;
			return iterator(e);
		}

		template<typename C>
		iterator insert(const C &e, const_iterator t) {
			if(t == begin()) {
				prepend(e);
				return begin();
			}
			if(t == end()) {
				append(e);
				return --end();
			}
			ListElem *pr = t.elem->prev;
			ListElem *ne = t.elem;
			ListElem *n = new ListElem(e, ne, pr);
			pr->next = n;
			ne->prev = n;
			lSize++;
			return iterator(n);
		}

		template<typename I>
		iterator insert(I beg, I en, iterator pos) {
			for(I it = beg; it != en; ++it) {
				insert(*it, pos);
			}
			return pos;
		}

		void clear() {
			lSize = 0;
			ListElem *next = 0;
			for(ListElem *i = head; i != tail; i = next) {
				next = i.next;
				delete i;
			}
		}

		template<typename C>
		void assign(const C &l) {
			if(&l != this) {
				assign(l.begin(), l.end());
			}
		}

		template<typename I>
		void assign(I b, I e) {
			clear();
			for(I i = b; i != e; i++) {
				append(*i);
			}
		}

		template<typename C>
		List<T> &operator=(const C &l) {
			assign(l);
			return *this;
		}

		List<T> &operator=(const List<T> &l) {
			assign(l);
			return *this;
		}

		uint size() const {
			return lSize;
		}

		bool isEmpty() const {
			return !lSize;
		}

		const T &first() const {
			return *begin();
		}

		const T &last() const {
			return *(--end());
		}

		T &first() {
			return *begin();
		}

		T &last() {
			return *(--end());
		}

		bool isSorted() const {
			if(isEmpty()) {
				return true;
			}
			const_iterator l = begin();
			for(const_iterator it = ++begin(); it != end(); ++it) {
				if(*it < *l) {
					return false;
				}
				l = it;
			}
			return true;
		}

		template<typename U>
		iterator findOne(const U &f, const_iterator from) {
			for(iterator i = from.nonConst(); i != end(); ++i) {
				if(f(*i)) {
					return i;
				}
			}
			return end();
		}

		template<typename U>
		const_iterator findOne(const U &f, const_iterator from) const {
			for(const_iterator i = from; i != end(); ++i) {
				if(f(*i)) {
					return i;
				}
			}
			return end();
		}

		template<typename U>
		uint countAll(const U &f) const {
			uint c = 0;
			for(const_iterator i = begin(); i != end(); ++i) {
				if(f(*i)) {
					c++;
				}
			}
			return c;
		}

		template<typename V>
		bool existsOne(const V &f) const {
			for(const_iterator i = begin(); i != end(); ++i) {
				if(f(*i)) {
					return true;
				}
			}
			return false;
		}

		template<typename U>
		iterator find(const U &f, iterator from) {
			return findOne(f, from);
		}

		template<typename U>
		const_iterator find(const U &f, const_iterator from) const {
			return findOne(f, from);
		}

		template<typename U>
		uint count(const U &f) const {
			return countAll(f);
		}

		template<typename V>
		bool exists(const V &f) const {
			return existsOne(f);
		}

		iterator find(const T &e) {
			return findOne([&](const T &t) { return t == e; }, begin());
		}

		iterator find(const T &e, const_iterator from) {
			return findOne([&](const T &t) { return t == e; }, from);
		}

		const_iterator find(const T &e) const {
			return findOne([&](const T &t) { return t == e; }, begin());
		}

		const_iterator find(const T &e, const_iterator from) const {
			return findOne([&](const T &t) { return t == e; }, from);
		}

		uint count(const T &e) const {
			return countAll([&](const T &t) { return t == e; });
		}

		bool exists(const T &e) const {
			return existsOne([&](const T &t) { return t == e; });
		}

		template<typename U>
		void foreach(const U &f) {
			std::for_each(begin(), end(), f);
		}

		template<typename U>
		void foreach(const U &f) const {
			std::for_each(begin(), end(), f);
		}

		template<typename V, typename C = List<typename std::result_of<V(const T &)>::type>>
		C mapped(const V &f) const {
			C a;
			foreach([&](const T &e) { a.insert(f(e)); });
			return a;
		}

		template<typename U, typename C = List<T>>
		C filtered(const U &f) const {
			C a;
			foreach([&](const T &e) {
				if(f(e)) {
					a.insert(e);
				}
			});
			return a;
		}

		template<typename C = List<T>>
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
			for(iterator it = begin(); it != end();) {
				if(!f(*it)) {
					it = remove(it);
				} else {
					++it;
				}
			}
		}

		const_iterator begin() const {
			return const_iterator(head);
		}

		const_iterator end() const {
			return const_iterator(tail);
		}

		const_iterator cbegin() const {
			return const_iterator(head);
		}

		const_iterator cend() const {
			return const_iterator(tail);
		}

		iterator begin() {
			return iterator(head);
		}

		iterator end() {
			return iterator(tail);
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
		bool operator!=(const C &l) const {
			return !operator==(l);
		}

		template<typename C>
		bool operator<(const C &l) const {
			iterator a = begin();
			auto b = l.begin();
			while(a != end() && b != l.end()) {
				if(*a++ != *b++) {
					return false;
				}
			}
			return size() < l.size();
		}

	private:
		void append() {
		}

		template<typename C>
		void appendDispatch(const C &t, TrueType) {
			ListElem *e = new ListElem(t, tail, 0);
			if(head == tail) {
				head = e;
			} else {
				tail->prev->next = e;
				e->prev = tail->prev;
			}
			tail->prev = e;
			lSize++;
		}

		template<typename C>
		void appendDispatch(const C &c, FalseType) {
			for(const auto &e : c) {
				append(e);
			}
		}

		template<typename C>
		void prependDispatch(const C &t, TrueType) {
			ListElem *e = new ListElem(t, head, 0);
			head->prev = e;
			head = e;
			lSize++;
		}

		template<typename C>
		void prependDispatch(const C &c, FalseType) {
			for(const auto &e : c) {
				prepend(e);
			}
		}


		uint lSize;
		ListElem *tail;
		ListElem *head;

};

}
}

template<typename T>
n::core::List<T> operator+(const T &i, const n::core::List<T> &a) {
	return n::core::List<T>(i, a);
}

template<typename T>
n::core::List<T> operator+(const n::core::List<T> &a, const T &i) {
	return n::core::List<T>(a, i);
}


#endif // N_CORE_LIST_H
