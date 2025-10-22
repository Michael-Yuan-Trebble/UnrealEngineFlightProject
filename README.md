# UnrealEngineFlightProject

Website for the project is located [at this link](https://unrealenginepersonalflightproject.netlify.app/)

A custom flight simulation project built in Unreal Engine 5 using C++ and Blueprints.
The goal of this project is to explore advanced gameplay programming and implementing flight mechanics and physics in the Unreal Engine.
Inspirations of my project come from flight games/simulators such as War Thunder, Ace Combat, and Project Wingman.

## Goals
Some of my goals of the project are:
- Learning UE5 C++ programming and separation of logic
- Learning how to implement aircraft flight physics
- Building HUD and UI systems needed for flight simulators
- Gain experience in designing modular and scalable game systems
- Gain experience in optimization in game making

## Features

### Thrust / Speed
Currently, the calculations for velocity use a Engine State Model in order to make slowing down and speeding up more pronounced. Each state has a designated speed when the speed nears that target, drag is applied using a Sigmoid Function.
Each aircraft has a maximum speed and acceleration value, with the acceleration showing the acceleration at standard thrust, with afterburning and air braking causing the acceleration to change drastically. Drag is also applied from both Angle of Attack and depending on the climb/dive angle of the aircraft. I plan to make these more extreme in the future however to make testing other functions easier I have made them relatively small.

### Angle of Attack (AoA)
Flight models have a simple implementation of Angle of Attack, i.e. the direction that the aircraft is heading doesn't not match where the aircraft's nose is pointing. In my implementation, angle of attack is shown through two lines,
the green line is where the nose is pointing while the blue line is the forward vector. If left untouched, the forward vector will naturally go back to being the same as the aircraft's nose forward, which a variable determing how fast this 
calculation happens is determined by a data asset in the Unreal Editor. Below are some visual examples of my implementation:

![AoA Demo](Gifs/AoA-Gif.gif)

![AoA Sideview Demo](Gifs/AoA-Angle2-Gif.gif)

Another Angle of Attack variable is the current orientation of the aircraft, as I've implemented a "downwards force" for the Angle of Attack when the aircraft starts going inverted toward the ground as seen below:

![AoA Gravity Demo](Gifs/Gravity-Gif.gif)

### Aircraft Selection / Display
Currently there is a level that displays all available aircraft and from that, select weapons and a special. This is then transfered from the gamemode's player state to the overall game instance in order to equip the right 
items in actual combat level. Hovering over options displays the object temporarily and clicking keeps that selection on screen, there are also aircraft designated "permanent", meaning they can't be sold and come free, while all the other aircraft have a cost and the button is clickable once the player has the right amount of currency.

### Basic Missile Functionality
There are two groups of missiles as of now, however they function the same except for visually. Infrared (IR) Missiles and Active Radar Homing (ARH) Missiles, both lock on and track a target after firing and upon hit deal damage. They also destroy themselves upon impacting the ground and can be fired without lock. Missiles have exhaust effects and ARH Missiles first go through a "drop" phase instead of immediately going off the rail like IR, currently they also use Projectile Movement and use stats from data assets. The plan for ARH missiles is to eventually have multilock on for all available missiles on pylons in order to have some distinction that ARH missiles are faster to fire due to their long range, Active Homing nature, and multiple data links on modern aircraft. As of now there is no plan to make how many ARH missiles can be simultaneously launched at once, however this may change if it helps gameplay and makes progression more significant.

![Missile Launch Demo](Gifs/Missile-Gif.gif)

### Re-Equipping and Grouping
Weapons are grouped with each other, making it so the player can fire all of the same type at once instead of switching constantly. Weapons can also re-equip after a cooldown, which is stored in a data asset.

![Re-Equip Demo](Gifs/ReEquip-Gif.gif)

### Shooting
Aircraft can shoot now and these bullets deal damage, destroy upon impact, and use impulse to traverse. They have a simple visual effect and aircraft have the ability to change bullets, each of which has its own data asset for stats. In the future I will add a visual in order to indicate lead needed to hit a target, mimicing a radar gun sight.

![Shooting-Demo](Gifs/Shoot-Gif.gif)

### Basic AI
Very primative AI as of now, it calculates the roll needed to pitch up into its given target and can move, however it doesn't implement ideas such as lag pursuit or changes state from attack.

## Tech Stack
- Unreal Engine 5 (C++ and Blueprints)
  - Niagara FX
  - UMG for HUD
  - Microsoft Visual Studio 2022
- Blender
  - All models and aircraft visual effects are made by me.
- Photoshop

## Future Plans
Some of my future goals are:
- Expanding and making a "tree" of options for aircraft, for example:
  - The F-15C is the "base" with the options of the F-15E (Strike) and F-15N (Experimental Naval) being available
- A more polished version of air combat with energy being more in play
- Advanced weather and extreme weather conditions, affecting AI and user experience
- Missile Tracking and Fooling via flares and chaff
- Implement a more robust HUD and display more necessary values to the user
