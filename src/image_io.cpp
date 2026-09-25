// Copyright 2026. All Rights Reserved.
//
// Description: Implementation of image I/O using stb_image and stb_image_write.

#include "image_io.h"

namespace canny_npp {

// Loads an image from disk and converts it to 8-bit grayscale.
bool LoadImageGrayscale(const std::string& filepath, Image8u* image) {
  // Stub implementation.
  return false;
}

// Saves an 8-bit grayscale image to disk.
bool SaveImageGrayscale(const std::string& filepath, const Image8u& image) {
  // Stub implementation.
  return false;
}

// Frees host memory allocated for an image.
void FreeImage(Image8u* image) {
  // Stub implementation.
}

}  // namespace canny_npp
