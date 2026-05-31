#include "ICharacterController.h"
#include <cmath>
#include <iostream>

namespace {

constexpr int kJointCount = 10;

float AngleFromXRotation(const arkheon_quat& q)
{
    return 2.0f * std::atan2(q.x, q.w);
}

void PrintPose(const char* label, const arkheon_bone_override out[kJointCount])
{
    std::cout << "\n" << label << "\n";
    for (int i = 0; i < kJointCount; ++i) {
        std::cout
            << "  joint[" << i << "]"
            << " apply=" << static_cast<int>(out[i].apply)
            << " angle_rad=" << AngleFromXRotation(out[i].local_rotation)
            << " quat=("
            << out[i].local_rotation.x << ", "
            << out[i].local_rotation.y << ", "
            << out[i].local_rotation.z << ", "
            << out[i].local_rotation.w << ")\n";
    }
}

void RunPose(
    void* handle,
    const char* label,
    arkheon_input_state input,
    arkheon_bone_state bones[66],
    arkheon_frame& frame,
    arkheon_env_api& env)
{
    arkheon_bone_override out[kJointCount] = {};
    arkheon_vec3 dt{};
    arkheon_quat dr{};

    for (int i = 0; i < 45; ++i) {
        frame.frame_number++;
        frame.simulation_time_s += frame.delta_time_s;
        arkheon_character_tick(
            handle,
            &frame,
            bones,
            out,
            &dt,
            &dr,
            &input,
            nullptr,
            &env
        );

        input = {};
    }

    PrintPose(label, out);
}

} // namespace

int main() {
    std::cout << "SDK: " << arkheon_character_sdk_version() << std::endl;
    std::cout << "Plugin: " << arkheon_character_plugin_name() << std::endl;

    float segs[10] = { 0 };
    void* h = arkheon_character_create(segs);

    arkheon_bone_state bones[66] = {};
    arkheon_bone_override out[10] = {};
    arkheon_vec3 dt{};
    arkheon_quat dr{};

    arkheon_frame frame{};
    frame.delta_time_s = 0.02;

    arkheon_env_api env{};

    arkheon_input_state open{};
    open.keys['4'] = 1;
    RunPose(h, "Open Palm", open, bones, frame, env);

    arkheon_input_state fist{};
    fist.hotkey_motion_a = 1;
    RunPose(h, "Fist", fist, bones, frame, env);

    arkheon_input_state point{};
    point.hotkey_motion_b = 1;
    RunPose(h, "Point", point, bones, frame, env);

    arkheon_input_state thumbs{};
    thumbs.hotkey_motion_c = 1;
    RunPose(h, "Thumbs Up", thumbs, bones, frame, env);

    arkheon_character_destroy(h);

    std::cout << "DONE" << std::endl;
}
