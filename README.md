# Paint
A simple paint application made using SDL2


## Features
- Color palette consisting of 9 colors
- 4 different writing modes (brush, straight line, circle and rectangle drawing)
- Erasing Mode
- Save painting feature

## Requirements

- Gcc compiler
- SDL2 library

## Compilation and Execution

1. **Clone the repository:**
    ```sh
    git clone <repository_url>
    cd <repository_directory>
    ```

2. **Compile the program:**
    ```sh
    gcc paint.c -o paint $(sdl2-config --cflags --libs) -lm
    ```

3. **Run the program:**
    ```sh
    ./paint
    ```


## Use of the Application
The application is supported with keyboard shortcuts, the '1' key to switch to brush mode, the '2' key to switch to straight line mode, the '3' key to switch to circle drawing mode and the '4' key to switch to the rectangle drawing key.

While drawing can be done with the left click of the mouse, the eraser works with the right click. For continuity, it is necessary to keep the relevant keys pressed.

While the 's' key of the keyboard is used to save the drawing, the 'space' key provides easy exit.


## Features Expected to Come in the Future
- Ability to change the size of brush and eraser
- Add text mode
- Ability to fill inside shapes
