// Arkheon Simulation Technologies
// Proprietary and Confidential.
// Unauthorized copying of this file, via any medium, is strictly prohibited.
// (c) Arkheon Simulation Technologies. All rights reserved.

#include "SimCharAnimNathanArmWavePlugin.h"

#include <model/AnimationModel.h>
#include <model/ModelFactoryRegistry.h>
#include <plugin/IModelPluginService.h>
#include <plugin/PluginContext.h>
#include <plugin/IPluginServices.h>

#include <cmath>
#include <initializer_list>
#include <string>
#include <string_view>
#include <unordered_set>

namespace arkheon::sample::simcharanimnathanarmwave {
namespace {

static constexpr double kPi = 3.14159265358979323846;
static constexpr double kDeg2Rad = kPi / 180.0;

static constexpr double kCyclePeriod = 8.0;
static constexpr double kIdleEnd = 1.5;
static constexpr double kRaiseEnd = 3.0;
static constexpr double kWristWaveEnd = 6.0;
static constexpr double kReturnEnd = 8.0;

static constexpr double kRightShoulderRestX = 72.0 * kDeg2Rad;
static constexpr double kRightShoulderRestY = 0.0;
static constexpr double kRightShoulderRestZ = 95.0 * kDeg2Rad;
static constexpr double kRightElbowRestX = 18.0 * kDeg2Rad;
static constexpr double kRightElbowRestY = 0.0;
static constexpr double kRightElbowRestZ = 6.0 * kDeg2Rad;

static constexpr double kLeftShoulderRestX = 72.0 * kDeg2Rad;
static constexpr double kLeftShoulderRestY = 0.0;
static constexpr double kLeftShoulderRestZ = -95.0 * kDeg2Rad;
static constexpr double kLeftElbowRestX = 18.0 * kDeg2Rad;
static constexpr double kLeftElbowRestY = 0.0;
static constexpr double kLeftElbowRestZ = -6.0 * kDeg2Rad;

static constexpr double kRightShoulderRaisedX = 58.0 * kDeg2Rad;
static constexpr double kRightShoulderRaisedY = 8.0 * kDeg2Rad;
static constexpr double kRightShoulderRaisedZ = 58.0 * kDeg2Rad;
static constexpr double kRightElbowRaisedX = 108.0 * kDeg2Rad;
static constexpr double kRightElbowRaisedY = 0.0;
static constexpr double kRightElbowRaisedZ = 18.0 * kDeg2Rad;

static constexpr double kRightShoulderFoldX = 70.0 * kDeg2Rad;
static constexpr double kRightShoulderFoldY = 4.0 * kDeg2Rad;
static constexpr double kRightShoulderFoldZ = 82.0 * kDeg2Rad;
static constexpr double kRightElbowFoldX = 72.0 * kDeg2Rad;
static constexpr double kRightElbowFoldY = 0.0;
static constexpr double kRightElbowFoldZ = 10.0 * kDeg2Rad;

static constexpr double kForearmWaveAmplitudeZ = 10.0 * kDeg2Rad;
static constexpr double kWaveFrequency = 6.0;

static constexpr double kHipRestX = 0.0;
static constexpr double kHipRestY = 0.0;
static constexpr double kKneeRestX = 0.0;
static constexpr double kAnkleRestX = 0.0;
static constexpr double kHipBalanceX = 1.5 * kDeg2Rad;
static constexpr double kHipBalanceY = 1.0 * kDeg2Rad;
static constexpr double kKneeBalanceX = 1.0 * kDeg2Rad;
static constexpr double kAnkleBalanceX = -0.6 * kDeg2Rad;

enum class ArmWaveState {
    Idle,
    RaiseArm,
    WristWave,
    FoldArmAndReturn
};

struct ResolvedJointSet {
    std::string rightShoulder = "rightShoulder";
    std::string rightElbow = "rightElbow";
    std::string leftShoulder = "leftShoulder";
    std::string leftElbow = "leftElbow";
    std::string rightHip = "rightHip";
    std::string leftHip = "leftHip";
    std::string rightKnee = "rightKnee";
    std::string leftKnee = "leftKnee";
    std::string rightAnkle = "rightAnkle";
    std::string leftAnkle = "leftAnkle";
};

[[nodiscard]] double clamp01(double value) noexcept {
    if (value < 0.0) {
        return 0.0;
    }
    if (value > 1.0) {
        return 1.0;
    }
    return value;
}

[[nodiscard]] double lerp(double a, double b, double t) noexcept {
    const double u = clamp01(t);
    return a + ((b - a) * u);
}

[[nodiscard]] double smoothstep(double t) noexcept {
    const double u = clamp01(t);
    return u * u * (3.0 - (2.0 * u));
}

[[nodiscard]] ArmWaveState resolveState(double cycle) noexcept {
    if (cycle < kIdleEnd) {
        return ArmWaveState::Idle;
    }
    if (cycle < kRaiseEnd) {
        return ArmWaveState::RaiseArm;
    }
    if (cycle < kWristWaveEnd) {
        return ArmWaveState::WristWave;
    }
    return ArmWaveState::FoldArmAndReturn;
}

[[nodiscard]] std::string resolveJointId(
    const std::unordered_set<std::string>& availableJointIds,
    std::initializer_list<std::string_view> candidates,
    std::string_view fallback) {
    if (availableJointIds.empty()) {
        return std::string(*candidates.begin());
    }

    for (std::string_view candidate : candidates) {
        const std::string candidateText(candidate);
        if (availableJointIds.find(candidateText) != availableJointIds.end()) {
            return candidateText;
        }
    }

    return std::string(fallback);
}

[[nodiscard]] ResolvedJointSet resolveJointSet(
    const arkheon::astsim::AnimationModelInput& input) {
    std::unordered_set<std::string> availableJointIds;
    availableJointIds.reserve(input.entity.joints.size());
    for (const auto& joint : input.entity.joints) {
        availableJointIds.insert(joint.jointId);
    }

    ResolvedJointSet joints;
    joints.rightShoulder = resolveJointId(
        availableJointIds,
        {"rightShoulder", "rightUpperArm", "upperarm_r"},
        "rightShoulder");
    joints.rightElbow = resolveJointId(
        availableJointIds,
        {"rightElbow", "rightForearm", "lowerarm_r"},
        "rightElbow");
    joints.leftShoulder = resolveJointId(
        availableJointIds,
        {"leftShoulder", "leftUpperArm", "upperarm_l"},
        "leftShoulder");
    joints.leftElbow = resolveJointId(
        availableJointIds,
        {"leftElbow", "leftForearm", "lowerarm_l"},
        "leftElbow");
    joints.rightHip = resolveJointId(
        availableJointIds,
        {"rightHip", "rightUpperLeg", "upperleg_r"},
        "rightHip");
    joints.leftHip = resolveJointId(
        availableJointIds,
        {"leftHip", "leftUpperLeg", "upperleg_l"},
        "leftHip");
    joints.rightKnee = resolveJointId(
        availableJointIds,
        {"rightKnee", "rightLowerLeg", "lowerleg_r"},
        "rightKnee");
    joints.leftKnee = resolveJointId(
        availableJointIds,
        {"leftKnee", "leftLowerLeg", "lowerleg_l"},
        "leftKnee");
    joints.rightAnkle = resolveJointId(
        availableJointIds,
        {"rightAnkle", "rightFoot", "foot_r"},
        "rightAnkle");
    joints.leftAnkle = resolveJointId(
        availableJointIds,
        {"leftAnkle", "leftFoot", "foot_l"},
        "leftAnkle");

    return joints;
}

[[nodiscard]] bool evaluateArmWaveAnimation(
    const arkheon::astsim::AnimationModelInput& input,
    arkheon::astsim::AnimationModelOutput& output) {
    static_cast<void>(input.deltaTimeSeconds);

    const double cycle = std::fmod(input.simulationTimeSeconds, kCyclePeriod);
    const ArmWaveState state = resolveState(cycle);
    const ResolvedJointSet joints = resolveJointSet(input);

    double rightShoulderX = kRightShoulderRestX;
    double rightShoulderY = kRightShoulderRestY;
    double rightShoulderZ = kRightShoulderRestZ;
    double rightElbowX = kRightElbowRestX;
    double rightElbowY = kRightElbowRestY;
    double rightElbowZ = kRightElbowRestZ;
    double balanceProgress = 0.0;

    switch (state) {
    case ArmWaveState::Idle:
        break;

    case ArmWaveState::RaiseArm: {
        const double progress = smoothstep((cycle - kIdleEnd) / (kRaiseEnd - kIdleEnd));
        balanceProgress = progress;
        rightShoulderX = lerp(kRightShoulderRestX, kRightShoulderRaisedX, progress);
        rightShoulderY = lerp(kRightShoulderRestY, kRightShoulderRaisedY, progress);
        rightShoulderZ = lerp(kRightShoulderRestZ, kRightShoulderRaisedZ, progress);
        rightElbowX = lerp(kRightElbowRestX, kRightElbowRaisedX, progress);
        rightElbowY = lerp(kRightElbowRestY, kRightElbowRaisedY, progress);
        rightElbowZ = lerp(kRightElbowRestZ, kRightElbowRaisedZ, progress);
        break;
    }

    case ArmWaveState::WristWave: {
        const double wave = std::sin(kWaveFrequency * (cycle - kRaiseEnd));
        balanceProgress = 1.0;
        rightShoulderX = kRightShoulderRaisedX;
        rightShoulderY = kRightShoulderRaisedY;
        rightShoulderZ = kRightShoulderRaisedZ;
        rightElbowX = kRightElbowRaisedX;
        rightElbowY = kRightElbowRaisedY;
        rightElbowZ = kRightElbowRaisedZ + (kForearmWaveAmplitudeZ * wave);
        break;
    }

    case ArmWaveState::FoldArmAndReturn: {
        const double returnTime = cycle - kWristWaveEnd;
        const double foldProgress = smoothstep(returnTime / ((kReturnEnd - kWristWaveEnd) * 0.35));
        const double lowerProgress = smoothstep((returnTime - ((kReturnEnd - kWristWaveEnd) * 0.35)) / ((kReturnEnd - kWristWaveEnd) * 0.65));
        balanceProgress = 1.0 - lowerProgress;

        const double foldedShoulderX = lerp(kRightShoulderRaisedX, kRightShoulderFoldX, foldProgress);
        const double foldedShoulderY = lerp(kRightShoulderRaisedY, kRightShoulderFoldY, foldProgress);
        const double foldedShoulderZ = lerp(kRightShoulderRaisedZ, kRightShoulderFoldZ, foldProgress);
        const double foldedElbowX = lerp(kRightElbowRaisedX, kRightElbowFoldX, foldProgress);
        const double foldedElbowY = lerp(kRightElbowRaisedY, kRightElbowFoldY, foldProgress);
        const double foldedElbowZ = lerp(kRightElbowRaisedZ, kRightElbowFoldZ, foldProgress);

        rightShoulderX = lerp(foldedShoulderX, kRightShoulderRestX, lowerProgress);
        rightShoulderY = lerp(foldedShoulderY, kRightShoulderRestY, lowerProgress);
        rightShoulderZ = lerp(foldedShoulderZ, kRightShoulderRestZ, lowerProgress);
        rightElbowX = lerp(foldedElbowX, kRightElbowRestX, lowerProgress);
        rightElbowY = lerp(foldedElbowY, kRightElbowRestY, lowerProgress);
        rightElbowZ = lerp(foldedElbowZ, kRightElbowRestZ, lowerProgress);
        break;
    }
    }

    const double rightHipX = kHipRestX + (kHipBalanceX * balanceProgress);
    const double leftHipX = kHipRestX + (kHipBalanceX * balanceProgress);
    const double rightHipY = kHipRestY - (kHipBalanceY * balanceProgress);
    const double leftHipY = kHipRestY + (kHipBalanceY * balanceProgress);
    const double kneeX = kKneeRestX + (kKneeBalanceX * balanceProgress);
    const double ankleX = kAnkleRestX + (kAnkleBalanceX * balanceProgress);

    output.clearExistingJointOverrides = true;
    output.jointOverrides.clear();
    output.jointOverrides.reserve(10);

    // Four-state natural wave. Nathan has no wrist/hand joint, so rightElbow
    // is the forearm proxy for the small bye-bye wave while the shoulder holds.
    output.jointOverrides.push_back({joints.rightShoulder, rightShoulderX, rightShoulderY, rightShoulderZ});
    output.jointOverrides.push_back({joints.rightElbow, rightElbowX, rightElbowY, rightElbowZ});
    output.jointOverrides.push_back({joints.leftShoulder, kLeftShoulderRestX, kLeftShoulderRestY, kLeftShoulderRestZ});
    output.jointOverrides.push_back({joints.leftElbow, kLeftElbowRestX, kLeftElbowRestY, kLeftElbowRestZ});
    output.jointOverrides.push_back({joints.rightHip, rightHipX, rightHipY, 0.0});
    output.jointOverrides.push_back({joints.leftHip, leftHipX, leftHipY, 0.0});
    output.jointOverrides.push_back({joints.rightKnee, kneeX, 0.0, 0.0});
    output.jointOverrides.push_back({joints.leftKnee, kneeX, 0.0, 0.0});
    output.jointOverrides.push_back({joints.rightAnkle, ankleX, 0.0, 0.0});
    output.jointOverrides.push_back({joints.leftAnkle, ankleX, 0.0, 0.0});

    return output.jointOverrides.size() == 10;
}

} // namespace

int SimCharAnimNathanArmWavePlugin::getInterfaceVersion() const {
    return 1;
}

arkheon::astlib::PluginMetadata SimCharAnimNathanArmWavePlugin::getMetadata() const {
    arkheon::astlib::PluginMetadata metadata;
    metadata.setPluginId("sim-char-anim-nathan-arm-wave");
    metadata.setVersion("1.0.0");
    metadata.setAuthor("Arkheon Sample");
    return metadata;
}

void SimCharAnimNathanArmWavePlugin::initialize(arkheon::astlib::PluginContext& context) {
    initialized_ = true;
    shutdown_ = false;
    animationRegistered_ = false;
    modelType_ = "animationModelNathanHuman";
    animationCode_ = "Arm Wave";

    modelFactoryRegistry_ = nullptr;
    if (context.services) {
        auto* rawService = context.services->getService(
            arkheon::astsim::IModelPluginService::kPluginServiceId);
        auto* service = static_cast<arkheon::astsim::IModelPluginService*>(rawService);
        modelFactoryRegistry_ = service ? &service->modelFactoryRegistry() : nullptr;
    }

    if (!modelFactoryRegistry_) {
        return;
    }

    auto* prototypeBase = modelFactoryRegistry_->getRegisteredPrototype(modelType_);
    auto* prototypeAnimationModel =
        dynamic_cast<arkheon::astsim::IAnimationModel*>(prototypeBase);
    if (!prototypeAnimationModel) {
        return;
    }

    animationRegistered_ = prototypeAnimationModel->registerAnimation(
        animationCode_,
        evaluateArmWaveAnimation);
}

void SimCharAnimNathanArmWavePlugin::tick(double dt) {
    static_cast<void>(dt);
    if (!initialized_ || shutdown_ || !modelFactoryRegistry_) {
        return;
    }
}

void SimCharAnimNathanArmWavePlugin::shutdown() {
    if (modelFactoryRegistry_ && animationRegistered_) {
        auto* prototypeBase = modelFactoryRegistry_->getRegisteredPrototype(modelType_);
        auto* prototypeAnimationModel =
            dynamic_cast<arkheon::astsim::IAnimationModel*>(prototypeBase);
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

} // namespace arkheon::sample::simcharanimnathanarmwave

extern "C" {

ARKHEON_ASTLIB_API arkheon::astlib::IPlugin* create_plugin() {
    return new arkheon::sample::simcharanimnathanarmwave::SimCharAnimNathanArmWavePlugin();
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
