#pragma once

#include "file.h"

#include <memory>
#include <string_view>
#include <unordered_map>
#include <vector>
#include <iostream>


class Filesystem : public std::enable_shared_from_this<Filesystem> {
public:
  Filesystem();

  virtual ~Filesystem() = default;

  /**
   * @param file - pointer to file object to register
   */
  bool register_file(std::string_view name,
                     const std::shared_ptr<File>& file);


  bool remove_file(std::string_view name);

  /**
   * @return false if the source name didn't exist, the dest name already
   * exists or if either name is empty
   */
  bool rename_file(std::string_view source, std::string_view dest);


  std::shared_ptr<File> get_file(std::string_view name) const;


  size_t get_file_count() const;

  size_t in_use() const;

  /**
   * get all files that have a size within the given bounds
   */
  std::vector<std::shared_ptr<File>> files_in_size_range(size_t max,
                                                         size_t min = 0) const;

  std::string file_overview(bool sort_by_size = false);

private:
   /**
   * string hasher helps to use string_view for lookup
   */
  struct string_hash{
      using hash_type = std::hash<std::string_view>;
      using is_transparent = void;

      size_t operator()(const char *str) const {return hash_type{}(str); }
      size_t operator()(std::string_view str) const {return hash_type{}(str); }
      size_t operator()(const std::string &str) const {return hash_type{}(str); }
  };

  std::unordered_map<std::string_view, std::shared_ptr<File>, string_hash, std::equal_to<>> files;
};
