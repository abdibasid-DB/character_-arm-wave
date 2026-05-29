# Character Animation

## Student

```text
Name: Abdibasid Hashi
ID: 210201993
```

## Project Description

This project is a human greeting animation for the NathanHuman character in N8RO.

The character starts in a T-pose. The right arm moves forward, the elbow bends, the palm turns to face forward, and the hand waves like saying hi or bye. The left arm also moves, and the legs make a small jump/bounce so the whole motion is more visible.

The animation name inside N8RO is:

```text
Arm Wave
```

## Main Motion

The greeting motion uses these steps:

```text
1. Start in T-pose
2. Move the right shoulder forward
3. Bend the right elbow
4. Turn the palm forward
5. Wave using the wrist/hand
6. Add left arm movement
7. Add leg bounce/jump
8. Return back to the starting pose
```

## Right Arm Keyframes

```text
Frame 1:   T-pose, all right arm rotations are 0
Frame 20:  Right shoulder Y = -40
Frame 40:  Right shoulder X = -35, Y = -55, Z = 8
           Right elbow Z = -95
Frame 60:  Right wrist Y = 105
Frame 66:  Right wrist Y = 115, Z = 45
Frame 72:  Right wrist Y = 95,  Z = -45
Frame 78:  Right wrist Y = 115, Z = 45
Frame 84:  Right wrist Y = 95,  Z = -45
Frame 90:  Right wrist Y = 115, Z = 45
Frame 110: Right arm returns to 0
```

## Joint Control

The plugin uses 10 joint angle outputs.

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

The right shoulder and right elbow create the main arm pose. The wrist/hand makes the wave. The left arm and legs add extra movement so the animation looks more active.

This project uses joint angles only. It does not use forces, torques, or full physics. The motion is made by changing the joint rotations smoothly between keyframes.

## Build

Open this folder:

```text
C:\N8RO\dev\samples\sim\sim-char-anim-nathan-arm-wave
```

Run:

```bat
build-release.cmd
```

The project should be built as:

```text
Release x64
```

## Install / Load

Copy the plugin DLL to:

```text
C:\N8RO\userPlugins\sim\
```

Restart N8RO after copying the DLL.

The plugin loaded by N8RO is:

```text
sim-char-anim-nathan-arm-wave.dll
```

## Submission Files

GitHub repo:

```text
https://github.com/abdibasid-DB/character_-arm-wave
```

Compiled DLL:

```text
character_plugin_210201993.dll
```

Demo video:

```text
video\arm_wave_demo_210201993.mp4
```

## Testing

I built the DLL in Release x64 and loaded it in N8RO. I checked that the Arm Wave animation runs, the right hand waves, the left arm moves, and the legs make the jump/bounce motion.
