# Design Notes

## Scope Used

This project follows the simplified N8RO instructions for the assignment:

```text
Build a closed-library model that determines 10 joint angles.
Joint angles only.
No forces.
No dynamics.
No contact physics.
Integrate into N8RO.
Verify motion through the GLB viewer.
```

The older PDF described a larger controller project with physics and more features. The later instructions were more focused, so I used the N8RO animation plugin path for this submission.

## Animation Goal

The goal is to make a clear human greeting wave using right-arm X/Y/Z keyframes:

```text
f001: shoulder 0/0/0, elbow 0/0/0, wrist 0/0/0
f020: shoulder 0/-40/0, elbow 0/0/0, wrist 0/0/0
f040: shoulder -35/-55/8, elbow 0/0/-95, wrist 0/0/0
f060: shoulder -35/-55/8, elbow 0/0/-95, wrist 0/105/0
f066: wrist Y 115, Z 45
f072: wrist Y 95, Z -45
f078: wrist Y 115, Z 45
f084: wrist Y 95, Z -45
f090: wrist Y 115, Z 45
f110: all right arm rotations back to 0
```

The character starts from a T-pose. The right shoulder rotates forward, the elbow bends strongly, the palm faces forward, and the wrist does the hi/bye wave. I also added a bigger full-body action: the left arm swings and the lower body makes a noticeable jump/bounce.

## Ten-Joint Output

The plugin writes 10 joint overrides each frame:

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

## Motion Roles

`rightShoulder` starts in the T-pose, turns the arm forward, stays mostly steady during the wave, and then lowers back to rest.

`rightElbow` bends so the forearm comes up near the head or shoulder. During the wave it only moves a little, because the wave should mainly come from the hand.

`rightWrist` or `rightHand` is used for the main wave when the skeleton provides that joint. If it is not available, the right elbow/forearm is used as a backup.

`leftShoulder` and `leftElbow` add the opposite arm movement so the animation has more action and does not look frozen.

The hip, knee, and ankle joints make a noticeable jump/bounce while the arm greeting is happening.

## Movement Details

The right arm does the main greeting:

```text
shoulder opens forward
elbow folds down and brings the forearm up
wrist turns the palm forward
wrist moves left and right for the wave
```

The rest of the body supports the motion:

```text
left shoulder and left elbow swing as the opposite arm
hips move slightly during the bounce
knees bend during the jump/bounce
ankles help the feet follow the bounce
```

I used smooth interpolation between the frame values. This makes the arm raise, wave, and lower without hard snapping.

## Limitations

This is a N8RO `IAnimationModel` plugin using Euler angle joint overrides. It is kinematic only, so it does not use forces, torques, full rigid-body dynamics, or contact physics.
