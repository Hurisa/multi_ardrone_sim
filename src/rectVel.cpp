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
#include <ardrone_autonomy/Navdata.h>

using namespace std;

#define PI 3.14159265

class RectifyVel{

private:

		
	ros::NodeHandle nh;
	ros::Subscriber CmdVelSub;
	ros::Subscriber NavVelSub;

	ros::Publisher	PubVel;

	geometry_msgs::Twist cmdVel;
	geometry_msgs::Twist Velocity;
	ardrone_autonomy::Navdata navdata;

	float realVx, realVy, Kx, Ky;

	string ns;

public:

	RectifyVel(){
	
		CmdVelSub 	= nh.subscribe("ref_vel", 10, &RectifyVel::getCmdVel, this);
		NavVelSub	= nh.subscribe("ardrone/navdata", 10, &RectifyVel::getNavVel, this);
		
		PubVel 		= nh.advertise<geometry_msgs::Twist>("cmd_vel", 10, this);

		ns	= ros::this_node::getNamespace();

		nh.getParam("/Kx",Kx);
		nh.getParam("/Ky",Ky);
	}

	void getCmdVel(const geometry_msgs::Twist& msg){		
		cmdVel=msg;
	}

	void getNavVel(const ardrone_autonomy::Navdata& msg){		
		navdata=msg;
		realVx = navdata.vx/1000;
		realVy = navdata.vy/1000;
	}

	void UpdateVel(){	

		Velocity.linear.x=cmdVel.linear.x+Kx*(cmdVel.linear.x-realVx);
		Velocity.linear.y=Ky*(cmdVel.linear.y-realVy);
		Velocity.linear.z=cmdVel.linear.z;

		Velocity.angular.x=0;
		Velocity.angular.y=0;
		Velocity.angular.z=cmdVel.angular.z;

		PubVel.publish(Velocity);
	}
};

int main(int argc, char** argv){

	ros::init(argc, argv, "VelLoop");	
	RectifyVel update;

	ros::Rate r(30);
	while(ros::ok()){

		update.UpdateVel();
		ros::spinOnce();
		r.sleep();		
	}
}
