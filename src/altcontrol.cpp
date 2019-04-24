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
#include <sensor_msgs/Range.h>
#include <ardrone_autonomy/Navdata.h>

#include <iostream>
#include <vector>
#include <string>
#include <math.h>       /* cos */

#include <multi_ardrone_sim/posevector.h>

using namespace std;

#define PI 3.14159265

class AltitudeControl{

private:

	float vz;													// vertical speed
	float Kz, refAltitude;										// Gain and altitude reference

	geometry_msgs::Twist Velocity;
	//sensor_msgs::Range range;	
	ardrone_autonomy::Navdata navdata;
	int altd;

	ros::NodeHandle nh;
	ros::Subscriber PoseSub;
	ros::Publisher	PubVel;

	double altitude;

public:

	AltitudeControl(){
	
		nh.getParam("/refAltitude", refAltitude); 
		nh.getParam("Kz", Kz); 

		PoseSub = nh.subscribe("ardrone/navdata", 10, &AltitudeControl::getAltitude, this);
		PubVel =  nh.advertise<geometry_msgs::Twist>("ZVel", 10, this);
	}

	void getAltitude(const ardrone_autonomy::Navdata& msg){
		cout<<"Hello"<<endl;
		navdata=msg;
		// range=msg;
		altd = navdata.altd;
		vz = Kz*(refAltitude-altd);
	
		Velocity.linear.x=0; 	Velocity.linear.y=0; 	Velocity.linear.z=vz;
		Velocity.angular.x=0; 	Velocity.angular.y=0; 	Velocity.angular.z=0;

		PubVel.publish(Velocity);
	}
};

int main(int argc, char** argv){

	ros::init(argc, argv, "AltCtrl");	
	AltitudeControl alt;
	cout<<"Entering Altitude Control loop"<<endl;
	ros::Rate r(10);
	while(ros::ok()){

		//cout<<"Altitude Control loop"<<endl;
		ros::spinOnce();
		r.sleep();		
	}
}
