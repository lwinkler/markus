Master branch
=============
- Implement different types of features (string, vector of floats, ...)
- Support for Aegisub .ass subtitle files
- Rewrite part of TrackerByFeatures
- Create polygon zones as parameters

Release 1.3.6
=============
- Release of Markus: Work on stability

Known problems
==============
- **Module UsbCam**: There is a crash if we reset this module through a command while using the GUI. This happens because the module is reseted while the Capture thread has not finished


Improvements
============
- Deserialization SHOULD ALWAYS check if the value exists
- JsonCpp has a bug for serializing floats ! The precision is often too high.
- Check that all parameters are always initialized
- InterruptionManager should be its own module and be automatically connected
- Simulations should handle caching automatically
