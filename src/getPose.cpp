#include "ros/ros.h"
#include <ros/console.h> 
#include <stdlib.h>
#include <geometry_msgs/TwistStamped.h>
#include <nav_msgs/Odometry.h>
#include <geometry_msgs/PoseStamped.h>
#include <std_msgs/String.h>

#include <iostream>
#include <vector>
#include <string>

#include <multi_ardrone_sim/posevector.h>

using namespace std;

class ArucoSub{

public:

	geometry_msgs::PoseStamped pose;
	ros::Subscriber arucopose;

	ArucoSub(string topic, ros::NodeHandle& nh){		
		arucopose = nh.subscribe(topic, 10, &ArucoSub::getPose, this);
		
	}	
	void getPose(const geometry_msgs::PoseStamped& msg){		
			pose.header = msg.header;
			pose.pose  = msg.pose;		
	}
};

vector<ArucoSub*> initSubs(ros::NodeHandle& n){
	vector<ArucoSub*> SubVector;
	vector<int> markersID;

	std::string ns = n.getNamespace();
	n.getParam("/markersID", markersID);
	for (int i = 0; i < markersID.size(); ++i)	{
		SubVector.push_back(new ArucoSub(ns+"/aruco"+to_string(markersID[i])+"/pose", n));		
	}
	return SubVector;
}

void PublishPoseVector(ros::Publisher PosesPub, vector<ArucoSub*> SubVector){

		multi_ardrone_sim::posevector msg;		
		for (int i = 0; i < SubVector.size(); i++){
			msg.poses.push_back(SubVector[i]->pose);
		}
		PosesPub.publish(msg);
}

int main(int argc, char** argv){

	ros::init(argc, argv, "getPosenode");	
	ros::NodeHandle n;
	
  	vector<ArucoSub*> SubVector = initSubs(n);
	ros::Publisher PosesPub=n.advertise<multi_ardrone_sim::posevector>("TagPoses",10);

	ros::Rate r(20);
	while(ros::ok()){

		PublishPoseVector(PosesPub, SubVector);
		ros::spinOnce();
		r.sleep();		
	}
}
