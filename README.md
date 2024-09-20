# Blackjack AI
An ai that uses reinforcement learning to play blackjack, written in C++.
## To build then run
```bash
g++ -c game_assets.cpp
g++ -c environment.cpp
g++ -c agents.cpp
g++ -c main.cpp
g++ -o passiveagent main.o agents.o environment.o game_assets.o
./passiveagent
```
