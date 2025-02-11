# 3D Interpolated Cube Drawing

In this project, a 3D interpolated cube drawing is implemented based on the specified features and requirements.

## Features

- **Indices Array**: Determines which points will form the triangles to be drawn.
- **Vertices Array**: Defines the color values for each point. Each point has 7 elements of data, including 3 elements for position and 4 elements for color information (RGBA). The 4th value is the alpha parameter.
- **Alpha Channel Activation**: Colors are blended by enabling the alpha channel with the commands `glEnable(GL_BLEND)` and `glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)`.
- **Model Matrix Calculation**: Using the `glm` library, the model matrix is calculated to determine the rotation of the cube.

## Technologies Used

- **OpenGL**: Used for drawing and rendering graphics.
- **GLM**: Used for calculating the transformation matrices of the graphics.

## Output:

! [output:](output.png)

