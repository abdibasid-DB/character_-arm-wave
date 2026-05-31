# Character Plugin 210201993

This is an N8RO character animation project.

Student ID: 210201993

## Project Idea

The project shows a simple human motion and hand/finger animation in the N8RO GLB viewer.

The selected states are:

1. Walk
2. Stop
3. Arms Extended
4. Fingers Close One by One
5. Fingers Open One by One

The character first walks in place, then stops. After stopping, the arms extend into a clear pose. One arm is forward and one arm is backward. Then the fingers close one by one and open one by one.

## Controlled 10 Joints

The hand motion controls 10 finger joint values:

1. Thumb base
2. Thumb tip
3. Index base
4. Index tip
5. Middle base
6. Middle tip
7. Ring base
8. Ring tip
9. Pinky base
10. Pinky tip

These joint values are used to show the fingers closing and opening.

## Build

Open the solution file in Visual Studio 2022:

```text
HandMotionPlugin.sln
```

Build settings:

```text
Configuration: Release
Platform: x64
```

The submitted Release x64 DLL path is:

```text
x64\Release\character_plugin_210201993.dll
```

Note: this folder keeps the original starter/plugin project structure. The final visible animation for this submission is the GLB model in the `assets` folder.

## Final GLB Model

The final animated model is:

```text
assets\human_model_nathan_walk_stop_fingers_210201993.glb
```

## Animation Script

The script used to generate the GLB animation is:

```text
tools\write_walk_stop_finger_sequence_glb.js
```

## Testing

Open the final GLB model in the N8RO GLB viewer and record the screen.

The video should clearly show:

- Walk
- Stop
- Arms Extended
- Fingers Close One by One
- Fingers Open One by One

## Submitted Files

- Source/project files: included in this repository
- Final GLB model: `assets\human_model_nathan_walk_stop_fingers_210201993.glb`
- Generation script: `tools\write_walk_stop_finger_sequence_glb.js`
- Email template: `SUBMISSION_EMAIL.txt`
- Public repository: `https://github.com/abdibasid-DB/character_-arm-wave`
- Video: record the animation in the N8RO GLB viewer and share the video link in the submission email
