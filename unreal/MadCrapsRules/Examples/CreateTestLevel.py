import unreal

# New map path
map_path = '/Game/Maps/TestMadCrapsMap'

def create_test_level():
    # Create a blank map
    try:
        unreal.EditorLevelLibrary.new_level(map_path)
    except Exception as e:
        unreal.log_warning('Could not create new level: {}'.format(e))

    # Load the class for TestLevelBootstrap
    cls = unreal.load_class(None, '/Script/MadCrapsRules.TestLevelBootstrap')
    if not cls:
        unreal.log_error('Failed to load TestLevelBootstrap class. Ensure the plugin is compiled and loaded.')
        return

    # Spawn the bootstrap actor at origin
    loc = unreal.Vector(0,0,0)
    rot = unreal.Rotator(0,0,0)
    actor = unreal.EditorLevelLibrary.spawn_actor_from_class(cls, loc, rot)
    if actor:
        unreal.log('Spawned TestLevelBootstrap in map: {}'.format(map_path))
    else:
        unreal.log_error('Failed to spawn TestLevelBootstrap')

    # Save the current level
    try:
        unreal.EditorLevelLibrary.save_current_level_as(map_path)
        unreal.log('Saved map at {}'.format(map_path))
    except Exception as e:
        unreal.log_warning('Failed to save map: {}'.format(e))

if __name__ == '__main__':
    create_test_level()
