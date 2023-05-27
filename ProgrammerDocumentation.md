# Programmer documentation
> This document outlines how the program is structured so that the future programmers can easily extend it, modify it and otherwise change it.

## Program structure
* There is one supporting part (data) and 3 main parts of the program.

## High lvl overview
The `main()` function calls game logic which in turn initializes the game with UI menu screens that return the necessary data for the initialization of the network communication part of the game and also for the gameplay part (ship location). After that we enter the gameloop and send/receive the hit locations/statuses to/from the enemy. We display the UI for a player to see on his turn. After ours or the enemies ships are depleted the game ends.

### The data 
* These are used throughout the program and connect various other parts.
* Most of them are simple `usings` for `std::` structures but there are also two enums and one lambda to create a hash for `position_t`(which itself is just an array of size_t of size 2) which is used in `std::unordered_set`.

#### The enums
##### `HitStatus`
* The `HitStatus` is used as a result and type to be send of a network communication.
    * We need to separate three possible states `Miss`, `Hit`, and `Sunken`.
        * The `Sunken` is needed since we don't store enemy information on our side, and without it we wouldn't be able to tell if the ship was sunken or not.
##### `DisplayType`
* The `DisplayType` is used by a UI layer, we need to separate five different types of places for the user.
    * `Empty` one just refers to the water (or non discovered space)
    * `Miss` is a space that we (or the opponent) fired at but no ship was there (discovered space)
    * `Ship` - There is a ship at this place - this is used as a indicator for us, not the opponent (we would display `Hit` instead)
    * `Hit` is used to display that we (or the opponent) had hit a ship
    * `Sunken` - the sunken places of a ship.

### The communication layer
> The Asio standalone library is used for this project
* This is the layer that handles all the peer-to-peer communication with the other client/server.
* There is an **out-facing** part of the layer which is just a client-server communication via TCP IPv4 sockets.
    * Some parts of the `Client` and `Server` classes could be abstracted, hence the `CommonCS` class.
* The **in-facing** part are just convertor function which take the raw bytes gotten from the communication and convert them to some game *data*.

### The UI layer
* The game has 3 screens.
    * **initial menu** 
        * used to setup the client/server
    * **placement menu** -
        * used to place the ship and check that the configuration is valid for the given rules.
    * **game screen** 
        * used as a final screen which the game is played on
        * user will select a space to fire at in his turn
* All of **menu** screens are set-upped in a way that the game logic calls them and they loop till they get the correct result.
    * *NOTE* This is one of the shortcomings of this project - the "correct" way would be to have a game centered around UI not its logic.
    * These menus always return some kind of data for the logic to use in the next game steps.
* The last **game screen** is called by the game-logic to get the next move from the player and to display the status of the game.
    * The result of this menu is a "fire-at" location that player chose

### The Game logic
* The game logic comprises of a `Start()` function that enters the game loop after initializing the Server/Client communication and after getting the ship locations.
* For the game logic alone we would just need a two (bool)arrays to mark our *hit/miss* ships and enemy *hit/miss* ships (and two numbers that would count down with each *sunk*).
    * But since we need to display the result in some way this is not enough and we required the display board array with the `DisplayType` that we give to the **game screen**.
* The game loop consists of a result from **game screen** on our turn, which is a *fire-at* location.
    * We send this location to the enemy and wait for an answer.
        * The `Hit`, and `Miss` are pretty easy to mark and process the notable exception is the `Sunken`.
        * If the ship is `Sunken` we need to mark the dead ship on our screen for the player to see. We need to get sunken and border coordinates from just a sunk location.
* If its enemy turn we wait for his *fire-at* location and then process if he had `Sunken`, `Hit` or `Miss` our ships and then send the result back.
    * We remember our ships and their positions and when a ship is hit we found it and remove such position.
        If there is no position for the ship we send the `Sunken` to the enemy.
    * We also need to update the **game screen**.
* We also need to check for the lose/win state.
* *NOTE* currently the Rules (number of ships and their size and also the game board size) is hardcoded. **But** this is not for the limitation of the *game logic*. The *game logic* would be able to handle it easily, the change in **UI** would be required for that change to work however.