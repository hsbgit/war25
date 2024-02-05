# war25
An inofficial open source remake of Warcraft II: Tides of Darkness

# Disclaimer
This open source project is not affiliated, associated, endorsed by, or in any way officially connected with Blizzard Entertainment, Inc., or any of its subsidiaries or its affiliates. The official Blizzard Entertainment website can be found at [http://www.blizzard.com](http://www.blizzard.com).

This project is a completely independent and non-commercial undertaking. It is not intended for sales or profit, and no monetary gain is derived from its operation. This project is intended for **educational and informational purposes** only.

It is important to note that in order to use this open source project, **an original version of Warcraft 2 is required**. This project does not provide any game assets from Warcraft 2. 

# What is war2.5
Warcraft II: Tides of Darkness was released in Europe in 1996. As a small kid, I immediately fell in love with this game. I spent hundreds of ours playing it, I had endless network battles with my father and sister. Since Warcraft II had a heavy influence on me and I am still in love of this game, I decided to create war2.5, which is an inofficial open source remake of Warcraft II. It has the following goals:

 - Keep the original Warcraft II feeling and be compatible with existing maps
 - While offering a modern GUI and higher resolutions
 - While adding new convient features such as unit production queues or multi unit selection

I started working on this in 2021 as a single developer during my free time. Now in January 2024 I realized that additional support is required and the next logical step is to release the current development state public on github.
## Some Impressions

`<a href="URL_zum_großen_Bild.jpg" target="_blank">
    <img src="URL_zum_kleinen_Bild.jpg" alt="Alternativer Text" width="200px">
</a>`

## Programming language: C++ and a bit of C
C++ is the programming language I started learning programming with. I wanted to create an open source project that uses object-oriented programming (OOP) in modern C++. Thus, classes, inheritance, polymorphism and further concepts are used. My goal was to write code that is maintainable and flexible while using modern C++ such as C++17 and C++20 features. 
You know Warcraft II and as a programmer you expect objects of type "Mage"? Then just look into Mage.h and Mage.cpp. Interested in how Peasants mine gold? Just open CompositeStateMining.h and have a look. No digging around in messy source files to find where a specific piece of logic might be located. 

## What is the current state of development of war2.5?
As written, I worked for three years in my rare freetime, completely alone on this project. Nonetheless, this project already has a quite advanced development state. war2.5 is somewhat playable! Just some of the things that work:

 - Compatible with 99% of the existing *.pud files. Some *.pud lead to crashes but this is sth. for later investigation
 - Sound
 - Full working minimap
 - All human and orc buildings are fully functional
 - Town halls and towers can be upgraded
 - Technology upgrades are working
 - Peasants can chop wood, mine gold
 - (Land) Units can move, patrol
 - Fully working AStar path finding
 - Some units already have their special abilities, e.g. demolition dwarfes can explode, ogre mages can cast runes, etc.

## Big Thanks
This work would definitely not have been possible without

 - [Warcraft II PUDs utilities](https://github.com/war2/war2tools) written by Jean Guyomarc'h to handle pud files and icons
 - [Wargus Importer and scripts for Warcraft II](https://github.com/Wargus/wargus)  which are used to initially extract media from the original Warcraft 2 copy

## What is missing in war2.5? What can I contribute?
Oh well, a lot of things are either missing, buggy or might be optimized. There are small things you could start with, e.g., when double clicking a unit all visible units of the same type should be selected. And there are big things you could contribute, e.g., optimizing pathfinding. Some more examples:

 - Do not allow Townhall placement not too close to gold mines
 - Correct unit animations, e.g., Peasants currently always carry wood
 - Peasants should only be able to build water buildings on coast
 - Air units are implemented, but buggy as they cover 4 tiles. If you move them around, you will likely get a crash
 - Same: Sea units are existing, but buggy as they cover 4 tiles. If you move them around, you will likely get a crash
 - Pathfinding: shortest entry/exit between a Townhall and Goldmine (so far gold is returned to the top left tile of a Townhall)
 - Correct forest and mountain tiles when wood is chopped or mountains exploded
 - Annoying units by clicking on them
 - Soundmanager: Just play a specific sound once per click to avoid "sound cracks" 
 - Meassing system. E.g., if a peasant says "Work done" you should be able to press space to get to the message origin

As you can see, there are plenty of places to start with. You contribution is highly welcomed!


# How to build
## Windows


## Linux





# Publication

Once you're happy with a file, you can publish it to different hosting platforms like **Blogger**, **Dropbox**, **Gist**, **GitHub**, **Google Drive**, **WordPress** and **Zendesk**. With [Handlebars templates](http://handlebarsjs.com/), you have full control over what you export.
