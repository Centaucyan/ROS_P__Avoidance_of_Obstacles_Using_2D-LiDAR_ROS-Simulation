#include "ros/ros.h"
#include "geometry_msgs/Twist.h"
#include "sensor_msgs/LaserScan.h"

float range_ahead;
float range_ahead_min;
float range_ahead_max;
ros::Time turn_start_time;

void scan_cb(const sensor_msgs::LaserScan::ConstPtr& msg){
    range_ahead = 3.0;      // 최소값을 찾기 전 대략 큰 값으로 초기화.
                            // 0.8m 이하 거리가 될 때 회전하도록 프로그램 하였으므로
                            // 0.8보다는 큰 값으로 초기화.
        
    for(int i=0; i<=15; i++){     
        // LiDAR가 측정 가능 거리 범위를 벗어난 값 버리기
        if((msg->ranges[i] >= msg->range_min) && (msg->ranges[i] <= msg->range_max)){
            if(msg->ranges[i] < range_ahead){
                range_ahead = msg->ranges[i];
            }  
        }
        if (i==15){
            range_ahead_min = msg->ranges[i];
        }
    }

    for(int i=359; i>=345; i--){
        if((msg->ranges[i] >= msg->range_min) && (msg->ranges[i] <= msg->range_max)){
            if(msg->ranges[i] < range_ahead){
                range_ahead = msg->ranges[i];
            }
        } 
        if (i==345){
            range_ahead_max = msg->ranges[i];
        }
    }
    printf("range_ahead: %f\n", range_ahead);
}

int main(int argc, char **argv){
    ros::init(argc, argv, "go_scan");
    ros::NodeHandle n;
    ros::Publisher cmd_pup = n.advertise<geometry_msgs::Twist>("cmd_vel", 1);
    ros::Subscriber scan_sub = n.subscribe<sensor_msgs::LaserScan>("scan", 1, scan_cb);  // LaserScan의 메시지 자료형 토픽는 scan이라는 이름으로 pub됨.

    ros::Rate loop_rate(20);
    geometry_msgs::Twist cmd;

    while (ros::ok()) {
        if (range_ahead < 0.8) {
            if (range_ahead_min >= range_ahead_max) {
                turn_start_time = ros::Time::now();
                while ((ros::Time::now() - turn_start_time).toSec() <= 2.5) {
                    cmd.linear.x = -0.08;
                    cmd.angular.z = 0.4; 

                    cmd_pup.publish(cmd);
                    loop_rate.sleep(); 
                }
            }
            
            if (range_ahead_min < range_ahead_max) {
                turn_start_time = ros::Time::now();
                while ((ros::Time::now() - turn_start_time).toSec() <= 2.5) {
                    cmd.linear.x = -0.08;
                    cmd.angular.z = -0.4;

                    cmd_pup.publish(cmd);
                    loop_rate.sleep();
                }
            }
        } else {
            cmd.linear.x = 0.6;
            cmd.angular.z = 0;
        }
        cmd_pup.publish(cmd);
        ros::spinOnce();
        loop_rate.sleep();
    }
}