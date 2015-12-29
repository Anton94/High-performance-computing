__kernel void helloOpenCLKernel(
   __global const float* in,
   __global float* out,
   int count)
{
   int i = get_global_id(0);
   if(i < count && i > 0)
   {
      out[i] = cbrt(in[i - 1] * in[i] * in[i + 1]);   	
   }
}
