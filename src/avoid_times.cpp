#include "ros/ros.h"
#include <ros/console.h> 
#include <stdlib.h>
#include <geometry_msgs/TwistStamped.h>
#include <nav_msgs/Odometry.h>
#include <geometry_msgs/PoseStamped.h>
#include <std_msgs/String.h>
#include <std_msgs/Bool.h>
#include <tf2/LinearMath/Quaternion.h>
#include <tf2/LinearMath/Matrix3x3.h>
#include <tf2_geometry_msgs/tf2_geometry_msgs.h>
#include <nav_msgs/Odometry.h>

#include <iostream>
#include <vector>
#include <string>
#include <math.h>       /* cos */

#include <multi_ardrone_sim/posevector.h>
#include <multi_ardrone_sim/avoid.h>

using namespace std;

#define PI 3.14159265


class Timer{

private:

	std_msgs::Bool avoidState;
	string ns;
	ros::Subscriber stateSub;

public:

	Timer(string nspace, ros::NodeHandle nh){
		ns=nspace;
		string Topic=ns+"/Avoiding";
		stateSub = nh.subscribe(Topic, 1, &Timer::updateState, this);
	}

	void updateState(std_msgs::Bool msg){
		avoidState=msg;
	}

	std_msgs::Bool getState(){
		return avoidState;
	}
};


int main(int argc, char** argv)
{
	
	ros::init(argc, argv, "AvoidTimes");	
	ros::NodeHandle nh;	

	ros::Publisher AvoidStatePub = nh.advertise<multi_ardrone_sim::avoid>("/SwarmAvoidState", 10);

	vector<string> namespaces;
	nh.getParam("/namespaces", namespaces);
	
	vector<Timer*> timers;

	for(int i=0 ; i < namespaces.size() ; i++){
		timers.push_back(new Timer(namespaces[i],nh));
	}

	multi_ardrone_sim::avoid AvoidVector;
	AvoidVector.state.resize(namespaces.size());

	ros::Rate r(30);
	while(ros::ok()){
		for (int i = 0; i < namespaces.size(); i++){
			AvoidVector.state[i]=timers[i]->getState();
		}

		AvoidStatePub.publish(AvoidVector);
		ros::spinOnce();
		r.sleep();		
	}
	return 0;
}