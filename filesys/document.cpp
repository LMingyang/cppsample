#include "document.h"
#include <algorithm>

Document::Document(FileContent &&content) : File{std::move(content)} {}


std::string_view Document::get_type() const {
    return "DOC";
}

size_t Document::get_raw_size() const {
  return this->get_size();
}

unsigned Document::get_character_count() const {
    const std::string& data = *this->content.get();
    return static_cast<unsigned>(std::count_if(data.cbegin(), data.cend(),
                                               [](auto c){
                                                   return !std::isspace(c);
                                               }));
}

void Document::update(FileContent &&new_content) {
    this->content = std::move(new_content);
}

