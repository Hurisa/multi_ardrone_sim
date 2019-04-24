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


class Supervisor{
private:	
	
	ros::Subscriber ClockSub, SwarmSub, SwarmAvoidSub;
	ros::Publisher SwarmStatePub, SwarmAvoidPub;
	ros::NodeHandle nh;
	int secs, init, RunTime;
	int N, RunLength;
	std_srvs::Empty resetWorldSrv;
	ros::MessageEvent<multi_ardrone_sim::posevector const> SwarmPoses;
	multi_ardrone_sim::posevector SwarmState;
	multi_ardrone_sim::avoid AvoidVector;

public:	
	int currentT; //rename to iter

	Supervisor(){
	ClockSub		=nh.subscribe("/clock",10, &Supervisor::setCurrentTime, this);
	SwarmSub 		=nh.subscribe("/uavposes",10, &Supervisor::setSwarmPoses, this);
	SwarmAvoidSub	=nh.subscribe("/SwarmAvoidState",10, &Supervisor::setSwarmAvoid, this);

	SwarmAvoidPub	=nh.advertise<multi_ardrone_sim::avoid>("/swarmAvoid", 10);
	SwarmStatePub	=nh.advertise<multi_ardrone_sim::posevector>("/swarmState", 10);
	nh.getParam("/N", N);
	nh.getParam("/RunLength", RunLength);
	setInitTime();
	secs=0;
	currentT=0;
	}

	void setCurrentTime(const rosgraph_msgs::Clock& msg){
		secs =ros::Time::now().toSec() - RunLength*currentT;
		//cout<<secs<<endl;
	}

	void setInitTime(){
	//ros::service::call("/gazebo/reset_simulation", resetWorldSrv);
	ros::service::call("/gazebo/reset_world", resetWorldSrv);
	init = 0; //ros::Time::now().toSec();	
	}

	int getSecs(){
		return secs;
	}
	
	int getN(){
		return N;
	}
	
	int getRunLength(){
		return RunLength;
	}

	void pauseSim(){
		ros::service::call("/gazebo/pause_physics", resetWorldSrv);	
	}

	void setSwarmPoses(const multi_ardrone_sim::posevector& msg){
		init++;
		SwarmState=msg;
		SwarmState.run=init;
	}

	void setSwarmAvoid(const multi_ardrone_sim::avoid& msg){
		AvoidVector=msg;
	}

	void PublishSwarmState(){
		SwarmStatePub.publish(SwarmState);
		SwarmAvoidPub.publish(AvoidVector);
	}

	ros::MessageEvent<multi_ardrone_sim::posevector const> getSwarmPoses(){
		return SwarmPoses;
	}

};

int main(int argc, char** argv){

	ros::init(argc, argv, "Supervisor");	
	Supervisor supervisor;

	ros::Rate r(30);

	std_srvs::Empty resetWorldSrv;
	ros::service::call("/gazebo/reset_simulation", resetWorldSrv);
	
	while(ros::ok()){		
		if (supervisor.getSecs()>supervisor.getRunLength()){
			supervisor.setInitTime();
			supervisor.currentT=supervisor.currentT+1;
			ros::param::set("/resetAvoidTimes", true);

		}
		if (supervisor.currentT>=supervisor.getN()){
			supervisor.pauseSim();
			system("rosnode kill -a");
		}
		else{
			supervisor.PublishSwarmState();
			ros::param::set("/resetAvoidTimes", false);
		}
		cout<<"seconds "<< supervisor.getSecs()<<" of "<<supervisor.getRunLength() <<" of run "<<supervisor.currentT<<" of total runs "<< supervisor.getN()<<endl;
		ros::spinOnce();
		r.sleep();		
	}
}