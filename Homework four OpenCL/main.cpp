//example for OpenCL kernel that accepts 2 array of floats(in and out)
//and computes out[i] = in[i] * in[i]

/*
    Fixes:  1) Added #include <cstdlib>
            2) Fixed #include "CL/cl.h"
            3) err = clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_GPU | CL_DEVICE_TYPE_CPU, 1, &device_id, NULL);
                So, NULL can`t be added there!
            4) Float comparison fixed- with epsilon value
*/


#include <stdio.h>
#include <cstdlib>
#include <cmath>
#ifdef __APPLE__
#   include "OpenCL/cl.h"
#else
#   include "CL/cl.h"
#endif


//macro we use to check if any of the OpenCL API returned error
//if so, print the file name and line number of the API call and exit from the program
#define CHECK_ERROR(X) __checkError(__FILE__, __LINE__, X)
inline void __checkError(const char* file, int line, cl_int error) {
    if (error != CL_SUCCESS) {
        printf("error %i in file %s, line %i\n", error, file, line);
        exit(EXIT_FAILURE);
    }
}


void helloOpenCL(const float* in, float* out, int count)
{
    printf("Calculating with %d elements\n", count);
    cl_int err = CL_SUCCESS; // error code returned from API calls

    // Connect to a compute device
    cl_device_id device_id;// compute device id

    cl_uint numPlatforms = 0;
    //when called with second arg NULL, clGetPlatformIDs returns in its
    //third arg the number of the platforms the system has
    err = clGetPlatformIDs(0, NULL, &numPlatforms);
    CHECK_ERROR(err);
    printf("Found %i platforms\n", (int)numPlatforms);
    if (numPlatforms == 0)
        exit(EXIT_FAILURE);

    //now, alloc one unique id per platform
    cl_platform_id* platforms = new cl_platform_id[numPlatforms];
    //and get the ids of all platforms the system has
    err = clGetPlatformIDs(numPlatforms, platforms, NULL);
    CHECK_ERROR(err);

    //print the name of each and every platfrom we had found
    for (int i = 0; i < numPlatforms; ++i) {
        char chBuffer[1024];
        err = clGetPlatformInfo(platforms[i], CL_PLATFORM_NAME, 1024, &chBuffer, NULL);
        CHECK_ERROR(err);
        printf("found platform '%i' = \"%s\"\n", i, chBuffer);
    }

    err = clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_GPU | CL_DEVICE_TYPE_CPU, 1, &device_id, NULL);
    CHECK_ERROR(err);

    //now, prepare the first device from the first platform
    //Create a compute context to bind with the device
    cl_context context; // compute context

    context = clCreateContext(0 /*platform id 0*/, 1 /*one device from it*/
                              , &device_id, NULL, NULL, &err);
    CHECK_ERROR(err);

    //Create a command queue. We will use those to fill it with commands
    //that we will send to the device to be executed
    cl_command_queue commands;// compute command queue

    commands = clCreateCommandQueue(context, device_id, 0, &err);
    CHECK_ERROR(err);

    // (3)Compile the kernel itself from the source buffer
    cl_program program; // compute program, may have multiple kernels in it

/*  Get the program from .cl file */

    FILE* programHandle;
    size_t programSize, kernelSourceSize;
    char *programBuffer;

    programHandle = fopen("kernel.cl", "r");
    fseek(programHandle, 0, SEEK_END);
    programSize = ftell(programHandle);
    rewind(programHandle);

    // read kernel source into buffer
    programBuffer = (char*) malloc(programSize + 1);
    programBuffer[programSize] = '\0';
    fread(programBuffer, sizeof(char), programSize, programHandle);
    fclose(programHandle);

/*  end of getting the kernel source */

    //create a program id
    program = clCreateProgramWithSource(context, 1, (const char **) & programBuffer, NULL, &err);
    CHECK_ERROR(err);


    //compile the program
    err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
    if (err != CL_SUCCESS) {
        size_t len;
        char buffer[2048];
        printf("Error: Failed to build program executable!\n");
        clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, &len);
        printf("%s\n", buffer);
    /* TO DO DELETE*/

        // read kernel source back in from program to check
        char * kernelSource = NULL;
        clGetProgramInfo(program, CL_PROGRAM_SOURCE, 0, NULL, &kernelSourceSize);
        kernelSource = (char*) malloc(kernelSourceSize);
        clGetProgramInfo(program, CL_PROGRAM_SOURCE, kernelSourceSize, kernelSource, NULL);
        printf("\nKernel source:\n\n%s\n", kernelSource);
        free(kernelSource);

    /*TO DO DELETE */
        exit(EXIT_FAILURE);
    }

    //get the kernel we want from the compiled program
    cl_kernel kernel;// compute kernel

    kernel = clCreateKernel(program, "helloOpenCLKernel", &err);
    CHECK_ERROR(err);

    //Create the input and output arrays in device memory for our calculation
    cl_mem input;  // handle to device memory used for the input array
    cl_mem output; // handle to device memory used for the output array

    //(4) allocate memory on the device
    //memory marked as input/output can increase the performance of the kernel
    input = clCreateBuffer(context,  CL_MEM_READ_ONLY,  sizeof(float) * count, NULL, &err);
    CHECK_ERROR(err);
    output = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(float) * count, NULL, &err);
    CHECK_ERROR(err);

    //Copy data to the device memory
    err = clEnqueueWriteBuffer(commands, input, CL_TRUE, 0, sizeof(float) * count, in, 0, NULL, NULL);
    CHECK_ERROR(err);

    //Prepare to call the kernel
    //Set the arguments to our compute kernel
    int countLessOne = count - 1;
    err = 0;
    err  = clSetKernelArg(kernel, 0, sizeof(cl_mem), &input);
    err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &output);
    err |= clSetKernelArg(kernel, 2, sizeof(unsigned int), &countLessOne);
    CHECK_ERROR(err);

    size_t global;  // number of thread blocks
    size_t local;   // thread block size

    //Get the maximum work group size for executing the kernel on the device
    err = clGetKernelWorkGroupInfo(kernel, device_id, CL_KERNEL_WORK_GROUP_SIZE, sizeof(local), &local, NULL);
    CHECK_ERROR(err);

    //(5) Execute the kernel over the entire range of our 1d input data set
    //using the maximum number of work group items for this device
    global = count;
    err = clEnqueueNDRangeKernel(commands, kernel, 1, NULL, &global, &local, 0, NULL, NULL);
    CHECK_ERROR(err);

    //Wait for the command commands to get serviced before reading back results
    clFinish(commands);

    //(6) Read back the results from the device to verify the output
    err = clEnqueueReadBuffer( commands, output, CL_TRUE, 0, sizeof(float) * count, out, 0, NULL, NULL );
    CHECK_ERROR(err);

    // Fill first and last element
    out[0] = cbrt(in[count - 1] * in[0] * in[1]);
    out[count - 1] = cbrt(in[count - 2] * in[count - 1] * in[0]);

    //Shutdown and cleanup
    clReleaseMemObject(input);
    clReleaseMemObject(output);
    clReleaseProgram(program);
    clReleaseKernel(kernel);
    clReleaseCommandQueue(commands);
    clReleaseContext(context);
    free(programBuffer);
    delete[] platforms;
    printf("...Done\n");
}

bool checkResultValues(const float* data, const float* result, int count, bool printValues, const float EPSILON)
{
    if (printValues)
            printf("Epsilon %.50f\n", EPSILON);

    unsigned int correct = 0;               // number of correct results returned
    float realValue;
    // Check first element
    realValue = cbrt(data[count - 1] * data[0] * data[1]);
    if(std::fabs(result[0] - realValue) < EPSILON)
        correct++;
    if (printValues)
        printf("Expected %f and result is %f\n", realValue, result[0]);
    for(int i = 1; i < count - 1; i++)
    {
        realValue = cbrt(data[i-1] * data[i] * data[i + 1]);
        if(std::fabs(result[i] - realValue) < EPSILON)
            correct++;
        if (printValues)
            printf("Expected %f and result is %f\n", (cbrt(data[i-1] * data[i] * data[i + 1])), result[i]);
    }
    // Check last element
    realValue = cbrt(data[count - 2] * data[count - 1] * data[0]);
    if(std::fabs(result[count - 1] - realValue) < EPSILON)
        correct++;
    if (printValues)
        printf("Expected %f and result is %f\n", realValue, result[count - 1]);

    //Print a brief summary detailing the results
    printf("Computed '%d/%d' correct values!\n", correct, (int)count);
}


// Test function. Makes buffers with the given size and fills it with random numbers, geometry mean of them (-1 0 and +1 elements) will be close to 1.
// If printValues is true- prints the values and expected once and such...
void test1(const size_t DATA_SIZE, bool printValues = true, bool correctnessValues = true)
{
    // allocate memory on the host and fill it with random data
    float * data = new float[DATA_SIZE];
    for(int i = 0; i < DATA_SIZE; i++)
    {
        data[i] = rand() / (float)RAND_MAX;
        //if (printValues)
        //    printf("%f ", data[i]);
    }

    float * result = new float[DATA_SIZE];

    helloOpenCL(data, result, DATA_SIZE);

    if (correctnessValues)
    {
        const float EPSILON = 1e-7; // For numbers around one...
        checkResultValues(data, result, DATA_SIZE, printValues, EPSILON);
    }

    delete [] data;
    delete [] result;
}
int main(int argc, char** argv)
{
    test1(16, true, true);
    test1(1000000, false, true);
    test1(10000000, false, false);
    test1(100000000, false, false);

    return 0;
}
