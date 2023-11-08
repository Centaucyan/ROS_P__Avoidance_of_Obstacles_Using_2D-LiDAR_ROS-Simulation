#include "ros/ros.h"
#include "geometry_msgs/Twist.h"
#include "sensor_msgs/LaserScan.h"

float range_ahead;

void scan_cb(const sensor_msgs::LaserScan::ConstPtr& msg){
    // range_ahead = msg -> ranges[0];
    range_ahead = 3.0;      // 최소값을 찾기 전 대략 큰 값으로 초기화.
                            // 0.8m 이하 거리가 될 때 회전하도록 프로그램 하였으므로 0.8보다는 큰 값으로 초기화.
    
    for(int i=0; i<=15; i++){
        // if(msg->ranges[i] >= msg->range_min){        
        // LiDAR가 측정 가능 거리 범위를 벗어난 값 버리기
        if((msg->ranges[i] >= msg->range_min) && (msg->ranges[i] <= msg->range_max)){
            if(msg->ranges[i] < range_ahead){
                range_ahead = msg->ranges[i];
            }  
        }
    }

    for(int i=359; i>=345; i--){
        // if(msg->ranges[i] >= msg->range_min){
        if((msg->ranges[i] >= msg->range_min) && (msg->ranges[i] <= msg->range_max)){
            if(msg->ranges[i] < range_ahead){
                range_ahead = msg->ranges[i];
            }
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

    while(ros::ok()){
        if(range_ahead < 0.8){      // ranges[0] 방향의 거리가 0.8m 미만이면
            cmd.linear.x = 0;
            cmd.angular.z = 0.4;    // 0.2rad/s 속도로 제자리 회전하라
        }
        else{                       // 0.8m 이상이면
            cmd.linear.x = 0.6;      // 0.3m/s로 직진하라.
            cmd.angular.z = 0;
        }
        cmd_pup.publish(cmd);
        ros::spinOnce();
        loop_rate.sleep();
    }
}