# GPU-Accelerated Canny Edge Detection using NVIDIA NPP

A high-performance, modular C++ application for GPU-accelerated Canny Edge Detection using the NVIDIA Performance Primitives (NPP) library. This project offloads intensive image filtering, gradient calculation, non-maximum suppression, and hysteresis thresholding to NVIDIA CUDA GPUs for real-time edge extraction throughput.

## What is Canny Edge Detection?

Canny edge detection is a multi-stage image processing algorithm designed to extract structural boundaries from digital images while suppressing noise. The pipeline involves Gaussian smoothing, Sobel gradient vector magnitude and direction computation, non-maximum suppression to thin edges down to 1-pixel wide contours, and double-threshold hysteresis to trace connected edge segments. By isolating high-gradient intensity changes, it highlights object contours essential for downstream vision tasks.

## How the GPU is Used

The entire Canny edge detection algorithm is executed on the GPU using NVIDIA Performance Primitives (`nppiFilterCannyBorder_8u_C1R`). The host application allocates device buffers with `cudaMalloc`, transfers single-channel grayscale host images to VRAM via `cudaMemcpy`, and invokes the NPP Canny kernel. NPP handles gradient magnitude calculation, 3x3 Sobel filtering, edge thinning, and hysteresis thresholding in parallel across CUDA thread blocks. Offloading these memory-bandwidth-bound pixel operations to GPU hardware yields multi-fold speedups over single-threaded host CPU loops.

## Dependencies

- **CUDA Toolkit** (installed at `/usr/local/cuda` with `nvcc` and `NPP` headers/libraries)
- **Host C++ Compiler**: `g++` supporting C++14 (`--std=c++14`)
- **Make** build tool

### Installation Command (Ubuntu/Debian)

```bash
sudo apt update
sudo apt install -y build-essential nvidia-cuda-toolkit
```

## Build Instructions

To compile the binary executable `bin/canny_npp`:

```bash
make
```

To clean build artifacts:

```bash
make clean
```

## Run Instructions

### Single Image Mode

```bash
./bin/canny_npp --input data/sample.png --output output/sample_edges.png --low 50 --high 150
```

### Batch Mode

```bash
./bin/canny_npp --batch data/ --output output/ --low 50 --high 150
```

Or run the automated execution script:

```bash
chmod +x run.sh
./run.sh
```

## Example Output

When processing an input image (e.g., `data/sample.png`), the application converts RGB input channels to 8-bit single-channel grayscale, executes Canny edge detection on VRAM, and saves the binary edge map (0 for background, 255 for edges) to `output/sample_edges.png`.

Batch execution logs each processed file to `output/log.txt`:
```text
[OK] data/sample1.png -> output/sample1_edges.png (1920x1080)
[OK] data/sample2.jpg -> output/sample2_edges.png (1280x720)
```

## Challenges / Lessons Learned

- **Dynamic Device Scratch Space**: NPP's `nppiFilterCannyBorder_8u_C1R` requires temporary device memory for multi-stage hysteresis tracking. Querying buffer size via `nppiFilterCannyBorderGetBufferSize` and allocating device scratch memory via `cudaMalloc` was critical to prevent CUDA memory access violations.
- **Strict Modularity & Host/Device Isolation**: Keeping `main.cpp` free of any CUDA header includes required encapsulating all CUDA/NPP calls inside `src/canny_npp.cu` and exposing a clean C++ host interface (`CannyOnDevice`).
- **Input Preprocessing & Grayscale Conversion**: Real-world image inputs vary between 3-channel RGB and 4-channel RGBA. Using `stb_image` with requested channel count forced to `1` ensured seamless automatic conversion to single-channel 8-bit grayscale host buffers before GPU transfer.
- **NPP Link Dependencies Optimization**: Analyzing NPP dependencies revealed that only `-lnppif` (NPP Image Filtering), `-lnppc` (NPP Core), and `-lcudart` are required for Canny edge detection, avoiding unnecessary linkage against all 11 NPP sub-libraries.

## Author & License

- **Author**: Capstone Project
- **License**: MIT License
