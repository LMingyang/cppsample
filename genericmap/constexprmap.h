#pragma once

#include <array>
#include <type_traits>
#include <utility>
#include <stdexcept>
#include <algorithm>


/**
 * compiletime generic lookup map
 */
template<typename K, typename V, size_t count>
class CexprMap {
public:
    using key_type = K;
    using value_type = V;

    template<class... Entries>
    constexpr CexprMap(Entries&&... entries) : values{std::forward<Entries>(entries)...}{
        verify_no_duplicates();
    }


    constexpr size_t size() const {
        return count;
    }

    constexpr bool contains(const K &key) const {
        return find(key) != values.end();
    }


    constexpr const V &get(const K &key) const {
        auto result = find(key);
        if (result == values.end()) {
            throw std::out_of_range{"get value failed"};
        }
        return result->second;
    }


    constexpr const V &operator [](const K &key) const {
        return get(key);
    }

private:
    /**
     * checks if keys are duplicated
     * throws std::invalid_argument
     */
    constexpr void verify_no_duplicates() const {
        for (auto first = values.begin(); first != values.end(); ++first) {
            auto current = *first;
            auto duplicate = std::find_if(first + 1, values.end(), [&current](auto &entry){
                return current.first == entry.first;
            });
            if (duplicate != values.end()) {
                throw std::invalid_argument{"keys duplicated"};
            }
        }
    }

    constexpr auto find(const K &key) const {
       return std::find_if(values.begin(), values.end(), [&key](auto &entry){
           return entry.first == key;
       });
    }

    std::array<std::pair<K, V>, count> values;
};


/**
 * helper function, returns CexprMap
 */
template<typename K, typename V, typename... Entries>
constexpr auto create_cexpr_map(Entries&&... entry) {
    return CexprMap<K, V, sizeof...(entry)>(std::forward<Entries>(entry)...);
}

/**
 * template deduction guide
 */
template<typename Entry, typename... Rest>
requires std::conjunction_v<std::is_same<Entry, Rest>...>
CexprMap(Entry, Rest&&...) -> CexprMap<typename Entry::first_type, typename Entry::second_type, sizeof...(Rest) + 1>;
