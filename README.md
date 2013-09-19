Markus:
=======

Markus is a program to create video detection applications. Its base philosophy is that an application can be separated into a set of simple modules. 

Each module implements a simple function (pattern detection, object tracking, background subtraction ...).

Compilation for Linux
---------------------
You will need to Install Opencv from sources (preferably). 
	http://opencv.org/downloads.html

Also Qt4 is needed.

First compilation commands:

	cmake .
	make update_module_list
	make

Then simply use 'make' as for any other projects. 

Adding new modules
------------------
Add a NewModule directory and NewModule.xml in modules/
then

	cmake .
	make update_module_list
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

Technical details
=================

The philosophy of Markus is very simple to grasp. An application for video analysis consists of different modules. Each module performs one simple operation on the incoming streams. A module has the following elements:

- Input streams: incoming streams
- Output streams: outcoming streams
- Parameters: list of parameters for the module operation

The values of the different parameters and the connections between modules is defined in a XML file (You can find samples inside the **projects/** directory)

Concepts:
---------
	- One module must be as simple as possible. If one module is too complex it must be splitted in two modules.
	- In case of inheritence the list of parameters is inherited as well.


Classes
-------
	- **Streams:** Streams can consist of video images, list of objects or states, ... They are inherited from the base Stream class
	- **Objects:** An object is an entity that was detected by a module. It has a position and size that allow it to be drawn on the stream. It also possesses a list of features.
	- **Features:** A double value that represent a quantity associated with an object. The mean, variance, min, max of the value is computed automatically. 
	- **Templates:** Templates are primarily used in trackers they differ slighly from objects 





	
