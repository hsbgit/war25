# AI Build Instruction

After every source-code change, always compile a fresh game executable before reporting completion. Running unit tests or syntax-only checks does not replace this requirement.

Use the repository's configured game build, resolve build errors that were caused by the change, and verify that the executable was recreated successfully. In the final response, state the executable path and whether the build succeeded. If an external dependency or environment problem makes the executable build impossible, clearly report the exact blocker instead of claiming completion.

Every bug found during implementation, review, testing, or refactoring must be fixed and protected by a meaningful unit test that reproduces the faulty behavior or verifies the corrected rule. Do not merely document or report a discovered bug while leaving it unresolved.

# Commit Privacy and Dependency Safety

Before creating a commit, inspect both the staged changes and the recent commit history. Do not commit if either contains confidential or machine-identifying information, including credentials, API keys, tokens, private keys, personal names or email addresses, usernames, home-directory or absolute workstation paths, hostnames, local network details, operating-system or distribution details, IDE metadata, build caches, or other information that identifies a person or computer. Include Git author and committer metadata in this review and use a neutral, privacy-preserving identity when the configured identity is personal or machine-specific. Report any existing finding instead of silently preserving or publishing it; rewriting existing history requires explicit approval.

Do not modify, stage, or commit vendored or external libraries, including SDL, SDL_image, SDL_mixer, or any path below `src/3rdparty`, unless the user explicitly requests that dependency change. Before committing, compare the staged file list and recent commits against external-library paths and stop if an unintended dependency change is present.

# Gameplay and Lifecycle Invariants

Treat the following behavior as intentional project rules, not as bugs. Preserve these rules during future implementation, review, and refactoring, and update their regression tests when behavior changes deliberately.

## Hidden resource collectors

- A peasant inside a gold mine and a tanker inside an oil platform are temporarily removed from the map.
- While inside the resource building, they cannot receive or execute player commands. Commands must not force them to leave early or be queued for execution after mining.
- They become commandable again only after the normal mining cycle places them back on the map.
- If an oil platform is destroyed, every tanker currently inside it is destroyed with it. The tanker must not be ejected, and Unholy Armor must not let it survive the platform's destruction.
- Gold-mine destruction follows the existing gold-mine lifecycle: hidden peasants are released safely, and the mine remains until all workers can be placed.

## Transports and passengers

- If a transport sinks, all loaded passengers die with it, including magically protected passengers. Their food usage must be released exactly once.
- Passengers walking toward a transport reserve capacity and an approach tile. An external command such as Stop, Move, Patrol, or Attack cancels that pending boarding request. Movement commands issued internally by the transport do not cancel it.
- Loaded passengers are off-map and cannot execute ordinary unit commands until unloaded.
- Unloading and emergency placement must validate the passenger's complete footprint and movement domain.

## Placement and hidden-unit exits

- Any fallback placement for a hidden or temporarily removed unit must validate the complete registered footprint, map bounds, occupancy layer, and land/sea/air domain. A free top-left tile alone is insufficient for 2x2 units.
- If a resource drop-off building is destroyed while a collector is hidden inside it, any surviving collector may reappear only at a valid full-footprint position for its domain.

## Food, construction, and production ownership

- Farms and town halls grant food capacity only after construction completes, and each building grants and removes its capacity exactly once. Cancelling an unfinished building must not subtract capacity it never granted.
- A pre-placed town hall grants exactly one food point; initialization must not count it twice.
- Completing a town hall must run the common building-completion lifecycle so its builder is released normally.
- A unit counts toward used food exactly once. Death or deletion before placement must release that food exactly once.
- A completed unit waiting for a free exit remains owned by the player's object list. Destroying its production building must retire it through normal owner-managed cleanup; never delete it directly while leaving a dangling owner-list pointer.

## Resource economy and maps

- A lumber mill grants the wood-delivery bonus only while it is alive and fully constructed.
- Exhausted or destroyed gold mines may redirect a worker to nearby wood only when valid wood is actually available.
- Melee PUD start markers create starting workers only when the map does not already define player forces; scenario maps must not receive extra workers.
- Forest and rock autotiling distinguishes diagonal neighbors so inside corners are not treated as straight edges.
