# Character Animation Submission

## Project

Human Motion: Arm Character Animation

Student:

```text
Abdibasid Hashi
ID: 210201993
```

This submission follows the latest simplified N8RO project scope from the teacher:

- closed-library model
- 10 joint angles
- kinematics only
- no forces
- no torques
- no full rigid-body dynamics
- integrated into N8RO
- verified through the GLB viewer

This submission contains a procedural NathanHuman arm-wave animation plugin for N8RO. The motion is registered as:

```text
Arm Wave
```

The animation follows this state flow:

```text
Idle -> Raise Arm -> Wrist Wave -> Lower Arm -> Idle
```

## Build

Open a terminal at:

```bat
C:\N8RO\dev\samples\sim\sim-char-anim-nathan-arm-wave
```

Run:

```bat
build-release.cmd
```

This builds the plugin in `Release | x64` and deploys it automatically.

## Output DLL

Built DLL:

```text
C:\N8RO\dev\samples\sim\sim-char-anim-nathan-arm-wave\bin\release\sim-char-anim-nathan-arm-wave.dll
```

Deployed DLL:

```text
C:\N8RO\userPlugins\sim\sim-char-anim-nathan-arm-wave.dll
```

Submission copy:

```text
C:\N8RO\submission\character_plugin_210201993.dll
```

For N8RO loading, the deployed plugin remains:

```text
sim-char-anim-nathan-arm-wave.dll
```

## Install / Load

For the current N8RO build, place the DLL in:

```text
C:\N8RO\userPlugins\sim\
```

Restart N8RO after replacing the DLL.

The mission script activates:

```text
Arm Wave
```

from:

```text
C:\N8RO\data\resources\missions\human_animation_loop.lua
```

## Selected Motion

Motion A:

```text
Arm Wave
```

The current implementation focuses on one complete motion: a natural arm-wave sequence.

## Joint / Control Approach

The plugin outputs exactly 10 joint overrides:

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

The right shoulder raises and holds the arm. The right elbow bends the arm and acts as the closest available forearm/wrist proxy for the small bye-bye wave, because this NathanHuman skeleton does not expose a separate wrist or hand joint. The left arm is explicitly held in a relaxed down pose. The hip, knee, and ankle joints provide small balance support when the right arm is raised.

This N8RO plugin uses joint-angle overrides through `IAnimationModel`, which matches the newer simplified email scope: joint angles only, integrated into N8RO, verified in the GLB viewer.

The older integration PDF described a larger APP `ICharacterController` project with PD/RBD physics. The later teacher emails narrowed the deadline scope, so this submission follows the newer N8RO kinematic scope.
