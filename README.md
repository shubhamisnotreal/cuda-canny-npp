# GPU-Accelerated Canny Edge Detection using NVIDIA NPP

A high-performance C++ application for GPU-accelerated Canny Edge Detection using NVIDIA Performance Primitives (NPP) library. This project offloads intensive image filtering, gradient calculation, non-maximum suppression, and hysteresis thresholding to the CUDA architecture for optimal execution speed.

## What is Canny Edge Detection?

Canny edge detection is a multi-stage image processing algorithm used to extract structural boundary information from digital images. It reduces the amount of data to be processed while preserving structural properties. The pipeline typically involves Gaussian blurring for noise reduction, Sobel gradient intensity and direction computation, non-maximum suppression to thin edges, and hysteresis thresholding to track connected edge segments.

## How the GPU is Used

The core edge detection pipeline is executed entirely on the GPU using NVIDIA Performance Primitives (NPP). Specifically, `nppiCanny_8u_C1R` handles gradient computation, magnitude calculation, edge thinning, and double-threshold hysteresis in a highly parallelized manner across GPU CUDA cores. Offloading these memory-bound pixel-level transformations to GPU hardware yields dramatic throughput improvements compared to single-threaded or CPU-bound implementations.

## Dependencies

- **CUDA Toolkit** (installed at `/usr/local/cuda` with `nvcc` and `NPP` libraries)
- **Host C++ Compiler**: `g++` supporting C++14 (`--std=c++14`)
- **Make** build tool

### Installation Command (Ubuntu/Debian)

```bash
sudo apt update
sudo apt install -y build-essential nvidia-cuda-toolkit
```

## Build Instructions

To build the executable binary `bin/canny_npp`:

```bash
make
```

To clean build artifacts:

```bash
make clean
```

## Run Instructions

### Single Image Processing

```bash
./bin/canny_npp --input data/sample.png --output output/sample_edges.png --low 50 --high 150
```

### Batch Processing

```bash
./bin/canny_npp --batch data/ --output output/ --low 50 --high 150
```

Or run the automated helper script:

```bash
./run.sh
```

## Example Output

The application reads grayscale or RGB images from `data/`, converts RGB to 8-bit single-channel grayscale if necessary, and writes binary edge maps (0 for non-edges, 255 for detected edge pixels) to `output/`.

## Challenges / Lessons Learned

- *Placeholder bullet 1*
- *Placeholder bullet 2*
- *Placeholder bullet 3*

## Author & License

- **Author**: Capstone Project
- **License**: MIT License
