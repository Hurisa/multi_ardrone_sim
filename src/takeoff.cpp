#include "ros/ros.h"
#include <ros/console.h> 
#include <stdlib.h>
#include <geometry_msgs/TwistStamped.h>
#include <nav_msgs/Odometry.h>
#include <geometry_msgs/PoseStamped.h>
#include <std_msgs/String.h>
#include <std_msgs/Float32.h>
#include <std_msgs/Bool.h>
#include <std_msgs/Empty.h>
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

class takeoff{

private:


	ros::Publisher	TakeoffPub;
	ros::NodeHandle nh;
	string ns;

	bool takeoffVar;
public:

	takeoff(){
	
		TakeoffPub	= nh.advertise<std_msgs::Empty>("ardrone/takeoff", 10, this);
		takeoffVar		= false;
		ns	= ros::this_node::getNamespace();

	}

	void init(){	

		if (!takeoffVar){	
			std_msgs::Empty empty;
			TakeoffPub.publish(empty);
			takeoffVar=true;
		}
	}
};

int main(int argc, char** argv){

	ros::init(argc, argv, "toff");	
	takeoff initTakeOff;

	ros::Rate r(30);
	while(ros::ok()){

		initTakeOff.init();
		ros::spinOnce();
		r.sleep();		
	}
}
