# 3d-solar-system-simulator
A real-time 3D Solar System simulator built using OpenGL + FreeGLUT in C++. Features include rotating and revolving planets, glowing sun, lighting, camera movement, z-buffering, stars, asteroid belt, and comet animation. Designed to demonstrate core computer graphics concepts like transformations, the 3D rendering pipeline, and real-time animation.

# Project Objective
The main objective of this project is to create a real-time 3D Solar System simulation using C++, OpenGL, and FreeGLUT.
The simulation demonstrates the application of major Computer Graphics concepts including:

1. 3D Viewing Pipeline
2. Geometric Transformations (Rotation, Revolution)
3. Object Modelling using primitives
4. Lighting and Shading
5. Z-buffering
6  Framebuffer and Double Buffering
7. Animation using delta-time

The goal is to create a complete 3D environment representing the solar system with smooth and realistic motion.

# Toolchain / Technologies Used

Component                                                               	Tool / Library
Programming Language	                       ->                                 C++
Graphics Library	                           ->                                OpenGL
Windowing / Input / Rendering	               ->                               FreeGLUT
Compiler	                                   ->                               MinGW g++
3D Modelling (Optional)	                     ->                             Autodesk Maya
Video Rendering	                             ->                    OBS Studio / Any Screen Recorder
Additional Effects	                         ->                  OpenGL blending, lighting, materials

# External Libraries Used

1. FreeGLUT — For window creation, input handling, rendering functions
2. OpenGL Utility Library (GLU) — For gluLookAt, gluPerspective

No other external plugin or framework is used (No PyOpenGL, Pymel, or Arnold shaders).

# Features

✔ Rotating and revolving 3D planets
✔ Glowing sun with emission lighting
✔ Cinematic orbit camera + Fly-through camera
✔ Asteroid belt
✔ Stars background
✔ Comet animation
✔ Smooth rendering using double buffering
✔ Z-buffer for correct object depth
✔ Real-time lighting and material effects

# Methodology

1️⃣ Project Setup

1. Install FreeGLUT
2. Link include and lib directories in g++
3. Create project folder with solar.cpp

2️⃣ Window & Rendering Loop

1. Use glutCreateWindow()
2. Use glutDisplayFunc() and glutIdleFunc()
3. Apply double-buffering using GLUT_DOUBLE

3️⃣ Camera System

Two modes:

1. Cinematic auto-orbit
2. Free-fly camera (W/A/S/D + mouse)
Implemented using:
gluLookAt(camX, camY, camZ,
          camX+fx, camY+fy, camZ+fz,
          0, 1, 0);

4️⃣ Object Modelling

All planets and sun built using:

glutSolidSphere(radius, slices, stacks);

1. Added size, tilt, color, revolution speed
2. Sun uses emission material to glow

5️⃣ Transformations

Revolution:

glRotatef(p.orbitAngle, 0, 1, 0);
glTranslatef(p.distance, 0, 0);

Self-Rotation:

glRotatef(p.selfAngle, 0, 1, 0);

6️⃣ Lighting

Sun as the only light source:

glEnable(GL_LIGHTING);
glEnable(GL_LIGHT0);

7️⃣ Z-buffer & Framebuffer

glEnable(GL_DEPTH_TEST);
glutSwapBuffers();

8️⃣ Animation

Based on delta time (dt)

p.orbitAngle += p.orbitSpeed * dt;
p.selfAngle  += p.rotationSpeed * dt;

# Screenshots

<img width="2559" height="1489" alt="Project Screenshot-1" src="https://github.com/user-attachments/assets/b7bd06ad-e22a-46b4-bc9d-e5bf7bbcaea8" />

<img width="2559" height="1485" alt="Project Screenshot-2" src="https://github.com/user-attachments/assets/b23ded2d-2a67-48ff-8b22-8879f4f0be5b" />

<img width="2556" height="1473" alt="Project Screenshot-3" src="https://github.com/user-attachments/assets/0b89ea05-5520-4706-b245-15cebcb73f64" />

<img width="2558" height="1424" alt="Project Screenshot-4" src="https://github.com/user-attachments/assets/57bf3aff-0468-470d-bcd5-61180ba875bb" />



# How to Compile

g++ "solar.cpp" ^
 -I"C:\library\freeglut\include" ^
 -L"C:\library\freeglut\lib" ^
 -lfreeglut -lopengl32 -lglu32 -lwinmm -lgdi32 ^
 -o solar.exe


# Run using:

solar.exe

# Controls
Key	                                                                              Action
W/A/S/D	                                          ->                            Move camera
Q/E	                                              ->                         Move camera up/down
Mouse Drag	                                      ->                            Look around
C	                                                ->                        Toggle cinematic mode
F	                                                ->                            Boost speed
Space	                                            ->                            Launch comet
T                                                 ->                           	Toggle trails
+ / -	                                            ->                              Zoom in/out
R	                                                ->                             Reset camera
ESC                                               ->                                Quit
                                                                                	
# References Used

1. OpenGL Red Book
2. FreeGLUT Official Documentation
3. LearnOpenGL.com
4. NeHe OpenGL Tutorials
5.StackOverflow Discussions
6. Wikipedia – Solar System Data
7. OpenGL.org
8. GeeksforGeeks – OpenGL Basics

# Challenges Faced

1. Setting up compiler and FreeGLUT paths
2. Managing multiple transformations
3. Achieving smooth camera movement
4. Getting lighting to look realistic
5. Depth sorting issues before enabling Z-buffer
6. Performance drop with large star fields

# Scope for Improvement

1. Add image textures for planets
2. Add GUI controls for camera and speed
3. Use shaders (Phong/Blinn-Phong)
4. Add more moons and objects
5. Add background music
6. Add interactive planet selection

# License

This project is open-source and licensed under the MIT License.
