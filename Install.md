# CheckDoseQt installation instructions

Currently tested in Ubuntu, Kubuntu 18.04.4 LTS and Windows 10

## Requirements

Mandatory:

* [eigen3](https://eigen.tuxfamily.org/dox)
* [jsoncpp](https://github.com/open-source-parsers/jsoncpp)
* [Qt5](https://www.qt.io)

Additional:

* [doxygen](https://doxygen.nl): for building the documentation.

```bash
	sudo apt install doxygen graphviz		#on Ubuntu
```

* qtcreator: recommended for editing project.

```bash
	sudo apt install build-essentials qt5-default qtcreator		#on Ubuntu
```

## Installing on Ubuntu (or similar)

### Using **qmake**

 Open __CheckDoseQt.pro__ file and add 
 **QT += gui widgets** at the end

```bash
	qmake CheckDoseQt.pro
```
Open the __Makefile__ and add the directory of your __Eigen__ installation in the **INCPATH** option.

For example, let's say your __Eigen__ installation path is `/usr/include/eigen-3.3.7/` then you need to add

```
	-I/usr/include/eigen-3.3.7/
```	
----

### Using **CMake**

Open __CMakeLists.txt__ file and, in case is needed, edit your **EIGEN3_DIR** and **JSON_DIR**
to match the path of your _eigen3_ and _jsoncpp_ installation path respectively.


#### Configuring with *`cmake`*

Create a folder for build binaries and configure using terminal:

```bash
	mkdir -p /path/to/checkdoseqt/build
	cd /path/to/checkdoseqt/build
	cmake /path/to/checkdoseqt
```

#### Configuring with *`Cmake`* gui app

Open CMake app (*cmake-qt-gui* if launched from terminal):

- Select source directory	(`/path/to/checkdoseqt`)
- Select where to build binaries directory	(`/path/to/checkdoseqt/build`)
- Configure
- Generate

#### Optionally build the binaries (works with cmake):

```bash
	cd /path/to/checkdoseqt/build
	make -j4 /path/to/checkdoseqt/build
```	
or 

```bash
	cmake --build /path/to/checkdoseqt/build
```	

## Installing on Windows

Contact:

[daroxs95](https://github.com/orgs/PotatoBite/people/daroxs95) or
[adabreug94](https://github.com/orgs/PotatoBite/people/adabreug94)

for instructions, but is mostly the same (plus the Windows bugs, disorder, chaos and stress).
However, deployment proccess would be realized on a x86_x64 MS Windows 10 PC 
(probably using Qt5 through mingw64), beacuse this is the main target release platafform.

## Building documentation

Use 

```bash
	doxygen Doxyfile
```	
to build documentation. Default build config uses `checkdoseqt-path/build/Docs/` as
base dir to generates the _html_ files and _.svg_ graphcis needed. 
Open the __*index.html*__ for accessing documentation.