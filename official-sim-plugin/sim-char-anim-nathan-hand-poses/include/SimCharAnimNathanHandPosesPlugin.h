// Arkheon Simulation Technologies
// Student sample plugin for Nathan hand poses.

#pragma once

#include <plugin/IPlugin.h>

#include <string>

namespace arkheon::astsim {
class ModelFactoryRegistry;
}

namespace arkheon::sample::simcharanimnathanhandposes {

class SimCharAnimNathanHandPosesPlugin final
    : public arkheon::astlib::IPlugin {
public:
    SimCharAnimNathanHandPosesPlugin() = default;
    ~SimCharAnimNathanHandPosesPlugin() override = default;

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
    std::string animationCode_ = "Hand Pose Cycle";
    arkheon::astsim::ModelFactoryRegistry* modelFactoryRegistry_ = nullptr;
};

} // namespace arkheon::sample::simcharanimnathanhandposes

extern "C" {
ARKHEON_ASTLIB_API arkheon::astlib::IPlugin* create_plugin();
ARKHEON_ASTLIB_API void destroy_plugin(arkheon::astlib::IPlugin* plugin);
ARKHEON_ASTLIB_API const char* get_plugin_signature();
}
