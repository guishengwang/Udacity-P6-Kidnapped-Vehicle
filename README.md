#  **Kidnapped Vehicle** 
## Udacity Self-Driving Car Nanodegree Project 6


**The goals / steps of this project are the following:**

* Establish communication between c++ program and simulator via uWebSocketIO
* Using particle filter to find the location of the kidnapped vehicle.
* Assign and update weight the particle for each step.
* Resample the particles to remove the paritiles with small weight.


[//]: # (Image References)

[image1]: ./images/prediction1.png "yaw rate 0"
[image2]: ./images/prediction2.png "yaw rate not 0"
[image3]: ./images/transformation.png "transformation"
[image4]: ./images/weight.png "weight"
[image10]: ./images/success.png  "success"

###  Environment set up

The environment setting is the exactly same as previous project (Extened Kalman Filter), I only need do the followings: 

* clone project from Udacity's repo on Github
```sh
git clone https://github.com/udacity/CarND-Kidnapped-Vehicle-Project
```
* edit the file of "particle_filter.cpp" and compile/build the excutalbe file to pass the simulator.

* push project to my repo on Github and submit the project
```sh
git push https://github.com/guishengwang/Udacity-P6-Kidnapped-Vehicle
```



### Submission

File Name | Description
----------|-----------
particle_filter.cpp | init, prediction, dataAssociation,updateWeights,resample
writeup_report.md | summary of the project and the same contents as README.MD


### Coding Process

There are 5 fuctions need to be completed. 
* init()
* prediction ()
* updateWeights () which calls the fucntion of dataAssociation ()
* resample ()

#### input data

The input data from simulator includes followings


* Map data

Landmark location in the map coordination, data was given by the file of "map_data.txt" in the folder of "data" as below

X  | Y | ID
----------|-----------|-----------
92.064|-34.777|1
61.109 |-47.132|2
17.42|-4.5993|3
...|...|...

* Controls

The fuction of main() gets control parameters from the simulator and passes to the function of prediction

parameter| description
----------|-----------
previous_velocity| vehicle velocity
previous_yawrate | vehicle yawrate

* Observations

The fuction of main() gets observations data from the simulator and passes to the function of updateWeights

parameter| description
----------|-----------
noisy_observations| observations data of landmark

* standard deviation of noise

standard deviation  |Value | Description
----------|-----------|-----------
sigma_pos [3]|  {0.3, 0.3, 0.01} |GPS measurement uncertainty 
sigma_landmark [2] | {0.3, 0.3} |Landmark measurement uncertainty


#### init()

The fuction of main passes the GPS data (x, y, theta) from the simulator to the fuction of init() as well as the standard devision of the noise. I created 100 particles by using the GPS data for location and noise standard deviation, and at last I assigned the same weight to each of these particles.


#### prediction ()

The prediction of the vehicle movement is quite straightforward and based on the formula below

* Yaw rate=0

 ![alt text][image1]
 
* and Yaw rate is not 0

  ![alt text][image2]

* After calucaltion of the new location and theta of the particle, we need to add the random Gaussian noise for poisition.

#### updateWeights ()

Based on the coordinations of each particle, we need to do following steps to assign the weight to each particle. 

* Select a group of landmarks with the range of the sensor

Given coordiantion the particle and landmarks, select the landmark only which has a distance less than the range of sensor. 

* Transformation

  ![alt text][image3]
  
The location of obervarions of landmark need to be transformed into map coordiantion based the particles data (x, y, theta), the above formula from class is used to achieved this step. 

* Assiciation by call the function of dataAssociation ()


  Nearest Neighbor method is used to find out the closed landmark to each observed measurement and assigned the id of the landmark to the measurement. 

* calcuation the weight of the particle.

 ![alt text][image4]
 
 Finally the weight of particle is calculated by using the above formula.



#### resample ()
Resampling is the most difficulty part for me to code. I watched the video made by Sebastian many times than I can count but still have trouble. My mentor did give a very good explanatio which help me a lot. 


## Running the Code

* Make a build directory: `mkdir build && cd build`
* Compile: `cmake .. && make` 
* Run it: `./particle_filter ` and the command line will show "Listening to port 4567"
* Run simulator, click "play" button , select " Project 3: Kidnapped Vehicle " and bash command line will show "Connected!!!".
* Click "Start" Button to start and the code will success if the simulator shows as below

 ![alt text][image10]


## Discussion
This is another interesting project. To be honest, at the begining, I didnot understand the logic behind the particle filter. Why we have to create so many particles and do these iterations, while I started to work on the project and try to put every piece of information together and I started to understand the logic. The most difficult part for me is the resample wheel while my mentor did help me try to undestand it.

Many thanks to the Udacity team and looking forward to the next challenge!




