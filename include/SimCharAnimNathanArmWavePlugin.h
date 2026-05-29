// Arkheon Simulation Technologies
// Proprietary and Confidential.
// Unauthorized copying of this file, via any medium, is strictly prohibited.
// (c) Arkheon Simulation Technologies. All rights reserved.

#pragma once

#include <plugin/IPlugin.h>

#include <string>

namespace arkheon::astsim {
class ModelFactoryRegistry;
}

namespace arkheon::sample::simcharanimnathanarmwave {

// Registers the "Arm Wave" procedural animation on animationModelNathanHuman.
// The controller is a closed-library kinematic model: it computes exactly
// 10 joint rotation overrides each evaluation frame and never applies forces,
// torques, rigid-body dynamics, or contact physics.
//
// Animation states (8-second cyclic period):
//   [0.0, 1.5)  Idle      - neutral standing pose.
//   [1.5, 3.0)  Raise Arm - right shoulder raises smoothly, elbow bends.
//   [3.0, 6.0)  Wave      - shoulder stays raised; elbow/wrist wave.
//   [6.0, 7.5)  Lower Arm - arm and posture offsets return smoothly.
//   [7.5, 8.0)  Idle      - neutral reset before repeating.
class SimCharAnimNathanArmWavePlugin final
    : public arkheon::astlib::IPlugin {
public:
    SimCharAnimNathanArmWavePlugin() = default;
    ~SimCharAnimNathanArmWavePlugin() override = default;

    [[nodiscard]] int getInterfaceVersion() const override;
    [[nodiscard]] arkheon::astlib::PluginMetadata getMetadata() const override;

    void initialize(arkheon::astlib::PluginContext& context) override;
    void tick(double dt) override;
    void shutdown() override;

private:
    bool initialized_ = false;
    bool shutdown_ = false;
    bool animationRegistered_ = false;
    std::string modelType_ = "animationModelNathanHuman";
    std::string animationCode_ = "Arm Wave";
    arkheon::astsim::ModelFactoryRegistry* modelFactoryRegistry_ = nullptr;
};

} // namespace arkheon::sample::simcharanimnathanarmwave

extern "C" {
ARKHEON_ASTLIB_API arkheon::astlib::IPlugin* create_plugin();
ARKHEON_ASTLIB_API void destroy_plugin(arkheon::astlib::IPlugin* plugin);
ARKHEON_ASTLIB_API const char* get_plugin_signature();
}
