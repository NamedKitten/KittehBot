// Copyright (c) 2017   Maks Mazurov
// Licensed under the MIT license (see LICENSE.md).

#ifndef LIBBF_STRING_UTILS_HPP
#define LIBBF_STRING_UTILS_HPP

#include <string>
#include <sstream>

namespace utils {
    /// Split strings by deliminator
    /// \tparam OutputIt   - must meet OutputIterator requirements, std::back_inserter usually used here.
    /// \param result      - begin iterator of range to which output will be written.
    ///
    /// \note Empty parts (such as ",," when splitting by commas) will be skipped.
    ///
    /// \example
    ///  \code {.cpp}
    ///     std::vector<std::string> result;
    ///     split("1,  3,,44   ,  ", std::back_inserter(result));
    ///     // result == { "1", "  3", "44   ", "  " }
    ///  \endcode
    template<typename CharType, typename OutputIt>
    void split(const std::basic_string<CharType>& string, CharType deliminator, OutputIt result) {
        std::basic_stringstream<CharType> ss(string);
        std::basic_string<CharType> buf;
        while (std::getline(ss, buf, deliminator)) {
            if (!buf.empty()) {
                *result = buf;
                result++;
            }
        }
    }

    /// Overload for convience. Behaves exactly same as split(string, deliminator, result) but returns Container.
    ///
    /// \tparam Container   - container to restore result, should support push_back operation and be DefaultConstructable.
    /// \return instance of Container with parts of input string.
    template<typename CharType, typename Container = std::vector<std::basic_string<CharType> > >
    Container split(const std::basic_string<CharType>& string, CharType deliminator) {
        Container result;
        split<CharType>(string, deliminator, std::back_inserter(result));
        return result;
    }

    /// Join strings.
    /// \tparam InputIt         - must meet InputIterator requirements.
    /// \tparam DeliminatorType - usually std::basic_string, const CharType* or CharType.
    /// \tparam CharType        - if not specified, will be deduced from InputIt (if it possible).
    /// \param begin            - begin iterator (should point to first string to join)
    /// \param end              - after-the-end iterator.
    ///
    /// Example:
    /// <code>
    ///  std::vector<std::string> strings { "1", "2", "3" };
    ///  join(strings.begin(), strings.end(), ',') == "1,2,3";
    /// </code>
    template<typename InputIt,
             typename CharType = typename std::iterator_traits<InputIt>::value_type::value_type,
             typename DeliminatorType>
    std::basic_string<CharType> join(InputIt begin, InputIt end, const DeliminatorType& deliminator) {
        std::basic_stringstream<CharType> joined_string;
        while (begin != end) {
            joined_string << *begin;
            ++begin;
            if (begin != end) {
                joined_string << deliminator;
            }
        }
        return joined_string.str();
    }

    /// In-place remove whitespace from start.
    /// \return Reference to input.
    ///
    /// Character will be treated as whitespace if ::isspace(character) returns true.
    ///
    /// \example <code> remove_leading_whitespace(" \t  \rasd  asd ") == "\rasd  asd" </code>
    template<typename CharType>
    std::basic_string<CharType>& remove_leading_whitespace(std::basic_string<CharType> &input) {
        if (input.empty()) return input;

        // We search for a iterators range of whitespace instead of just
        // calling input.erase for each whitespace we found to reduce reallocations.

        auto leading_space_it = input.begin();
        while (leading_space_it != input.end() && isspace(*leading_space_it)) ++leading_space_it;

        if (leading_space_it != input.begin()) {
            input.erase(input.begin(), leading_space_it);
        }

        return input;
    }

    /// In-place remove whitespace from end.
    /// \return Reference to input.
    ///
    /// Character will be treated as whitespace if ::isspace(character) returns true.
    ///
    /// \example <code> remove_trailing_whitespace(" as asd  asd\r  \t  ") == " as asd  asd\r" </code>
    template<typename CharType>
    std::basic_string<CharType>& remove_trailing_whitespace(std::basic_string<CharType>& input) {
        if (input.empty()) return input;

        // We search for a iterators range of whitespace instead of just
        // calling input.erase for each whitespace we found to reduce reallocations.

        typename std::basic_string<CharType>::reverse_iterator trailing_space_it = input.rbegin();
        while (trailing_space_it != input.rend() && isspace(*trailing_space_it)) ++trailing_space_it;

        if (trailing_space_it != input.rbegin()) {
            auto distance_from_begin = input.rend() - trailing_space_it;
            input.erase(distance_from_begin, input.size() - distance_from_begin);
        }

        return input;
    }

    /// Remove whitespace from start of string.
    /// Behaves same as remove_leading_whitespace but doesn't modifes input and returns string copy.
    template<typename CharType>
    std::basic_string<CharType> without_leading_whitespace(std::basic_string<CharType> input) {
        return remove_leading_whitespace<CharType>(input);
    }

    /// Remove whitespace from end of string.
    /// Behaves same as remove_trailing_whitespace but doesn't modifes input and returns string copy.
    template<typename CharType>
    std::basic_string<CharType> without_trailing_whitespace(std::basic_string<CharType> input) {
        return remove_trailing_whitespace<CharType>(input);
    }
}

#endif // LIBBF_STRING_UTILS_HPP
