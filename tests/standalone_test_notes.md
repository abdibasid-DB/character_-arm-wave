# Standalone Test Notes

The older project document describes a standalone test harness for the APP `ICharacterController` ABI.

This current project is a N8RO simulation animation plugin using:

```text
arkheon::astsim::IAnimationModel
```

It does not expose:

```text
arkheon_character_tick(...)
arkheon_character_create(...)
arkheon_character_destroy(...)
```

Because of that, the older standalone harness does not apply directly to this DLL.

Checks done for this version:

```text
Release x64 build succeeds.
DLL deploys to C:\N8RO\userPlugins\sim\.
Mission script activates Arm Wave.
Plugin outputs exactly 10 joint overrides.
Right arm wave, left arm motion, and leg bounce were checked in N8RO.
```

For the simplified N8RO version of the assignment, I checked the animation through the N8RO GLB viewer after deploying the DLL.
