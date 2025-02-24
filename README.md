Raymarching Shader in OpenGL

This project runs a Shadertoy-style fragment shader locally using OpenGL and GLFW. It renders a fullscreen raymarching scene with camera movement and shading effects.

🚀 Demo (Shadertoy Version)

[You can view the original shader on Shadertoy:🔗 Shadertoy Demo](https://www.shadertoy.com/view/3f23RV)

📦 Dependencies

Ensure you have the following installed:

GLFW (Window & Input handling)

GLEW (OpenGL extensions loader)

OpenGL 3.3+

🛠 Installing Dependencies (Linux/macOS)

sudo apt-get install libglfw3-dev libglew-dev

🛠 Installing Dependencies (Windows)

Download and install:

GLFW

GLEW

Use MinGW or MSVC for compilation

🏃‍♂️ Running the Project

g++ -o shader_app RayMarching.cpp -lGL -lGLEW -lglfw
./shader_app

📜 Features

✅ Shadertoy-style Fragment Shader✅ Fullscreen Rendering with OpenGL✅ Raymarching Scene with Camera Movement✅ Uniforms for iTime, iMouse, and iResolution
