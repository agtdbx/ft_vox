# Scop
This project have for goal to display a 3D object contain in .obj file.

## Setup
You need to do the following command to install some requirements.
```bash
make install
```
Other requierements will be installed locally by meson.
If you prefer to install all requirements, you can use instead this command.
```bash
make full_install
```

## Compilation
The compilation will be done by the meson. To facilitate it's usage, a make file is present.
For compiling, you just need to do this.
```bash
make
```

## Usage
### Start
```bash
cd release
./scop <path/to/file.obj> [path/to/texture.png]
```

### Keys
**Camera mode** :
- front			w
- back			s
- left			a
- right			d
- up			space
- down			left shift
- turn left		left
- turn right	right
- turn up		up
- turn down		down
- sprint		left control (for movement)
- p				print camera info

\
**Object mode** :
- +z			w
- -z			s
- -x			a
- +x			d
- +y			q
- -y			e
- turn +y		left
- turn -y		right
- turn -x		up
- turn +x		down
- turn +z		right shift
- turn -z		right control
- sprint		left control (for movement and rotation)
- =				scale up
- \-			scale down

\
**Other**
- tab			Switch between camera mode and object mode
- r				Enable/Disable object auto rotate
- t				Enable/Disable object texture
- g				Enable/Disable object gray scale
- l				Enable/Disable object lightning
