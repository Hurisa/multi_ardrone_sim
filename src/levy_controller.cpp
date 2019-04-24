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

#include <iostream>
#include <vector>
#include <string>
#include <math.h>       /* cos */
     
#include <multi_ardrone_sim/posevector.h>

using namespace std;

#define PI 3.14159265

class LevyBoid{

private:

	float theta, jump, CurrentDistance;
	float miu;
	geometry_msgs::Pose Pose;
	geometry_msgs::Twist v;
	tf2::Quaternion Q;	
	tf2::Matrix3x3 Rotation;
	double roll, pitch, yaw;
	float w;
	ros::NodeHandle nh;
	ros::Subscriber PoseSub;
	ros::Publisher PubVel;

	bool changeTheta;

	string ns;

public:

	LevyBoid(){

		PoseSub = nh.subscribe("ParrotPose", 10, &LevyBoid::getCurrentDistance, this);
		PubVel	= nh.advertise<geometry_msgs::Twist>("levy_topic",10, this);
		CurrentDistance=0;
		jump=0;
		changeTheta=false;
		ns	= ros::this_node::getNamespace();
		nh.getParam("/miu", miu);
	}

	void getCurrentDistance(const geometry_msgs::Pose& msg){
		geometry_msgs::Pose LastPose=Pose;

		if (!isnan(msg.position.x)){Pose=msg;}
	  
		CurrentDistance+=sqrt(pow(Pose.position.x-LastPose.position.x,2)+pow(Pose.position.y-LastPose.position.y,2));
		//cout<< " --------- Quadrotor " << nh.getNamespace()<<" ---------------" <<endl;
		checkJump();
		
		//cout<< "Current Distance" << CurrentDistance<<endl;
		//cout<<"Jump" << jump<<endl;
 		ComputeVel();
	}	

	void checkJump(){

		if (CurrentDistance>=jump)
		{
			CurrentDistance=0;
			struct timeval time;
		    gettimeofday(&time,NULL);
		    srand((time.tv_sec * 1000) + (time.tv_usec / 1000));

		    double mu=miu;
		    double sigma=10.0;
		    double muOne=mu-1;
		    double muTwo=2-mu;
		    double x, y;
		    double U1, U2, U3, phi, r;

		    U1=double(rand())/double(RAND_MAX)*2-1;
        	U2=double(rand())/double(RAND_MAX)*2-1;
        	U3=double(rand())/double(RAND_MAX)*2-1;

		    U1 = U1*(PI/2);	U2 = (U2+1)/2; phi = U3 * M_PI;

	        r = (sin(muOne * U1) / pow(cos(U1), 1/muOne) ) * pow((cos(muTwo * U1) / U2), (muTwo / muOne));
	        x = r * cos(phi); y = r * sin(phi);

	        theta=atan2(y,x); jump=sqrt(pow(x,2)+pow(y,2));
	 
	        changeTheta=true;
		}
	}

	void ComputeVel(){

		tf2::fromMsg(Pose.orientation, Q);
 		Rotation.setRotation(Q);	
 		Rotation.getEulerYPR(yaw, pitch, roll);

		if (theta<0){theta+=2*PI;}
		if (yaw<0){yaw+=2*PI;}
		//cout<<"abs(theta-yaw) "<<abs(theta-yaw)<<endl;
		if (abs(theta-yaw)>0.1 && changeTheta){
			w=(theta-yaw);
			if(abs(theta-yaw)>PI){w=-w;}
		}
		else{
			w=0;
			changeTheta=false;
		}
		v.linear.x=0; 	v.linear.y=0; 	v.linear.z=0;
		v.angular.x=0; 	v.angular.y=0; 	v.angular.z=w;	
		//cout<<"namespace"<< ns << " levy angular "<<w<<endl;
		PubVel.publish(v);
	}	
};

int main(int argc, char** argv){

	ros::init(argc, argv, "Levy");	

	LevyBoid boid;

	ros::Rate r(30);
	while(ros::ok()){

		ros::spinOnce();
		r.sleep();		
	}
}
