#include "pch.h"
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include "ICharacterController.h"

namespace {

constexpr int kJointCount = 10;
constexpr float kBlendSpeed = 8.0f;

enum HandPose {
    OpenPalm = 0,
    Fist = 1,
    Point = 2,
    ThumbsUp = 3
};

struct PoseDefinition {
    const char* name;
    float angles[kJointCount];
};

// Angles are radians. Joint order:
// thumb_base, thumb_tip, index_base, index_tip, middle_base, middle_tip,
// ring_base, ring_tip, pinky_base, pinky_tip.
constexpr PoseDefinition kPoses[] = {
    { "Open Palm", { 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f } },
    { "Fist",      { 0.95f, 0.85f, 1.25f, 1.10f, 1.30f, 1.12f, 1.25f, 1.08f, 1.15f, 0.98f } },
    { "Point",     { 0.90f, 0.75f, 0.05f, 0.00f, 1.25f, 1.10f, 1.22f, 1.05f, 1.10f, 0.95f } },
    { "Thumbs Up", { 0.00f, 0.00f, 1.25f, 1.08f, 1.28f, 1.10f, 1.22f, 1.05f, 1.12f, 0.95f } }
};

const char* PoseName(HandPose pose)
{
    return kPoses[static_cast<int>(pose)].name;
}

float Clamp01(float value)
{
    if (value < 0.0f) return 0.0f;
    if (value > 1.0f) return 1.0f;
    return value;
}

arkheon_quat RotationForFingerCurl(float angle_rad)
{
    const float x = angle_rad;
    const float y = angle_rad * 0.35f;
    const float z = -angle_rad * 0.55f;
    const float cx = cosf(x * 0.5f);
    const float sx = sinf(x * 0.5f);
    const float cy = cosf(y * 0.5f);
    const float sy = sinf(y * 0.5f);
    const float cz = cosf(z * 0.5f);
    const float sz = sinf(z * 0.5f);

    return arkheon_quat{
        (sx * cy * cz) + (cx * sy * sz),
        (cx * sy * cz) - (sx * cy * sz),
        (cx * cy * sz) + (sx * sy * cz),
        (cx * cy * cz) - (sx * sy * sz)
    };
}

bool IsKeyPressed(const arkheon_input_state* input, int key)
{
    return input && key >= 0 && key < 256 && input->keys[key] != 0;
}

} // namespace

struct Controller {
    HandPose target_pose = OpenPalm;
    HandPose printed_pose = OpenPalm;
    float current_angles[kJointCount] = {};
};

extern "C" {

    __declspec(dllexport) uint32_t arkheon_character_sdk_version(void) {
        return ARKHEON_CHARACTER_SDK_VERSION;
    }

    __declspec(dllexport) const char* arkheon_character_plugin_name(void) {
        return "Hand Motion Plugin";
    }

    __declspec(dllexport) void arkheon_character_get_motion_clips(
        void* handle, int32_t out_clip_ids[3])
    {
        out_clip_ids[0] = 0;
        out_clip_ids[1] = 1;
        out_clip_ids[2] = 2;
    }

    __declspec(dllexport) void* arkheon_character_create(
        const float segment_lengths_m[10])
    {
        (void)segment_lengths_m;
        Controller* controller = new Controller();
        printf("[HandMotionPlugin] Created. Keys: 1=Fist, 2=Point, 3=Thumbs Up, 4=Open Palm.\n");
        return controller;
    }

    __declspec(dllexport) void arkheon_character_destroy(void* handle) {
        delete static_cast<Controller*>(handle);
    }

    __declspec(dllexport) int32_t arkheon_character_tick(
        void* handle,
        const arkheon_frame* frame,
        const arkheon_bone_state in_bones[66],
        arkheon_bone_override out_overrides[10],
        arkheon_vec3* out_root_translation_delta,
        arkheon_quat* out_root_rotation_delta,
        const arkheon_input_state* input,
        const arkheon_mission_goal* goal,
        const arkheon_env_api* env)
    {
        Controller* c = static_cast<Controller*>(handle);

        if (!c || !out_root_translation_delta || !out_root_rotation_delta || !out_overrides) {
            return 1;
        }

        (void)in_bones;
        (void)goal;
        (void)env;

        *out_root_translation_delta = arkheon_vec3{ 0.0f, 0.0f, 0.0f };
        *out_root_rotation_delta = arkheon_quat{ 0.0f, 0.0f, 0.0f, 1.0f };

        if (input) {
            if (input->hotkey_motion_a || IsKeyPressed(input, '1')) c->target_pose = Fist;
            if (input->hotkey_motion_b || IsKeyPressed(input, '2')) c->target_pose = Point;
            if (input->hotkey_motion_c || IsKeyPressed(input, '3')) c->target_pose = ThumbsUp;
            if (IsKeyPressed(input, '4') || IsKeyPressed(input, 'O')) c->target_pose = OpenPalm;
        }

        if (c->printed_pose != c->target_pose) {
            c->printed_pose = c->target_pose;
            printf("[HandMotionPlugin] Target pose: %s\n", PoseName(c->target_pose));
        }

        const double raw_dt = frame ? frame->delta_time_s : 0.016;
        const float dt = raw_dt > 0.0 ? static_cast<float>(raw_dt) : 0.016f;
        const float blend = Clamp01(dt * kBlendSpeed);
        const PoseDefinition& target = kPoses[static_cast<int>(c->target_pose)];

        for (int i = 0; i < kJointCount; i++) {
            c->current_angles[i] += (target.angles[i] - c->current_angles[i]) * blend;
            out_overrides[i].apply = 1;
            out_overrides[i].local_rotation = RotationForFingerCurl(c->current_angles[i]);
        }

        return 0;
    }

}
