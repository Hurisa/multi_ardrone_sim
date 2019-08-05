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

using namespace std;

#define PI 3.14159265

class Ground{
private:
	
	ros::Publisher  flockCmd, avoidState;


	ros::Subscriber uavPoses;
	geometry_msgs::Pose pose;
	tf2::Quaternion Q;	
	tf2::Matrix3x3 Rotation;
	double roll, pitch, yaw;	
	vector<double> distances;
	string ns;

	float radius, separation_radius;

public:	
	

	Ground(ros::NodeHandle& nh, string nspace, double SwarmSize){
		flockCmd 	= nh.advertise<geometry_msgs::Twist>(nspace+"/flocking",10, this);
		avoidState 	= nh.advertise<std_msgs::Bool>(nspace+"/Avoiding",10, this);
	

		uavPoses = nh.subscribe(nspace+"/ParrotPose",10, &Ground::updatePose, this);
		distances.resize(SwarmSize);	
		ns=nspace;
		
		nh.getParam("/radius", radius);
		nh.getParam("/separation_radius", separation_radius);

		
	}
	void updatePose(const geometry_msgs::Pose& msg){
		pose=msg;			
 		tf2::fromMsg(pose.orientation, Q);
 		Rotation.setRotation(Q);	
 		Rotation.getEulerYPR(yaw, pitch, roll);
	}
	double getYaw(){
		return yaw;
	}
	double getx(){
		return pose.position.x;
	}
	double gety(){
		return pose.position.y;
	}
	string getns(){
		return ns;
	}
	float getRadius(){
		return radius;
	}
	float getSepartionRadius(){
		return separation_radius;
	}
	void setDistance(int i, double d){
		distances[i]=d;
	}
	tf2::Matrix3x3 getRotMatrix(){
		return Rotation;
	}
	double getDistance(int i){
		return distances[i];
	}
	void PublishMsg(geometry_msgs::Twist p){
		flockCmd.publish(p);
	}

	geometry_msgs::Pose getPose(){
		return pose;
	}

	void pubAvoid(std_msgs::Bool msg){
		avoidState.publish(msg);
	}

};
void setNeighbourhood(vector<Ground*>& ground, vector<double>& CosThetaNeighbours, vector<double>&  SinThetaNeighbours, vector<double>& xPosSeparation, vector<double>&  yPosSeparation, vector<double>& xPosCohesion, vector<double>&  yPosCohesion, int j){

CosThetaNeighbours.resize(0); SinThetaNeighbours.resize(0);
	xPosSeparation.resize(0); yPosSeparation.resize(0);
	  xPosCohesion.resize(0); yPosCohesion.resize(0);

	for (int i = 0; i < ground.size(); i++){		
		if ( i!=j){
			ground[j]->setDistance(i, sqrt(pow(ground[i]->getx()-ground[j]->getx(),2) + pow(ground[i]->gety()-ground[j]->gety(),2)));
			if (ground[j]->getDistance(i)<ground[j]->getRadius()){
				//cout<<"I am drone "<<j<<" have a neighbour"<<endl;
				CosThetaNeighbours.push_back(cos(ground[i]->getYaw()));
				SinThetaNeighbours.push_back(sin(ground[i]->getYaw()));
				float sr=ground[j]->getSepartionRadius();
				if (ground[j]->getDistance(i)<ground[j]->getSepartionRadius())
				{
					xPosSeparation.push_back(ground[i]->getx()); yPosSeparation.push_back(ground[i]->gety());
				}
				else{
					xPosCohesion.push_back(ground[i]->getx()); yPosCohesion.push_back(ground[i]->gety());
				}
			}
		}
	}
}

geometry_msgs::Twist  setAlignment(vector<Ground*>& ground, int j, vector<double>& CosThetaNeighbours, vector<double>& SinThetaNeighbours ){
	geometry_msgs::Twist p;
	if (CosThetaNeighbours.size()>0){

		CosThetaNeighbours.push_back(cos(ground[j]->getYaw()));
		SinThetaNeighbours.push_back(sin(ground[j]->getYaw()));

		float CosAvg=(accumulate(CosThetaNeighbours.begin(), CosThetaNeighbours.end(), 0.00) / CosThetaNeighbours.size());
		float SinAvg=(accumulate(SinThetaNeighbours.begin(), SinThetaNeighbours.end(), 0.00) / SinThetaNeighbours.size());

		float YawAvg=atan2(SinAvg, CosAvg);
		
		if (YawAvg<0){YawAvg=YawAvg+2*PI;}

		double temYaw, w;
		if (ground[j]->getYaw()<0){
			temYaw = ground[j]->getYaw()+2*PI;
		}
		else{
			temYaw = ground[j]->getYaw();
		}

		w=1*(YawAvg-temYaw);
		if(abs(YawAvg-temYaw)>PI){w=-w;}

		p.angular.x=0; p.angular.y=0; p.angular.z=2*w;
		p.linear.x=0; p.linear.y=0; p.linear.z=0;
	}
	else{
		p.angular.x=0; p.angular.y=0; p.angular.z=0;
		p.linear.x=0; p.linear.y=0; p.linear.z=0;
	}

	return p;

}

geometry_msgs::Twist setSeparation(vector<Ground*>& ground, int j, vector<double>& xPosSeparation, vector<double>&  yPosSeparation){
		geometry_msgs::Twist p;
       	if (xPosSeparation.size()>0){
       		tf2::Matrix3x3 M(ground[j]->getRotMatrix());
       		tf2::Matrix3x3 Mt;

       		float XSep=(accumulate(xPosSeparation.begin(), xPosSeparation.end(), 0.00) / xPosSeparation.size());
			float YSep=(accumulate(yPosSeparation.begin(), yPosSeparation.end(), 0.00) / yPosSeparation.size());
			tf2::Vector3 v;
			float xC=XSep-ground[j]->getx();
			float yC=YSep-ground[j]->gety();
			v.setX(xC); v.setY(yC); v.setZ(0);
			float vx = (M.tdotx(v));
			float vy = (M.tdoty(v));

			float dtheta=atan2(-vy,-vx);
			if (abs(dtheta)>0.05 && dtheta>0){
				p.angular.x=0; p.angular.y=0; p.angular.z=0.75*abs(dtheta);				
			}
			else if(abs(dtheta)>0.05 && dtheta<0){
				p.angular.x=0; p.angular.y=0; p.angular.z=-0.75*abs(dtheta);		
			}
			else{
				p.angular.x=0; p.angular.y=0; p.angular.z=0;				
			}
			p.linear.x=-0.2*vx; p.linear.y=-0.2*vy; p.linear.z=0;
       	}
       	else{
       		p.angular.x=0; 		p.angular.y=0; 	p.angular.z=0;
			p.linear.x=0; 	p.linear.y=0; 	p.linear.z=0;
			
       	}
       	return p;

}

geometry_msgs::Twist setCohesion(vector<Ground*>& ground, int j, vector<double>& xPosCohesion, vector<double>&  yPosCohesion){
	geometry_msgs::Twist p;
	    if (xPosCohesion.size()>0){
       		tf2::Matrix3x3 M(ground[j]->getRotMatrix());
       		tf2::Matrix3x3 Mt;

       		float XCoh=(accumulate(xPosCohesion.begin(), xPosCohesion.end(), 0.00) / xPosCohesion.size());
			float YCoh=(accumulate(yPosCohesion.begin(), yPosCohesion.end(), 0.00) / yPosCohesion.size());
			tf2::Vector3 v;
			float xC=XCoh-ground[j]->getx();
			float yC=YCoh-ground[j]->gety();
			v.setX(xC); v.setY(yC); v.setZ(0);
			float vx = (M.tdotx(v));
			float vy = (M.tdoty(v));

			float dtheta=atan2(vy,vx);
			if (abs(dtheta)>0.05 && dtheta>0){
				p.angular.x=0; p.angular.y=0; p.angular.z=0.75*abs(dtheta);				
			}
			else if(abs(dtheta)>0.05 && dtheta<0){
				p.angular.x=0; p.angular.y=0; p.angular.z=-0.75*abs(dtheta);		
			}
			else{
				p.angular.x=0; p.angular.y=0; p.angular.z=0;				
			}
			p.linear.x=0; p.linear.y=0; p.linear.z=0;
       	}
       	else{
       		p.angular.x=0; 		p.angular.y=0; 	p.angular.z=0;
			p.linear.x=0; 	p.linear.y=0; 	p.linear.z=0;
			
       	}
       	return p;

}

void flocking(vector<Ground*>& ground, int j, int alpha, int gamma, int beta){

	vector<double> CosThetaNeighbours, SinThetaNeighbours;
	vector<double> xPosSeparation, yPosSeparation;
	vector<double> xPosCohesion, yPosCohesion;
	string paramStr = ground[j]->getns()+"/Avoiding";

	setNeighbourhood(ground,CosThetaNeighbours, SinThetaNeighbours, xPosSeparation, yPosSeparation, xPosCohesion, yPosCohesion,j);
	
	geometry_msgs::Twist AlVel=setAlignment(ground, j, CosThetaNeighbours, SinThetaNeighbours);
	geometry_msgs::Twist SeVel=setSeparation(ground, j, xPosSeparation, yPosSeparation);
	geometry_msgs::Twist CoVel=setCohesion(ground, j, xPosCohesion, yPosCohesion);

	geometry_msgs::Twist p;
	std_msgs::Bool avoiding;
	if (SeVel.angular.z==0){
		//ros::param::set(paramStr, false);
		avoiding.data = false;
		p.angular.z=alpha*AlVel.angular.z+beta*CoVel.angular.z;
	}
	else{
		//cout<<paramStr<<endl;
		//ros::param::set(paramStr, true);
		avoiding.data = true;
		p.angular.z=(2*alpha*AlVel.angular.z+gamma*SeVel.angular.z+beta*CoVel.angular.z)/(2*alpha+gamma+beta);
	}

	p.angular.x=0; 				p.angular.y=0; 				
	p.linear.x=SeVel.linear.x; 	p.linear.y=SeVel.linear.y; 	p.linear.z=0;
	

	ground[j]->pubAvoid(avoiding);
	ground[j]->PublishMsg(p);
}




int main(int argc, char** argv){

	ros::init(argc, argv, "flocking");	
	ros::NodeHandle nh;	

	vector<string> namespaces;
	int alpha,gamma,beta; //Flocking parameter
	nh.getParam("/namespaces", namespaces);
	nh.getParam("/Alpha", alpha);
	nh.getParam("/Gamma", gamma);
	nh.getParam("/Beta" , beta);
	
	vector<Ground*> ground;

	ros::Publisher 	PoseVectorPub = nh.advertise<multi_ardrone_sim::posevector>("/uavposes", 10);
	multi_ardrone_sim::posevector PoseVector;
	PoseVector.poses.resize(namespaces.size());
	

	for (int i = 0; i < namespaces.size(); i++){
		ground.push_back(new Ground(nh, namespaces[i], namespaces.size()));		
	}

	ros::Rate r(30);
	while(ros::ok()){

		for (int i = 0; i < namespaces.size(); i++)
		{
			flocking(ground, i, alpha, gamma, beta);
			PoseVector.poses[i]=ground[i]->getPose();
		}
		PoseVectorPub.publish(PoseVector);
		ros::spinOnce();
		r.sleep();		
	}
}
