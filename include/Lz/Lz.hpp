#pragma once

#ifndef LZ_LZ_HPP
#define LZ_LZ_HPP

#include "Lz/CartesianProduct.hpp"
#include "Lz/ChunkIf.hpp"
#include "Lz/Chunks.hpp"
#include "Lz/Enumerate.hpp"
#include "Lz/Exclude.hpp"
#include "Lz/Except.hpp"
#include "Lz/Flatten.hpp"
#include "Lz/Generate.hpp"
#include "Lz/GroupBy.hpp"
#include "Lz/JoinWhere.hpp"
#include "Lz/Random.hpp"
#include "Lz/Range.hpp"
#include "Lz/Repeat.hpp"
#include "Lz/TakeEvery.hpp"
#include "Lz/Unique.hpp"

#include "Lz/FunctionTools.hpp"
// Function tools includes:
// Concatenate.hpp
// Filter.hpp
// Join.hpp
// Map.hpp
// StringSplitter.hpp
// Take.hpp
// Zip.hpp

namespace lz {
template<LZ_CONCEPT_ITERATOR Iterator>
class IterView;


/**
 * Converts an iterable into a IterView, where one can chain iterators using dot operator (.filter().map().select().any())
 * @param iterable The iterable to view over.
 * @return An iterator view object.
 */
template<LZ_CONCEPT_ITERATOR Iterator>
constexpr IterView<Iterator> toIterRange(Iterator begin, Iterator end) {
	return lz::IterView<Iterator>(std::move(begin), std::move(end));
}

/**
 * Converts an iterable into a IterView, where one can chain iterators using dot operator (.filter().map().select().any())
 * @param iterable The iterable to view over.
 * @return An iterator view object.
 */
template<LZ_CONCEPT_ITERABLE Iterable>
constexpr IterView<internal::IterTypeFromIterable<Iterable>> toIter(Iterable&& iterable) {
	return lz::toIterRange(internal::begin(std::forward<Iterable>(iterable)), internal::end(std::forward<Iterable>(iterable)));
}


template<LZ_CONCEPT_ITERATOR Iterator>
class IterView final : public internal::BasicIteratorView<Iterator> {
	using Base = internal::BasicIteratorView<Iterator>;
	using Traits = std::iterator_traits<Iterator>;

public:
	using iterator = Iterator;
	using const_iterator = iterator;
	using difference_type = typename Traits::difference_type;

	using value_type = typename Traits::value_type;
	using reference = typename Traits::reference;

	constexpr IterView(Iterator begin, Iterator end) :
		Base(std::move(begin), std::move(end)) {}

	constexpr IterView() = default;

	//! See Concatenate.hpp for documentation.
	template<LZ_CONCEPT_ITERABLE... Iterables>
	LZ_CONSTEXPR_CXX_14 IterView<internal::ConcatenateIterator<Iterator, internal::IterTypeFromIterable<Iterables>...>>
	concat(Iterables&& ... iterables) const {
		return lz::toIter(lz::concat(*this, std::forward<Iterables>(iterables)...));
	}

	//! See Enumerate.hpp for documentation.
	template<LZ_CONCEPT_ARITHMETIC Arithmetic = int>
	LZ_CONSTEXPR_CXX_17 IterView<internal::EnumerateIterator<Iterator, Arithmetic>> enumerate(const Arithmetic begin = 0) const {
		return lz::toIter(lz::enumerate(*this, begin));
	}

	//! See Exclude.hpp for documentation.
	LZ_CONSTEXPR_CXX_17 IterView<internal::ExcludeIterator<Iterator>> exclude(const difference_type from,
																			  const difference_type to) const {
		return lz::toIter(lz::exclude(*this, from, to));
	}

	//! See Join.hpp for documentation.
	IterView<internal::JoinIterator<Iterator>> join(std::string delimiter) const {
		return lz::toIter(lz::join(*this, std::move(delimiter)));
	}

	//! See Map.hpp for documentation
	template<class UnaryFunction>
	constexpr IterView<internal::MapIterator<Iterator, UnaryFunction>> map(UnaryFunction unaryFunction) const {
		return lz::toIter(lz::map(*this, std::move(unaryFunction)));
	}

	//! See Take.hpp for documentation.
	template<class UnaryPredicate>
	constexpr IterView<Iterator> takeWhile(UnaryPredicate predicate) const {
		return lz::toIter(lz::takeWhile(*this, std::move(predicate)));
	}

	//! See Take.hpp for documentation.
	LZ_CONSTEXPR_CXX_17 IterView<Iterator> take(const difference_type amount) const {
		return lz::toIter(lz::take(*this, amount));
	}

	//! See Take.hpp for documentation.
	LZ_CONSTEXPR_CXX_17 IterView<Iterator> drop(const difference_type amount) const {
		return lz::toIter(lz::drop(*this, amount));
	}

	//! See Take.hpp for documentation.
	LZ_CONSTEXPR_CXX_17 IterView<Iterator> slice(const difference_type from, const difference_type to) const {
		return lz::toIter(lz::slice(*this, from, to));
	}

	//! See Take.hpp for documentation.
	constexpr IterView<internal::TakeEveryIterator<Iterator>> takeEvery(const difference_type offset,
																		const difference_type start = 0) const {
		return lz::toIter(lz::takeEvery(*this, offset, start));
	}

	//! See Chunks.hpp for documentation
	LZ_CONSTEXPR_CXX_17 IterView<internal::ChunksIterator<Iterator>> chunks(const std::size_t chunkSize) const {
		return lz::toIter(lz::chunks(*this, chunkSize));
	}

	//! See Zip.hpp for documentation.
	template<LZ_CONCEPT_ITERABLE... Iterables>
	LZ_CONSTEXPR_CXX_14 IterView<internal::ZipIterator<Iterator, internal::IterTypeFromIterable<Iterables>>...>
	zip(Iterables&& ... iterables) const {
		return lz::toIter(lz::zip(*this, std::forward<Iterables>(iterables)...));
	}

	//! See FunctionTools.hpp `zipWith` for documentation
	template<class Fn, class... Iterables>
	LZ_CONSTEXPR_CXX_14 auto zipWith(Fn fn, Iterables&& ... iterables) const ->
	IterView<decltype(std::begin(lz::zipWith(std::move(fn), *this, std::forward<Iterables>(iterables)...)))> {
		return lz::toIter(lz::zipWith(std::move(fn), *this, std::forward<Iterables>(iterables)...));
	}

	//! See FunctionTools.hpp `as` for documentation.
	template<class T>
	constexpr IterView<internal::MapIterator<Iterator, internal::ConvertFn<T>>> as() const {
		return lz::toIter(lz::as<T>(*this));
	}

	//! See FunctionTools.hpp `reverse` for documentation.
	LZ_CONSTEXPR_CXX_17 IterView<std::reverse_iterator<Iterator>> reverse() const {
		return lz::toIter(lz::reverse(*this));
	}

	//! See FunctionTools.hpp `reverse` for documentation.
	LZ_CONSTEXPR_CXX_14 IterView<internal::ZipIterator<Iterator, Iterator>> pairwise() const {
		return lz::toIter(lz::pairwise(*this));
	}

	//! See CartesianProduct.hpp for documentation
	template<class... Iterables>
	LZ_CONSTEXPR_CXX_14 IterView<internal::CartesianProductIterator<Iterator, internal::IterTypeFromIterable<Iterables>...>>
	cartesian(Iterables&& ... iterables) const {
		return lz::toIter(lz::cartesian(*this, std::forward<Iterables>(iterables)...));
	}

	//! See Flatten.hpp for documentation
	template<int N = lz::internal::CountDims<std::iterator_traits<Iterator>>::value - 1>
	constexpr IterView<internal::FlattenIterator<Iterator, N>> flatten() const {
		return lz::toIter(lz::flatten(*this));
	}

	/**
	 * Gets the nth element from this sequence.
	 * @param n The offset.
	 * @return The element referred to by `begin() + n`
	 */
	LZ_CONSTEXPR_CXX_17 reference next(const difference_type n) const {
		using std::next; using lz::next;
		return *next(Base::begin(), n);
	}

	//! See FunctionTools.hpp `length` for documentation.
	LZ_CONSTEXPR_CXX_17 difference_type distance() const {
		using lz::distance; using std::distance;
		return distance(Base::begin(), Base::end());
	}

	//! See FunctionTools.hpp `isEmpty` for documentation.
	constexpr bool isEmpty() const {
		return lz::isEmpty(*this);
	}

	//! See FunctionTools.hpp `hasOne` for documentation.
	constexpr bool hasOne() const {
		return lz::hasOne(*this);
	}

	//! See FunctionTools.hpp `hasMany` for documentation.
	constexpr bool hasMany() const {
		return lz::hasMany(*this);
	}

	//! See FunctionTools.hpp `first` for documentation.
	constexpr reference first() const {
		return lz::first(*this);
	}

	//! See FunctionTools.hpp `last` for documentation.
	constexpr reference last() const {
		return lz::last(*this);
	}

	//! See FunctionTools.hpp `firstOr` for documentation.
	template<class T>
	constexpr value_type firstOr(const T& defaultValue) const {
		return lz::firstOr(*this, defaultValue);
	}

	//! See FunctionTools.hpp `lastOr` for documentation.
	template<class T>
	constexpr value_type lastOr(const T& defaultValue) const {
		return lz::lastOr(*this, defaultValue);
	}

#ifdef LZ_HAS_EXECUTION

	//! See Filter.hpp for documentation.
	template<class UnaryPredicate, class Execution = std::execution::sequenced_policy>
	LZ_CONSTEXPR_CXX_20 IterView<internal::FilterIterator<Iterator, UnaryPredicate, Execution>>
	filter(UnaryPredicate predicate, Execution exec = std::execution::seq) const {
		return lz::toIter(lz::filter(*this, std::move(predicate), exec));
	}

	//! See Except.hpp for documentation.
	template<class IterableToExcept, class Execution = std::execution::sequenced_policy, class Compare = std::less<>>
	LZ_CONSTEXPR_CXX_20
	IterView<internal::ExceptIterator<Iterator, internal::IterTypeFromIterable<IterableToExcept>, Compare, Execution>>
	except(IterableToExcept&& toExcept, Compare compare = {}, Execution exec = std::execution::seq) const {
		return lz::toIter(lz::except(*this, toExcept, std::move(compare), exec));
	}

	//! See Unique.hpp for documentation.
	template<class Execution = std::execution::sequenced_policy, class Compare = std::less<>>
	constexpr IterView<internal::UniqueIterator<Execution, Iterator, Compare>>
	unique(Compare compare = {}, Execution exec = std::execution::seq) const {
		return lz::toIter(lz::unique(*this, std::move(compare), exec));
	}

	//! See ChunkIf.hpp for documentation
	template<class UnaryPredicate, class Execution = std::execution::sequenced_policy>
	LZ_CONSTEXPR_CXX_20 IterView<internal::ChunkIfIterator<Iterator, UnaryPredicate, Execution>>
	chunkIf(UnaryPredicate predicate, Execution execution = std::execution::seq) const {
		return lz::toIter(lz::chunkIf(*this, std::move(predicate), execution));
	}

	//! See FunctionTools.hpp `filterMap` for documentation.
	template<class UnaryMapFunc, class UnaryFilterFunc, class Execution = std::execution::sequenced_policy>
	LZ_CONSTEXPR_CXX_20 IterView<internal::MapIterator<internal::FilterIterator<Iterator, UnaryFilterFunc, Execution>, UnaryMapFunc>>
	filterMap(UnaryFilterFunc filterFunc, UnaryMapFunc mapFunc, Execution exec = std::execution::seq) const {
		return lz::toIter(lz::filterMap(*this, std::move(filterFunc), std::move(mapFunc), exec));
	}

	//! See FunctionTools.hpp `select` for documentation.
	template<class SelectorIterable, class Execution = std::execution::sequenced_policy>
	LZ_CONSTEXPR_CXX_20 auto select(SelectorIterable&& selectors, Execution exec = std::execution::seq) const {
		return lz::toIter(lz::select(*this, std::forward<SelectorIterable>(selectors), exec));
	}

	//! See JoinWhere.hpp for documentation
	template<class IterableB, class SelectorA, class SelectorB, class ResultSelector, class Execution = std::execution::sequenced_policy>
	LZ_CONSTEXPR_CXX_20 IterView<internal::JoinWhereIterator<Iterator, internal::IterTypeFromIterable<IterableB>,
		SelectorA, SelectorB, ResultSelector, Execution>>
	joinWhere(IterableB&& iterableB, SelectorA a, SelectorB b, ResultSelector resultSelector,
		   	  Execution execution = std::execution::seq) const {
		return lz::toIter(lz::joinWhere(*this, iterableB, std::move(a), std::move(b), std::move(resultSelector), execution));
	}

	//! See Take.hpp for documentation
	template<class UnaryPredicate, class Execution = std::execution::sequenced_policy>
	LZ_CONSTEXPR_CXX_20 IterView<Iterator> dropWhile(UnaryPredicate predicate, Execution exec = std::execution::seq) const {
		return lz::toIter(lz::dropWhile(*this, std::move(predicate), exec));
	}

	//! See GroupBy.hpp for documentation
	template<class Comparer = std::equal_to<>, class Execution = std::execution::sequenced_policy>
	LZ_CONSTEXPR_CXX_20 IterView<internal::GroupByIterator<Iterator, Comparer, Execution>>
	groupBy(Comparer comparer = {}, Execution execution = std::execution::seq) const {
		return lz::toIter(lz::groupBy(*this, std::move(comparer), execution));
	}

	//! See FunctionTools.hpp `trim` for documentation
	template<class UnaryPredicateFirst, class UnaryPredicateLast, class Execution = std::execution::sequenced_policy>
	LZ_CONSTEXPR_CXX_20 auto trim(UnaryPredicateFirst first, UnaryPredicateLast last,
								  Execution execution = std::execution::seq) const ->
	decltype(lz::toIter(lz::trim(*this, std::move(first), std::move(last), execution))) {
		return lz::toIter(lz::trim(*this, std::move(first), std::move(last), execution));
	}

	//! See FunctionTools.hpp `firstOrDefault` for documentation.
	template<class T, class U, class Execution = std::execution::sequenced_policy>
	LZ_CONSTEXPR_CXX_20 value_type firstOrDefault(const T& toFind, const U& defaultValue, Execution exec = std::execution::seq) const {
		return lz::firstOrDefault(*this, toFind, defaultValue, exec);
	}

	//! See FunctionTools.hpp `firstOrDefaultIf` for documentation.
	template<class UnaryPredicate, class U, class Execution = std::execution::sequenced_policy>
	LZ_CONSTEXPR_CXX_20
	value_type firstOrDefaultIf(UnaryPredicate predicate, const U& defaultValue, Execution exec = std::execution::seq) const {
		return lz::firstOrDefaultIf(*this, std::move(predicate), defaultValue, exec);
	}

	//! See FunctionTools.hpp `lastOrDefault` for documentation.
	template<class T, class U, class Execution = std::execution::sequenced_policy>
	LZ_CONSTEXPR_CXX_20 value_type lastOrDefault(const T& toFind, const U& defaultValue, Execution exec = std::execution::seq) const {
		return lz::lastOrDefault(*this, toFind, defaultValue, exec);
	}

	//! See FunctionTools.hpp `lastOrDefaultIf` for documentation.
	template<class UnaryPredicate, class U, class Execution = std::execution::sequenced_policy>
	LZ_CONSTEXPR_CXX_20
	value_type lastOrDefaultIf(UnaryPredicate predicate, U&& defaultValue, Execution exec = std::execution::seq) const {
		return lz::lastOrDefaultIf(*this, std::move(predicate), defaultValue, exec);
	}

	//! See FunctionTools.hpp `indexOf` for documentation.
	template<class T, class Execution = std::execution::sequenced_policy>
	LZ_CONSTEXPR_CXX_20 difference_type indexOf(const T& value, Execution exec = std::execution::seq) const {
		return lz::indexOf(*this, value, exec);
	}

	//! See FunctionTools.hpp `indexOfIf` for documentation.
	template<class UnaryPredicate, class Execution = std::execution::sequenced_policy>
	LZ_CONSTEXPR_CXX_20 difference_type indexOfIf(UnaryPredicate predicate, Execution exec = std::execution::seq) const {
		return lz::indexOfIf(*this, std::move(predicate), exec);
	}

	//! See FunctionTools.hpp `contains` for documentation.
	template<class T, class Execution = std::execution::sequenced_policy>
	LZ_CONSTEXPR_CXX_20 bool contains(const T& value, Execution exec = std::execution::seq) const {
		return lz::contains(*this, value, exec);
	}

	//! See FunctionTools.hpp `containsIf` for documentation.
	template<class UnaryPredicate, class Execution = std::execution::sequenced_policy>
	LZ_CONSTEXPR_CXX_20 bool containsIf(UnaryPredicate predicate, Execution exec = std::execution::seq) const {
		return lz::containsIf(*this, std::move(predicate), exec);
	}

	/**
	 * Iterates over the sequence generated so far.
	 * @param func A function to apply over each element. Must have the following signature: `void func(value_type)`
	 * @param exec The execution policy.
	 */
	template<class UnaryFunc, class Execution = std::execution::sequenced_policy>
	LZ_CONSTEXPR_CXX_20 IterView<Iterator>& forEach(UnaryFunc func, Execution exec = std::execution::seq) {
		if constexpr (internal::checkForwardAndPolicies<Execution, Iterator>()) {
			std::for_each(Base::begin(), Base::end(), std::move(func));
		}
		else {
			std::for_each(exec, Base::begin(), Base::end(), std::move(func));
		}
		return *this;
	}

	/**
	 * Performs a left fold with as starting point `init`. Can be used to for e.g. sum all values. For this use:
	 * `[](value_type init, value_type next) const { return init + value_type; }`
	 * @param init The starting value
	 * @param function A binary function with the following signature `value_type func(value_type init, value_type element)`
	 */
	template<class T, class BinaryFunction, class Execution = std::execution::sequenced_policy>
	LZ_CONSTEXPR_CXX_20 T foldl(T&& init, BinaryFunction function, Execution exec = std::execution::seq) const {
		if constexpr (internal::checkForwardAndPolicies<Execution, Iterator>()) {
			static_cast<void>(exec);
			return std::reduce(Base::begin(), Base::end(), std::forward<T>(init), std::move(function));
		}
		else {
			return std::reduce(exec, Base::begin(), Base::end(), std::forward<T>(init), std::move(function));
		}
	}

	/**
	 * Performs a right fold with as starting point `init`. Can be used to for e.g. sum all values. For this use:
	 * `[](value_type init, value_type next) const { return init + value_type; }`
	 * @param init The starting value
	 * @param function A binary function with the following signature `value_type func(value_type init, value_type element)`
	 */
	template<class T, class BinaryFunction, class Execution = std::execution::sequenced_policy>
	LZ_CONSTEXPR_CXX_20 T foldr(T&& init, BinaryFunction function, Execution exec = std::execution::seq) const {
		auto reverseView = lz::reverse(*this);
		if constexpr (internal::checkForwardAndPolicies<Execution, Iterator>()) {
			static_cast<void>(exec);
			return std::reduce(reverseView.begin(), reverseView.end(), std::forward<T>(init), std::move(function));
		}
		else {
			return std::reduce(exec, reverseView.begin(), reverseView.end(), std::forward<T>(init), std::move(function));
		}
	}

	/**
	 * Sums the sequence generated so far.
	 * @param exec The execution policy.
	 */
	template<class Execution = std::execution::sequenced_policy>
	LZ_CONSTEXPR_CXX_20 value_type sum(Execution exec = std::execution::seq) const {
		return this->foldl(value_type(), std::plus<>(), exec);
	}

	/**
	 * Gets the min value of the current iterator view.
	 * @param cmp The comparer. operator< is assumed by default.
	 * @param exec The execution policy.
	 * @return The min element.
	 */
	template<class Compare = std::less<>, class Execution = std::execution::sequenced_policy>
	LZ_CONSTEXPR_CXX_20 reference max(Compare cmp = {}, Execution exec = std::execution::seq) const {
		LZ_ASSERT(!lz::isEmpty(*this), "sequence cannot be empty in order to get max element");
		if constexpr (internal::checkForwardAndPolicies<Execution, Iterator>()) {
			static_cast<void>(exec);
			return *std::max_element(Base::begin(), Base::end(), std::move(cmp));
		}
		else {
			return *std::max_element(exec, Base::begin(), Base::end(), std::move(cmp));
		}
	}

	/**
	 * Gets the min value of the current iterator view.
	 * @param cmp The comparer. operator< is assumed by default.
	 * @param exec The execution policy.
	 * @return The min element.
	 */
	template<class Compare = std::less<>, class Execution = std::execution::sequenced_policy>
	LZ_CONSTEXPR_CXX_20 reference min(Compare cmp = {}, Execution exec = std::execution::seq) const {
		LZ_ASSERT(!lz::isEmpty(*this), "sequence cannot be empty in order to get min element");
		if constexpr (internal::checkForwardAndPolicies<Execution, Iterator>()) {
			static_cast<void>(exec);
			return *std::min_element(Base::begin(), Base::end(), std::move(cmp));
		}
		else {
			return *std::min_element(exec, Base::begin(), Base::end(), std::move(cmp));
		}
	}

	/**
	 * Checks if all of the elements meet the condition `predicate`. `predicate` must return a bool and take a `value_type` as parameter.
	 * @param predicate The function that checks if an element meets a certain condition.
	 * @param exec The execution policy.
	 */
	template<class UnaryPredicate, class Execution = std::execution::sequenced_policy>
	LZ_CONSTEXPR_CXX_20 bool all(UnaryPredicate predicate, Execution exec = std::execution::seq) const {
		if constexpr (internal::checkForwardAndPolicies<Execution, Iterator>()) {
			static_cast<void>(exec);
			return std::all_of(Base::begin(), Base::end(), std::move(predicate));
		}
		else {
			return std::all_of(exec, Base::begin(), Base::end(), std::move(predicate));
		}
	}

	/**
	 * Checks if any of the elements meet the condition `predicate`. `predicate` must return a bool and take a `value_type` as parameter.
	 * @param predicate The function that checks if an element meets a certain condition.
	 * @param exec The execution policy.
	 */
	template<class UnaryPredicate, class Execution = std::execution::sequenced_policy>
	LZ_CONSTEXPR_CXX_20 bool any(UnaryPredicate predicate, Execution exec = std::execution::seq) const {
		if constexpr (internal::checkForwardAndPolicies<Execution, Iterator>()) {
			static_cast<void>(exec);
			return std::any_of(Base::begin(), Base::end(), std::move(predicate));
		}
		else {
			return std::any_of(exec, Base::begin(), Base::end(), std::move(predicate));
		}
	}

	/**
	 * Checks if none of the elements meet the condition `predicate`. `predicate` must return a bool and take a `value_type` as parameter.
	 * @param predicate The function that checks if an element meets a certain condition.
	 * @param exec The execution policy.
	 */
	template<class UnaryPredicate, class Execution = std::execution::sequenced_policy>
	LZ_CONSTEXPR_CXX_20 bool none(UnaryPredicate predicate, Execution exec = std::execution::seq) {
		return !this->all(std::move(predicate), exec);
	}

	/**
	 * Counts how many occurrences of `value` are in this.
	 * @param value The value to count
	 * @return The amount of counted elements equal to `value`.
	 */
	template<class T, class Execution = std::execution::sequenced_policy>
	LZ_CONSTEXPR_CXX_20 difference_type count(const T& value, Execution execution = std::execution::seq) const {
		if constexpr (internal::checkForwardAndPolicies<Execution, Iterator>()) {
			static_cast<void>(execution);
			return std::count(Base::begin(), Base::end(), value);
		}
		else {
			return std::count(execution, Base::begin(), Base::end(), value);
		}
	}

	/**
	 * Counts how many occurrences times the unary predicate returns true.
	 * @param predicate The function predicate that must return a bool.
	 * @return The amount of counted elements.
	 */
	template<class UnaryPredicate, class Execution = std::execution::sequenced_policy>
	LZ_CONSTEXPR_CXX_20 difference_type countIf(UnaryPredicate predicate, Execution execution = std::execution::seq) const {
		if constexpr (internal::checkForwardAndPolicies<Execution, Iterator>()) {
			static_cast<void>(execution);
			return std::count_if(Base::begin(), Base::end(), std::move(predicate));
		}
		else {
			return std::count_if(execution, Base::begin(), Base::end(), std::move(predicate));
		}
	}

	/**
	 * Sorts the sequence with the default (operator<) comparer.
	 * @param execution The execution policy.
	 * @return A reference to this.
	 */
	template<class BinaryPredicate = std::less<>, class Execution = std::execution::sequenced_policy>
	LZ_CONSTEXPR_CXX_20 IterView<Iterator>& sort(BinaryPredicate predicate = {}, Execution execution = std::execution::seq) {
		if constexpr (internal::checkForwardAndPolicies<Execution, IterView>()) {
			static_cast<void>(execution);
			std::sort(Base::begin(), Base::end(), std::move(predicate));
		}
		else {
			std::sort(execution, Base::begin(), Base::end(), std::move(predicate));
		}
		return *this;
	}

	/**
	 * Checks whether the sequence is sorted, using the standard (operator<) compare.
	 * @param exec The execution policy.
	 * @return True if the sequence is sorted given by the `predicate` false otherwise.
	 */
	template<class BinaryPredicate = std::less<>, class Execution = std::execution::sequenced_policy>
	LZ_CONSTEXPR_CXX_20 bool isSorted(BinaryPredicate predicate = {}, Execution exec = std::execution::seq) const {
		if constexpr (internal::checkForwardAndPolicies<Execution, Iterator>()) {
			return std::is_sorted(Base::begin(), Base::end(), std::move(predicate));
		}
		else {
			return std::is_sorted(exec, Base::begin(), Base::end(), std::move(predicate));
		}
	}

#else // ^^^ lz has execution vvv ! lz has execution
	//! See Filter.hpp for documentation
	template<class UnaryPredicate>
	IterView<internal::FilterIterator<Iterator, UnaryPredicate>> filter(UnaryPredicate predicate) const {
		return lz::toIter(lz::filter(*this, std::move(predicate)));
	}

	//! See Except.hpp for documentation
	template<class IterableToExcept, class Compare = std::less<internal::ValueTypeIterable<IterableToExcept>>>
	IterView<internal::ExceptIterator<Iterator, internal::IterTypeFromIterable<IterableToExcept>, Compare>>
	except(IterableToExcept&& toExcept, Compare compare = {}) const {
		return lz::toIter(lz::except(*this, toExcept, std::move(compare)));
	}

	//! See Unique.hpp for documentation
	template<class Compare = std::less<internal::ValueType<Iterator>>>
	constexpr IterView<internal::UniqueIterator<Iterator, Compare>> unique(Compare compare = {}) const {
		return lz::toIter(lz::unique(*this, std::move(compare)));
	}

	//! See ChunkIf.hpp for documentation
	template<class UnaryPredicate>
	IterView<internal::ChunkIfIterator<Iterator, UnaryPredicate>> chunkIf(UnaryPredicate predicate) const {
		return lz::toIter(lz::chunkIf(*this, std::move(predicate)));
	}

	//! See FunctionTools.hpp `filterMap` for documentation
	template<class UnaryMapFunc, class UnaryFilterFunc>
	IterView<internal::MapIterator<internal::FilterIterator<Iterator, UnaryFilterFunc>, UnaryMapFunc>>
	filterMap(UnaryFilterFunc filterFunc, UnaryMapFunc mapFunc) const {
		return lz::toIter(lz::filterMap(*this, std::move(filterFunc), std::move(mapFunc)));
	}

	//! See FunctionTools.hpp `select` for documentation
	template<class SelectorIterable>
	auto select(SelectorIterable&& selectors) const ->
	decltype(lz::toIter(lz::select(*this, std::forward<SelectorIterable>(selectors)))) {
		return lz::toIter(lz::select(*this, std::forward<SelectorIterable>(selectors)));
	}

	//! See JoinWhere.hpp for documentation
	template<class IterableB, class SelectorA, class SelectorB, class ResultSelector>
	LZ_CONSTEXPR_CXX_20 IterView<internal::JoinWhereIterator<Iterator, internal::IterTypeFromIterable<IterableB>,
		SelectorA, SelectorB, ResultSelector>>
	joinWhere(IterableB&& iterableB, SelectorA a, SelectorB b, ResultSelector resultSelector) const {
		return lz::toIter(lz::joinWhere(*this, iterableB, std::move(a), std::move(b), std::move(resultSelector)));
	}

	//! See Take.hpp for documentation
	template<class UnaryPredicate>
	IterView<Iterator> dropWhile(UnaryPredicate predicate) const {
		return lz::toIter(lz::dropWhile(*this, std::move(predicate)));
	}

	//! See GroupBy.hpp for documentation
	template<class Comparer = std::equal_to<internal::ValueType<Iterator>>>
	IterView<internal::GroupByIterator<Iterator, Comparer>> groupBy(Comparer comparer = {}) const {
		return lz::toIter(lz::groupBy(*this, std::move(comparer)));
	}

	//! See FunctionTools.hpp `trim` for documentation
	template<class UnaryPredicateFirst, class UnaryPredicateLast>
	auto trim(UnaryPredicateFirst first, UnaryPredicateLast last) const ->
	decltype(lz::toIter(lz::trim(*this, std::move(first), std::move(last)))) {
		return lz::toIter(lz::trim(*this, std::move(first), std::move(last)));
	}

	//! See FunctionTools.hpp `firstOrDefault` for documentation
	template<class T, class U>
	value_type firstOrDefault(T&& toFind, U&& defaultValue) const {
		return lz::firstOrDefault(*this, toFind, defaultValue);
	}

	//! See FunctionTools.hpp `firstOrDefaultIf` for documentation
	template<class UnaryPredicate, class U>
	value_type firstOrDefaultIf(UnaryPredicate predicate, U&& defaultValue) const {
		return lz::firstOrDefaultIf(*this, std::move(predicate), defaultValue);
	}

	//! See FunctionTools.hpp `lastOrDefault` for documentation
	template<class T, class U>
	value_type lastOrDefault(T&& toFind, U&& defaultValue) const {
		return lz::lastOrDefault(*this, toFind, defaultValue);
	}

	//! See FunctionTools.hpp `lastOrDefaultIf` for documentation
	template<class UnaryPredicate, class U>
	value_type lastOrDefaultIf(UnaryPredicate predicate, U&& defaultValue) const {
		return lz::lastOrDefaultIf(*this, std::move(predicate), defaultValue);
	}

	//! See FunctionTools.hpp `indexOf` for documentation
	template<class T>
	difference_type indexOf(const T& value) const {
		return lz::indexOf(*this, value);
	}

	//! See FunctionTools.hpp `indexOfIf` for documentation
	template<class UnaryPredicate>
	difference_type indexOfIf(UnaryPredicate predicate) const {
		return lz::indexOfIf(*this, std::move(predicate));
	}

	//! See FunctionTools.hpp `contains` for documentation
	template<class T>
	bool contains(const T& value) const {
		return lz::contains(*this, value);
	}

	//! See FunctionTools.hpp `containsIf` for documentation
	template<class UnaryPredicate>
	bool containsIf(UnaryPredicate predicate) const {
		return lz::containsIf(*this, std::move(predicate));
	}

	/**
	 * Iterates over the sequence generated so far.
	 * @param func A function to apply over each element. Must have the following signature: `void func(value_type)`
	 */
	template<class UnaryFunc>
	IterView<Iterator>& forEach(UnaryFunc func) {
		std::for_each(Base::begin(), Base::end(), std::move(func));
		return *this;
	}

	/**
	 * Performs a left fold with as starting point `init`. Can be used to for e.g. sum all values. For this use:
	 * `[](value_type init, value_type next) const { return init + value_type; }`
	 * @param init The starting value
	 * @param function A binary function with the following signature `value_type func(value_type init, value_type element)`
	 */
	template<class T, class BinaryFunction>
	T foldl(T&& init, BinaryFunction function) const {
		return std::accumulate(Base::begin(), Base::end(), std::forward<T>(init), std::move(function));
	}

	/**
	 * Performs a right fold with as starting point `init`. Can be used to for e.g. sum all values. For this use:
	 * `[](value_type init, value_type next) const { return init + value_type; }`
	 * @param init The starting value
	 * @param function A binary function with the following signature `value_type func(value_type init, value_type element)`
	 */
	template<class T, class BinaryFunction>
	T foldr(T&& init, BinaryFunction function) const {
		auto reverseView = lz::reverse(*this);
		return std::accumulate(reverseView.begin(), reverseView.end(), std::forward<T>(init), std::move(function));
	}

	/**
	 * Sums the sequence generated so far.
	 */
	value_type sum() const {
		return this->foldl(value_type(), std::plus<value_type>());
	}

	/**
	 * Gets the max value of the current iterator view.
	 * @param cmp The comparer. operator< is assumed by default.
	 * @return The max element.
	 */
	template<class Compare = std::less<value_type>>
	reference max(Compare cmp = std::less<value_type>()) const {
		LZ_ASSERT(!lz::isEmpty(*this), "sequence cannot be empty in order to get max element");
		return *std::max_element(Base::begin(), Base::end(), std::move(cmp));
	}

	/**
	 * Gets the min value of the current iterator view.
	 * @param cmp The comparer. operator< is assumed by default.
	 * @return The min element.
	 */
	template<class Compare = std::less<value_type>>
	reference min(Compare cmp = std::less<value_type>()) const {
		LZ_ASSERT(!lz::isEmpty(*this), "sequence cannot be empty in order to get min element");
		return *std::min_element(Base::begin(), Base::end(), std::move(cmp));
	}

	/**
	 * Checks if all of the elements meet the condition `predicate`. `predicate` must return a bool and take a `value_type` as parameter.
	 * @param predicate The function that checks if an element meets a certain condition.
	 */
	template<class UnaryPredicate>
	bool all(UnaryPredicate predicate) const {
		return std::all_of(Base::begin(), Base::end(), std::move(predicate));
	}

	/**
	 * Checks if any of the elements meet the condition `predicate`. `predicate` must return a bool and take a `value_type` as parameter.
	 * @param predicate The function that checks if an element meets a certain condition.
	 */
	template<class UnaryPredicate>
	bool any(UnaryPredicate predicate) const {
		return std::any_of(Base::begin(), Base::end(), std::move(predicate));
	}

	/**
	 * Checks if none of the elements meet the condition `predicate`. `predicate` must return a bool and take a `value_type` as parameter.
	 * @param predicate The function that checks if an element meets a certain condition.
	 */
	template<class UnaryPredicate>
	bool none(UnaryPredicate predicate) const {
		return !this->all(std::move(predicate));
	}

	/**
	 * Counts how many occurrences of `value` are in this.
	 * @param value The value to count
	 * @return The amount of counted elements equal to `value`.
	 */
	template<class T>
	difference_type count(const T& value) const {
		return std::count(Base::begin(), Base::end(), value);
	}

	/**
	 * Counts how many occurrences times the unary predicate returns true.
	 * @param predicate The function predicate that must return a bool.
	 * @return The amount of counted elements.
	 */
	template<class UnaryPredicate>
	difference_type countIf(UnaryPredicate predicate) const {
		return std::count_if(Base::begin(), Base::end(), std::move(predicate));
	}

	/**
	 * Sorts the sequence with the default (operator<) comparer.
	 * @return A reference to this.
	 */
	template<class BinaryPredicate = std::less<internal::ValueType<Iterator>>>
	IterView<Iterator>& sort(BinaryPredicate predicate = {}) {
		std::sort(Base::begin(), Base::end(), std::move(predicate));
		return *this;
	}

	/**
	 * Checks whether the sequence is sorted, using the standard (operator<) compare.
	 * @return True if the sequence is sorted given by the `predicate` false otherwise.
	 */
	template<class BinaryPredicate = std::less<internal::ValueType<Iterator>>>
	bool isSorted(BinaryPredicate binaryPredicate = {}) const {
		return std::is_sorted(Base::begin(), Base::end(), std::move(binaryPredicate));
	}
#endif // LZ_HAS_EXECUTION
};
}

#endif // LZ_LZ_HPP