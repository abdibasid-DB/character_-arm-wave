# Character Animation 

## Project

Human Motion: Greeting Animation

Student:

```text
Abdibasid Hashi
ID: 210201993
```

This project follows the simplified N8RO scope that was given for the assignment:

- closed-library model
- 10 joint angles
- kinematics only
- no forces
- no torques
- no full rigid-body dynamics
- integrated into N8RO
- verified through the GLB viewer

I made a procedural greeting animation for the NathanHuman character in N8RO. The animation is registered as:

```text
Arm Wave
```

The motion is made from these right-arm keyframes:

```text
Frame 1:   T-pose, all right arm rotations 0
Frame 20:  Right shoulder Y = -40
Frame 40:  Right shoulder X = -35, Y = -55, right elbow Z = -95
Frame 60:  Right wrist Y = 105, palm faces forward
Frame 66:  Right wrist Y = 115, Z = 45
Frame 72:  Right wrist Y = 95, Z = -45
Frame 78:  Right wrist Y = 115, Z = 45
Frame 84:  Right wrist Y = 95, Z = -45
Frame 90:  Right wrist Y = 115, Z = 45
Frame 110: Right arm returns to 0
```

The motion starts from a T-pose. The right shoulder moves forward, the right elbow bends, the palm turns forward, and the wrist makes a clear hi/bye wave. I also added bigger body movement so the animation is easier to see: the left arm moves for balance and the legs make a small jump/bounce during the greeting.

## Submission Files

GitHub repo:

```text
https://github.com/abdibasid-DB/character_-arm-wave
```

Compiled DLL:

```text
character_plugin_210201993.dll
```

Screen recording:

```text
video\arm_wave_demo_210201993.mp4
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

This builds the plugin in `Release | x64` and copies it to the user plugin folder.

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

This implementation focuses on one complete greeting motion.

## Joint / Control Approach

The plugin outputs 10 joint angle overrides. If a right wrist or hand joint is available, I use it for the wave. If it is not available, the plugin still keeps 10 joints by using the other lower-body joint:

```text
rightShoulder
rightElbow
rightWrist/rightHand if available
leftShoulder
leftElbow
rightHip
leftHip
rightKnee
leftKnee
rightAnkle
leftAnkle if no wrist/hand joint is available
```

The character starts in a T-pose with the right arm rotations at zero. The right shoulder turns forward, the elbow bends strongly, and the wrist turns so the palm faces forward. The right hand wave is larger now, using wrist Y and wrist Z changes so it is easier to see. I also added a bigger full-body action: the left arm swings for balance and the legs make a noticeable jump/bounce using the hips, knees, and ankles. At the end, the right arm returns to zero.

The plugin uses joint-angle overrides through `IAnimationModel`. The movement is kinematic. I am not applying forces or torques. I used smooth interpolation between the keyframes so the motion does not snap from one pose to another.

The older PDF had a bigger controller setup with physics, but the newer instructions were smaller. For that reason, this version uses the N8RO kinematic animation plugin approach.
