# Vox
This project have for goal to display a 3D cube world, generate proceduraly, like minecraft.

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
./ft_vox
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
