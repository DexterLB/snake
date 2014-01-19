Snake
=====
A clone of the classic game snake, built on Qt 5 (with C++), and uses standard
GUI widgets for rendering.

Quickstart
==========

```
mkdir build
cd build
qmake ../src
make
cd ..
./build/snake
```

Compiling
=========
The only required dependency is Qt 5.0+ (the game has only been tested with
the open-source version).
Download links: http://qt-project.org/downloads
(or better, use your distro's packages)

Then compile as a normal Qt project:

```
mkdir build
cd build
qmake ../src
make -j4
```

(replace j4 with the appropriate number of jobs for your system)

Running
=======
Make sure that the `config` directory is in one of the following locations:

```
./snake
~/.snake
~/.config/snake
```
and contains a proper `levels.json`

e.g. under Windows the first case will be easiest (copy the config directory
in the same folder as snake.exe)

#### Distribution
For binary distribution the game requires the following libraries:

```
icudt51
icuin51
icuuc51
libgcc
libstdc
pthread
Qt5Core
Qt5Gui
Qt5Widgets
```
And on 64-bit windows also requires `platforms/qwindows` (from Qt)

Gameplay
========
The game should be self explanatory, just jump into it

Configuration
=============
The game is configured using JSON. It first looks for `config/levels.json`,
which has the following syntax:

```
{
    "levels": [
        {
            "name": "My Awesome Level",
            "file": "awesome.json"
        },
        {
            "name": "Epic level!",
            "file": "epic.json"
        }
    ]
}
```
Each level has its own JSON file, which describes all the settings.

#### Settings
`size`: an array with 2 integers. Guess what they are.
`bg-clr`: background colour (accepts HTML notation)
`bg-img`: background image (overrides `bg-clr`)
`gameover-img`: an image that is displayed upon game over. Can have alpha.
`speed`: time (in milliseconds) between steps
`aspect`: aspect ratio of each node. Defaults to 1.

#### Nodes
The game is based on "nodes", where each node is a single square on the field.
In the JSON file you can set a number of starting position nodes, but the
only ones that are necessary are a few nodes of type `body`, which is part
of the snake's body.

Nodes are specified in the `nodes` array

Each node has these properties:
`type`: one of `body`, `apple`, `obstacle` or `void` (that last one is just an 
image that does nothing and the snake passes through).

`attr`: an integer that is used for determining the pixmap which will render
on the screen. Reserved `attr`'s are:

```
0: Snake torso
1: Snake head
2: Snake tail
3: Apple
4: Snake head (fat, the head upon eating an apple)
5: Snake torso (fat, the part of the snake that is digesting an apple)
```

`position`: an array with 2 items (you guessed it, x and y), e.g. [3, 4]

`orientation`: duh!

`bend`: a bend of some node (when making turns etc.). Valid values are
`none`, `cw` (clockwise) and `ccw` (counterclockwise)

#### Pixmaps
Pixmaps are specified in the `pixmaps` array. Each pixmap has these properties:

`attrs`: an array of integers that specifies which attributes will render with
this pixmap

`file`: the image file that contains the actual pixmap
(png|jpg|anything that Qt supports, see docs for QPixmap)

`bend`: the bends this pixmap will be used for. If this is not specified it
defaults to all bends.

#### For real see the examples in the config directory

Technical documentation (how it works)
======================================
#### Use the force: read the source!
Technical docs are doxygen-style, embedded in the comments.


#### Have fun playing the game!
