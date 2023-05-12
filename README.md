# NetShip
NetShip is a battleship like peer-to-peer console game written as cpp programming project. 

## How to build
### Dependencies
* `asio` library
* `ftxui` library (not required, will be handled by fetch content if not downloaded)

### Without nix
* ensure you have all the dependencies
* `mkdir build && cd build`
* `cmake -DCMAKE_BUILD_TYPE=Release ..`
* `cmake --build .`
* the resulting binary will be in `build` and will be named `netShip`

### With nix
* dependencies are automatically handled by nix
* `nix build`
* the resulting binary will be in `result/bin` and will be named `netShip`

## How to play
* you will controll the ui using your **keyboard and mouse**
### Initial menu
* you will be able to choose if you want to be a **server or a client**
* a **port number**
* an IPv4 **ip adress**
* *after you have setup all of this click the button*

### Placement menu
* place your ships on the board
    * you will be placing only parts of the ships the correctnes of the configuration will be checked by the button
* **rules:**
    * there needs to be one space bettween the ships (even diagonal)
    * on the left side you can see ship rules (how many of what ship you need to place)
        * more or less ships placed is *not allowed*
* *after you place your ships click the button to check the configuration and advance to the next screen*

### Game screen
* on the left side you can see your configuration
* on the right side you can see enemy board that you have uncovered
* on the right side of the enemy board you can see a current game configuration
    * it is not dynamic so it always shows all the ships
* you can select a space to fire at it
    * when you press the button a shot will be fired at enemy
* the hit status will be presented to you in text and on the board
    * M = Miss
    * S = Ship
    * H = Hit (or on your board sunk if all of the ship is hit)
    * RED on enemy board = sunk
        * if you sunk an enemy ship the places next to it will be indicated as a miss 
            * this is to indicated that there is no ship around said ship and that you don't need to uncover those spaces
* the game ends when you or your enemy have sunk all of the ships

## TODO:
### UI based changes
> these changes require extensive UI change and almost 0 logic change
* choose who plays first
    * rn its server
* custom settings
    * how many and of what ships
    * size of the game board