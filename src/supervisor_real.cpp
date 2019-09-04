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

	int secs, init, RunTime, InitialTimeSec;
	int N, RunLength;
	std_srvs::Empty resetWorldSrv;
	ros::MessageEvent<multi_ardrone_sim::posevector const> SwarmPoses;
	multi_ardrone_sim::posevector SwarmState;
	multi_ardrone_sim::avoid AvoidVector;


public:	
	int currentT; //rename to iter
	rosbag::Bag bag;
	Supervisor(ros::NodeHandle nh){
	//ClockSub		=nh.subscribe("/clock",10, &Supervisor::setCurrentTime, this);
	SwarmSub 		=nh.subscribe("/uavposes",10, &Supervisor::setSwarmPoses, this);

	SwarmStatePub	=nh.advertise<multi_ardrone_sim::posevector>("/swarmState", 10);
	nh.getParam("/N", N);
	nh.getParam("/RunLength", RunLength);
	setInitTime();
	secs=0;
	currentT=0;
	
	InitialTimeSec=ros::Time::now().toSec();
	}

	void setCurrentTime(){
		secs =ros::Time::now().toSec() - InitialTimeSec;
	}

	void setInitTime(){
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
	
	}

	ros::MessageEvent<multi_ardrone_sim::posevector const> getSwarmPoses(){
		return SwarmPoses;
	}

	multi_ardrone_sim::posevector const getSwarmState(){
		return SwarmState;
	}



};

int main(int argc, char** argv){

	ros::init(argc, argv, "Supervisor");	
	ros::NodeHandle nh;

	Supervisor supervisor(nh);
	ros::Rate r(30);

	int alpha, run_no;
	float  miu;
	bool levy;
	nh.getParam("/miu", miu);
	nh.getParam("/Alpha", alpha);
	nh.getParam("/UseLevy", levy);
	nh.getParam("/run_no", run_no);

	//std_srvs::Empty resetWorldSrv;
	//ros::service::call("/gazebo/reset_simulation", resetWorldSrv);
	
	string filename = "real_"+to_string(levy)+"_Alpha_"+to_string(alpha)+"_"+to_string(supervisor.getRunLength())+"_"+to_string(miu)+"_"+to_string(run_no)+".bag";
	supervisor.bag.open(filename, rosbag::bagmode::Write);
	while(ros::ok()){	
		supervisor.setCurrentTime();	
		
		
		if (supervisor.getSecs()>supervisor.getRunLength()){
			supervisor.setInitTime();
			supervisor.currentT=supervisor.currentT+1;
			

		}
		if (supervisor.currentT>=supervisor.getN()){
			system("rosrun multi_ardrone_sim landall");
			supervisor.bag.close();
			system("rosnode kill -a");
		}
		else{
			supervisor.PublishSwarmState();
			supervisor.bag.write("/SwarmState",ros::Time::now(),supervisor.getSwarmState());
			
		}
		cout<<"seconds "<< supervisor.getSecs()<<" of "<<supervisor.getRunLength() <<" of run "<<supervisor.currentT<<" of total runs "<< supervisor.getN()<<endl;
		ros::spinOnce();
		r.sleep();		
	}
}