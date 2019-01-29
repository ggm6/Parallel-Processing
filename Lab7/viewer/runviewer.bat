"C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v8.0\bin\nvcc" -I "C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v8.0\include" -I "C:\ProgramData\NVIDIA Corporation\CUDA Samples\v8.0\common\inc" viewer.cu -l freeglut -o viewer.exe -arch=sm_20 -Wno-deprecated-gpu-targets

viewer.exe