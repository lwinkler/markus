Markus:
=======

Markus is a program to create video detection applications. Its base philosophy is that an application can be separated into a set of simple modules. 

Each module implements a simple function (pattern detection, object tracking, background subtraction ...). Each module can have input an output streams of different types.

Compilation for Linux
---------------------

### Dependancies
In all cases you will need the following packages to compile Markus:
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

These libraries and utilities can be installed to create advanced modules:
	- *CMake GUI* : to set the compilation flags visually (packages cmake-curses-gui or cmake-qt-gui)
	- *Qt Creator* : a full IDE for C++ and QT (package qtcreator)
	- *Python*: for some modules (package python-dev  python-opencv python-sklearn python-skimage python-pip)
		- *Python deps*: sudo pip install xmltodict
		- *Nagios plugin*: for monitoring: sudo pip install nagiosplugin
		- *PythonMongo and PythonGridfs*: Tools to interact with a Mongo database, for advanced use
	- *CxxTests** : To run the unit test
	- *Curl*: To use an input camera with call to a specific webservice
	- *Avtools*: Video manipulation tools to use the benchmark of test

	sudo apt-get install cmake-qt-gui cmake-curses-gui qtcreator python-dev cxxtest libcurl4-openssl-dev python-opencv python-sklearn python-skimage libav-tools  python-pymongo python-gridfs

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

A few useful tricks:
* Left click on a stream lets you select different controls to send to the module: e.g. change the value of a parameter or set the position on a video file.
* Double click on a stream prints info in logs based on cursor position: e.g. value of pixel or object.
* The last GUI configuration for each XML is saved automatically.
* A last_config.xml file is automatically created. It contains the last project with the modifications made during the run.

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

AllModules.cpp is missing or a module is unknown
------------------------------------------------
Possible error messages:

	ERROR [main] Exception raised (string) : Module type unknown : <some_module>

Solution: You need to regenerate the list of modules. Simply run 

	cmake .
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
- Controllers: A controller allows the user to interact with the module at runtime (for advanced use). It is used for example to change the value of a parameter or to interact with a file player.

The values of the different parameters and the connections between modules is defined in a XML file (You can find samples inside the **projects/** directory)

Concepts:
---------
- One module must be as simple as possible. If one module is too complex it must be split into two different modules.
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


Simulations (advanced)
-----------
Markus can also create create variations of an XML configuration. It can be used to:
- Vary the resolution of certain modules
- Vary the values of any parameter in an XML config
- Use the same XML config on a list of videos

### Definition in XML
The variations can be specified directly in the XML config (see examples below) and can be used to generate a simulation directory with:

	./markus -S projects/ObjectTracker.sim.xml

	2015-05-29 10:54:12,199 [0x7f0c7389ea40] INFO  [Manager] Create manager
	2015-05-29 10:54:12,199 [0x7f0c7389ea40] INFO  [Module] Create module Input
	2015-05-29 10:54:12,199 [0x7f0c7389ea40] INFO  [Module] Create module RenderObjects
	2015-05-29 10:54:12,199 [0x7f0c7389ea40] INFO  [Module] Create module BgrSubRunAvg
	2015-05-29 10:54:12,199 [0x7f0c7389ea40] INFO  [Module] Create module TrackerByFeatures
	2015-05-29 10:54:12,200 [0x7f0c7389ea40] INFO  [Module] Create module SegmenterContour
	2015-05-29 10:54:12,241 [0x7f0c7389ea40] INFO  [Simulation] 5 simulations generated in directory simulation_20150529_105412
	2015-05-29 10:54:12,241 [0x7f0c7389ea40] INFO  [Simulation] Launch with: make -f simulation_20150529_105412/simulation.make -j4
	2015-05-29 10:54:12,241 [0x7f0c7389ea40] INFO  [Manager] Manager: 0 frames processed in 0 ms (-nan frames/s)
	2015-05-29 10:54:12,245 [0x7f0c7389ea40] INFO  [Module] Module Input : 0 frames processed (tproc=0ms, tconv=0ms, twait=0ms), -nan fps
	2015-05-29 10:54:12,245 [0x7f0c7389ea40] INFO  [Module] Module RenderObjects : 0 frames processed (tproc=0ms, tconv=0ms, twait=0ms), -nan fps
	2015-05-29 10:54:12,246 [0x7f0c7389ea40] INFO  [Module] Module BgrSubRunAvg : 0 frames processed (tproc=0ms, tconv=0ms, twait=0ms), -nan fps
	2015-05-29 10:54:12,246 [0x7f0c7389ea40] INFO  [Module] Module TrackerByFeatures : 0 frames processed (tproc=0ms, tconv=0ms, twait=0ms), -nan fps
	2015-05-29 10:54:12,246 [0x7f0c7389ea40] INFO  [Module] Module SegmenterContour : 0 frames processed (tproc=0ms, tconv=0ms, twait=0ms), -nan fps
	2015-05-29 10:54:12,258 [0x7f0c7389ea40] INFO  [Context] Removing empty directory out/out_20150529_105412


#### Example 1
Here we have a simulation with 2 variations. The first variation changes the value of max_matching_distance in TrackerByFeatures (10 values are chosen in range 0 to 100) and the second changes the resoultion based on the JSON file *tools/evaluation/ranges/resolutions.json* (3 different resolutions).

	<application name="ObjectTracker" description="">
		<variations>
			<var module="TrackerByFeatures" param="max_matching_distance" nb="10 range="[0:100]"/>
			<var module="TrackerByFeatures" parameters="width,height" references="width,height" file="tools/evaluation/ranges/resolutions.json"/>
		</variations>
		<module name="Input" id="0">
		...

The result is a simulation directory containing 30 different run of markus.

#### Example 2
Here we have a simulation with 1 variation. It changes the video file to use (as specified in the JSON file) as well as other parameters (LogEvent.gt_file,manager.arguments,LogEvent.gt_video):

Notes:
- *manager.arguments* can add extra arguments to the markus command: here we use it to override part of the config (-x SZTR100.xml) with an XML config related to the video.
- Unlike example 1 this will only generate one run of markus per video. This is one single variation, not 2.

	<variations>
	<var modules="Input,LogEvent,manager,LogEvent" parameters="file,gt_file,arguments,gt_video" references="video,annotation,arguments,video" file="my_videos.json">
	</var>
	</variations>

*my_video.json*:

	{
		"SZTRA101a01": {
			"video": "videos/ilids_SterileZone/SZTR/video/SZTRA101a01.mov.mkv",
				"annotation": "video-annotations/ilids_SterileZone/SZTR/video/SZTRA101a01.srt",
				"arguments": "-x video-annotations/ilids_SterileZone/SZTR/video/SZTR100.xml"
		},

		"SZTRA101a02": {
			"video": "videos/ilids_SterileZone/SZTR/video/SZTRA101a02.mov.mkv",
			"annotation": "video-annotations/ilids_SterileZone/SZTR/video/SZTRA101a02.srt",
			"arguments": "-x video-annotations/ilids_SterileZone/SZTR/video/SZTR100.xml"
		},
		...
	}

### Running a simulation
he simulation directory contains all files needed to run the different variations as well as a makefile. A simulation can be run with:

	make -f simulation_20141202_143429/simulation.make -j4

Where the -j argument passed to make specifies the number of simultaneous process to run. This should match the number of cores.

### Aggregating results
To get aggregated results (summaries of different variation runs on the same html) you can use different scripts available with Markus.To aggregate all the results:

	tools/evaluation/aggregate.py simulation_20141202_143429/results/

To aggregate results with respect to one variation of one parameter you may use the .txt files that are automatically generated in your simulation directory. They contain a list of parameter variations to aggregate. They may be in the agr/ directory.

E.g. You have configured a variation on each video file of one set that contains one variation of the resolution (small and medium). Then use this command to aggregate all results that were made with a small resolution.

	tools/evaluation/aggregate.py simulation_20141128_170932/results -f simulation_20141128_170932/small.txt -o small.html
