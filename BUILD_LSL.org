* Build LSL library on windows
You can also follow this guide https://github.com/sccn/labstreaminglayer/blob/master/doc/BUILD.md.
** Requirements
- [**Cmake**](https://cmake.org/download/)
Download and install cmake.
- [**Qt**](https://www.qt.io/download)
Download qt installer (open source version).
- [**Boost libraries**](https://sourceforge.net/projects/boost/files/boost-binaries/)
Download boost lib (last binaries version).
- [**Git**](https://git-for-windows.github.io/)
Download and install git.
- [**MinGW32**](https://sourceforge.net/projects/mingw-w64/) (download and execute the installer for windows, chose i686_64 architecture, add the the bin folder address of minGW to the PATH environement variable) 

** Steps 
- Clone the repository and go inside.
```bash
git clone --recurse-submodules https://github.com/labstreaminglayer/labstreaminglayer.git &&cd labstreaminglayer
```
- Create a build repository and go inside.
```bash
mkdir build && cd build
```
Configure and generate the project with the GUI.
```bash
cmake-gui
```
Click on "configure". Select the MinGW MakeFile.
Check if the boost libraries and Qt are correct.
Then click on "generate".
In the command prompt, build the project.
```bash
cmake --build . --config Release --target install
```
