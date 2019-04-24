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

class VelocityUpdate{

private:

	geometry_msgs::Pose Pose;
	geometry_msgs::Twist Velocity, FieldVel, VerticalVel, flockingRules, LevyTwist;
	bool levy;
		
	ros::NodeHandle nh;
	ros::Subscriber FieldSub;
	ros::Subscriber AltCtrlSub;
	ros::Subscriber FlockSub;
	ros::Subscriber PoseSub;
	ros::Subscriber LevySub;
	ros::Subscriber AvoidSub;

	ros::Publisher	PubVel;
	ros::Publisher	TakeoffPub;

	string ns;


	double altitude;
	bool takeoff, avoid;

public:

	VelocityUpdate(){
	
		FieldSub 	= nh.subscribe("fieldVel", 10, &VelocityUpdate::getFieldVel, this);
		AltCtrlSub	= nh.subscribe("ZVel", 10, &VelocityUpdate::getZVel, this);
		FlockSub	= nh.subscribe("flocking", 10, &VelocityUpdate::getFlocking, this);
		LevySub		= nh.subscribe("levy_topic", 10, &VelocityUpdate::getLevy, this);
		AvoidSub    = nh.subscribe("Avoiding", 10, &VelocityUpdate::getAvoid, this);
		
		PubVel 		= nh.advertise<geometry_msgs::Twist>("cmd_vel", 10, this);
		TakeoffPub	= nh.advertise<std_msgs::Empty>("ardrone/takeoff", 10, this);
		takeoff		= false;

		ns	= ros::this_node::getNamespace();

		nh.getParam("/UseLevy", levy);
	}

	void getFieldVel(const geometry_msgs::Twist& msg){		
		FieldVel=msg;
	}

	void getZVel(const geometry_msgs::Twist& msg){		
		VerticalVel=msg;
	}

	void getFlocking(const geometry_msgs::Twist& msg){		
		flockingRules=msg;
	}
	void getLevy(const geometry_msgs::Twist& msg){		
		LevyTwist=msg;
	}

	void getAvoid(const std_msgs::Bool& msg){
		 avoid=msg.data;
	}


	void UpdateVel(){	

		if (!takeoff){	
			std_msgs::Empty empty;
			TakeoffPub.publish(empty);
			takeoff=true;
		}

		tf2::Vector3 fieldVel, Angflocking, Linflocking;
		tf2::fromMsg(flockingRules.angular,  Angflocking);
		tf2::fromMsg(flockingRules.linear,   Linflocking);
		tf2::fromMsg(FieldVel.angular, fieldVel);

		if(FieldVel.angular.z!=0){
			Velocity.angular=tf2::toMsg(fieldVel);
			Velocity.linear=tf2::toMsg(Linflocking);
			if (Velocity.linear.x==0){
				Velocity.linear.x=0.2;
			}
		}
		else{
			if (LevyTwist.angular.z!=0 && levy){
				Velocity.angular=LevyTwist.angular;
				Velocity.linear=tf2::toMsg(Linflocking);
				if (Velocity.linear.x==0){
					Velocity.linear.x=0.2;
				}
			}
			else{
			    Velocity.angular=tf2::toMsg(Angflocking);
			    //Velocity.linear=tf2::toMsg(Linflocking);
			    if (!avoid){Velocity.linear.x=0.2;}
				else{Velocity.linear.x=0.1;}

				Velocity.linear.y=0;
			}
		}
		Velocity.linear.z=VerticalVel.linear.z;

		PubVel.publish(Velocity);
	}
};

int main(int argc, char** argv){

	ros::init(argc, argv, "VelCtrl");	
	VelocityUpdate update;

	ros::Rate r(30);
	while(ros::ok()){

		update.UpdateVel();
		ros::spinOnce();
		r.sleep();		
	}
}
