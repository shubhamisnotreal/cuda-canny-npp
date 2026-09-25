// Copyright 2026. All Rights Reserved.
//
// Description: CUDA & NPP implementation of Canny Edge Detection.

#include "canny_npp.h"

namespace canny_npp {

// Performs Canny edge detection on host grayscale image using NVIDIA NPP on GPU.
bool CannyOnDevice(const uint8_t* host_input, int width, int height,
                   int low_threshold, int high_threshold,
                   uint8_t* host_output) {
  // Stub implementation.
  return false;
}

}  // namespace canny_npp
