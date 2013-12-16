Markus:
=======

Markus is a program to create video detection applications. Its base philosophy is that an application can be separated into a set of simple modules. 

Each module implements a simple function (pattern detection, object tracking, background subtraction ...).

Compilation for Linux
---------------------
You will need to Install Opencv from sources (preferably). 
	http://opencv.org/downloads.html

Also Qt4 is needed.

### Dependancies
In all cases you will need:
	- *CMake*
	- *Qt4*
	- *OpenCV 2*
		- from sources http://opencv.org/downloads.html
		- or via a package installer
	- *LibTinyXML*
	- *LibLog4Cxx*
	- *awk*: used at compile time

On Ubuntu the apt-get install command is:

	sudo apt-get install cmake libqt4-core  libqt4-dev libtinyxml-dev  libopencv-core-dev libopencv-highgui-dev liblog4cxx10-dev

Other dependencies:
	- *CMake GUI* : (or cmake-curses-gui) to set the compilation flags visually
	- *Qt Creator* : a full IDE for C++ and QT
	- *Python*
	- *CppUnit** : 

	sudo apt-get install cmake-qt-gui cmake-curses-gui qt-creator python-dev libcppunit-dev

Alternatively you can compile OpenCV from sources. In this case use:

	git clone https://github.com/Itseez/opencv.git

Checkout a stable version (e.g.):

	git checkout 2.4.6.2-rc1

For compilation use the following flags.

	cmake \
        -DCMAKE_VERBOSE_MAKEFILE=ON \
        -DCMAKE_BUILD_TYPE=Debug        \
        -DBUILD_NEW_PYTHON_SUPPORT=ON   \
        -DWITH_FFMPEG=ON        \
        -DWITH_GSTREAMER=OFF    \
        -DWITH_GTK=ON   \
        -DWITH_JASPER=ON        \
        -DWITH_JPEG=ON  \
        -DWITH_PNG=ON   \
        -DWITH_TIFF=ON  \
        -DWITH_OPENEXR=ON \
        -DWITH_PVAPI=ON \
        -DWITH_UNICAP=OFF       \
        -DWITH_1394=ON  \
        -DWITH_V4L=ON   \
        -DWITH_EIGEN=ON \
        -DWITH_XINE=OFF \
        -DBUILD_TESTS=OFF \
        -DCMAKE_SKIP_RPATH=ON \
        -DWITH_CUDA=OFF

        make
        sudo make install



###  Compilation
First compilation commands:

	cmake .
	make clean
	make update_modules_list
	make

Then simply use 'make' as for any other projects. 

Adding new modules
------------------
Add a NewModule directory and NewModule.xml in modules/
then

	cmake .
	make clean
	make update_modules_list
	make

To run:
-------

run with the default configuration as defined by config.xml:

	./markus 

or run any example application present in the project/ folder:

e.g. : 
	./markus project/FourCascades.xml


Troubleshooting
===============

AllModules.h is missing
-----------------------
Solution:  simply run 

	make update_module_list


FindOpenCV.cmake is missing
---------------------------
At compilation CMake may complain that FindOpencv.cmake is missing. In this case you should compile OpenCV from sources. This problem happens in Ubuntu since version 13.04.

Technical details
=================

The philosophy of Markus is very simple to grasp. An application for video analysis consists of different modules. Each module performs one simple operation on the incoming streams. A module has the following elements:

- Input streams: incoming streams
- Output streams: outcoming streams
- Parameters: list of parameters for the module operation

The values of the different parameters and the connections between modules is defined in a XML file (You can find samples inside the **projects/** directory)

Concepts:
---------
- One module must be as simple as possible. If one module is too complex it must be split in two modules.
- In case of inheritance the list of parameters is inherited as well.


Classes
-------
- **Streams:** Streams can consist of video images, list of objects or states, ... They are inherited from the base Stream class. 
- **Objects:** An object is an entity that was detected by a module. It has a position and size that allow it to be drawn on the stream. It also possesses a list of features.
- **Features:** A double value that represent a quantity associated with an object. The mean, variance, min, max of the value is computed automatically. 
- **Templates:** Templates are primarily used in trackers they differ slighly from objects as they have a unique id and are persistent in time.

--------------------------------------------------------------------------------
Developer documentation
=======================
This section contains documentation that is meant for developers or people who need an advanced knowledge of the functionalities.

Processing and time
-------------------
A module will only process one frame if a set of conditions is met. Mainly there are two types of conditions.

- **Modules with a defined frame rate (auto_process=1):** These modules will read the input stream and process it at a rate given by the fps parameter. The ProcessFrame method of these modules will be called by a timer.
- **Module with a fixed frame rate (auto_process=0):** These modules will be called each time the previous module produces an output. If the fps parameter is 0 they will process each time after the previous module. If fps>0 they will check the time stamp from the input first.

Time values are given by a time stamp that is linked with the input stream. They match the time at which the frame was extracted by a module of the Input class.







test	
