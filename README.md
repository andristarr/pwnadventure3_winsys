# pwnadventure3_winsys
My solution for the pwn adventure 3 CTF for Windows.


Ideas taken from LiveOverflow's linux solution.
Credits to GuidedHacking for help.

Inject into game, can be used through the game chat. Supported commands:

ws [desired walk speed]            - changes walking speed of player
js [desired jump speed]            - changes jump height of player
tp [x coord] [y coord] [z coord]   - teleports to 3d coord
tpz [z coord]                      - teleports player on the z axis
egg                                - exports actor positions (this includes the eggs as well)
fre                                - freezes player position
unf                                - disables player position freeze
