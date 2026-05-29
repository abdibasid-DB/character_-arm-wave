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
#include <iterator>
#include <string>
#include <string_view>
#include <unordered_set>

namespace arkheon::sample::simcharanimnathanarmwave {
namespace {

static constexpr double kPi = 3.14159265358979323846;
static constexpr double kDeg2Rad = kPi / 180.0;

static constexpr double kFramesPerSecond = 30.0;
static constexpr double kCycleFrames = 110.0;
static constexpr double kCyclePeriod = kCycleFrames / kFramesPerSecond;

static constexpr double kRightShoulderTposeX = 0.0;
static constexpr double kRightShoulderTposeY = 0.0;
static constexpr double kRightShoulderTposeZ = 0.0;
static constexpr double kRightElbowTposeX = 0.0;
static constexpr double kRightElbowTposeY = 0.0;
static constexpr double kRightElbowTposeZ = 0.0;

static constexpr double kRightShoulderOpenX = 0.0;
static constexpr double kRightShoulderOpenY = -40.0 * kDeg2Rad;
static constexpr double kRightShoulderOpenZ = 0.0;

static constexpr double kRightWristPalmUpX = 0.0;
static constexpr double kRightWristPalmUpY = -45.0 * kDeg2Rad;
static constexpr double kRightWristPalmUpZ = 0.0;

static constexpr double kRightShoulderRaisedX = -35.0 * kDeg2Rad;
static constexpr double kRightShoulderRaisedY = -55.0 * kDeg2Rad;
static constexpr double kRightShoulderRaisedZ = 8.0 * kDeg2Rad;
static constexpr double kRightElbowRaisedX = 0.0;
static constexpr double kRightElbowRaisedY = 0.0;
static constexpr double kRightElbowRaisedZ = -95.0 * kDeg2Rad;
static constexpr double kRightWristForwardX = 0.0;
static constexpr double kRightWristForwardY = 105.0 * kDeg2Rad;
static constexpr double kRightWristForwardZ = 0.0;

static constexpr double kRightWristWaveLeftZ = 45.0 * kDeg2Rad;
static constexpr double kRightWristWaveRightZ = -45.0 * kDeg2Rad;

static constexpr double kLeftShoulderTposeX = 0.0;
static constexpr double kLeftShoulderTposeY = 0.0;
static constexpr double kLeftShoulderTposeZ = 0.0;
static constexpr double kLeftElbowTposeX = 0.0;
static constexpr double kLeftElbowTposeY = 0.0;
static constexpr double kLeftElbowTposeZ = 0.0;

static constexpr double kRightShoulderRelaxedX = 0.0;
static constexpr double kRightShoulderRelaxedY = 0.0;
static constexpr double kRightShoulderRelaxedZ = 0.0;
static constexpr double kRightElbowRelaxedX = 0.0;
static constexpr double kRightElbowRelaxedY = 0.0;
static constexpr double kRightElbowRelaxedZ = 0.0;

static constexpr double kLeftShoulderRelaxedX = 0.0;
static constexpr double kLeftShoulderRelaxedY = 0.0;
static constexpr double kLeftShoulderRelaxedZ = 0.0;
static constexpr double kLeftElbowRelaxedX = 0.0;
static constexpr double kLeftElbowRelaxedY = 0.0;
static constexpr double kLeftElbowRelaxedZ = 0.0;

static constexpr double kLeftShoulderSwingAmplitudeX = 24.0 * kDeg2Rad;
static constexpr double kLeftShoulderSwingAmplitudeY = 10.0 * kDeg2Rad;
static constexpr double kLeftShoulderSwingAmplitudeZ = -35.0 * kDeg2Rad;
static constexpr double kLeftElbowSwingAmplitudeX = 28.0 * kDeg2Rad;
static constexpr double kLeftElbowSwingAmplitudeZ = -14.0 * kDeg2Rad;

static constexpr double kRightShoulderRestX = kRightShoulderRelaxedX;
static constexpr double kRightShoulderRestY = kRightShoulderRelaxedY;
static constexpr double kRightShoulderRestZ = kRightShoulderRelaxedZ;
static constexpr double kRightElbowRestX = kRightElbowRelaxedX;
static constexpr double kRightElbowRestY = 0.0;
static constexpr double kRightElbowRestZ = kRightElbowRelaxedZ;

static constexpr double kLeftShoulderRestX = kLeftShoulderRelaxedX;
static constexpr double kLeftShoulderRestY = kLeftShoulderRelaxedY;
static constexpr double kLeftShoulderRestZ = kLeftShoulderRelaxedZ;
static constexpr double kLeftElbowRestX = kLeftElbowRelaxedX;
static constexpr double kLeftElbowRestY = kLeftElbowRelaxedY;
static constexpr double kLeftElbowRestZ = kLeftElbowRelaxedZ;

static constexpr double kHipRestX = 0.0;
static constexpr double kHipRestY = 0.0;
static constexpr double kKneeRestX = 0.0;
static constexpr double kAnkleRestX = 0.0;
static constexpr double kHipBalanceX = 1.5 * kDeg2Rad;
static constexpr double kHipBalanceY = 1.0 * kDeg2Rad;
static constexpr double kKneeBalanceX = 1.0 * kDeg2Rad;
static constexpr double kAnkleBalanceX = -0.6 * kDeg2Rad;

struct ArmPose {
    double frame;
    double shoulderX;
    double shoulderY;
    double shoulderZ;
    double elbowX;
    double elbowY;
    double elbowZ;
    double wristX;
    double wristY;
    double wristZ;
};

static constexpr ArmPose kRightArmKeyframes[] = {
    {1.0, kRightShoulderTposeX, kRightShoulderTposeY, kRightShoulderTposeZ, kRightElbowTposeX, kRightElbowTposeY, kRightElbowTposeZ, 0.0, 0.0, 0.0},
    {20.0, kRightShoulderOpenX, kRightShoulderOpenY, kRightShoulderOpenZ, kRightElbowTposeX, kRightElbowTposeY, kRightElbowTposeZ, 0.0, 0.0, 0.0},
    {30.0, kRightShoulderOpenX, kRightShoulderOpenY, kRightShoulderOpenZ, kRightElbowTposeX, kRightElbowTposeY, kRightElbowTposeZ, kRightWristPalmUpX, kRightWristPalmUpY, kRightWristPalmUpZ},
    {40.0, kRightShoulderRaisedX, kRightShoulderRaisedY, kRightShoulderRaisedZ, kRightElbowRaisedX, kRightElbowRaisedY, kRightElbowRaisedZ, 0.0, 0.0, 0.0},
    {60.0, kRightShoulderRaisedX, kRightShoulderRaisedY, kRightShoulderRaisedZ, kRightElbowRaisedX, kRightElbowRaisedY, kRightElbowRaisedZ, kRightWristForwardX, kRightWristForwardY, kRightWristForwardZ},
    {66.0, kRightShoulderRaisedX, kRightShoulderRaisedY, kRightShoulderRaisedZ, kRightElbowRaisedX, kRightElbowRaisedY, kRightElbowRaisedZ, kRightWristForwardX, 115.0 * kDeg2Rad, kRightWristWaveLeftZ},
    {72.0, kRightShoulderRaisedX, kRightShoulderRaisedY, kRightShoulderRaisedZ, kRightElbowRaisedX, kRightElbowRaisedY, kRightElbowRaisedZ, kRightWristForwardX, 95.0 * kDeg2Rad, kRightWristWaveRightZ},
    {78.0, kRightShoulderRaisedX, kRightShoulderRaisedY, kRightShoulderRaisedZ, kRightElbowRaisedX, kRightElbowRaisedY, kRightElbowRaisedZ, kRightWristForwardX, 115.0 * kDeg2Rad, kRightWristWaveLeftZ},
    {84.0, kRightShoulderRaisedX, kRightShoulderRaisedY, kRightShoulderRaisedZ, kRightElbowRaisedX, kRightElbowRaisedY, kRightElbowRaisedZ, kRightWristForwardX, 95.0 * kDeg2Rad, kRightWristWaveRightZ},
    {90.0, kRightShoulderRaisedX, kRightShoulderRaisedY, kRightShoulderRaisedZ, kRightElbowRaisedX, kRightElbowRaisedY, kRightElbowRaisedZ, kRightWristForwardX, 115.0 * kDeg2Rad, kRightWristWaveLeftZ},
    {110.0, kRightShoulderRelaxedX, kRightShoulderRelaxedY, kRightShoulderRelaxedZ, kRightElbowRelaxedX, kRightElbowRelaxedY, kRightElbowRelaxedZ, 0.0, 0.0, 0.0},
};

struct ResolvedJointSet {
    std::string rightShoulder = "rightShoulder";
    std::string rightElbow = "rightElbow";
    std::string rightWrist;
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

[[nodiscard]] double pulseBetweenFrames(double frame, double startFrame, double endFrame) noexcept {
    const double progress = clamp01((frame - startFrame) / (endFrame - startFrame));
    return std::sin(kPi * progress);
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

[[nodiscard]] std::string resolveOptionalJointId(
    const std::unordered_set<std::string>& availableJointIds,
    std::initializer_list<std::string_view> candidates) {
    if (availableJointIds.empty()) {
        return {};
    }

    for (std::string_view candidate : candidates) {
        const std::string candidateText(candidate);
        if (availableJointIds.find(candidateText) != availableJointIds.end()) {
            return candidateText;
        }
    }

    return {};
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
    joints.rightWrist = resolveOptionalJointId(
        availableJointIds,
        {"rightWrist", "rightHand", "hand_r", "wrist_r"});
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

[[nodiscard]] ArmPose interpolateRightArmPose(double frame) noexcept {
    if (frame <= kRightArmKeyframes[0].frame) {
        return kRightArmKeyframes[0];
    }

    for (int i = 1; i < static_cast<int>(std::size(kRightArmKeyframes)); ++i) {
        const ArmPose& previous = kRightArmKeyframes[i - 1];
        const ArmPose& next = kRightArmKeyframes[i];
        if (frame <= next.frame) {
            const double progress = smoothstep((frame - previous.frame) / (next.frame - previous.frame));
            return {
                frame,
                lerp(previous.shoulderX, next.shoulderX, progress),
                lerp(previous.shoulderY, next.shoulderY, progress),
                lerp(previous.shoulderZ, next.shoulderZ, progress),
                lerp(previous.elbowX, next.elbowX, progress),
                lerp(previous.elbowY, next.elbowY, progress),
                lerp(previous.elbowZ, next.elbowZ, progress),
                lerp(previous.wristX, next.wristX, progress),
                lerp(previous.wristY, next.wristY, progress),
                lerp(previous.wristZ, next.wristZ, progress),
            };
        }
    }

    return kRightArmKeyframes[std::size(kRightArmKeyframes) - 1];
}

[[nodiscard]] bool evaluateArmWaveAnimation(
    const arkheon::astsim::AnimationModelInput& input,
    arkheon::astsim::AnimationModelOutput& output) {
    static_cast<void>(input.deltaTimeSeconds);

    const double cycle = std::fmod(input.simulationTimeSeconds, kCyclePeriod);
    const double cycleFrame = 1.0 + std::fmod(cycle * kFramesPerSecond, kCycleFrames);
    const ArmPose rightArmPose = interpolateRightArmPose(cycleFrame);
    const ResolvedJointSet joints = resolveJointSet(input);

    const double jumpPulse = pulseBetweenFrames(cycleFrame, 20.0, 100.0);
    const double wavePulse = pulseBetweenFrames(cycleFrame, 60.0, 90.0);
    const double excitedSwing = std::sin((cycleFrame - 20.0) * kPi / 12.0) * jumpPulse;
    const double jumpKneeX = 42.0 * kDeg2Rad * jumpPulse;
    const double jumpHipX = -16.0 * kDeg2Rad * jumpPulse;
    const double jumpAnkleX = -22.0 * kDeg2Rad * jumpPulse;
    const double sideBalanceY = 7.0 * kDeg2Rad * wavePulse;
    const double bodyBounceX = 6.0 * kDeg2Rad * std::sin((cycleFrame - 20.0) * kPi / 8.0) * jumpPulse;

    const double leftShoulderX = kLeftShoulderRestX + (kLeftShoulderSwingAmplitudeX * jumpPulse);
    const double leftShoulderY = kLeftShoulderRestY + (kLeftShoulderSwingAmplitudeY * excitedSwing);
    const double leftShoulderZ = kLeftShoulderRestZ + (kLeftShoulderSwingAmplitudeZ * jumpPulse);
    const double leftElbowX = kLeftElbowRestX + (kLeftElbowSwingAmplitudeX * jumpPulse);
    const double leftElbowY = kLeftElbowRestY;
    const double leftElbowZ = kLeftElbowRestZ + (kLeftElbowSwingAmplitudeZ * excitedSwing);

    const double rightHipX = kHipRestX + jumpHipX + bodyBounceX;
    const double leftHipX = kHipRestX + jumpHipX + bodyBounceX;
    const double rightHipY = kHipRestY - sideBalanceY;
    const double leftHipY = kHipRestY + sideBalanceY;
    const double kneeX = kKneeRestX + jumpKneeX;
    const double ankleX = kAnkleRestX + jumpAnkleX;

    output.clearExistingJointOverrides = true;
    output.jointOverrides.clear();
    output.jointOverrides.reserve(10);

    // Right arm follows the requested X/Y/Z keyframes. Legs add a small
    // jump/bounce so all 10 output joints contribute to the motion.
    // Use a real wrist/hand joint when available; otherwise the forearm carries
    // the small hi-wave motion while the shoulder stays stable.
    output.jointOverrides.push_back({joints.rightShoulder, rightArmPose.shoulderX, rightArmPose.shoulderY, rightArmPose.shoulderZ});
    output.jointOverrides.push_back({joints.rightElbow, rightArmPose.elbowX, rightArmPose.elbowY, rightArmPose.elbowZ});
    if (!joints.rightWrist.empty()) {
        output.jointOverrides.push_back({joints.rightWrist, rightArmPose.wristX, rightArmPose.wristY, rightArmPose.wristZ});
    }
    output.jointOverrides.push_back({joints.leftShoulder, leftShoulderX, leftShoulderY, leftShoulderZ});
    output.jointOverrides.push_back({joints.leftElbow, leftElbowX, leftElbowY, leftElbowZ});
    output.jointOverrides.push_back({joints.rightHip, rightHipX, rightHipY, 0.0});
    output.jointOverrides.push_back({joints.leftHip, leftHipX, leftHipY, 0.0});
    output.jointOverrides.push_back({joints.rightKnee, kneeX, 0.0, 0.0});
    output.jointOverrides.push_back({joints.leftKnee, kneeX, 0.0, 0.0});
    output.jointOverrides.push_back({joints.rightAnkle, ankleX, 0.0, 0.0});
    if (joints.rightWrist.empty()) {
        output.jointOverrides.push_back({joints.leftAnkle, ankleX, 0.0, 0.0});
    }

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
