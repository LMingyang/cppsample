#include "audio.h"

Audio::Audio(FileContent &&content, unsigned duration)
    : File{std::move(content)}, duration{duration} {}

std::string_view Audio::get_type() const {
    return "AUD";
}

size_t Audio::get_raw_size() const {
  return 48000 * 2 * this->duration * 2;
}

unsigned Audio::get_duration() const { return this->duration; }

void Audio::update(FileContent &&new_content, unsigned int new_duration) {
    this->content = std::move(new_content);
    this->duration = new_duration;
}

