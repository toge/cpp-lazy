#pragma once
#include <type_traits>


namespace detail
{
	template<class Container, class Function>
	class ConstWhereIterator
	{
		using const_iterator = typename std::decay_t<Container>::const_iterator;
	public:
		using iterator_category = std::forward_iterator_tag;
		using value_type = typename const_iterator::value_type;
		using difference_type = typename const_iterator::difference_type;
		using pointer = typename const_iterator::pointer;
		using reference = typename const_iterator::reference;

	private:
		const_iterator _iterator{};
		const_iterator _end{};
		Function _function{nullptr};

	public:
		ConstWhereIterator(const_iterator begin, const_iterator end, Function function, const bool isEndIterator) :
			_iterator(std::move(begin)),
			_end(std::move(end)),
			_function(function)
		{
			if (isEndIterator)
			{
				return;
			}
			_iterator = std::find_if(_iterator, _end, _function);
		}

		reference operator*() const
		{
			return *_iterator;
		}

		pointer operator->() const
		{
			return _iterator.operator->();
		}

		bool operator!=(const ConstWhereIterator& other) const
		{
			return _iterator != other._end;
		}

		bool operator==(const ConstWhereIterator& other) const
		{
			return !(*this == other);
		}

		ConstWhereIterator& operator++()
		{
			if (_iterator != _end)
			{
				_iterator = std::find_if(_iterator + 1, _end, _function);
			}
			return *this;
		}

		ConstWhereIterator operator++(int)
		{
			auto tmp = *this;
			++*this;
			return tmp;
		}

		ConstWhereIterator operator+(const difference_type other) const
		{
			auto tmp = *this;
			for (difference_type i = 0; i != other; ++i, ++tmp) {}
			return tmp;
		}
	};

	template<class Container, class Function>
	class WhereIterator : public ConstWhereIterator<Container, Function>
	{
		using iterator = typename std::decay_t<Container>::iterator;
		using Base = ConstWhereIterator<Container, Function>;
		
	public:
		using iterator_category = typename iterator::iterator_category;
		using value_type = typename iterator::value_type;
		using difference_type = typename iterator::difference_type;
		using pointer = typename iterator::pointer;
		using reference = typename iterator::reference;

		WhereIterator(iterator begin, iterator end, Function function, const bool isEndIterator):
			ConstWhereIterator<Container, Function>(begin, end, function, isEndIterator)
		{
		}

		reference operator*()
		{
			return const_cast<reference>(Base::operator*());
		}

		pointer operator->()
		{
			return const_cast<pointer>(Base::operator->());
		}

		bool operator!=(const WhereIterator& other)
		{
			return Base::operator!=(other);
		}
		
		WhereIterator& operator++()
		{
			Base::operator++();
			return *this;
		}

		WhereIterator operator++(int)
		{
			auto tmp = *this;
			Base::operator++();
			return tmp;
		}
	};
}

namespace lz
{
	template<class Container, class Function>
	class WhereObject
	{
		Container&& _container;
		Function _function;

		using ContainerAlias = std::decay_t<Container>;
	public:
		using value_type = typename ContainerAlias::value_type;
		using size_type = typename ContainerAlias::size_type;
		using difference_type = typename ContainerAlias::difference_type;
		using pointer = typename ContainerAlias::pointer;
		using const_pointer = typename ContainerAlias::const_pointer;
		using reference = typename ContainerAlias::reference;
		using const_reference = typename ContainerAlias::const_reference;

		using iterator = detail::WhereIterator<Container, Function>;
		using const_iterator = detail::ConstWhereIterator<Container, Function>;

		WhereObject(Container&& container, Function function):
			_container(std::forward<Container>(container)),
			_function(function)
		{
		}

		iterator begin()
		{
			return iterator(_container.begin(), _container.end(), _function, false);
		}

		iterator end()
		{
			return iterator(_container.begin(), _container.end(), _function, true);
		}
		
		const_iterator begin() const
		{
			return const_iterator(_container.begin(), _container.end(), _function, false);
		}

		const_iterator end() const
		{
			return const_iterator(_container.begin(), _container.end(), _function, true);
		}

		std::vector<value_type> toVector() const
		{
			return std::vector<value_type>(begin(), end());
		}

		template<size_t N>
		std::array<value_type, N> toArray() const
		{
			std::array<value_type, N> container{};
			detail::fillContainer(begin(), container);
			return container;
		}
	};
	
	template<class Container, class Function>
	WhereObject<Container, Function> where(Container&& container, Function predicate)
	{
		return WhereObject<Container, Function>(std::forward<Container>(container), predicate);
	}
}