# AI Build Instruction

After every source-code change, always compile a fresh game executable before reporting completion. Running unit tests or syntax-only checks does not replace this requirement.

Use the repository's configured game build, resolve build errors that were caused by the change, and verify that the executable was recreated successfully. In the final response, state the executable path and whether the build succeeded. If an external dependency or environment problem makes the executable build impossible, clearly report the exact blocker instead of claiming completion.
