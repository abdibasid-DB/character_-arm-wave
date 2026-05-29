# Design Notes

## Scope Used

This project follows the latest simplified N8RO instructions from the teacher:

```text
Build a closed-library model that determines 10 joint angles.
Joint angles only.
No forces.
No dynamics.
No contact physics.
Integrate into N8RO.
Verify motion through the GLB viewer.
```

The older PDF integration spec described a fuller APP controller with quaternion outputs, PD/RBD physics, hotkeys, locomotion, and mission goals. The later emails narrowed the current deadline scope, so this design targets the N8RO kinematic plugin path.

## Animation Goal

The goal is a natural bye-bye arm wave:

```text
Idle -> Raise Arm -> Wrist Wave -> Lower Arm -> Idle
```

The character should not remain in a T-pose. Both arms are explicitly posed down in the idle state before the right arm moves.

## Ten-Joint Output

The plugin writes these 10 joints every evaluation:

```text
rightShoulder
rightElbow
leftShoulder
leftElbow
rightHip
leftHip
rightKnee
leftKnee
rightAnkle
leftAnkle
```

## Motion Roles

`rightShoulder` raises and holds the arm.

`rightElbow` bends the arm and performs the small waving motion. This is used as the forearm/wrist proxy because the current NathanHuman skeleton metadata exposes no wrist or hand joint.

`leftShoulder` and `leftElbow` hold the left arm relaxed down beside the torso.

`rightHip`, `leftHip`, `rightKnee`, `leftKnee`, `rightAnkle`, and `leftAnkle` add subtle balance support so the 10-joint output is purposeful.

## Current Angle Summary

Idle:

```text
Right shoulder: X 72, Y 0, Z 95
Right elbow:    X 18, Y 0, Z 6
Left shoulder:  X 72, Y 0, Z -95
Left elbow:     X 18, Y 0, Z -6
```

Raised:

```text
Right shoulder: X 58, Y 8, Z 58
Right elbow:    X 108, Y 0, Z 18
```

Wave:

```text
Right shoulder: fixed at X 58, Y 8, Z 58
Right elbow:    X 108, Y 0, Z 18 +/- 10
```

Balance support:

```text
Hips:   X +1.5 deg, Y +/-1.0 deg
Knees:  X +1.0 deg
Ankles: X -0.6 deg
```

## Limitations

This is a N8RO `IAnimationModel` plugin using Euler angle joint overrides. It is intentionally kinematic and does not implement forces, torques, full rigid-body dynamics, or contact physics.
