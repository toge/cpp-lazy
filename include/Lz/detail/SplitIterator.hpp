#pragma once

#ifndef LZ_SPLIT_ITERATOR_HPP
#define LZ_SPLIT_ITERATOR_HPP

#include <string>

#include "LzTools.hpp"

#if CPP_LAZY_HAS_INCLUDE(<string_view>) && __cplusplus >= 201703L
#include <string_view>
#endif


namespace lz {
    template<class, class>
    class StringSplitter;

    namespace internal {
        template<class String>
        struct SplitViewIteratorHelper {
            std::string delimiter{};
            const String& string = String();

            SplitViewIteratorHelper(std::string delimiter, const String& string) :
                delimiter(std::move(delimiter)),
                string(string)
            {}

            SplitViewIteratorHelper() = default;
        };


        template<class SubString, class String>
        class SplitIterator {
            std::size_t _currentPos{}, _last{};
            mutable SubString _substring{};
            const SplitViewIteratorHelper<String>* _splitIteratorHelper{};


            friend class StringSplitter<SubString, String>;


        public:
            using iterator_category = std::input_iterator_tag;
            using value_type = SubString;
            using reference = Conditional<std::is_same<SubString, std::string>::value, SubString&, SubString>;
            using difference_type = std::ptrdiff_t;
            using pointer = FakePointerProxy<reference>;

            SplitIterator(const std::size_t startingPosition, const SplitViewIteratorHelper<String>* splitIteratorHelper) :
                _currentPos(startingPosition),
                _splitIteratorHelper(splitIteratorHelper) {
                // Micro optimization, check if object is created from begin(), only then we want to search
                if (startingPosition == 0) {
                    _last = _splitIteratorHelper->string.find(_splitIteratorHelper->delimiter, _currentPos);
                }
            }

            SplitIterator() = default;

            // Returns a reference to a std::string if C++14, otherwise it returns a std::string_view by value
            Conditional<std::is_same<SubString, std::string>::value, SubString&, SubString> operator*() const {
                if (_last != std::string::npos) {
                    _substring = SubString(&_splitIteratorHelper->string[_currentPos], _last - _currentPos);
                }
                else {
                    _substring = SubString(&_splitIteratorHelper->string[_currentPos]);
                }
                return _substring;
            }

            pointer operator->() const {
                return FakePointerProxy<decltype(**this)>(**this);
            }

            bool operator!=(const SplitIterator& other) const {
                return _currentPos != other._currentPos;
            }

            bool operator==(const SplitIterator& other) const {
                return !(*this != other);
            }

            SplitIterator& operator++() {
                const std::size_t delimLen = _splitIteratorHelper->delimiter.length();
                const std::size_t stringLen = _splitIteratorHelper->string.length();

                if (_last == std::string::npos) {
                    _currentPos = stringLen;
                }
                else if (_last == stringLen - delimLen) {
                    // Check if ends with delimiter
                    _last = std::string::npos;
                    _currentPos = _splitIteratorHelper->string.length();
                }
                else {
                    _currentPos = _last + delimLen;
                    _last = _splitIteratorHelper->string.find(_splitIteratorHelper->delimiter, _currentPos);
                }

                return *this;
            }

            SplitIterator operator++(int) {
                SplitIterator tmp(*this);
                ++* this;
                return tmp;
            }
        };
    }
}

#endif