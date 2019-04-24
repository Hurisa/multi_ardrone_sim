/* COULD INCLUDE AN ESTIMATION BASED ON VELOCITY AND PREVIOUS POSITIONING  */

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
#include <std_msgs/UInt32MultiArray.h>
#include <aruco_msgs/MarkerArray.h>

using namespace std;

#define PI 3.14159265

class PoseVector{
private:
	vector<int>   markersID;
	vector<float> markersXc;
	vector<float> markersYc;
	vector<float> markersYaw;

	multi_ardrone_sim::posevector poses;
	aruco_msgs::MarkerArray MarkerArray;
	std_msgs::UInt32MultiArray DetectedMarkers;


	ros::Subscriber PoseVectorSub;
	ros::Subscriber GroundTruthSub;
	//ros::Subscriber MarkerIdSub;
	ros::Subscriber MarkerPoseSub;
	
	ros::Publisher 	PosePub;

	ros::NodeHandle nh;

	vector<int>	  msgSeq;	
	vector<geometry_msgs::Pose> MarkerAbsolutePose;
	geometry_msgs::Pose CameraPose;

	float CameraYaw;

	tf2::Quaternion QAxisRot;
	tf2::Matrix3x3 MRot; 
	float AxisYawRot;

	bool useGroundTruth;

public:	
	PoseVector(){		
		
		nh.getParam("/markersID", markersID);
		nh.getParam("/markersXc", markersXc);
		nh.getParam("/markersYc", markersYc);
		nh.getParam("/markersYaw",markersYaw);		
		nh.getParam("/AxisYawRot", AxisYawRot);
		nh.getParam("/useGroundTruth", useGroundTruth);
		
		vector<int> v(markersID.size(), 0);
		msgSeq=v;

		QAxisRot.setRPY(0, 0, AxisYawRot); 
		MRot.setRotation(QAxisRot);

		for (int i = 0; i < markersID.size(); ++i){

			geometry_msgs::Pose MarkerPose;
			MarkerPose.position.x=markersXc[i];
			MarkerPose.position.y=markersYc[i];
			MarkerPose.position.z=0;

 			tf2::Quaternion q; 
   			q.setRPY( 0, 0, markersYaw[i]);  

   			MarkerPose.orientation=tf2::toMsg(q);
   			MarkerAbsolutePose.push_back(MarkerPose);		
		}	

		if (useGroundTruth){GroundTruthSub 	= nh.subscribe("ground_truth/state", 10, &PoseVector::ground_truth, this);}
		else{MarkerPoseSub 		= nh.subscribe("aruco_marker_publisher/markers", 10, &PoseVector::getCameraPose,this);}			

		PosePub				= nh.advertise<geometry_msgs::Pose>("ParrotPose", 10 ,this);
	}

	//// Functions for single tag publisher 
	void getCameraPose(const aruco_msgs::MarkerArray& msg){

		MarkerArray = msg;
		vector<float> RotComponentX, RotComponentY, posX, posY, posZ;	

	
		for (int i = 0; i < MarkerArray.markers.size(); i++){
			int pos = find(markersID.begin(), markersID.end(), MarkerArray.markers[i].id) - markersID.begin();

			RotOrigin2Camera(RotComponentX, RotComponentY, i, pos);	
			PosOrigin2Camera(posX, posY, posZ, i, pos);
		}


		// Computes Camera Orientation
		float sumx = accumulate(RotComponentX.begin(), RotComponentX.end(), 0.00);
		float sumy = accumulate(RotComponentY.begin(), RotComponentY.end(), 0.00);	
		CameraYaw = atan2(sumy,sumx);											// Camera yaw  wrt the world frame
		tf2::Quaternion qCamera;
   		qCamera.setRPY(0, 0, CameraYaw); 										
   		CameraPose.orientation = tf2::toMsg(qCamera);							// Conversion from Quaternion to msg
   		//cout<<"estimated yaw "<< CameraYaw <<endl;
   		RotComponentX.clear(); RotComponentY.clear();	   			

   		// Computes Camera Position
   		CameraPose.position.x=(accumulate(posX.begin(), posX.end(), 0.00) / posX.size());
		CameraPose.position.y=(accumulate(posY.begin(), posY.end(), 0.00) / posY.size());
		CameraPose.position.z=(accumulate(posZ.begin(), posZ.end(), 0.00) / posZ.size());
   		//cout<<" Estimated Position "<<CameraPose.position.x<<" "<<CameraPose.position.y<<" "<<endl;
   		posX.clear(); posY.clear(); posZ.clear(); 

   		// Publishes Pose msg
   		PosePub.publish(CameraPose); 

	}

	void RotOrigin2Camera(vector<float>& xx, vector<float>& yy, int i, int j){

		double roll, pitch, yaw;

		tf2::Quaternion QTag2Camera;			
 		tf2::fromMsg(MarkerArray.markers[i].pose.pose.orientation, QTag2Camera);
 		tf2::Matrix3x3 MTag2Camera(QTag2Camera);						// Rotation Matrix from Tag to the Camera
 					
 		tf2::Quaternion QOrgin2Tag;
 		tf2::fromMsg(MarkerAbsolutePose[j].orientation, QOrgin2Tag);
 		tf2::Matrix3x3 MOrgin2Tag(QOrgin2Tag);							// Rotation Matrix from the Origin to Tag
 			 				
 		tf2::Matrix3x3 MOrgin2Camera(MOrgin2Tag);
 		MOrgin2Camera = MOrgin2Tag * MTag2Camera * MRot;				// Rotation Matrix from Origin to Camera
 		MOrgin2Camera.getEulerYPR(yaw, pitch, roll);
 		xx.push_back(cos(yaw));
 		yy.push_back(sin(yaw));		

	}

	void PosOrigin2Camera(vector<float>& posX, vector<float>& posY, vector<float>& posZ, int i, int j){

		float markerX, markerY, markerZ;
		tf2::Vector3 pos; tf2::Vector3 tempos;							// tempos 	-> Tag position read from topic ; pos -> Tag position in the Parrot's frame
		tf2::Vector3 p; tf2::Vector3 p_dash; tf2::Vector3 T;			// p 		-> position of the Tag in the World Frame; 
																		// p_dash 	-> position of the Tag in a Rotated World frame (no translation)
																		// T 		-> Position of the origin of the Parrot's frame wrt a Rotated World frame
		tf2::fromMsg(MarkerArray.markers[i].pose.pose.position, tempos);
				
		pos.setZ(-tempos.getZ()); pos.setY(-tempos.getX());	
		pos.setX(-tempos.getY()+0.15);
		markerX=MarkerAbsolutePose[j].position.x;
		markerY=MarkerAbsolutePose[j].position.y;
		markerZ=MarkerAbsolutePose[j].position.z;
		
		p.setX(markerX); p.setY(markerY); p.setZ(markerZ);

		tf2::Quaternion qCamera;				
   		qCamera.setRPY(0, 0, CameraYaw); 
   		tf2::Matrix3x3 MOrigin2Camera(qCamera);							// Gets the rotation matrix of the Parrot frame wrt to the World Frame
   		tf2::Matrix3x3 MCamera2Origin;
		MCamera2Origin=MOrigin2Camera.transpose();						// Gets the inverse rotation matrix of the Parrot frame wrt to the World Frame
																		// (Note: For rotation matrixes transpose = inverse)
		p_dash.setX(MOrigin2Camera.tdotx(p));							
		p_dash.setY(MOrigin2Camera.tdoty(p));
		p_dash.setZ(MOrigin2Camera.tdotz(p));

		T=p_dash-pos;

		posX.push_back(MCamera2Origin.tdotx(T));						// Rotates vector T to the Original World Frame	
		posY.push_back(MCamera2Origin.tdoty(T));
		posZ.push_back(-pos.getZ());
	}


	/*

	void getCameraPose(const multi_ardrone_sim::posevector& msg){		
		
		poses=msg;		
		
		vector<float> RotComponentX, RotComponentY, posX, posY, posZ;			// Estimated Parrot's pose components given by each detected Tag

		for (int i = 0; i < markersID.size(); i++){	
			if (poses.poses[i].header.seq>msgSeq[i]){							// For each new Tag msg

				msgSeq[i]=poses.poses[i].header.seq;							// Update Sequence numbers for new msgs
				// 	Tag Orienations
				RotOrigin2Camera(RotComponentX, RotComponentY, i);				// Push back the orientation components according to the ith Tag
				//	Tag Postions				
				PosOrigin2Camera(posX, posY, posZ, i);
				}		
		}

		// Computes Camera Orientation
		float sumx = accumulate(RotComponentX.begin(), RotComponentX.end(), 0.00);
		float sumy = accumulate(RotComponentY.begin(), RotComponentY.end(), 0.00);	
		CameraYaw = atan2(sumy,sumx);											// Camera yaw  wrt the world frame
		tf2::Quaternion qCamera;
   		qCamera.setRPY(0, 0, CameraYaw); 										
   		CameraPose.orientation = tf2::toMsg(qCamera);							// Conversion from Quaternion to msg
   		//cout<<"estimated yaw "<< CameraYaw <<endl;
   		RotComponentX.clear(); RotComponentY.clear();	   			

   		// Computes Camera Position
   		CameraPose.position.x=(accumulate(posX.begin(), posX.end(), 0.00) / posX.size());
		CameraPose.position.y=(accumulate(posY.begin(), posY.end(), 0.00) / posY.size());
		CameraPose.position.z=(accumulate(posZ.begin(), posZ.end(), 0.00) / posZ.size());
   		//cout<<" Estimated Position "<<CameraPose.position.x<<" "<<CameraPose.position.y<<" "<<endl;
   		posX.clear(); posY.clear(); posZ.clear(); 

   		// Publishes Pose msg
   		PosePub.publish(CameraPose); 		
	}

	void RotOrigin2Camera(vector<float>& xx, vector<float>& yy, int i){

		double roll, pitch, yaw;

		tf2::Quaternion QTag2Camera;
 		tf2::fromMsg(poses.poses[i].pose.orientation, QTag2Camera);
 		tf2::Matrix3x3 MTag2Camera(QTag2Camera);						// Rotation Matrix from Tag to the Camera
 					
 		tf2::Quaternion QOrgin2Tag;
 		tf2::fromMsg(MarkerAbsolutePose[i].orientation, QOrgin2Tag);
 		tf2::Matrix3x3 MOrgin2Tag(QOrgin2Tag);							// Rotation Matrix from the Origin to Tag
 			 				
 		tf2::Matrix3x3 MOrgin2Camera(MOrgin2Tag);
 		MOrgin2Camera = MOrgin2Tag * MTag2Camera * MRot;				// Rotation Matrix from Origin to Camera
 		MOrgin2Camera.getEulerYPR(yaw, pitch, roll);
 		xx.push_back(cos(yaw));
 		yy.push_back(sin(yaw));		

	}

	void PosOrigin2Camera(vector<float>& posX, vector<float>& posY, vector<float>& posZ, int i){

		float markerX, markerY, markerZ;
		tf2::Vector3 pos; tf2::Vector3 tempos;							// tempos 	-> Tag position read from topic ; pos -> Tag position in the Parrot's frame
		tf2::Vector3 p; tf2::Vector3 p_dash; tf2::Vector3 T;			// p 		-> position of the Tag in the World Frame; 
																		// p_dash 	-> position of the Tag in a Rotated World frame (no translation)
																		// T 		-> Position of the origin of the Parrot's frame wrt a Rotated World frame
		tf2::fromMsg(poses.poses[i].pose.position, tempos);
				
		pos.setZ(-tempos.getZ()); pos.setY(-tempos.getX());	
		pos.setX(-tempos.getY()+0.15);
		markerX=MarkerAbsolutePose[i].position.x;
		markerY=MarkerAbsolutePose[i].position.y;
		markerZ=MarkerAbsolutePose[i].position.z;
		
		p.setX(markerX); p.setY(markerY); p.setZ(markerZ);

		tf2::Quaternion qCamera;				
   		qCamera.setRPY(0, 0, CameraYaw); 
   		tf2::Matrix3x3 MOrigin2Camera(qCamera);							// Gets the rotation matrix of the Parrot frame wrt to the World Frame
   		tf2::Matrix3x3 MCamera2Origin;
		MCamera2Origin=MOrigin2Camera.transpose();						// Gets the inverse rotation matrix of the Parrot frame wrt to the World Frame
																		// (Note: For rotation matrixes transpose = inverse)
		p_dash.setX(MOrigin2Camera.tdotx(p));							
		p_dash.setY(MOrigin2Camera.tdoty(p));
		p_dash.setZ(MOrigin2Camera.tdotz(p));

		T=p_dash-pos;

		posX.push_back(MCamera2Origin.tdotx(T));						// Rotates vector T to the Original World Frame	
		posY.push_back(MCamera2Origin.tdoty(T));
		posZ.push_back(-pos.getZ());
	}
	*/

	void ground_truth(const nav_msgs::Odometry& msg){

		geometry_msgs::PoseWithCovariance TruePose;
		TruePose = msg.pose;
		tf2::Quaternion QTruePose;
 		tf2::fromMsg(TruePose.pose.orientation, QTruePose);
 		tf2::Matrix3x3 MTruePose(QTruePose);

 		double roll, pitch, yaw;
 		MTruePose.getEulerYPR (yaw,pitch,roll);
 		//cout<<"real yaw "<< yaw <<endl;
 		tf2::Vector3 pos;
		tf2::fromMsg(TruePose.pose.position, pos);
		//cout<<"real position "<<pos.getX()<<" "<<pos.getY()<<endl;

		geometry_msgs::Pose TestPose;
		TestPose = msg.pose.pose;

		PosePub.publish(TestPose);
	}
};



int main(int argc, char** argv){

	ros::init(argc, argv, "localization");	
	PoseVector posevector;

	ros::Rate r(30);
	while(ros::ok()){

		
		ros::spinOnce();
		r.sleep();		
	}
}
