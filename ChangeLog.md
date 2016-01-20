Known problems
==============
- **Module UsbCam**: There is a crash if we reset this module through a command while using the GUI. This happens because the module is reseted while the Capture thread has not finished


Improvements
============
- Deserialization SHOULD ALWAYS check if the value exists
- JsonCpp has a bug for serializing floats !
- Check that all parameters are always initialized
