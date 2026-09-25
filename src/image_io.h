// Copyright 2026. All Rights Reserved.
//
// Description: Header file for image input/output operations using stb_image.

#ifndef SRC_IMAGE_IO_H_
#define SRC_IMAGE_IO_H_

#include <cstdint>
#include <string>

namespace canny_npp {

// Structure representing a single-channel 8-bit grayscale image.
struct Image8u {
  int width;
  int height;
  uint8_t* data;
};

// Loads an image from disk and converts it to 8-bit grayscale.
bool LoadImageGrayscale(const std::string& filepath, Image8u* image);

// Saves an 8-bit grayscale image to disk in PNG format.
bool SaveImageGrayscale(const std::string& filepath, const Image8u& image);

// Frees host memory allocated for an image by stb_image or malloc.
void FreeImage(Image8u* image);

}  // namespace canny_npp

#endif  // SRC_IMAGE_IO_H_
