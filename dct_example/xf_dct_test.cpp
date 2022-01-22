/*
 * Copyright 2021 Xilinx, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "common/xf_headers.hpp"
#include "dct.h"
#include "xcl2.hpp"

// ********************************************************
int main() {
  short a[N_DCT], b[N_DCT], b_prime[N_DCT], x[10 * N_DCT];
  int retval = 0, i, j, z;
  FILE *fp;
  cl_int err;

  std::cout << "V3" << std::endl;
  std::cout << "Opening in.dat" << std::endl;

  fp = fopen("in.dat", "r");
  // Read 640 entries from the File
  for (i = 0; i < (10 * N_DCT); i++) {
    int tmp;
    fscanf(fp, "%d", &tmp);
    x[i] = tmp;
  }
  fclose(fp);

  std::cout << "Getting OpenCL Device" << std::endl;

  // OpenCL section:
  // Get the device:
  std::vector<cl::Device> devices = xcl::get_xil_devices();
  cl::Device device = devices[0];

  // Context, command queue and device name:
  OCL_CHECK(err, cl::Context context(device, NULL, NULL, NULL, &err));
  OCL_CHECK(err, cl::CommandQueue queue(context, device,
                                        CL_QUEUE_PROFILING_ENABLE, &err));
  OCL_CHECK(err,
            std::string device_name = device.getInfo<CL_DEVICE_NAME>(&err));

  std::cout << "INFO: Device found - " << device_name << std::endl;

  // Load binary:
  std::string binaryFile = xcl::find_binary_file(device_name, "krnl_dct");
  cl::Program::Binaries bins = xcl::import_binary_file(binaryFile);
  devices.resize(1);
  OCL_CHECK(err, cl::Program program(context, devices, bins, NULL, &err));
  // Create a kernel:
  OCL_CHECK(err, cl::Kernel kernel(program, "dct_accel", &err));

  // Allocate the buffers:
  OCL_CHECK(err,
            cl::Buffer bufin(context, CL_MEM_READ_ONLY, sizeof(a), NULL, &err));
  OCL_CHECK(err, cl::Buffer bufout(context, CL_MEM_WRITE_ONLY, sizeof(b), NULL,
                                   &err));

  // Set kernel arguments:
  OCL_CHECK(err, err = kernel.setArg(0, bufin));
  OCL_CHECK(err, err = kernel.setArg(1, bufout));

  // Initialize the buffers:
  cl::Event event;

  // Call the DCT function 10 times for Dataflow
  for (i = 0; i < 10; i++) {
    //printf("Checkpoint: i = %d\n", i);
    // Iterate through the large x[] array to populate the smaller input array
    // a[]
    for (j = 0; j < N_DCT; j++) {
      a[j] = x[j + (N_DCT * i)];
      //printf("Checkpoint: j = %d; ", j);
    }  // End j_loop
    //printf("\n");

    OCL_CHECK(err,
              queue.enqueueWriteBuffer(bufin,      // buffer on the FPGA
                                       CL_TRUE,    // blocking call
                                       0,          // buffer offset in bytes
                                       sizeof(a),  // Size in bytes
                                       a,          // Pointer to the data to copy
                                       nullptr, &event));

    // Execute the kernel:
    OCL_CHECK(err, err = queue.enqueueTask(kernel));

    // Copy Result from Device Global Memory to Host Local Memory
    queue.enqueueReadBuffer(bufout,   // This buffers data will be read
                            CL_TRUE,  // blocking call
                            0,        // offset
                            sizeof(b),
                            b,  // Data will be stored here
                            nullptr, &event);

    // Clean up:
    queue.finish();

    // For testing purposes, save the first iteration of b[] to b_prime[]
    // To compare to out.golden.dat
    if (i == 0) {
      std::cout << "Copying array b to b_prime" << std::endl;
      for (z = 0; z < N_DCT; z++) {
        b_prime[z] = b[z];
      }  // End For
    }    // End IF
  }      // End i_loop

  fp = fopen("out.dat", "w");
  // printf(" Din Dout \n");
  for (i = 0; i < N_DCT; i++) {
    fprintf(fp, "%d \n", b_prime[i]);
    // printf("  %d   %d\n", a[i], b[i]);
  }
  fclose(fp);

  // Compare the results file with the golden results
  //retval = system("diff --brief -w out.dat out.golden.dat");
  retval = system("diff -w -i -B -b -a out.dat out.golden.dat");
  if (retval != 0) {
    std::cout << "Test failed  !!!" << std::endl;
    retval = 1;
  } else {
    std::cout << "Test passed !" << std::endl;
  }

  return retval;
}
