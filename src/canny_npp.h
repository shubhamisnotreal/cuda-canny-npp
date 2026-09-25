// Copyright 2026. All Rights Reserved.
//
// Description: Header file for GPU-accelerated Canny Edge Detection using NPP.

#ifndef SRC_CANNY_NPP_H_
#define SRC_CANNY_NPP_H_

#include <cstdint>
#include <string>

namespace canny_npp {

// Performs Canny edge detection on host grayscale image using NVIDIA NPP on GPU.
bool CannyOnDevice(const uint8_t* host_input, int width, int height,
                   int low_threshold, int high_threshold,
                   uint8_t* host_output);

}  // namespace canny_npp

#endif  // SRC_CANNY_NPP_H_
