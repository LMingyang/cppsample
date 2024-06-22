#include "image.h"

Image::Image(FileContent &&content, resolution_t res)
    : File{std::move(content)}, resolution{res} {}


std::string_view Image::get_type() const {
    return "IMG";
}

size_t Image::get_raw_size() const {
  //return colored ? resolution.at(0) * resolution.at(1) * 4 : resolution.at(0) * resolution.at(1);
    return this->resolution.at(0) * this->resolution.at(1) * 4 ;
}

auto Image::get_resolution() const -> resolution_t { return this->resolution; }

void Image::update(FileContent &&new_content, Image::resolution_t size) {
    this->content = std::move(new_content);
    this->resolution = size;
}

