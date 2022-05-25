# Ray Tracing

This is a simple Whitted ray tracing implemented in C++. About the Whitted ray tracing in detail, please refer to [^1].

## Features
- Ray-tracing polygons and spheres
- Recursive refractions and reflections, and terminal contribution with Phong lighting model
- Distributed ray tracing(stochastic) - soft shadows and motion blurs as time flows, and glossiness
- Assigning texture maps and normal maps to objects
- Importing polygons from .obj files
- Exporting a result as a .bmp image file
- Multithreading

## Environment and Prerequisites
> Note: You can run this only on Linux. If you're running Windows, using WSL(Windows Subsystem for Linux) is recommended.  

g++ is required for OpenMP multicore processing and glm must also be installed in advance. Just enter the following commands for g++ and glm respectively.
```
sudo apt-get install g++
sudo apt-get install libglm-dev
```

## How to run
To compile, type the following command.
```
make
```
To ray-trace the scene 0 or scene 1, enter the following commands respectively.
```
make run_zero
make run_one
```

Even though the ray-tracing is done with multiple cores, it takes time to complete an entire scene. The result is exported to the `./Results` directory as a .bmp image.  

## Implementation
The implementation is modularized into four major parts; `main`, `Scene`, `Shape`, `Light`.

- In the `main.cpp`, it loads .obj 3d model data from the `./Models` and .bmp textures from the `./Textures`. Other scene setups are done here. Take a careful look at the setup process and then they could be modified to make a different scene. After that, the ray-tracing flow goes on and on. When the ray-tracing is done, the output is exported. You may modify the constants in the `main.cpp` to adjust the resulting image size or camera options.

- The `Scene` class and several functions that enables the actual composition of a scene and simulations of lights are implemented in the `Scene.h` and `Scene.cpp`. Using these functions, spheres and faces are inserted into a scene, be hit by a ray, and then illuminations are calculated on those surfaces.

- In the `Shape.h` and `Shape.cpp` files, the `Shape` class and other helper functions are declared. The `Shape` class is again inherited by the `Face` and `Sphere` classes, each of which presents a single ray-traced entity. Each `Shape` has members `transform`, `diffuse`, `specular`, `emission`, `refractionFactor` and many others.

- The codes of `Light.h` and `Light.cpp` implements the `Light` class that defines a single light entity. The position, aiming target, color, attenuation by distance, the parallelism of rays, and area size, of a lighting unit, are declared as members of the `Light`.

`bmpLib.h` and `bmpLib.cpp` implements functions that read and write a .bmp image file. The code is from ProjectPhysX's one in [^2] and was slightly modified.

`objImporter.h` and `objImporter.cpp` composes a .obj file importer and were implemented by myself. The importer reads a .obj file in a given path, then returns the polygons as a vector of faces. Be noticed that it can only parse triangulated .obj files.

## Results
- Scene 0  
![Scene0](https://user-images.githubusercontent.com/36808324/170076844-95bc86c4-e042-4631-b3ca-5789fedfd23e.png)

- Scene 1  
![Scene1](https://user-images.githubusercontent.com/36808324/170076865-32a4ff2d-7f61-47c6-9934-1b6ef5adfc0a.png)

## References
[^1]: https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-overview/ray-tracing-rendering-technique-overview
[^2]: https://stackoverflow.com/questions/36288421/c-create-png-bitmap-from-array-of-numbers/58395323
