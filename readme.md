# wkRealTime v.0.0.4d
_Experimental Worms Armageddon WormKit module that implements real-time online multiplayer for racing schemes._

## Installation
Place **wkZRealTime.dll** and **wkRealTime.ini** in Worms Armageddon installation directory and ensure that WormKit modules are enabled in Advanced Settings.

## Supported schemes
The module is currently meant to be used in racing schemes (for example: Big RR, TTRR, Bungee Race, Parachute Race, Battle Race) where worms don't directly interact with each other or destroy the land.
In general, the scheme requires the following options:
- Infinite turn time (available in main screen)
- Loss of control doesn't end turn (LDET, available in 3.8 scheme options under Gameplay 1 menu)
- Weapon use doesn't end turn (SDET, available in 3.8 scheme options under Gameplay 1 menu)
- Indestructible land (available under Cheats in general scheme options)
- Infinite worm health (available under Cheats in general scheme options)
- Infinite ammunition for all available weapons
- No crate spawns
- Phased worms (Allied & Enemy) set to Worms+Weapons+Damage (available in 3.8 options under Physics menu)
- Antisink (available in 3.8 scheme options under RubberWorm menu)
- Teams should have only 1 worm

Support for combat schemes might be possible with future versions of the module.

## Enabling real-time mode
The real-time mode can be enabled in two ways:
- In multiplayer lobby: the host must type **/realtime on** in lobby chat and start the game normally. The game will allow all teams to move immediately after round starts. This way is recommended and appears to introduce less latency.
- Directly in game: all players must type **/realtime on** in game chat, and once it's done, the player currently holding turn must skip the turn. On the next turn real-time mode will be started.

## General notes and bugs
- This module is in early stage of development and has several limitations and bugs. Real-time should work in both Direct IP and WormNAT2 games, but it's recommended to use Direct IP games for less latency and connectivity issues.
- The game makes distinction between "turn-holding" and "inactive" teams. In real-time mode all teams have control of worms, but one team is priviledged and holds the turn - it's name and health bar should be flashing in bottom HUD. Certain actions should be only done by the turn-holder team to avoid bugs and disconnects.
- When placing the worms on game start, only one worm should be placed at a time, according to current turn-holder.
- To end the round, either close the game with ALT-F4 and rehost or sink all worms except the current turn-holder.
- Sometimes, players will appear to lose connectivity - they will be able to see worm movement of other teams and read chat, but their chat messages and movement won't be seen by other players. In some cases, their movement and chat will be delayed by several seconds. This is a bug of the module that is more likely to happen when hosting with WormNAT2.
- All players must use the same version of the module. Players can display their module version by typing **/realtime** in chat.
- When using teleport for the first time in round, the camera will be broken. Mouse cursor movement of other players is not visible.
- Replays recorded by the game in real-time mode are currently broken.
- The module is not compatible with wkBigLobby.
