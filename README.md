# ngrav

This is a simple simulation of a system with n particles with a gravitational force attracting them. In order to reduce the computational cost of this, the Barnes-Hut-Tree method was used.

This was a one-weekend project, so some parts are rather unrefined.

## Usage

To start real-time simulation enter:

```
./ngrav
```

If the frame rate is too low, you can precalculate the simulation to a file using

```
./ngrav -r
```

and render the result using

```
./ngrav -p FILE
```

## Keyboard Controls

|Key | Function|
|----|---------|
|`←``↑``↓``→` | Rotate View |
|`z` | Zoom out |
|`x` | Zoom in |
|`c` | Show traces (hold down) |
