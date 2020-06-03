#Battle Ship

This is a C implementation of the popular game Battle Ship

This game has a modular design to facilitate implementing new data structures for the game tray as well as other ways to transmit data between different shells.

Currently implemented are: Matrixes and quad trees for the game tray.
    Inter process communication (To allow for a game to occur on more than one shell): Text files, Shared memory and Sockets.
    With sockets you can have each player on a separate machine (If all ports are correctly configured and the provided IP address is correct)
    The default port used by the game is 8080.

Compilation instructions:
    Before any compilation, running * make clean * is recommended.

    The modifiable parameters are:
        GAME_TYPE - Defaults to TWO_SHELL. Can be ONE_SHELL (To be used only with SAME_SHELL COMMUNICATION) to run the game on one shell or TWO_SHELL to run the game on two different shells
        COMMUNICATION - Defaults to SHARED_MEM. Can be SAME_SHELL (To be used only with ONE_SHELL GAME_TYPE), TEXT to use text files, SHARED_MEM to use shared memory and NETWORK to use sockets
        STORAGE - Defaults to QUAD. Can be MATRIX to use the matrix or QUAD to use the quad tree

    To modify these parameters use the command * make * as follows:
        make GAME_TYPE=TWO_SHELL COMMUNICATION=TEXT STORAGE=QUAD
    Any of the parameters can be omitted if you don't wish to change from the default.
    Some example commands:

    Compiling the program to run on only one shell with Quadtrees:
        make GAME_TYPE=ONE_SHELL COMMUNICATION=SAME_SHELL
    Compiling the program to run on two shells using text files for communication and Matrixes for game storage:
        make COMMUNICATION=TEXT STORAGE=MATRIX
    Compiling the program to run on two shells using Sockets (Networking) for communication and Quadtrees for game storage:
        make COMMUNICATION=NETWORK
