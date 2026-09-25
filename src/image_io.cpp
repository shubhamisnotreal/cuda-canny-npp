// Copyright 2026. All Rights Reserved.
//
// Description: Implementation of image I/O using stb_image and stb_image_write.

#include "image_io.h"

#include <iostream>
#include <string>

#define STB_IMAGE_IMPLEMENTATION
#include "../third_party/stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../third_party/stb_image_write.h"

namespace canny_npp {

// Loads an image from disk and converts it to 8-bit single-channel grayscale.
bool LoadImageGrayscale(const std::string& filepath, Image8u* image) {
  if (image == nullptr) {
    std::cerr << "Error: Image pointer is null." << std::endl;
    return false;
  }

  int width = 0;
  int height = 0;
  int channels = 0;

  // Force loading as 1 channel (grayscale)
  uint8_t* data = stbi_load(filepath.c_str(), &width, &height, &channels, 1);
  if (data == nullptr) {
    std::cerr << "Error: Failed to load image from " << filepath
              << " - " << stbi_failure_reason() << std::endl;
    return false;
  }

  image->width = width;
  image->height = height;
  image->data = data;
  return true;
}

// Saves an 8-bit single-channel grayscale image to disk in PNG format.
bool SaveImageGrayscale(const std::string& filepath, const Image8u& image) {
  if (image.data == nullptr || image.width <= 0 || image.height <= 0) {
    std::cerr << "Error: Invalid image data for saving." << std::endl;
    return false;
  }

  int stride_in_bytes = image.width;
  int result = stbi_write_png(filepath.c_str(), image.width, image.height, 1,
                              image.data, stride_in_bytes);
  if (result == 0) {
    std::cerr << "Error: Failed to write output image to " << filepath
              << std::endl;
    return false;
  }

  return true;
}

// Frees host memory allocated for an image by stb_image.
void FreeImage(Image8u* image) {
  if (image != nullptr && image->data != nullptr) {
    stbi_image_free(image->data);
    image->data = nullptr;
    image->width = 0;
    image->height = 0;
  }
}

}  // namespace canny_npp
