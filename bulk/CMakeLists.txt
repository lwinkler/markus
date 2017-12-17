
add_library(bulk
Factories.cpp
Module.cpp
ModuleClassifyEvents.cpp
ModulePython.cpp
Manager.cpp
Context.cpp
MkDirectory.cpp
Input.cpp
BackgroundSubtraction.cpp
Parameter.cpp
ParameterEnum.cpp
ParameterStructure.cpp
ParameterT.cpp
Polygon.cpp
Stream.cpp
StreamImage.cpp
StreamEvent.cpp
StreamObject.cpp
StreamState.cpp
StreamNum.cpp
Object.cpp
Event.cpp
FeatureFloatInTime.cpp
FeatureHistory.cpp
FeatureVector.cpp
Configurable.cpp
ConfigXml.cpp
ModuleTimer.cpp
MkException.cpp
Controller.cpp
ControllerParameterT.cpp
ControllerInputStream.cpp
ControllerModule.cpp
ControllerManager.cpp
ControllerEvent.cpp
ControllerBackground.cpp
Processable.cpp
Simulation.cpp
InterruptionManager.cpp
ParameterEnumT.gen.cpp
)

# Work in progress to add precompiled headers for Module.h
# add_library(Module.h.gch
# 	Module.h
# )
# set_target_properties(Module.h.gch PROPERTIES LINKER_LANGUAGE C)
# target_compile_options(Module.h.gch BEFORE PUBLIC -x c++-header)
