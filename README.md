# Blackjack AI
An AI that uses reinforcement learning through Monte Carlo methods to play blackjack. Written purely in C++ taking some inspiration from Openai's Gym environment.
## To build then run
1. Firstly clone the repository onto your system.
```bash
git clone https://github.com/toluwalase104/blackjack_ai
```
 2. Set up CMake with your IDE, here is a resource for VsCode:
  https://code.visualstudio.com/docs/cpp/CMake-linux
 3. Using CMake, in VsCode: Click on the build button along the bottom (on the left) of the IDE
![Screenshot (148)](https://github.com/user-attachments/assets/27f96188-4b0b-4d40-b2a5-03bab9d7ff78)

 4. Navigate to the CMake side tab section on the left, click the debug option under the list of project options.
![Screenshot (146)](https://github.com/user-attachments/assets/b5b39f8b-88ae-46bc-a841-aa3edf452d5f)


**LEGACY**
```bash
g++ -c function.cpp
g++ -c game_assets.cpp
g++ -c environment.cpp
g++ -c agents.cpp
g++ -c main.cpp
g++ -o passiveagent main.o agents.o environment.o game_assets.o function.o
./passiveagent
```
