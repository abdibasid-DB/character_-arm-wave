# Nathan Hand Pose Simulation Plugin

This optional N8RO simulation plugin registers `Hand Pose Cycle` on `animationModelNathanHuman`.

The main submitted animation is the GLB model in:

```text
assets\human_model_nathan_walk_stop_fingers_210201993.glb
```

The simulation plugin source is included to show how a mission can request the hand-pose cycle through the N8RO animation system.

## Build

Run `setup.cmd` from the N8RO release folder, open `sim-char-anim-nathan-hand-poses.slnx`, and build:

```text
Configuration: Release
Platform: x64
```

The output DLL is:

```text
bin\release\sim-char-anim-nathan-hand-poses.dll
```

## Mission

The mission script is available at:

```text
mission\human_animation_loop.lua
```
