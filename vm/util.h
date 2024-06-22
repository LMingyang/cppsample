#pragma once

#include <sstream>
#include <string>
#include <vector>


namespace vm::util {

/**
 * split a string at a given delimiter
 */
std::vector<std::string> split(std::string_view txt, char delimiter);


/**
 * return the whitespace-stripped input string
 */
std::string strip(std::string_view inpt);


namespace detail {

/**
 * cuts a string at given delimiter
 */
template<typename ret_t>
void split_fill(std::string_view txt, char delimiter, ret_t result) {
    std::stringstream splitter;
    splitter.str(std::string{txt});
    std::string part;

    while (std::getline(splitter, part, delimiter)) {
        *result = part;
        result++;
    }
}

} // namespace detail



} // namespace vm::util
