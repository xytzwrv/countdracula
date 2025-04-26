# Count Dracula Discord Bot

A modular Discord bot with chess functionality built using C++ and the D++ (DPP) library.

## Features

- **Modular Architecture**: Easy to add new features through the modular system
- **Greeting Commands**: Simple command to say hello
- **Chess Game**: Play chess against other users with visual board representation
  - Start games with other users
  - Make moves using standard UCI notation
  - Visual representation of the board using SVG
  - Basic move validation and game state tracking

## Prerequisites

- C++ compiler with C++17 support
- D++ (DPP) library
- CMake (recommended for building)
- Discord Bot Token

## Installation

1. Install the D++ library:

```bash
git clone https://github.com/brainboxdotcc/DPP.git
cd DPP
mkdir build
cd build
cmake ..
make -j$(nproc)
sudo make install
```

2. Build the bot:

```bash
mkdir build
cd build
cmake ..
make
```

## Configuration

Set your Discord bot token as an environment variable:

```bash
export DISCORD_BOT_TOKEN=your_token_here
```

## Running the Bot

From the build directory:

```bash
./countdracula
```

## Commands

- `/helloworld` - Says hello from the greetings module
- `/start_chess @user` - Starts a new chess game with the mentioned user
- `/move e2e4` - Makes a chess move in UCI notation (e.g., e2e4)

## Chess Module Details

The chess module allows users to play chess with each other. It features:

- Standard chess rules (partial implementation)
- UCI notation for moves (e.g., e2e4)
- SVG board rendering
- Game state tracking
- Turn management

## Project Structure

```
countdracula/
├── main.cpp                   # Main entry point
├── modules/                   # Modular components
│   ├── greetings_module.cpp   # Simple greeting module
│   ├── greetings_module.hpp   # Header for greeting module
│   └── chess/                 # Chess module
│       ├── chess_module.cpp   # Chess implementation
│       └── chess_module.hpp   # Chess module header
```

## CMake Configuration

Create a `CMakeLists.txt` file in the project root:

```cmake
cmake_minimum_required(VERSION 3.10)
project(countdracula)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find DPP library
find_package(DPP REQUIRED)

# Include directories
include_directories(${CMAKE_SOURCE_DIR})

# Add executable
add_executable(countdracula
    main.cpp
    modules/greetings_module.cpp
    modules/chess/chess_module.cpp
)

# Link libraries
target_link_libraries(countdracula dpp)
```

## License

MIT License

## Notes on the Chess Implementation

The Chess module is a simplified implementation of chess with the following limitations:

1. Only basic move validation is implemented (primarily for pawns)
2. Advanced rules like castling, en passant, and promotion are not implemented
3. The game state tracking is simplified
4. The SVG rendering is basic but functional

These limitations could be addressed in future updates.
