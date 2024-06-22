#pragma once

#include <memory>
#include <string>
#include <string_view>

#include "filecontent.h"


// forward declarations
class Filesystem;


/**
 * base class for File objects.
 */
class File {
    friend class Filesystem;

public:

    // no default constructor
    File() = delete;

    virtual ~File() = default;

    virtual std::string_view get_type() const = 0;

    // Get the content size of the file
    virtual size_t get_size() const;

    virtual size_t get_raw_size() const = 0;

    /**
     * rename the file only works when the file is registered
     * @return if false, it will leave everything as before and return false.
     */
    bool rename(std::string_view new_name);


    const std::string& get_name() const;


    const FileContent& get_content() const;

protected:


    File(FileContent&& content,
         std::string_view name="");


    FileContent content;


    std::weak_ptr<Filesystem> filesystem;

private:

    std::string name;
};
