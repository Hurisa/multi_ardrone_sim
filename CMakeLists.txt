cmake_minimum_required(VERSION 2.8.3)
project(multi_ardrone_sim)
set(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")
find_package(catkin REQUIRED  
  gazebo_ros
  gazebo_ros_control
  geometry_msgs
  roscpp
  std_msgs
  tf
  tf2_msgs
  message_generation
  rosbag
)

## Generate messages in the 'msg' folder
add_message_files(
  FILES
  posevector.msg
  avoid.msg
)


generate_messages(
  DEPENDENCIES
  geometry_msgs
  std_msgs
)

catkin_package(
  CATKIN_DEPENDS catkin message_runtime rosbag
)
include_directories(${catkin_INCLUDE_DIRS})





#add_executable(getPose 		  src/getPose.cpp)
add_executable(aruco_localization src/aruco_localization.cpp)
add_executable(potential_field	  src/potential_field.cpp)
add_executable(altcontrol	  src/altcontrol.cpp)
add_executable(ctrl_velocity	  src/ctrl_velocity.cpp)
add_executable(flocking	  	  src/flocking.cpp)
add_executable(levy_controller	  src/levy_controller.cpp)
add_executable(supervisor	  src/supervisor.cpp)
add_executable(avoid_times	  src/avoid_times.cpp)
add_executable(takeoff	  src/takeoff.cpp)
add_executable(land	  src/land.cpp)
add_executable(rectVel	  src/rectVel.cpp)
add_executable(reynolds	  src/reynolds.cpp)
add_executable(landall	  src/landall.cpp)
add_executable(supervisor_real	  src/supervisor_real.cpp)

#target_link_libraries(getPose 			${catkin_LIBRARIES})
target_link_libraries(aruco_localization 	${catkin_LIBRARIES})
target_link_libraries(potential_field		${catkin_LIBRARIES})
target_link_libraries(altcontrol		${catkin_LIBRARIES})
target_link_libraries(ctrl_velocity		${catkin_LIBRARIES})
target_link_libraries(flocking			${catkin_LIBRARIES})
target_link_libraries(levy_controller		${catkin_LIBRARIES})
target_link_libraries(supervisor		${catkin_LIBRARIES})
target_link_libraries(avoid_times		${catkin_LIBRARIES})
target_link_libraries(takeoff		${catkin_LIBRARIES})
target_link_libraries(land	${catkin_LIBRARIES})
target_link_libraries(rectVel	${catkin_LIBRARIES})
target_link_libraries(reynolds	${catkin_LIBRARIES})
target_link_libraries(landall	${catkin_LIBRARIES})
target_link_libraries(supervisor_real	${catkin_LIBRARIES})

#add_executable(<tareget *.o> src/<filename>.cpp)

#target_link_libraries(<tareget *.o> ${catkin_LIBRARIES})

