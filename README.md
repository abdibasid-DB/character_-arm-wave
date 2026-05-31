# N8RO Character Plugin 210201993

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

In short, the model implements walking, stopping, arm extension, and finger motion. The character transitions from walking to a standing pose, extends the arms, closes the fingers one by one, and opens the fingers one by one.

## Selected 10 Hand Joint Controls

The main hand motion controls these 10 finger joint values:

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

These joint values are used to show the fingers closing and opening one by one.

## Other Body Joints Used In The Animation

The character also uses the existing body skeleton for the normal walking and arm movement:

- Hip
- Left and right upper legs
- Left and right lower legs
- Left and right feet
- Spine joints
- Left and right shoulders
- Left and right upper arms
- Left and right lower arms
- Left and right hands

The body joints make the character walk and move the arms. The selected 10 hand controls make the finger motion clear after the character stops.

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

## Load In N8RO

Open the N8RO GLB viewer and load:

```text
assets\human_model_nathan_walk_stop_fingers_210201993.glb
```

The submitted Release x64 DLL is included in:

```text
x64\Release\character_plugin_210201993.dll
```

## How It Works

The project uses the provided `ICharacterController` interface. The plugin writes 10 hand joint rotation values through `out_overrides[10]`.

The finger values are blended smoothly to close and open the fingers one by one. The GLB model also contains the walking, stopping, and arm movement. The body motion uses the existing character skeleton, including the hip, legs, feet, spine, shoulders, arms, and hands.

The project uses direct joint rotation control. It does not use PD forces or a physics simulation.

## Final GLB Model

The final animated model is:

```text
assets\human_model_nathan_walk_stop_fingers_210201993.glb
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
- Public repository: `https://github.com/abdibasid-DB/n8ro-walk-stop-finger-motion-210201993`
- Video: https://drive.google.com/file/d/1oiD7dW4kdMTdFpXi9c_Gff_2tFqQ-ZcD/view?usp=sharing
