Markus:
=======

Markus is a program to create video detection applications. Its base philosophy is that an application can be separated into a set of simple modules. 

Each module implements a simple function (pattern detection, object tracking, background subtraction ...).

Compilation for Linux
---------------------
You will need to Install Opencv from sources (preferably). 
	http://opencv.org/downloads.html

Also Qt4 is needed.

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
