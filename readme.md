# Garden Simulator

A 3D garden simulator built with Qt and OpenGL. This is part of a series of projects, which all relate back to my smart home project, and the goal is to eventually incorporate this in. Right now, this is just a proof of concept for it, but still pretty cool, in my opinion. 
You can find the smart home project here:
https://github.com/rrussoneu/RPi_Home

## Features

- Interactive 3D garden environment
- Real-time temperature and moisture monitoring
- Drag-and-drop plant placement system
- Dynamic lighting based on temperature
- Support for multiple plant types (Carrots, Pumpkins, Tomatoes)
- Environmental sensor integration capabilities


### Video Demo
YouTube:
[![Garden Simulation Demo](https://img.youtube.com/vi/xmJIMEWbbcs/maxresdefault.jpg)](https://www.youtube.com/watch?v=xmJIMEWbbcs)

### MVC Architecture
The project follows the Model-View-Controller pattern:

- Model: Handles garden state, plant data, and environmental parameters
- View: OpenGL-based 3D visualization and Qt UI components
- Controller: Manages user interactions and updates between model and view

### Technical Features

- Real-time 3D rendering using OpenGL
- Custom shader implementation for environmental effects
- Mock sensor system for temperature and moisture simulation
- Extensible plant system 

### Future Plans
- Bigger Goals
  - The first main goal is to refactor the code for this project to incorporate different OOP design patterns and firm up the architecture before incorporating this into my smart home system
  - Add a sprinkler system representation to remotely water plants and display a nice visual when watering
  - Most importantly, add this to my smart home system for garden monitoring and automated plant watering

- Smaller Goals
  - Additional plant varieties that are in my real garden
  - Real sensor integration
  - Weather effects

## Dependencies
- Qt 6
- OpenGL 3.3+
- assimp

## Models
The plant models used here are from:
https://www.fab.com/listings/cd55e9c9-3d7f-43c9-bef5-a8e78a983dbb
