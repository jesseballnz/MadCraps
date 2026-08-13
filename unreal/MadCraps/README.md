# MadCraps UE5 Project Scaffold

This is a minimal Unreal Engine 5 project scaffold for MadCraps.

## Included

- `MadCraps.uproject`
- `Config/` with basic project and renderer settings
- `Content/` and `Content/Maps/` placeholders
- `Plugins/` placeholder for project-local plugins

## Enable the `MadCrapsRules` plugin

The repo already contains the plugin scaffold here:

- `../MadCrapsRules`

To use it with this project:

1. From `MadCraps/unreal/MadCraps`, place the plugin inside `Plugins/`:
   - copy `../MadCrapsRules` to `Plugins/MadCrapsRules`, or
   - create a symlink named `Plugins/MadCrapsRules` that points to `../MadCrapsRules`
2. Open `MadCraps.uproject` in Unreal Engine 5.
3. If Unreal prompts to rebuild modules, allow it and generate project files if needed.
4. Confirm the plugin is enabled under `Edit -> Plugins -> Gameplay -> MadCraps Rules`.

The `.uproject` already declares `MadCrapsRules` as enabled, so once the plugin is present under `Plugins/`, Unreal should detect it automatically.
