// WARNING: This file is generated automatically !!


#include "modules/BlobSegmenter/BlobSegmenter.h"
#include "modules/SlitCam/SlitCam.h"
#include "modules/VideoOutput/VideoOutput.h"
#include "modules/BackgroundSubtractorSimple/BackgroundSubtractorSimple.h"
#include "modules/VideoFileReader/VideoFileReader.h"
#include "modules/TemporalDifferencing/TemporalDifferencing.h"
#include "modules/CascadeDetector/CascadeDetector.h"
#include "modules/UsbCam/UsbCam.h"
#include "modules/SimpleTracker/SimpleTracker.h"
#include "modules/NetworkCam/NetworkCam.h"
#include "modules/BackgroundExtractor/BackgroundExtractor.h"
#include "modules/AnalyseStatistics/Statistics.h"
Module * createNewModule(const ConfigReader& rx_configReader)
{
Module * tmp = NULL;
const string moduleClass = rx_configReader.SubConfig("parameters").SubConfig("param", "class").GetValue();
if(false){}
else if(moduleClass.compare("Statistics") == 0) {tmp = new Statistics(rx_configReader);} 
else if(moduleClass.compare("BackgroundExtractor") == 0) {tmp = new BackgroundExtractor(rx_configReader);} 
else if(moduleClass.compare("BackgroundSubtractorSimple") == 0) {tmp = new BackgroundSubtractorSimple(rx_configReader);} 
else if(moduleClass.compare("BlobSegmenter") == 0) {tmp = new BlobSegmenter(rx_configReader);} 
else if(moduleClass.compare("CascadeDetector") == 0) {tmp = new CascadeDetector(rx_configReader);} 
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

