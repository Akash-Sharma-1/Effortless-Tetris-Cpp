
# Effortless Tetris with C++ 

A simpleton tetris game which u can fire up in ur cmd anytime ! 

## Description

Too modest to brag about its features 

(This game is a hack project built to understand the working of GUI based ideas in c++ and also its relation to Windows Dev APIs) 

## Getting Started

### How to play 
- Use LEFT and RIGHT to move the block piece
- Use DOWN to bring it down (faster than the normal speed)
- Use UP to rotate the piece

I failed at making the controls more complicated :) 


### Basic Requirements

Its a basic tetris game, you shouldn't even be looking at this section
But since we are talking about it, the codebase would need : 
- Gcc compiler
- Windows


### Executing program

There are two steps to create the executable for the game : 
- Load and compile all the resource files
    - ```windres -i  coloringBlocks.rc -o coloringBlocks.o```
- Compile the codebase
    - `gcc -mwindows tetris.cpp coloringBlocks.o -o tetris`


You can play the game by opening the executable `tetris.exe` in Windows

## Issues + Contribution
- Do raise any issues if you face any, unless you are genuinely struggling to level up :p
- It would be great if anyone could give me tips on improving the UI implementation for involving more complex stuff like leaderboard, scores etc



## License

Who would steal my tetris game that ends after 100 seconds :p

## Useful resources
- [Windows Dev API Doc](https://docs.microsoft.com/en-us/windows/win32/api/)
- [Old yet interesting lecture on Windows GUI programming](https://ecs.syr.edu/faculty/Fawcett/handouts/coretechnologies/WindowsProgramming/Win32Prog/Don_Hobson/Don_Hobson.htm)