// Copyright 2021 Toyota Research Institute
// @file
// This file is a basic maliput plugin implementation which aims to
// be a test tool in order to verify the correct behaviour of MaliputPluginManager class.
// This file shall not be modified.

#include <string>

#include "maliput/plugin/maliput_plugin_type.h"

namespace maliput {
namespace plugin {

// Implementation of these methods is a requirement of MaliputPlugin.
// @{
extern "C" std::string GetMaliputPluginId() {
  // This id is repeated on purpose in other implementation.
  return std::string("multiply_integers_test_plugin");
}
// This function must return a valid GetMaliputPluginType so MaliputPlugin can load it.
// RoadNetworkLoader type was arbitrarily selected.
extern "C" maliput::plugin::MaliputPluginType GetMaliputPluginType() {
  return maliput::plugin::MaliputPluginType::kRoadNetworkLoader;
}
// }@

// Custom function callable from MaliputPlugin::ExecuteSymbol method.
extern "C" std::string GetAString() { return "LoremIpsum"; };

}  // namespace plugin
}  // namespace maliput
