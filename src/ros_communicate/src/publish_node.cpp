#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <vector>
#include <unistd.h>
#include <iostream>
#include <ros/ros.h>
#include <std_msgs/Bool.h>
#include <std_msgs/String.h>
#include <sstream>

using namespace std;
std_msgs::String message;
ros::Publisher pub;

void error(const char*msg){
    perror(msg);
    exit(1);
}

void socket_server()
{
	int socket_fd = socket(AF_INET,SOCK_STREAM,0);

    if(socket_fd == -1)
	{
		cout<<"socket failed!" <<endl;
		exit(-1);
	}

	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(10002);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	int res = bind(socket_fd,(struct sockaddr*)&addr,sizeof(addr));
	if(res == -1)
	{
		cout<<"bind failed" <<endl;
		exit(-1);
	}

	cout<<"bind ok"<<endl;
	listen(socket_fd,30);

	struct sockaddr_in client;
	socklen_t len = sizeof(client);
	int fd = accept(socket_fd,(struct sockaddr*)&client,&len);
	if(fd == -1)
	{
		cout <<"accept failde"<<endl;
		exit(-1);
	}
    cout<<"test 1"<<endl;

	char *ip = inet_ntoa(client.sin_addr);
	cout<<"client ["<<ip<<"] connect !"<<endl;

	char buffer[255] = {};
    stringstream ss;
    int n;
	while(ros::ok())
	{
	    ss.str(std::string());
        bzero(buffer,255);
        n = read(fd,buffer,255);
        if(n<0) error("ERROR reading from socket");
        ss<< buffer;
        message.data = ss.str();
        ROS_INFO("%s",message.data.c_str());

		pub.publish(message);
        ROS_INFO("please input numbers,1:red,2:green,3:blue");
        int number;
        cin>>number;
        if(number >3){
            ROS_INFO("input error");
            continue;
        }
        if(number == -1){
            ROS_INFO("return");
            close(fd);
            close(socket_fd);
        }
        
        stringstream send;
        send<<number;
        n = write(fd,send.str().c_str(),sizeof(send.str().c_str()));
        if (n < 0) error("ERROR writing to socket");
        	
	}
}

int main(int argc,char**argv)
{
	ros::init(argc,argv,"publish_node");
	ros::NodeHandle nh;
	pub = nh.advertise<std_msgs::String>("navigation",100);

    ros::Duration d(0.01);
    
    
	socket_server();
	return 0;
}
