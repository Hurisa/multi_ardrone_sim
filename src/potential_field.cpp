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

class PotentialField{

private:

	float w;			// Angular speed
	float xLowerLimit, xUpperLimit, yLowerLimit, yUpperLimit;
	vector<double> FieldValue;
	vector<double> YawComponents;
	geometry_msgs::Pose Pose;
	geometry_msgs::Twist Velocity;
		
	ros::NodeHandle nh;
	ros::Subscriber PoseSub;
	ros::Publisher	PubVel;

	double roll, pitch, yaw;
	tf2::Quaternion Q;
	tf2::Matrix3x3 M;

public:

	PotentialField(){
	
		nh.getParam("/xLowerLimit", xLowerLimit); nh.getParam("/xUpperLimit", xUpperLimit);
		nh.getParam("/yLowerLimit", yLowerLimit); nh.getParam("/yUpperLimit", yUpperLimit);
		
		FieldValue.resize(2);
		YawComponents.resize(2);		
		PoseSub = nh.subscribe("ParrotPose", 10, &PotentialField::getW, this);
		PubVel =  nh.advertise<geometry_msgs::Twist>("fieldVel", 10, this);
	}

	void getW(const geometry_msgs::Pose& msg){
		
		Pose=msg;
		
		if (msg.position.x<xLowerLimit){FieldValue[0] = 1;}
		else if(msg.position.x>xUpperLimit){FieldValue[0] = -1;}
		else{FieldValue[0] = 0;}
		
		if (msg.position.y<yLowerLimit){FieldValue[1] = 1;}
		else if(msg.position.y>yUpperLimit){FieldValue[1] = -1;}
		else{FieldValue[1] = 0;}
		
 		tf2::fromMsg(Pose.orientation, Q);
 		M.setRotation(Q);
 		M.getEulerYPR(yaw, pitch, roll);
 		YawComponents[0]=cos(yaw); YawComponents[1]=sin(yaw); 		

 		float xprod = -(FieldValue[0]*YawComponents[1]-FieldValue[1]*YawComponents[0]);
 		float fieldYaw = atan2(FieldValue[1],FieldValue[0]);
 		float normCoef = -cos(fieldYaw-yaw)-(-1)/(0-(-1));


 		if (xprod<0){w=-normCoef;}
 		else if(xprod>0){w=normCoef;}
 		else{w=0;}
 		// use only the sign. get the cos of the angle normalize between -1 and 0 and invert it

		Velocity.angular.z=w;

		PubVel.publish(Velocity);
	}
};

int main(int argc, char** argv){

	ros::init(argc, argv, "potential");	
	PotentialField field;

	ros::Rate r(30);
	while(ros::ok()){

		
		ros::spinOnce();
		r.sleep();		
	}
}
