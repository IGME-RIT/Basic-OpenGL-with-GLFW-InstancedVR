Documentation Author: Niko Procopi 2020

This tutorial was designed for Visual Studio 2019
If the solution does not compile, retarget the solution
to a different version of the Windows SDK. If you do not
have any version of the Windows SDK, it can be installed
from the Visual Studio Installer Tool

Instanced Rendering VR
Prerequisites
	All other VR tutorials
	Instanced Rendering OpenGL

Spoiler, this does not have any benefit or harm to performance as we hoped.
However, now the code is cleaner, and now we "know" that it wont boost FPS. 
It's always important to experiment with different ideas to see what works. 

In this tutorial we use a Viewport Array, which is natively 
supported by Geometry Shaders in all modern graphics 
card drivers, from all vendors (Nvidia, AMD, Intel).

Instead of using Geometry Shaders, we use an extensions
that allows us to use this feature with Instanced Rendering.
The following extensions are required to make this work
	Vertex Shader
		GL_NV_viewport_array2
		GL_ARB_shader_viewport_layer_array
	Fragment Shader
		GL_ARB_fragment_layer_viewport

In mesh.cpp we replace glDrawElements with glDrawElementsInstanced
so that all geometry is drawn twice, once for each eye.

In main.cpp, we have a function called "IsExtensionSupported" which checks
if GPU extensions are compatible with the drivers being used. We now only 
draw each once (as far as C++ is concerned), cutting our C++ into half. We 
use glViewportIndexedf to build an array of two viewports, which the shaders
can toggle between

In vertex.glsl, we include the two extensions mentioned above, and then set
	gl_ViewportIndex = gl_InstanceID
This sets the first instance to the left eye, and the second instance to the right.
Then, we use gl_InstanceID to swap "cameraView" matrices between the 
left eye's matrix and the right eye's matrix. gl_ViewportIndex determines
where the image is rendered, after being exported from the shader, while
in the rasterizer. 

In fragment.glsl, the extension GL_ARB_fragment_layer_viewport is added, 
and no other changes are made. Without this, the fragment shader can not
import data from the rasterizer, and everything breaks.

Some drivers are smart enough to automatically enable GL_ARB_fragment_layer_viewport 
after noticing the extensions in the vertex shader, without needing to put it in
the fragment shader, but it's a coin-toss

Benchmark on GTX 1050:

Previous tutorial (no instanced rendering)

	With blur, in center of world 
	Performance of Vertex shader and Fragment shader
		1st Eye: 0.427ms
		2nd Eye: 0.429ms
		Blur eyes: 1.66ms
		Full Frame: 2.54ms

	No blur, camera way off in the void
	Performance of Vertex shader
		1st Eye: 0.243ms
		2nd Eye: 0.248ms
		Full Frame: 0.502ms

Current tutorial (instanced rendering)

	With blur, in center of world 
	Performance of Vertex shader and Fragment shader
		Both eyes: 0.855ms
		Blur eyes: 1.66ms
		Full Frame: 0.253ms

	No blur, camera way off in the void
	Performance of Vertex shader
		Both eyes: 0.487
		Full Frame: 0.503ms	

With this information, we know that almost zero time was saved by
reducing CPU overhead when the command buffer was shortened. 
A difference might be more noticable if the scene was absolutely huge,
because in theory CPU overhead is reduced by "some" amount. 

Next we will experiment with other optimizations