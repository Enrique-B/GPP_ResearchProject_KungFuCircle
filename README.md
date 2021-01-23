# KungFuCircle
![Project Image](https://github.com/Enrique-B/GPP_ResearchProject_KungFuCircle/blob/master/PicturesForReadMe/KungFuCircle.gif)

---
### Table of Contents
- [Introduction](#Introduction)
- [HowItWorks](#HowItWorks)
- [Framework](#Framework)
- [Implementation](#Implementation)
- [FutureWork](#FutureWork)
- [References](#References)

## Introduction
Have you ever wondered why your fighting game you're making feels unbalanced, that you never have control over how many enemies attack, and what attacks they use. 
do you feel that you have no control over this and are thinking how games like Prince of Persia and Assassins Creed have done it? The answer is probably the Kung Fu Circle. 
The Kung Fu Circle works like in the typical kung fu movies where enemies are standing around the main character and attacking one by one.

## How It Works
![HowItWorksImage](https://github.com/Enrique-B/GPP_ResearchProject_KungFuCircle/blob/master/PicturesForReadMe/KungFuCircle.png)

The player has a circular grid around him that's world space aligned and has 8 nodes.
The grid has a grid capacity (amount of enemies that can attack at once) and an attack capacity (limits the amount of attacks from the enemies and types of attacks)
Every enemy has a grid weight and every attack has an attack weight. 
In the grid we also have 3 circles (in the reference I used they only use 2 but I made a 3rd one so we can access all the enemies and let them wait outside the other 2) 

- The attack circle or inner circle
- The approach circle or outer circle 
- The waiting circle 

Let’s start from furthest away from the player to the closest 
The furthest one is the waiting circle where the enemies are waiting to get their request accepted to go to the approach circle. 
To get that request accepted we need nodes available so the enemy can walk there, and we also need to make sure that the enemy his
grid weight together with the other enemies their grid weight must be less than the grid capacity. 
When the enemy is in the approach circle, they have to request (this is similar like the request to get into the approach circle) if they can attack the player.
Whenever they attacked the player they have to get out of the attack and approach circle and wait in the waiting circle to wait for their request to get accepted to go back to the approach circle.


## Framework
I didn't use Unreal Engine or Unity for this project but the Elite Framework from Digital Arts and Entertainment (https://www.digitalartsandentertainment.be/)
provided for the course Gameplay Programming

I personally made some changes to the BlendedSteeringbehavior so I could change the individual behaviors their TargetData and weight in the behavior.
The system isn't perfect and it's not idiot proof but it does it's job

## Implementation
![FiniteStateMachine](https://github.com/Enrique-B/GPP_ResearchProject_KungFuCircle/blob/master/PicturesForReadMe/KungFuCircle%20StateMachine.png)

In my implementation I made a finite state machine state machine to control the enemies their behavior and in the blackboard I put something they called the Stage Manager.
The stage manager has the player's grid and decides if an enemy goes into the approach circle and if he can attack and chooses what attack the enemy will use it's kind of a commander in an army. 


## FutureWork
The implantation isn't perfect by far. The enemies are sometimes still missing the player (probably because of the linear drag) and having to return to the player 
that snowball effects to hitting other enemies and them missing the player. The same thing happens when they're returning from the player to the waiting circle. 

There are still some features that I left out of scope that I still want to implement later on in a different framework like unreal or unity 
- making sure that the enemies can’t attack when they're out of the camera vision
- making sure that the enemies go around the circle to get to the node instead of crossing it and hitting other enemies in the process 
- making a similar system like this but then for ranged enemies 
- implementing that the player can walk and looking if there is an enemy closer that can attack

## References
http://www.gameaipro.com/GameAIPro/GameAIPro_Chapter28_Beyond_the_Kung-Fu_Circle_A_Flexible_System_for_Managing_NPC_Attacks.pdf
