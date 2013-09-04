// WARNING: This file is generated automatically !!
#ifndef ALL_MODULES_H
#define ALL_MODULES_H
#include "modules/AllModules.h"
#include "modules/BackgroundExtractor/BackgroundExtractor.h"
#include "modules/BackgroundSubtractorSimple/BackgroundSubtractorSimple.h"
#include "modules/BgrSubMOG2/BgrSubMOG2.h"
#include "modules/BgrSubMOG/BgrSubMOG.h"
#include "modules/BlobSegmenter/BlobSegmenter.h"
#include "modules/CascadeDetector/CascadeDetector.h"
#include "modules/Logger/Logger.h"
#include "modules/Morph/Morph.h"
#include "modules/MotionDetector/MotionDetector.h"
#include "modules/NetworkCam/NetworkCam.h"
#include "modules/SimpleTracker/SimpleTracker.h"
#include "modules/SlitCam/SlitCam.h"
#include "modules/TemporalDifferencing/TemporalDifferencing.h"
#include "modules/UsbCam/UsbCam.h"
#include "modules/VideoFileReader/VideoFileReader.h"
#include "modules/VideoOutput/VideoOutput.h"
Module * createNewModule(const ConfigReader& rx_configReader)
{
Module * tmp = NULL;
const string moduleClass = rx_configReader.SubConfig("parameters").SubConfig("param", "class").GetValue();
if(false){}
else if(moduleClass.compare("BackgroundExtractor") == 0) {tmp = new BackgroundExtractor(rx_configReader);} 
else if(moduleClass.compare("BackgroundSubtractorSimple") == 0) {tmp = new BackgroundSubtractorSimple(rx_configReader);} 
else if(moduleClass.compare("BgrSubMOG2") == 0) {tmp = new BgrSubMOG2(rx_configReader);} 
else if(moduleClass.compare("BgrSubMOG") == 0) {tmp = new BgrSubMOG(rx_configReader);} 
else if(moduleClass.compare("BlobSegmenter") == 0) {tmp = new BlobSegmenter(rx_configReader);} 
else if(moduleClass.compare("CascadeDetector") == 0) {tmp = new CascadeDetector(rx_configReader);} 
else if(moduleClass.compare("Logger") == 0) {tmp = new Logger(rx_configReader);} 
else if(moduleClass.compare("Morph") == 0) {tmp = new Morph(rx_configReader);} 
else if(moduleClass.compare("MotionDetector") == 0) {tmp = new MotionDetector(rx_configReader);} 
else if(moduleClass.compare("NetworkCam") == 0) {tmp = new NetworkCam(rx_configReader);} 
else if(moduleClass.compare("SimpleTracker") == 0) {tmp = new SimpleTracker(rx_configReader);} 
else if(moduleClass.compare("SlitCam") == 0) {tmp = new SlitCam(rx_configReader);} 
else if(moduleClass.compare("TemporalDifferencing") == 0) {tmp = new TemporalDifferencing(rx_configReader);} 
else if(moduleClass.compare("UsbCam") == 0) {tmp = new UsbCam(rx_configReader);} 
else if(moduleClass.compare("VideoFileReader") == 0) {tmp = new VideoFileReader(rx_configReader);} 
else if(moduleClass.compare("VideoOutput") == 0) {tmp = new VideoOutput(rx_configReader);} 
else throw("Module type unknown : " + moduleClass);
return tmp;
}
#endif
