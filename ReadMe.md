## COMP465
## Hongyou Xiong

# Warbirds Simulation Phase 3

**Summary:**
Phase 3 of the WarBirds simulation targets shaders and texture buffers. The goal is to generate a skybox for the Ruber star system, which is comprised of 6 planes along the Positive X Y and Z directions. I implemented this using 6 different planes, each rotated and translated to the corresponding locations and then applied a texture to each.

The other half of phase 3 was to implement 4 different lighting models that could be toggled on or off. Default ambient lighting is the first one. This can be turned off by pressing the A key. The second lighting model is the Head Light model. This applies a light in the direction of the looking vector in the view matrix. The ambient lighting portion of the head lights can be turned off if the default ambience is also turned off.

The third model is the point lighting model. This applies from the origin (Ruber) and radiates outwards. There is no attenuation on this light source, which means that the light source does not diminish with distance.

The last lighting model is the spot light model. This acts much like a flashlight. There's a direction the spotlight points in, and the point its looking at has the highest increase in intensity. it drops off as the light spreads outwards and there's a cutoff angle that prevents light after a certain angle. For this phase, it is being added to the follow camera for the ship and there will be a cutoff angle of about 5 degrees. Unfortunately I was unable to get this working prior to submission due to time constraints. I have spot light that functions based on the orientation of the follow camera but it doesn't line up correctly. Right now I have the debug colors set where if the angle is within 5 degrees, the color will be red. The color outside of that would be whatever the radian value of the angle is when the spotlight is turned on. It would be easy to turn those off but I believe it is easier for you to see that it is somewhat working with the debug on.
 
**Points of Interest:**
	* Skybox is comprised of 6 different textures on 6 different planes.
	* Spot Light is half-working. Cutoff angle is 5 degrees. when turned on, everything outside cone is colored white -> gray based on the radian angle between the spotlight and the pixel. Everything in the cone is red. Easiest to see using the top camera while holding left or right on the ship. Turning roughly 90 degrees to the left will cause the cone to hit Ruber at start location. Removed attenuation to help debug but unfortunately was unable to finish.

**Changes to the specs:**
	* Skybox is not affected by lighting. Always on.

**Changes from Phase 2 feedback**
	* Adjusted turn speed to .02f instead of .2f. Had the values set 10x higher than intended.

# Warbirds Simulation Phase 2

**Summary:**
Phase 2 of the WarBirds simulation targets user input, gravity, and orientation. The ship that was originally spawn in phase 1 now has controls to move as well as rotate along all three axes. I made sure to implement a system that would allow for some amount of key buffering so that you can rotate in two directions or three at the same time.

There was also work to make sure missiles would trace a smooth pathing, which I attempted to build my own solution. However reading through more documentation, I found that GLM has a built-in rotation function to give you the rotation between two vectors in quaternion form. I used this to create a function to "orientTowards". Another thing to do would be to smooth out the rotations by giving a maximum rotation per update, but did not feel like it was necessary. Missiles felt smooth enough.

I separated out more of the files as different classes grew bigger and contained more logic. Warbirds, Missiles, and the Missile bases were all separated into their header and cpp files. I could not #include the includes465 library, which made me have to duplicate the "colinear" function just in case I needed it.

I also fixed the cameras in Phase 1 as they did not correctly follow the right trajectory as it followed each planetoid.

I added in some logic to make the ship as well as its missiles to stop rendering if they were destroyed. While doing this with the missile bases however, there was some corruption of buffers and I did not have enough time to investigate what went wrong there so those do not support that functionality.

Gravity can be enabled. I opted to have all the mini planets produce no gravity and only gravity is exerted from the Ruber sun.

Also because the planets spawn so close to the warship, I added in a short delay the start while the simulation is preparing to render so that a stray missile from Secundus would not kill the player prior to having a chance to react now that missiles are faster.

I ended up spending a lot of time here working on trying to get the quaternions to work properly. I missed one multiplication where the rotations were out of order which ended up costing a lot of time.

**Changes to the specs:**
	The second phase was the create missiles that would home onto the targets after a short period of getting ready. I adjusted some of the values because it was hard to test with such long time in between pushing the buttons. As a result:
	* Missiles from the Warbird velocity increased from 10 --> 30
	* Missiles from the Missile silos increased from 5 --> 25
	* Windup period decreased from 200 --> 50
	* Missiles are aimed from starbases at the Warbird instead of in the direction of the planet in order to avoid missiles clipping back through the base

**Added Keys:**
	* w - Warps you to the next Planetoid (Unum or Duo)
	* f - Fires a missile from the warbird
	* g - Toggles Ruber's gravity
	* t - Change the time quantum to increase simulation speed or slow it down
	* s - Change the forward thruster speed on the WarBird
	* Arrow up and Arrow down - control forward and backward thruster
	* Arrow left and Arrow right - control yaw of the ship
	* Ctrl+ArrowUp and Ctrl+ArrowDown - control pitch of the ship
	* Ctrl+ArrowLeft and Ctrl+ArrowRight - control roll of the ship

# Warbirds Simulation Phase 1

**Summary:**

Phase 1 of the WarBirds simulation was targetted at having a model made for each entity (Ruber, Unum, Duo, Primus, Secundus, WarBird and a missile). In addition to these, there were 5 different types of cameras, and Primus and Secundus rotated around Duo.

I chose to move as much of the standalone variables I saw in the tutorials and other example code into a more object oriented manner. I created a SimObject class which stores the various variables pertaining to each individual object, and use that instead of arrays. Also, I moved away from char* and standalone arrays for Standard Template Library objects like std::string and std::vector. These tend to be a little easier to work with.

The title bar is updated on idle while the simulation is updated based on the timerDelay set at the top of the class.

**How To Run:**
	1. Extract folder in a location next to the include465 directory.
	2. Compile
	3. Run

**Keys:**
	* q or Q will exit the program
	* v or V will cycle the cameras forward
	* x or X will cycle the cameras backwards

**Camera Order:**
	* Front
	* Top
	* Warbird
	* Unum
	* Duo

**Code Files:**
	* WarBirdsSim.cpp contains most of the logic
	* SimObject.h defines the SimObject class and its various subclasses
	* SimObject.cpp contains the information for how to update rotation as well as all the defined constants
	* \*.tri files included in the Models directory are the ones being used in the simulation.

**NOTE:** Markdown is used in the ReadMe in case I want to store code files in GitHub at some point.
