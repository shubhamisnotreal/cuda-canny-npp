// Copyright 2026. All Rights Reserved.
//
// Description: CUDA & NPP implementation of Canny Edge Detection.

#include "canny_npp.h"

#include <cstdlib>
#include <iostream>

#include <cuda_runtime.h>
#include <npp.h>
#include <nppi.h>
#include <nppi_filtering_functions.h>

#define CUDA_CHECK(call)                                                    \
  do {                                                                      \
    cudaError_t err = (call);                                               \
    if (err != cudaSuccess) {                                               \
      std::cerr << "CUDA Error at " << __FILE__ << ":" << __LINE__ << " - " \
                << cudaGetErrorString(err) << " (" << err << ")"            \
                << std::endl;                                               \
      std::exit(EXIT_FAILURE);                                              \
    }                                                                       \
  } while (0)

#define NPP_CHECK(call)                                                     \
  do {                                                                      \
    NppStatus status = (call);                                              \
    if (status != NPP_SUCCESS) {                                            \
      std::cerr << "NPP Error at " << __FILE__ << ":" << __LINE__ << " - "  \
                << "NppStatus code: " << status << std::endl;               \
      std::exit(EXIT_FAILURE);                                              \
    }                                                                       \
  } while (0)

namespace canny_npp {

// Performs Canny edge detection on host grayscale image using NVIDIA NPP on GPU.
bool CannyOnDevice(const uint8_t* host_input, int width, int height,
                   int low_threshold, int high_threshold,
                   uint8_t* host_output) {
  if (host_input == nullptr || host_output == nullptr) {
    std::cerr << "Error: Null host pointers passed to CannyOnDevice."
              << std::endl;
    return false;
  }
  if (width <= 0 || height <= 0) {
    std::cerr << "Error: Invalid image dimensions." << std::endl;
    return false;
  }

  size_t image_size = static_cast<size_t>(width) * height * sizeof(uint8_t);
  Npp8u* d_input = nullptr;
  Npp8u* d_output = nullptr;
  Npp8u* d_scratch = nullptr;

  CUDA_CHECK(cudaMalloc(reinterpret_cast<void**>(&d_input), image_size));
  CUDA_CHECK(cudaMalloc(reinterpret_cast<void**>(&d_output), image_size));
  CUDA_CHECK(cudaMemcpy(d_input, host_input, image_size,
                        cudaMemcpyHostToDevice));

  NppiSize roi = {width, height};
  NppiPoint offset = {0, 0};
  int pitch = width * sizeof(Npp8u);

  int buffer_size = 0;
  NPP_CHECK(nppiFilterCannyBorderGetBufferSize(roi, &buffer_size));

  CUDA_CHECK(cudaMalloc(reinterpret_cast<void**>(&d_scratch), buffer_size));

  NPP_CHECK(nppiFilterCannyBorder_8u_C1R(
      d_input, pitch, roi, offset, d_output, pitch, roi, NPP_FILTER_SOBEL,
      NPP_MASK_SIZE_3_X_3, static_cast<Npp16s>(low_threshold),
      static_cast<Npp16s>(high_threshold), nppiNormL2,
      NPP_BORDER_REPLICATE, d_scratch));

  CUDA_CHECK(cudaMemcpy(host_output, d_output, image_size,
                        cudaMemcpyDeviceToHost));

  CUDA_CHECK(cudaFree(d_scratch));
  CUDA_CHECK(cudaFree(d_output));
  CUDA_CHECK(cudaFree(d_input));

  return true;
}

}  // namespace canny_npp
