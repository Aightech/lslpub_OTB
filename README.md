* lslpub_OTB
[Documentation](https://aightech.github.io/lslpub_OTB/html/d3/dcc/md__r_e_a_d_m_e.html)

** Presentation
C++ programs that gets the data from the OTBiolab quattrocento device and publishes them in a LSL stream.

** Usage
*** Requirements
None.
*** Create wired connection
Set uo manually the IPv4:
|     address |       netmask |     gateway |
|-------------+---------------+-------------|
| 169.254.1.0 | 255.255.255.0 | 169.254.1.0 |
|             |               |             |
**Ubuntu 18**: Sttings>Network> add a new wired connection. In IPv4 category. 
**Windows 10**: Network Connections > Ethernet > properties > IPV4 > properties
*** Usage
- Clone the repository and go inside.
```bash
git clone https://github.com/Aightech/lslpub_OTB.git && cd lslpub_OTB
```
- Run the compiled program in the bin directory.
- Run it with the option ```bash -h``` to get the available option.
*** Build
**** Ubuntu 18 
- Clone the repository and go inside.
```bash git clone https://github.com/Aightech/lslpub_OTB.git && cd lslpub_OTB ```
- Create a build directory and go inside.
- Configure the project.
- Build the project.
```bash mkdir build && cd build && cmake .. && cmake --build . ```
**** Windows 10
***** 2.2.1 **Requirements**
- [CMake](https://cmake.org/download/) (download and execute the installer for windows , add to the PATH variable)
- [MinGW32](https://sourceforge.net/projects/mingw-w64/) (download and execute the installer for windows, chose i686_64 architecture, add the the bin folder address of minGW to the PATH environement variable).
- [**Git**](https://git-for-windows.github.io/) Download and install git.
***** Steps
- Clone the repository and go inside.
```bash
git clone https://github.com/Aightech/lslpub_OTB.git && cd lslpub_OTB
```
- Create a build directory.
- Configure and generate the CMake project.
- Build the project.
```bash
mkdir build && cd build && cmake .. -G "MinGW Makefiles" && mingw32-make
```




**** Note 
**Rq:** *The exe file is called lslpub_OTB. This file has also been copied in the bin floder of the git repository root.*
