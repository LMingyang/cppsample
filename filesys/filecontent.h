#pragma once

#include <cstdint>
#include <memory>
#include <string>


/**
 * the file content in the string is wrapped so multiple files can point to the same content
 */
class FileContent {
public:
    FileContent() = default;
    FileContent(const std::string& content);
    FileContent(std::string&& content);
    FileContent(const char* content);


    size_t get_size() const;


    std::shared_ptr<const std::string> get() const;

    // add automatic comparisons
    bool operator ==(const FileContent &) const = default;

protected:

    std::shared_ptr<std::string> content;
};
