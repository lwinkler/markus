// WARNING: This file is generated automatically !!
#ifndef ALL_MODULES_H
#define ALL_MODULES_H
#include "modules/AllModules.h"
#include "modules/BgrSubMOG2/BgrSubMOG2.h"
#include "modules/BgrSubMOG/BgrSubMOG.h"
#include "modules/BgrSubRunAvg/BgrSubRunAvg.h"
#include "modules/CascadeDetector/CascadeDetector.h"
#include "modules/Logger/Logger.h"
#include "modules/Mask/Mask.h"
#include "modules/Morph/Morph.h"
#include "modules/MotionDetector/MotionDetector.h"
#include "modules/NetworkCam/NetworkCam.h"
#include "modules/old_modules/BackgroundExtractor/BackgroundExtractor.h"
#include "modules/SegmenterBlob/SegmenterBlob.h"
#include "modules/SegmenterContour/SegmenterContour.h"
#include "modules/SimpleTracker/SimpleTracker.h"
#include "modules/SlitCam/SlitCam.h"
#include "modules/TempDiff/TempDiff.h"
#include "modules/UsbCam/UsbCam.h"
#include "modules/VideoFileReader/VideoFileReader.h"
#include "modules/VideoOutput/VideoOutput.h"
Module * createNewModule(const ConfigReader& rx_configReader)
{
Module * tmp = NULL;
const string moduleClass = rx_configReader.SubConfig("parameters").SubConfig("param", "class").GetValue();
if(false){}
else if(moduleClass.compare("BgrSubMOG2") == 0) {tmp = new BgrSubMOG2(rx_configReader);} 
else if(moduleClass.compare("BgrSubMOG") == 0) {tmp = new BgrSubMOG(rx_configReader);} 
else if(moduleClass.compare("BgrSubRunAvg") == 0) {tmp = new BgrSubRunAvg(rx_configReader);} 
else if(moduleClass.compare("CascadeDetector") == 0) {tmp = new CascadeDetector(rx_configReader);} 
else if(moduleClass.compare("Logger") == 0) {tmp = new Logger(rx_configReader);} 
else if(moduleClass.compare("Mask") == 0) {tmp = new Mask(rx_configReader);} 
else if(moduleClass.compare("Morph") == 0) {tmp = new Morph(rx_configReader);} 
else if(moduleClass.compare("MotionDetector") == 0) {tmp = new MotionDetector(rx_configReader);} 
else if(moduleClass.compare("NetworkCam") == 0) {tmp = new NetworkCam(rx_configReader);} 
else if(moduleClass.compare("SegmenterBlob") == 0) {tmp = new SegmenterBlob(rx_configReader);} 
else if(moduleClass.compare("SegmenterContour") == 0) {tmp = new SegmenterContour(rx_configReader);} 
else if(moduleClass.compare("SimpleTracker") == 0) {tmp = new SimpleTracker(rx_configReader);} 
else if(moduleClass.compare("SlitCam") == 0) {tmp = new SlitCam(rx_configReader);} 
else if(moduleClass.compare("TempDiff") == 0) {tmp = new TempDiff(rx_configReader);} 
else if(moduleClass.compare("UsbCam") == 0) {tmp = new UsbCam(rx_configReader);} 
else if(moduleClass.compare("VideoFileReader") == 0) {tmp = new VideoFileReader(rx_configReader);} 
else if(moduleClass.compare("VideoOutput") == 0) {tmp = new VideoOutput(rx_configReader);} 
else throw("Module type unknown : " + moduleClass);
return tmp;
}
#endif
