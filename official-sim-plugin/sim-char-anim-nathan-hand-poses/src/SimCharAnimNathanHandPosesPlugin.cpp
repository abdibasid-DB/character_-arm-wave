// Arkheon Simulation Technologies
// Student sample plugin for Nathan hand poses.

#include "SimCharAnimNathanHandPosesPlugin.h"

#include <model/AnimationModel.h>
#include <model/ModelFactoryRegistry.h>
#include <plugin/IModelPluginService.h>
#include <plugin/PluginContext.h>
#include <plugin/IPluginServices.h>

#include <array>
#include <cmath>
#include <string>
#include <unordered_set>

namespace arkheon::sample::simcharanimnathanhandposes {
namespace {

struct JointAlias {
    const char* primary;
    const char* fallback;
};

struct Pose {
    const char* name;
    std::array<double, 10> zCurl;
    std::array<double, 10> ySpread;
};

constexpr std::array<JointAlias, 10> kJointAliases = {{
    {"thumbBase", "leftAnkle"},
    {"thumbTip", "rightAnkle"},
    {"indexBase", "leftKnee"},
    {"indexTip", "rightKnee"},
    {"middleBase", "leftHip"},
    {"middleTip", "rightHip"},
    {"ringBase", "leftShoulder"},
    {"ringTip", "rightShoulder"},
    {"pinkyBase", "leftElbow"},
    {"pinkyTip", "rightElbow"},
}};

constexpr Pose kFist {
    "Fist",
    {0.58, 0.46, 1.72, 2.05, 1.72, 2.05, 1.72, 2.05, 1.72, 2.05},
    {0.82, 0.48, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0}
};

constexpr Pose kPoint {
    "Point",
    {0.58, 0.46, -0.18, -0.04, 1.72, 2.05, 1.72, 2.05, 1.72, 2.05},
    {0.82, 0.48, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0}
};

constexpr Pose kThumbsUp {
    "Thumbs Up",
    {0.18, 0.10, 1.72, 2.05, 1.72, 2.05, 1.72, 2.05, 1.72, 2.05},
    {-1.42, -1.05, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0}
};

constexpr Pose kOpenPalm {
    "Open Palm",
    {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
    {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0}
};

[[nodiscard]] bool hasJoint(
    const std::unordered_set<std::string>& availableJointIds,
    const char* jointId) {
    if (!jointId || *jointId == '\0') {
        return false;
    }
    if (availableJointIds.empty()) {
        return true;
    }
    return availableJointIds.find(jointId) != availableJointIds.end();
}

[[nodiscard]] const char* selectJointId(
    const std::unordered_set<std::string>& availableJointIds,
    const JointAlias& alias) {
    if (hasJoint(availableJointIds, alias.primary)) {
        return alias.primary;
    }
    if (hasJoint(availableJointIds, alias.fallback)) {
        return alias.fallback;
    }
    return nullptr;
}

[[nodiscard]] const Pose& poseAt(double localTimeSeconds) {
    const double cycle = std::fmod(localTimeSeconds, 40.0);
    if (cycle < 10.0) {
        return kFist;
    }
    if (cycle < 20.0) {
        return kPoint;
    }
    if (cycle < 30.0) {
        return kThumbsUp;
    }
    return kOpenPalm;
}

[[nodiscard]] bool evaluateHandPoseCycle(
    const arkheon::astsim::AnimationModelInput& input,
    arkheon::astsim::AnimationModelOutput& output) {
    std::unordered_set<std::string> availableJointIds;
    availableJointIds.reserve(input.entity.joints.size());
    for (const auto& joint : input.entity.joints) {
        availableJointIds.insert(joint.jointId);
    }

    const Pose& pose = poseAt(input.simulationTimeSeconds);

    output.clearExistingJointOverrides = true;
    output.jointOverrides.clear();
    output.jointOverrides.reserve(kJointAliases.size());

    for (std::size_t i = 0; i < kJointAliases.size(); ++i) {
        const char* jointId = selectJointId(availableJointIds, kJointAliases[i]);
        if (!jointId) {
            continue;
        }

        output.jointOverrides.push_back({jointId, 0.0, pose.ySpread[i], pose.zCurl[i]});
    }

    return !output.jointOverrides.empty();
}

} // namespace

int SimCharAnimNathanHandPosesPlugin::getInterfaceVersion() const {
    return 1;
}

arkheon::astlib::PluginMetadata SimCharAnimNathanHandPosesPlugin::getMetadata() const {
    arkheon::astlib::PluginMetadata metadata;
    metadata.setPluginId("sim-char-anim-nathan-hand-poses");
    metadata.setVersion("1.0.0");
    metadata.setAuthor("Abdirahman Yahya");
    return metadata;
}

void SimCharAnimNathanHandPosesPlugin::initialize(arkheon::astlib::PluginContext& context) {
    initialized_ = true;
    shutdown_ = false;
    animationRegistered_ = false;
    modelType_ = "animationModelNathanHuman";
    animationCode_ = "Hand Pose Cycle";

    modelFactoryRegistry_ = nullptr;
    if (context.services) {
        auto* rawService = context.services->getService(arkheon::astsim::IModelPluginService::kPluginServiceId);
        auto* service = static_cast<arkheon::astsim::IModelPluginService*>(rawService);
        modelFactoryRegistry_ = service ? &service->modelFactoryRegistry() : nullptr;
    }

    if (!modelFactoryRegistry_) {
        return;
    }

    auto* prototypeBase = modelFactoryRegistry_->getRegisteredPrototype(modelType_);
    auto* prototypeAnimationModel = dynamic_cast<arkheon::astsim::IAnimationModel*>(prototypeBase);
    if (!prototypeAnimationModel) {
        return;
    }

    animationRegistered_ = prototypeAnimationModel->registerAnimation(
        animationCode_,
        evaluateHandPoseCycle);
}

void SimCharAnimNathanHandPosesPlugin::tick(double dt) {
    static_cast<void>(dt);
    if (!initialized_ || shutdown_ || !modelFactoryRegistry_) {
        return;
    }
}

void SimCharAnimNathanHandPosesPlugin::shutdown() {
    if (modelFactoryRegistry_ && animationRegistered_) {
        auto* prototypeBase = modelFactoryRegistry_->getRegisteredPrototype(modelType_);
        auto* prototypeAnimationModel = dynamic_cast<arkheon::astsim::IAnimationModel*>(prototypeBase);
        if (prototypeAnimationModel) {
            static_cast<void>(prototypeAnimationModel->registerAnimation(
                animationCode_,
                arkheon::astsim::IAnimationModel::AnimationEvaluationFunction {}));
        }
    }
    animationRegistered_ = false;
    shutdown_ = true;
    modelFactoryRegistry_ = nullptr;
}

} // namespace arkheon::sample::simcharanimnathanhandposes

extern "C" {

ARKHEON_ASTLIB_API arkheon::astlib::IPlugin* create_plugin() {
    return new arkheon::sample::simcharanimnathanhandposes::SimCharAnimNathanHandPosesPlugin();
}

ARKHEON_ASTLIB_API void destroy_plugin(arkheon::astlib::IPlugin* plugin) {
    if (plugin) {
        delete plugin;
    }
}

ARKHEON_ASTLIB_API const char* get_plugin_signature() {
    return "ARKHEON_PLUGIN_V1";
}

} // extern "C"
