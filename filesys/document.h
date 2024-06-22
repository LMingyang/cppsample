#pragma once

#include "file.h"

class Document : public File {
public:
  Document(FileContent &&content = {});

  std::string_view get_type() const override;


  size_t get_raw_size() const override;


  unsigned get_character_count() const;

  void update(FileContent &&new_content);

};
