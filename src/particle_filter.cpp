/**
 * particle_filter.cpp
 *
 * Created on: Dec 12, 2016
 * Author: Tiffany Huang
 * upgraded  by: Guisheng Wang
 */

#include "particle_filter.h"

#include <math.h>
#include <algorithm>
#include <iostream>
#include <iterator>
#include <numeric>
#include <random>
#include <string>
#include <vector>
#include "helper_functions.h"

using namespace std;
using std::string;
using std::vector;
using std::normal_distribution;

static int NUM_PARTICLES=100;
static double  EPS=0.0001;

std::default_random_engine gen;


void ParticleFilter::init(double x, double y, double theta, double std[]) {
  /**
   * TODO: Set the number of particles. Initialize all particles to 
   *   first position (based on estimates of x, y, theta and their uncertainties
   *   from GPS) and all weights to 1. 
   * TODO: Add random Gaussian noise to each particle.
   * NOTE: Consult particle_filter.h for more information about this method 
   *   (and others in this file).
   */

  if (is_initialized) {
    return;
  }

  num_particles = NUM_PARTICLES;  // TODO: Set the number of particles
  // std[] is the std 
  double  std_x=std[0];
  double  std_y=std[1];
  double  std_theta=std[2];

  //  x, y are  location from GPS
  normal_distribution<double> dist_x(x,std_x);
  normal_distribution<double> dist_y(y,std_y);
  normal_distribution<double> dist_theta(theta, std_theta);

  for (int i=0;i<num_particles;i++) {

    Particle p;
    p.id=i;
    p.x=dist_x(gen);
    p.y=dist_y(gen);
    p.theta=dist_theta(gen);
    p.weight=1.0;
    particles.push_back(p);


  }

  is_initialized=true;

  return;

}

void ParticleFilter::prediction(double delta_t, double std_pos[], 
                                double velocity, double yaw_rate) {
  /**
   * TODO: Add measurements to each particle and add random Gaussian noise.
   * NOTE: When adding noise you may find std::normal_distribution 
   *   and std::default_random_engine useful.
   *  http://en.cppreference.com/w/cpp/numeric/random/normal_distribution
   *  http://www.cplusplus.com/reference/random/default_random_engine/
   */

  
  normal_distribution<double> dist_x(0,std_pos[0]);
  normal_distribution<double> dist_y(0,std_pos[1]);
  normal_distribution<double> dist_theta(0, std_pos[2]);

  for (int i=0; i <num_particles;i++) {
    if(fabs(yaw_rate)<EPS){
      particles[i].x+=velocity*delta_t*cos(particles[i].theta);
      particles[i].y+=velocity*delta_t*sin(particles[i].theta);
    }
    else{
      particles[i].x+=velocity/yaw_rate*(sin(particles[i].theta+yaw_rate*delta_t)-sin(particles[i].theta));
      particles[i].y+=velocity/yaw_rate*(cos(particles[i].theta)-cos(particles[i].theta+yaw_rate*delta_t));
      particles[i].theta+=yaw_rate*delta_t;
    }


    particles[i].x+=dist_x(gen);
    particles[i].y+=dist_y(gen);
    particles[i].theta+=dist_theta(gen);
  }

}

void ParticleFilter::dataAssociation(vector<LandmarkObs> predicted, 
                                     vector<LandmarkObs>& observations) {
  /**
   * TODO: Find the predicted measurement that is closest to each 
   *   observed measurement and assign the observed measurement to this 
   *   particular landmark.
   * NOTE: this method will NOT be called by the grading code. But you will 
   *   probably find it useful to implement this method and use it as a helper 
   *   during the updateWeights phase.
   */
  int n_observation=observations.size();
  int n_predicted=predicted.size();

  for (int i=0; i<n_observation;i++){
    double minD=std::numeric_limits<double>::max();
    for (int j=0;j<n_predicted;j++){
      double distance=dist(observations[i].x, observations[i].y, predicted[j].x, predicted[j].y);
      if (distance <minD) {
        minD=distance;
        observations[i].id=predicted[j].id;
      }

    }

  }

}

void ParticleFilter::updateWeights(double sensor_range, double std_landmark[], 
                                   const vector<LandmarkObs> &observations, 
                                   const Map &map_landmarks) {
  /**
   * TODO: Update the weights of each particle using a mult-variate Gaussian 
   *   distribution. You can read more about this distribution here: 
   *   https://en.wikipedia.org/wiki/Multivariate_normal_distribution
   * NOTE: The observations are given in the VEHICLE'S coordinate system. 
   *   Your particles are located according to the MAP'S coordinate system. 
   *   You will need to transform between the two systems. Keep in mind that
   *   this transformation requires both rotation AND translation (but no scaling).
   *   The following is a good resource for the theory:
   *   https://www.willamette.edu/~gorr/classes/GeneralGraphics/Transforms/transforms2d.htm
   *   and the following is a good resource for the actual equation to implement
   *   (look at equation 3.33) http://planning.cs.uiuc.edu/node99.html
   */
  for (int i=0; i<num_particles;i++){

    double particle_x=particles[i].x;
    double particle_y=particles[i].y;
    double particle_theta=particles[i].theta;
    vector<LandmarkObs> lm_in_range;

    for (unsigned int j=0; j<map_landmarks.landmark_list.size();j++) {


      particles[i].weight=1.0;

      float lm_x=map_landmarks.landmark_list[j].x_f;
      float lm_y=map_landmarks.landmark_list[j].y_f;
      int lm_id=map_landmarks.landmark_list[j].id_i;
      double distance =dist(particle_x,particle_y,lm_x,lm_y);
      if (distance < sensor_range){
        lm_in_range.push_back(LandmarkObs{lm_id,lm_x,lm_y});
      }
    }

    // convert coordinates
    vector<LandmarkObs> obs_m;
    double cos_theta=cos(particle_theta);
    double sin_theta=sin(particle_theta);

    for (unsigned int j=0; j<observations.size();j++){

      LandmarkObs tmp;
      tmp.x=observations[j].x*cos_theta-observations[j].y*sin_theta+particle_x;
      tmp.y=observations[j].x*sin_theta+observations[j].y*cos_theta+particle_y;
      obs_m.push_back(tmp);
    }

    dataAssociation(lm_in_range, obs_m);

    for (unsigned int j=0; j<obs_m.size();j++) {

      Map::single_landmark_s landmark=map_landmarks.landmark_list.at(obs_m[j].id-1);
      double x_term=pow(obs_m[j].x-landmark.x_f,2)/(2*pow(std_landmark[0],2));
      double y_term=pow(obs_m[j].y-landmark.y_f,2)/(2*pow(std_landmark[1],2));
      double w=exp(-(x_term+y_term))/(2*M_PI*std_landmark[0]*std_landmark[1]);
      particles[i].weight*=w;

    }

    weights.push_back(particles[i].weight);

  }


}



void ParticleFilter::resample() {


  vector<double> weights;
  double maxWeight=numeric_limits<double>::min();
  for (int i=0;i<=num_particles;i++) {
    weights.push_back(particles[i].weight);
    if (particles[i].weight>maxWeight) {
      maxWeight=particles[i].weight;
    }
  }

  uniform_real_distribution<double> distDouble(0.0, maxWeight);
  uniform_int_distribution<int> distInt(0, num_particles-1);
  int index=distInt(gen);
  double beta=0.0;
  vector<Particle> resampledParticles;
  for (int i=0;i<num_particles;i++) {
    beta+=distDouble(gen)*2.0;
    while (beta>weights[index]) {
      beta-=weights[index];
      index=(index+1)%num_particles;
    }
    resampledParticles.push_back(particles[index]);
  }

  particles=resampledParticles;

}


void ParticleFilter::SetAssociations(Particle& particle, 
                                     const vector<int>& associations, 
                                     const vector<double>& sense_x, 
                                     const vector<double>& sense_y) {
  // particle: the particle to which assign each listed association, 
  //   and association's (x,y) world coordinates mapping
  // associations: The landmark id that goes along with each listed association
  // sense_x: the associations x mapping already converted to world coordinates
  // sense_y: the associations y mapping already converted to world coordinates
  particle.associations= associations;
  particle.sense_x = sense_x;
  particle.sense_y = sense_y;
}

string ParticleFilter::getAssociations(Particle best) {
  vector<int> v = best.associations;
  std::stringstream ss;
  copy(v.begin(), v.end(), std::ostream_iterator<int>(ss, " "));
  string s = ss.str();
  s = s.substr(0, s.length()-1);  // get rid of the trailing space
  return s;
}

string ParticleFilter::getSenseCoord(Particle best, string coord) {
  vector<double> v;

  if (coord == "X") {
    v = best.sense_x;
  } else {
    v = best.sense_y;
  }

  std::stringstream ss;
  copy(v.begin(), v.end(), std::ostream_iterator<float>(ss, " "));
  string s = ss.str();
  s = s.substr(0, s.length()-1);  // get rid of the trailing space
  return s;
}