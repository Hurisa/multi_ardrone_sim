#include "ros/ros.h"
#include <ros/console.h> 
#include <stdlib.h>
#include <geometry_msgs/TwistStamped.h>
#include <nav_msgs/Odometry.h>
#include <geometry_msgs/PoseStamped.h>
#include <std_msgs/String.h>
#include <tf2/LinearMath/Quaternion.h>
#include <tf2/LinearMath/Matrix3x3.h>
#include <tf2_geometry_msgs/tf2_geometry_msgs.h>
#include <nav_msgs/Odometry.h>

#include <iostream>
#include <vector>
#include <string>
#include <math.h>       /* cos */

#include <multi_ardrone_sim/posevector.h>

using namespace std;

#define PI 3.14159265


class Timer{

private:
	vector<float> avoidingTimes;	

public:


};


int main(int argc, char const *argv[])
{
	
	ros::init(argc, argv, "AvoidTimes");	
	ros::NodeHandle nh;	

	vector<string> namespaces;
	nh.getParam("/namespaces", namespaces);
	
	vector<Timer*> timers;

	ros::Rate r(30);
	while(ros::ok()){

		ros::spinOnce();
		r.sleep();		
	}
	return 0;
}