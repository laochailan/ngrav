# ngrav

This is a simple simulation of a system with n particles with a gravitational force attracting them. In order to reduce the computational cost of this, the Barnes-Hut-Tree method was used.

This was a one-weekend project, so some parts are rather unrefined.

## Usage

This is the really unrefined part:

```
./ngrav
```

does the calculation and saves it into a file.

```
./ngrav FILENAME
```

renders the result from a saved file.

```
./ngrav something something
```

More than one argument makes ngrav calculate and render at the same time. Low frame rate. But you can press `T` to see the Barnes-Hut-Tree.
