// Copyright 2026. All Rights Reserved.
//
// Description: Header file for GPU-accelerated Canny Edge Detection using NPP.

#ifndef SRC_CANNY_NPP_H_
#define SRC_CANNY_NPP_H_

#include <cstdint>

namespace canny_npp {

// Executes Canny Edge Detection on host grayscale buffer using NVIDIA NPP.
// Allocates GPU memory, copies host_input to device, invokes NPP Canny filter,
// copies result back to host_output, and cleans up GPU resources.
bool CannyOnDevice(const uint8_t* host_input, int width, int height,
                   int low_threshold, int high_threshold,
                   uint8_t* host_output);

}  // namespace canny_npp

#endif  // SRC_CANNY_NPP_H_
