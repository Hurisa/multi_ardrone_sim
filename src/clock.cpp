#include "ros/ros.h"
#include <ros/console.h> 
#include <stdlib.h>
#include <geometry_msgs/TwistStamped.h>
#include <nav_msgs/Odometry.h>
#include <geometry_msgs/PoseStamped.h>
#include <std_msgs/String.h>
#include <std_msgs/Float32.h>
#include <std_msgs/Empty.h>
#include <tf2/LinearMath/Quaternion.h>
#include <tf2/LinearMath/Matrix3x3.h>
#include <tf2_geometry_msgs/tf2_geometry_msgs.h>
#include <nav_msgs/Odometry.h>
#include <rosgraph_msgs/Clock.h>
#include <time.h>
#include <iostream>
#include <vector>
#include <string>
#include <math.h>       /* cos */
#include <std_srvs/Empty.h>
#include <multi_ardrone_sim/posevector.h>
#include <rosbag/bag.h>
#include <multi_ardrone_sim/avoid.h>

using namespace std;



int main(int argc, char** argv){

	ros::init(argc, argv, "ClokcServer");	
	ros::NodeHandle nh;

	ros::Rate r(100);
	ros::Publisher ClockPub=nh.advertise<rosgraph_msgs::Clock>("/clock", 10);


	
	rosgraph_msgs::Clock t;
	while(ros::ok()){		

		t.clock = ros::Time::now();

		ClockPub.publish(t);
		ros::spinOnce();
		r.sleep();		
	}
}