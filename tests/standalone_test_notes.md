# Standalone Test Notes

The older downloaded term-project spec describes a standalone harness for the APP `ICharacterController` ABI.

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

Therefore, the APP standalone harness from the older integration spec does not apply directly to this DLL.

Current verification performed:

```text
Release x64 build succeeds.
DLL deploys to C:\N8RO\userPlugins\sim\.
Mission script activates Arm Wave.
Plugin outputs exactly 10 joint overrides.
```

For the current simplified N8RO deadline, verification is through the N8RO GLB viewer after deploying the DLL.
