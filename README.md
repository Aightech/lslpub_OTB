# lslpub_OTB
C++ programs that gets the data from the OTBiolab software and pub them in a LSL stream.

## 1 Architecture
### 1.1 INPUTS:
- from OTBiolab software
### 1.2 OUTPUTS:
- LSL stream

## 2 Installation
### 2.1 Ubuntu 18
#### 2.1.1 **Requirements**
None.
#### 2.1.2 Steps
Create a build directory and go inside.
```bash
mkdir build && cd build
```
Configure the project.
```bash
cmake ..
```
Build the project.
```bash
cmake --build . 
```
**Rq:** *The exe file is called lslpub_OTB. This file has also been copied in the bin floder of the git repository root.*

### 2.2 Windows 10
#### 2.2.1 **Requirements**
- **Cmake**
Download and install cmake: https://cmake.org/download/ (download the installer)
#### 2.2.2 Steps
Clone the repository.
```bash
git clone https://github.com/Aightech/lslpub_OTB.git
```
Go inside the repository.
```bash
cd lslpub_OTB
```
Create a build directory and go inside.
```bash
mkdir build && cd build
```
Configure the project.
```bash
cmake ..
```
Build the project.
```bash
cmake --build . --config Release 
```
**Rq:** *The exe file is called lslpub_OTB. This file has also been copied in the bin floder of the git repository root.*

#### 2.2.3 Build LSL library on windows
You can also follow this guide https://github.com/sccn/labstreaminglayer/blob/master/doc/BUILD.md.
##### 2.2.3.1 **Requirements**
- **Cmake**
Download and install cmake: https://cmake.org/download/ (download the installer)
- **Qt**
Download qt installer : https://www.qt.io/download (open source version)
- **Boost libraries**
Download boost lib: https://sourceforge.net/projects/boost/files/boost-binaries/ (last binaries version)
- **Git**
Download and install git: https://git-for-windows.github.io/

##### 2.2.3.2 Steps 
Clone the repository:
```bash
git clone --recurse-submodules https://github.com/labstreaminglayer/labstreaminglayer.git
```
Go in the repository.
```bash
cd labstreaminglayer
```
Create a build repository and go inside.
```bash
mkdir build && cd build
```
Configure and generate the project with the GUI.
```bash
cmake-gui
```
Click on "configure". Select the right version of visualstudio.
Check if the boost libraries and Qt are correct.
Then click on "generate".
In the command prompt, build the project.
```bash
cmake --build . --config Release --target install
```

