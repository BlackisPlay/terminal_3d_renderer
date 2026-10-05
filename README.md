# Terminal 3D Renderer

<!-- Add a screenshot or GIF of your renderer here: ![Renderer Demo](demo.gif) -->

A simple ASCII 3D renderer written in C that displays rotating 3D models directly in your terminal.

## Compilation

Compile the C source code using `gcc`. The math library (`-lm`) is required, and the `-O3` flag is highly recommended for optimal rendering performance:

```bash
gcc main.c ray_triangle.c stl_loader.c -o animate -O3 -lm
```

## Execution

Run the executable and provide an `.stl` file as an argument:

```bash
./animate MODEL_NAME.stl
```

### Examples

```bash
./animate donut.stl
# or
./animate monkey.stl
```

## Configuration

You can customize the renderer by modifying the macros in the source code before compiling:

* **Field Size:** To change the dimensions of the terminal rendering field, modify the `WIDTH` and `HEIGHT` macros in `main.c`.
* **Model Size:** To change the scale of the rendered model, modify the `MAX_VERTEX_DIST` macro in `ray_triangle.c`.

## Controls

* **Exit:** Press `Ctrl + C` to stop the animation and exit the program.
