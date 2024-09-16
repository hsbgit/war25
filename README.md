# war2.5
An inofficial open source remake of Warcraft II: Tides of Darkness

# Disclaimer
This open source project is not affiliated, associated, endorsed by, or in any way officially connected with Blizzard Entertainment, Inc., or any of its subsidiaries or its affiliates. The official Blizzard Entertainment website can be found at http://www.blizzard.com.

This project is a completely independent and non-commercial undertaking. It is not intended for sales or profit, and no monetary gain is derived from its operation. This project is intended for **educational and informational purposes** only.

It is important to note that in order to use this open source project, **an original version of Warcraft 2 is required**. This project/repository does not provide any game assets from Warcraft 2.


# What is war2.5
Warcraft II: Tides of Darkness was released in Europe in 1996. As a small kid, I immediately fell in love with this game. I spent hundreds of ours playing it, I had endless network battles with my family and friends. Since Warcraft II had a heavy influence on me and I am still in love with this game, I decided to create war2.5, which is an inofficial open source remake of Warcraft II. It has the following goals:

- Keep the original Warcraft II feeling and be compatible with existing maps
- While offering a modern GUI and higher resolutions
- While adding new convient features such as unit production queues or multi unit selection
- Plus: Provide the opportunity to learn how to program (RTS) games

I started working on this in 2021 as a single developer during my free time. Now in January 2024 I realized that additional support is required and the next logical step is to release the current development state public on github.

[See Section *How to build*](#how-to-build)

[See Section *How to run*](#how-to-run-linuxwindows)

## First Impressions
Play your favorite map in any season. Press w,s,a,d to switch the season on the fly without reloading/restarting. Minimap can be seen at the top right.

<p align="center">
  <img src="https://github.com/hsbgit/war25/blob/main/github_media/0%20summer.png" width="45%"/>
  <img src="https://github.com/hsbgit/war25/blob/main/github_media/0%20swamp.png" width="45%"/>
</p>
<p align="center">
  <img src="https://github.com/hsbgit/war25/blob/main/github_media/0%20wasteland.png" width="45%"/>
  <img src="https://github.com/hsbgit/war25/blob/main/github_media/0%20winter.png" width="45%"/>
</p>


## Programming language: C++ and a bit of C
C++ is the programming language I started learning programming with. I wanted to create an open source project that uses object-oriented programming (OOP) in modern C++. Thus, classes, inheritance, polymorphism and further concepts are used. My goal was to write code that is maintainable and flexible while using modern C++ such as C++17 and C++20 features. You know Warcraft II and as a programmer you expect objects of type "Mage"? Then just look into Mage.h and Mage.cpp. Interested in how Peasants mine gold? Just open CompositeStateMining.h and have a look. 

The current game is designed using the RTS lockstep approach. This means, all relevant actions such as unit production or movements result in events that are executed at the beginning of a new gameframe (=tick). This is especially important when considering so far missing multiplayer code. The current code is far from perfect, but I think it allows you to contribute. You contribution is highly welcomed!

[See Section *How to build*](#how-to-build)

## What is the current state of development of war2.5?
As written, I worked for three years in my rare freetime, completely alone on this project. Nonetheless, this project has a quite advanced development state. war2.5 is at least a bit playable! Many game mechanics have already been implemented. However, several major components are still missing, such as proper animation and time/frame management, network, AI... Just some of the things that work:

-	Compatible with 99% of the existing *.pud files. Some *.pud lead to crashes but this is sth. for later investigation
-	Sound
-	Full working minimap
-	All human and orc buildings are fully functional
-	Town halls and towers can be upgraded
-	Technology upgrades are working
-	Peasants can chop wood, mine gold
-	(Land) Units can move, patrol
-	Fully working AStar path finding
-	Some units already have their special abilities, e.g. demolition dwarfes can explode, ogre mages can cast runes, etc.
-	...

[See Section *How to run*](#how-to-run)


## Big Thanks
This work would definitely not have been possible without

 - [Warcraft II PUDs utilities](https://github.com/war2/war2tools) written by Jean Guyomarc'h to handle pud files and icons
 - [Wargus Importer and scripts for Warcraft II](https://github.com/Wargus/wargus) which are used to initially extract media from the original Warcraft 2 copy

## What is missing in war2.5? What can I contribute?
Oh well, a lot of things are either missing, buggy or might be optimized. There are small things you could start with, e.g., when double clicking a unit all visible units of the same type should be selected. And there are big things you could contribute, e.g., optimizing pathfinding. Some more examples:

- Do not allow Townhall placement too close to goldmines
- Fix unit animations, e.g., Peasants currently always carry wood
- Peasants should only be able to build water buildings on the coastline, not on grass...
- Air units are implemented, but buggy as they cover 4 tiles. If you move them around, you will likely get a crash
- Same: Sea units are existing, but buggy as they cover 4 tiles. If you move them around, you will likely get a crash
- Pathfinding: shortest entry/exit between a Townhall and Goldmine (so far gold is returned to the top left tile of a Townhall)
- Correct forest and mountain tiles when wood is chopped or mountains exploded
- Annoying units by clicking on them
- Soundmanager: Just play a specific sound once per gameframe to avoid "sound cracks"
- Message system. E.g., if a peasant says "Work done" you should be able to press space to get to the message origin

As you can see, there are plenty of places to start with. You contribution is highly welcomed!

## More Impressions
**Debug mode** allows for, e.g., inspecting tiles: 
<p align="center">
  <img src="https://github.com/hsbgit/war25/blob/main/github_media/1%20debug_mode.png" width="100%"/>
</p>


**Exploration** is partially working:
<p align="center">
  <img src="https://github.com/hsbgit/war25/blob/main/github_media/2%20exploration.png" width="100%"/>
</p>


Buildings **have production queues**:
<p align="center">
  <img src="https://github.com/hsbgit/war25/blob/main/github_media/3%20production%20queues.png" width="60%"/>
</p>


Select and command as many units as you like:
<p align="center">
  <img src="https://github.com/hsbgit/war25/blob/main/github_media/4%20multi%20unit%20selection.png" width="60%"/>
</p>


Pathfinding using **AStar**:
<p align="center">
  <img src="https://github.com/hsbgit/war25/blob/main/github_media/5%20path%20finding%201.png" width="55%"/>
  <img src="https://github.com/hsbgit/war25/blob/main/github_media/5%20path%20finding%202.png" width="55%"/>
</p>


Play the original **game music** (or add own sound files):
<p align="center">
  <img src="https://github.com/hsbgit/war25/blob/main/github_media/6%20music%20player.png" width="40%"/>
</p>


Use the **pud browser** to load any map you want:
<p align="center">
  <img src="https://github.com/hsbgit/war25/blob/main/github_media/7%20pud%20browser.png" width="60%"/>
</p>


<p align="center">
  <img src="https://github.com/hsbgit/war25/blob/main/github_media/7%20pud%20browser%20map%20loaded.png" width="80%"/>
</p>



# How to build
## Linux
Building on linux has been done on Ubuntu.
**Note:** war2.5 can be built&run on Ubuntu also inside VirtualBox. 

### 1. Install Required Packages
```bash
sudo apt install libsdl2-image-2.0-0 libsdl2-image-dev libsdl2-mixer-2.0-0 libsdl2-mixer-dev libsdl2-ttf-2.0-0 libsdl2-ttf-dev git cmake  
```

### 2. Compile&Install war2tools
```bash
git clone https://github.com/war2/war2tools.git
cd war2tools
mkdir build
cd build
cmake ..
make
sudo make install
```

### 3. Compile&Install glfw
```bash
git clone https://github.com/glfw/glfw.git
cd glfw
mkdir build
cd build
cmake ..
make
sudo make install
```

### 4. Build war25
```bash
git clone https://github.com/hsbgit/war25.git
cd war25/src/build
cmake ..
make -j4
```
**DONE**

## Windows
Unfortunately, we have can’t simply do *sudo apt-get install* all required libs when using windows. Hence, we have to get the following third party libraries on our own. However, we do not build them; We simply use the provided releases.

**To make things even more simple, a fully functional set of libs is already included in 3rdparty/win. This means the following steps are not required unless you want to change to another lib version.**

### SDL2
* https://github.com/libsdl-org/SDL/releases
* Download pre-compiled release for visual studio
* E.g., SDL2-devel-2.30.0-VC.zip
* Unzip into src/3rdparty/win/, e.g., src/3rdparty/win/SDL2-2.30.0/

### SDL_image
* https://github.com/libsdl-org/SDL_image/releases
* Download pre-compiled release for visual studio
* E.g., SDL2_image-devel-2.8.2-VC.zip
* Unzip into src/3rdparty/win/

### SDL_mixer
* https://github.com/libsdl-org/SDL_mixer/releases
* Download pre-compiled release for visual studio
* E.g., SDL2_mixer-devel-2.8.0-VC.zip
* Unzip into src/3rdparty/win/

### war2tools
war2tools has no pre-compiled release. Thus, we have to compile this lib on our own:

* Clone https://github.com/war2/war2tools.git
* Extract
* Open with cmake-gui
* Set "Where to build the binaries" to a new "build" folder
* Generate
* Compile with visual studio
* Copy build/libpud/Release/libpud.lib and libwar2.lib into src/3rdparty/win/war2tools/lib
* Copy include folder to src/3rdparty/win/war2tools/include

### CMakeLists.txt
Change the windows src/3rdparty/win/ paths to the paths corresponding with the correct lib version numbers.

### CMake (cmake-gui)
* Install and start cmake-gui
* _Where is the source code:_ Enter absolute path to _src/_ folder
* _Where to build the binaries:_ Enter absolute path to existing _src/build_ folder
* Press _Configure_ button
* Press _Generate_ button
* E.g., Visual Studio solution file (*.sln) has been generated into _src/build_, open it

**Done**, compiling and linking should work.

### Required DLLs
Copy DLLs from their respective lib folders to the build folder
* SDL2_image.dll
* SDL2_mixer.dll
* SDL2.dll


# How to run (Linux&Windows)
This repository does not contain any Warcraft 2 assets. Hence, to run war2.5, a Warcraft 2 Battle.Net edition is required for data extraction. Your CD folder should look like this:

<p align="center">
  <img src="https://github.com/hsbgit/war25/blob/main/github_media/war2_iso_contents.png"/>
</p>

As a first step, we have to extract the assets (sounds, graphics, etc.) from the CD. This work would definitely not be possible without [Wargus Importer and scripts for Warcraft II](https://github.com/Wargus/wargus) which are used to initially extract assets from your original Warcraft 2 copy.

* [Download the wartool executable](https://github.com/hsbgit/war25/raw/main/wartool/wartool.zip). The wartool executable as a part of the Wargus project allows extracting required assets from your Battle.Net CD.
* Use wartool (Linux) or wartool.exe (Windows)
* Run (replace _your_warcraft2_cd_folder_ with the actual folder path):
```bash
wartool.exe your_warcraft2_cd_folder extracted_data
```
* Folder _extracted_data_ should now contain several folders
* Copy the folders _extracted_data/graphics_, _extracted_data/music_ and _extracted_data/sounds_ into the war2.5 _build/data_ folder
* The final result should look like this (the folders _maps_ and _tilesets_ where already included inside the  _build/data_ folder):

<p align="center">
  <img src="https://github.com/hsbgit/war25/blob/main/github_media/war25_build_data.png"/>
</p>

That's it, start war2.5!
