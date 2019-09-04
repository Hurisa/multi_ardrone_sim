#include "ros/ros.h"
#include <ros/console.h> 
#include <stdlib.h>
#include <geometry_msgs/TwistStamped.h>
#include <nav_msgs/Odometry.h>
#include <geometry_msgs/PoseStamped.h>
#include <std_msgs/String.h>
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

class landOne{

private:


	//ros::NodeHandle nh;
	ros::Publisher	landPub;

	string ns;
	bool landVar;

public:

	landOne(ros::NodeHandle& nh, string nspace){
	
		landPub	= nh.advertise<std_msgs::Empty>(nspace+"/ardrone/land", 10, this);
		landVar	= false;
		ns	= ros::this_node::getNamespace();

	}

	void init(){	

		if (!landVar){	
			std_msgs::Empty empty;
			landPub.publish(empty);
			landVar=true;
		}
	}
};

int main(int argc, char** argv){

	ros::init(argc, argv, "landall");	
	ros::NodeHandle nh;	

	vector<string> namespaces;
	nh.getParam("/namespaces", namespaces);


	vector<landOne*> land;


	for (int i = 0; i < namespaces.size(); i++){
		land.push_back(new landOne(nh, namespaces[i]));			
	}
	


	ros::Rate r(30);
	while(ros::ok()){

		for (int i = 0; i < namespaces.size(); i++){
			land[i]->init();
		}


		ros::spinOnce();
		r.sleep();		
	}
}
