/**
Software License Agreement (BSD)

\file      driver.cpp
\authors   Mike Purvis <mpurvis@clearpathrobotics.com>
           Mike Irvine <mirvine@clearpathrobotics.com>
\modified  Bismaya Sahoo <bsahoo@uwaterloo.ca>
\copyright Copyright (c) 2013, Clearpath Robotics, Inc., All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
following conditions are met:
 * Redistributions of source code must retain the above copyright notice, this list of conditions and the following
   disclaimer.
 * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following
   disclaimer in the documentation and/or other materials provided with the distribution.
 * Neither the name of Clearpath Robotics nor the names of its contributors may be used to endorse or promote products
   derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include "roboteq_driver/controller.h"
#include "roboteq_driver/channel.h"
#include <geometry_msgs/Twist.h>
#include "ros/ros.h"


int main(int argc, char **argv) {
  ros::init(argc, argv, "~");
  ros::NodeHandle nh("~");

  std::string port = "/dev/ttyUSB0";
  int32_t baud = 9600;
  nh.param<std::string>("port", port, port);
  nh.param<int32_t>("baud", baud, baud);

  // Interface to motor controller.
  roboteq::Controller controller(port.c_str(), baud);

  
  

  // Setup channels.
  if (nh.hasParam("channels")) {
    XmlRpc::XmlRpcValue channel_namespaces;
    nh.getParam("channels", channel_namespaces);
    ROS_ASSERT(channel_namespaces.getType() == XmlRpc::XmlRpcValue::TypeArray);
    for (int i = 0; i < channel_namespaces.size(); ++i) 
    {
      ROS_ASSERT(channel_namespaces[i].getType() == XmlRpc::XmlRpcValue::TypeString);
      controller.addChannel(new roboteq::Channel(1 + i, channel_namespaces[i], &controller));
    }
  } else {
    // Default configuration is a single channel in the node's namespace.
    controller.addChannel(new roboteq::Channel(1, "~", &controller));
  } 

  // Attempt to connect and run.
  while (ros::ok()) {
    //Connect
    ROS_DEBUG("Attempting connection to %s at %i baud.", port.c_str(), baud);
    controller.connect();
    ROS_WARN_STREAM("Port Open. Controller Connected");
  
   int err=controller.initialize();

   if (err==1)
   {
    ROS_WARN_STREAM("No Message Receieved");
   }

    //Check to see if feedback is present
    int fdbck_status=controller.isFeedbackPresent();
    ROS_INFO("Feedback Status::%d ",fdbck_status);
    sleep(3);

    controller.showFeedbackPos();
    sleep(10);
    //Move Brakes to start Position

    //Check if both brakes are working
    ROS_WARN_STREAM("Left Brake Check");
    controller.leftBrake();  
    sleep(1);
    ROS_WARN_STREAM("Left Brake Check Reverse");
    controller.zeroBrakeL();  
    sleep(1);
    ROS_WARN_STREAM("Right Brake Check");
    controller.rightBrake();  
    sleep(1);
    ROS_WARN_STREAM("Right Brake Check Reverse");
    controller.zeroBrakeR();  
    sleep(1);

    if (controller.connected()) {
      ros::AsyncSpinner spinner(1);
      spinner.start();
      while (ros::ok()) {
        controller.spinOnce();
      }
      // Make sure brakes are in the initial position.
//      controller.zeroBrakeR();
 //     controller.zeroBrakeL();
      spinner.stop();
    } else {
      ROS_DEBUG("Problem connecting to serial device.");
      ROS_ERROR_STREAM_ONCE("Problem connecting to port " << port << ". Trying again every 1 second.");
      sleep(1);
    }  
  }

  return 0;
}
