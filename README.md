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

**CAUTION**: Be careful if you install libopencv-dev as this installs nvidia packages along. This might harm your computer http://askubuntu.com/questions/243043/after-logging-in-theres-a-black-screen-and-my-cursor-nothing-else-in-ubuntu

	sudo apt-get install cmake libqt4-core  libqt4-dev libtinyxml-dev libopencv-dev liblog4cxx10-dev libjsoncpp-dev libboost-all-dev

### Alternative: use Qt5

Markus can also be compiled with Qt5. The packages required for qt webkit are a bit more complicated in this case:
	
	sudo apt-get install qt5-default libqt5webkit5-dev qtquick1-5-dev qtlocation5-dev qtsensors5-dev qtdeclarative5-dev
	ccmake .

Then enable compilation with Qt5 in the menu.

### Other dependencies
	- *CMake GUI* : to set the compilation flags visually (packages cmake-curses-gui or cmake-qt-gui)
	- *Qt Creator* : a full IDE for C++ and QT (package qtcreator)
	- *Python*: for some modules (package python-dev  python-opencv python-sklearn python-skimage python-pip)
		- *Python deps*: sudo pip install xmltodict
	- *CppUnit** : To run the unit test (package libcppunit-dev)
	- *Curl*: To use an input camera with call to a specific webservice
	- *Avtools*: Video manipulation tools to use the benchmark of test
	- *PythonMongo and PythonGridfs*: Tools to interact with a Mongo database, for advanced use

	sudo apt-get install cmake-qt-gui cmake-curses-gui qtcreator python-dev libcppunit-dev libcurl4-openssl-dev python-opencv python-sklearn python-skimage libav-tools  python-pymongo python-gridfs

### Compile OpenCV from sources (not recommended)
Alternatively you can compile OpenCV from sources. Do not forget to install the depandencies with apt-get build-dep first:
	
	sudo apt-get build-dep libcv-dev

Clone the github repository:

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

Then simply use 'make' as for any other projects. 

Getting started
===============
Editor
------
You will get a good idea of the Markus philosophy and its modular way of thinking by opening one of the projects via the project editor:

	./markus -e projects/FourCascades.xml

or

	./markus -e projects/ObjectTracker.xml

Get a look at the different projects in the project direcory. You will see different classes of modules that are linked together. The links represent streams.

	- **Green links**: Video streams
	- **Blue links** : Streams of objects
	- **Red streams**: Streams of events
	- **Yellow streams** : Streams of states (True/False)

GUI
---
The GUI lets you visualize the work of the different modules in the project. You can see the content of each stream:

	./markus  projects/ObjectTracker.xml in/input.mp4 -p Input.loop=1

Then via the GUI menu, select:
	
	View > View 2x2

Then pick different modules and streams to be visualized.

### Note on video inputs
By convension each project should contain one module named "Input". If a video file is specified in command line this module will be overrided and the video file will be used as input.

E.g. This will use the USB camera of your laptop as input (as specified in the project XML)

	./markus projects/FourCascades.xml

This will use input.mp4 as input

	./markus projects/FourCascades.xml in/input.mp4

Adding new modules
------------------
Add a NewModule directory in modules/. You can use other modules as example  
then:

	cmake .
	make update_modules_list


Troubleshooting
===============

AllModules.h is missing or a module is unknown
-----------------------------------------------
Possible error messages:

	ERROR [main] Exception raised (string) : Module type unknown : <some_module>

Solution: You need to regenerate the list of modules simply run 

	make update_module_list
	make


FindOpenCV.cmake is missing
---------------------------
At compilation CMake may complain that FindOpencv.cmake is missing. In this case you should compile OpenCV from sources. This problem happens in Ubuntu since version 13.04.

Technical details
=================

The philosophy of Markus is very simple to grasp. An application for video analysis consists of different modules. Each module performs one simple operation on the incoming streams. A module has the following elements:

- Input streams: incoming streams
- Output streams: outcoming streams
- Parameters: list of parameters for the module operation
- Controllers: A controller allows the user to interact with the module at runtime (for advanced use). It is used for example to change the value of a parameter or to interact with a file player.

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

