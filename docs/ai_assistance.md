# AI Assistance Log

AI assistance was used to:

- Read and summarize the teacher integration requirements.
- Compare the N8RO plugin workflow with the APP `ICharacterController` term-project spec.
- Help debug the arm pose, especially the T-pose/rest-pose issue.
- Help update README/design documentation.

Accepted changes:

- Registering the animation code as `Arm Wave`.
- Keeping exactly 10 joint outputs.
- Explicitly posing both shoulders and elbows for a relaxed rest pose.
- Using `rightElbow` as a forearm/wrist proxy because the current NathanHuman skeleton exposes no wrist or hand joint.
- Adding small balance motion to hips, knees, and ankles.

Rejected or postponed:

- Full APP `ICharacterController` ABI implementation, because the starter pack/header/project files are not present in this workspace yet.
- Physics/PD/RBD controller implementation, because the current N8RO plugin API is an angle-override animation model rather than the APP controller ABI.
