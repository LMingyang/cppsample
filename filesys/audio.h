#pragma once

#include "file.h"


class Audio : public File {
public:
  Audio(FileContent &&content = {}, unsigned duration = 0);

  std::string_view get_type() const override;

  size_t get_raw_size() const override;

  unsigned get_duration() const;

  void update(FileContent &&new_content, unsigned new_duration);

private:
  unsigned duration;
};
