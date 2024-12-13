Starter repository for the course [CSE333 - *Computer Graphics*](https://techtree.iiitd.edu.in/viewDescription/filename?=CSE333) (Monsoon 2024), IIIT Delhi.  
  
This repository contains all of my assignment solutions completed throughout the course.  
The course project has been added as a submodule and can also be accessed at [surface-reconstruction](https://github.com/rahisenpai/surface-reconstruction).

## Prerequisites
Run the following commands on the Linux terminal.
```sh
sudo  apt-get  update
sudo  apt  install  build  essential
sudo  apt  install  g++
sudo  apt  install  mesa-common-dev
sudo  apt  install  mesa-utils
sudo  apt  install  make  cmake  cmake-curses-gui
sudo  apt-get  install  libglew-dev  libglfw3-dev  libglm-dev  libglu1-mesa-dev
```

## Instructions
- Build: `cmake CMakeLists.txt` in the directory containing CMakeLists
- Make: `make` to generate object files and compile them in a single execuatble
- Run: `./<executable-name>`