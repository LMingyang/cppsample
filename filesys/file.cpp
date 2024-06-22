#include "file.h"

#include "filesystem.h"

size_t File::get_size() const { return this->content.get_size(); }

bool File::rename(std::string_view new_name) {

  // check if a filesystem actually exists, then rename in the filesystem
  auto fsptr = this->filesystem.lock();
  if(not fsptr){
      return false;
  }

  return fsptr->rename_file(this->name, new_name);
}

const std::string &File::get_name() const { return this->name; }

const FileContent &File::get_content() const { return this->content; }


File::File(FileContent &&content, std::string_view name) : content{std::move(content)}, name{name} {}
