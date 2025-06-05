mkdir Debug
cd Debug
call "D:\Program Files (x86)\Microsoft Visual Studio\2019\Professional\VC\Auxiliary\Build\vcvarsamd64_x86.bat"
cmake  -G "Visual Studio 16 2019"  -A x64 -DCMAKE_BUILD_TYPE=Debug ../
msbuild /m FlowchartTool.sln /p:Platform=x64 /p:Configuration=Debug