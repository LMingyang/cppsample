#include "filesystem.h"

#include <algorithm>
#include <iomanip>
#include <numeric>
#include <sstream>

Filesystem::Filesystem() = default;

bool Filesystem::register_file(std::string_view name,

                               const std::shared_ptr<File>& file) {
  // sanity checks
  if(name.empty() || this->files.contains(name) || !file){
      return false;
  }

  auto thisptr = this->shared_from_this();

  // check if the file already has a filesystem
  auto filesys = file->filesystem.lock();
  if(filesys && filesys != thisptr){
      return false;
  }

  file->filesystem = std::move(thisptr);
  file->name = name;

  this->files.emplace(name,file);
  return true;
}

bool Filesystem::remove_file(std::string_view name) {

    if(name.empty()){
        std::cout << "is empty";
        return false;
    }

    auto it = this->files.find(name);
    if(it == this->files.end()){
        return false;
    }

    this->files.erase(it);

    return true;
}

bool Filesystem::rename_file(std::string_view source, std::string_view dest) {

  if(source.empty() || dest.empty()){
      return false;
  }

  auto sit = this->files.find(source);
  auto dit = this->files.find(dest);

  if(sit == std::end(this->files) || dit != std::end(this->files)){
      return false;
  }

    auto file = std::move(sit->second);
    file->name = dest;

    this->files.erase(sit);
    this->files.emplace(dest, std::move(file));

    return true;
}

std::shared_ptr<File> Filesystem::get_file(std::string_view name) const {

  if(name.empty()){
      return nullptr;
  }

  auto it = this->files.find(name);

  return it != files.end() ? it->second : nullptr;
}

size_t Filesystem::get_file_count() const {
  return this->files.size();;
}

size_t Filesystem::in_use() const {
  return static_cast<size_t>(std::accumulate(files.begin(), files.end(), 0,
                                             [](size_t sum, const auto& pair) {
                                                 return sum + pair.second->get_size();
                                             }));
}

// convenience function to see what files are stored
std::string Filesystem::file_overview(bool sort_by_size) {
  std::ostringstream output;
  std::vector<std::pair<std::string, std::shared_ptr<File>>> elems{
      std::begin(this->files), std::end(this->files)
  };

  std::sort(std::begin(elems), std::end(elems),
            [sort_by_size](auto lhs, auto rhs){
                if (sort_by_size) {
                    return lhs.second->get_size() < rhs.second->get_size();
                } else {
                    return lhs.second->get_name() < rhs.second->get_name();
                }
  });

  output << this->files.size()  << " files in filesystem: " << std::endl;
  output << "File name:          Type:   Size:\n";

  for (auto&& entry : this->files) {
       output << std::left << std::setw(20) << entry.second->get_name()
              << std::setw(8) << entry.second->get_type()
              << entry.second->get_size() << std::endl;
  }
  output << std::endl;
  return std::move(output).str();
}

std::vector<std::shared_ptr<File>>
Filesystem::files_in_size_range(size_t max, size_t min) const {

  std::vector<std::shared_ptr<File>> result;
  for(const auto& item : files){
      size_t size = item.second->get_size();
      if(size <= max && size >= min){
          result.push_back(item.second);
      }
  }
  return result;
}
