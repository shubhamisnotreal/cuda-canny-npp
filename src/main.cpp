// Copyright 2026. All Rights Reserved.
//
// Description: Main entry point and CLI parser for GPU Canny Edge Detection.

#include <dirent.h>
#include <sys/stat.h>

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

#include "canny_npp.h"
#include "image_io.h"

namespace canny_npp {

// Structure holding command-line configuration options.
struct Options {
  std::string input_path;
  std::string output_path;
  std::string batch_dir;
  int low_threshold = 50;
  int high_threshold = 150;
  bool help_requested = false;
};

// Prints CLI application usage information.
void PrintUsage(const char* prog_name) {
  std::cout << "Usage: " << prog_name << " [options]\n\n"
            << "Options:\n"
            << "  --input <path>   Path to single input image (required unless --batch is set)\n"
            << "  --output <path>  Path for output image or directory\n"
            << "  --low <int>      Low threshold value (default: 50)\n"
            << "  --high <int>     High threshold value (default: 150)\n"
            << "  --batch <dir>    Process all .png/.jpg/.pgm images in directory\n"
            << "  -h, --help       Print usage and exit\n";
}

// Parses command-line arguments into Options structure.
bool ParseArgs(int argc, char* argv[], Options* opts) {
  for (int i = 1; i < argc; ++i) {
    std::string arg = argv[i];
    if (arg == "-h" || arg == "--help") {
      opts->help_requested = true;
      return true;
    } else if (arg == "--input" && i + 1 < argc) {
      opts->input_path = argv[++i];
    } else if (arg == "--output" && i + 1 < argc) {
      opts->output_path = argv[++i];
    } else if (arg == "--low" && i + 1 < argc) {
      opts->low_threshold = std::atoi(argv[++i]);
    } else if (arg == "--high" && i + 1 < argc) {
      opts->high_threshold = std::atoi(argv[++i]);
    } else if (arg == "--batch" && i + 1 < argc) {
      opts->batch_dir = argv[++i];
    } else {
      std::cerr << "Unknown or incomplete option: " << arg << std::endl;
      return false;
    }
  }
  return true;
}

// Helper to check if a file extension matches supported image formats.
bool HasSupportedImageExtension(const std::string& filename) {
  size_t dot_pos = filename.find_last_of('.');
  if (dot_pos == std::string::npos) return false;
  std::string ext = filename.substr(dot_pos);
  std::transform(ext.begin(), ext.end(), ext.begin(),
                 [](unsigned char c) { return std::tolower(c); });
  return (ext == ".png" || ext == ".jpg" || ext == ".jpeg" || ext == ".pgm");
}

// Joins directory and filename into a clean path.
std::string JoinPath(const std::string& dir, const std::string& file) {
  if (dir.empty()) return file;
  if (dir.back() == '/' || dir.back() == '\\') return dir + file;
  return dir + "/" + file;
}

// Gets filename stem without extension.
std::string GetFileStem(const std::string& filepath) {
  size_t last_slash = filepath.find_last_of("/\\");
  std::string filename = (last_slash == std::string::npos)
                             ? filepath
                             : filepath.substr(last_slash + 1);
  size_t last_dot = filename.find_last_of('.');
  if (last_dot == std::string::npos) return filename;
  return filename.substr(0, last_dot);
}

// Processes a single image file through Canny edge detection pipeline.
bool ProcessSingleImage(const std::string& in_path, const std::string& out_path,
                        int low_thresh, int high_thresh,
                        std::string* error_reason) {
  Image8u in_img = {0, 0, nullptr};
  if (!LoadImageGrayscale(in_path, &in_img)) {
    if (error_reason) *error_reason = "Failed to load input image";
    return false;
  }

  std::vector<uint8_t> out_buffer(in_img.width * in_img.height);
  bool success = CannyOnDevice(in_img.data, in_img.width, in_img.height,
                               low_thresh, high_thresh, out_buffer.data());

  if (!success) {
    if (error_reason) *error_reason = "GPU Canny execution failed";
    FreeImage(&in_img);
    return false;
  }

  Image8u out_img = {in_img.width, in_img.height, out_buffer.data()};
  bool saved = SaveImageGrayscale(out_path, out_img);
  FreeImage(&in_img);

  if (!saved) {
    if (error_reason) *error_reason = "Failed to write output image";
    return false;
  }

  return true;
}

// Batch mode execution scanning input directory and processing images.
int ProcessBatch(const std::string& batch_dir, const std::string& output_dir,
                 int low_thresh, int high_thresh) {
  DIR* dir = opendir(batch_dir.c_str());
  if (!dir) {
    std::cerr << "Error: Cannot open directory " << batch_dir << std::endl;
    return EXIT_FAILURE;
  }

  struct dirent* entry = nullptr;
  int processed_count = 0;
  while ((entry = readdir(dir)) != nullptr) {
    std::string fname = entry->d_name;
    if (fname == "." || fname == "..") continue;
    if (!HasSupportedImageExtension(fname)) continue;

    std::string in_path = JoinPath(batch_dir, fname);
    std::string stem = GetFileStem(fname);
    std::string out_path = JoinPath(output_dir, stem + "_edges.png");

    Image8u temp_img = {0, 0, nullptr};
    std::string err_reason;
    if (LoadImageGrayscale(in_path, &temp_img)) {
      int w = temp_img.width;
      int h = temp_img.height;
      FreeImage(&temp_img);

      if (ProcessSingleImage(in_path, out_path, low_thresh, high_thresh,
                             &err_reason)) {
        std::cout << "[OK] " << in_path << " -> " << out_path << " (" << w
                  << "x" << h << ")" << std::endl;
      } else {
        std::cout << "[FAIL] " << in_path << ": " << err_reason << std::endl;
      }
    } else {
      std::cout << "[FAIL] " << in_path << ": Failed to load image" << std::endl;
    }
    processed_count++;
  }
  closedir(dir);
  return EXIT_SUCCESS;
}

}  // namespace canny_npp

int main(int argc, char* argv[]) {
  canny_npp::Options opts;
  if (!canny_npp::ParseArgs(argc, argv, &opts)) {
    canny_npp::PrintUsage(argv[0]);
    return EXIT_FAILURE;
  }

  if (opts.help_requested) {
    canny_npp::PrintUsage(argv[0]);
    return EXIT_SUCCESS;
  }

  if (!opts.batch_dir.empty()) {
    if (opts.output_path.empty()) {
      std::cerr << "Error: --output directory is required for batch mode."
                << std::endl;
      return EXIT_FAILURE;
    }
    return canny_npp::ProcessBatch(opts.batch_dir, opts.output_path,
                                   opts.low_threshold, opts.high_threshold);
  }

  if (opts.input_path.empty() || opts.output_path.empty()) {
    std::cerr << "Error: --input and --output are required." << std::endl;
    canny_npp::PrintUsage(argv[0]);
    return EXIT_FAILURE;
  }

  std::string err_reason;
  if (!canny_npp::ProcessSingleImage(opts.input_path, opts.output_path,
                                     opts.low_threshold, opts.high_threshold,
                                     &err_reason)) {
    std::cerr << "Error processing image: " << err_reason << std::endl;
    return EXIT_FAILURE;
  }

  std::cout << "Successfully processed " << opts.input_path << " -> "
            << opts.output_path << std::endl;
  return EXIT_SUCCESS;
}
