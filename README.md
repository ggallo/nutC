# nutC

A simple shell-based multiplayer game developed in C. Collect the most acorns before winter sets in.

## Building
Supports OS X 11.10 or later, and Ubuntu 12.04 or later. Otherwise, good luck!
```
make all
```
Creates two executables, `server.out` and `client.out`.

## Usage
### server
```
./server.out <height> <width> <numAcorns>
```
Starts the game on a `height` by `width` board, with <numAcorns> randomly placed around the board. Begins listening for connections, and starts the game when `CLIENT_MIN` clients have joined. Ends once all acorns have been collected.

### client
```
./client.out <client_id>
```
Begins a client shell with an ID of `player_id`. The player connects automatically to the server, unless `CLIENT_MAX` players have already joined. During the game, the player types UP, DOWN, LEFT, or RIGHT to navigate the board, and get a running total of the number of acorns they have collected so far.
