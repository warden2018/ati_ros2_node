#include <thread>
#include <memory>
#include <AtiFTSensor.h>
// ROS includes
#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
#include "geometry_msgs/msg/wrench.hpp"


using namespace std::chrono_literals;

class FTPublisher : public rclcpp::Node
{
public:
 FTPublisher(/* args */)
 : Node("atiPublisher")
 {
      auto timerCallback =
                [this]() -> void {
        auto message = std_msgs::msg::String();
        message.data = "Getting FT sensor data.";
        RCLCPP_INFO(this->get_logger(),"Publishing message '%s' ", message.data.c_str());

        sensor.getStatus(rdt_seq, ft_seq, status);
        sensor.getFT(&F_[0], &T_[0]);

        // RCLCPP_INFO(this->get_logger(),"Publishing message '%s' ", message.data.c_str());

        // RCLCPP_INFO(this->get_logger(),"Publishing message '%s' ", message.data.c_str());

        printf("rdt_seq=%d, ft_seq=%d, status=%d\n", rdt_seq, ft_seq, status);
        printf("%4.2f \t %4.2f \t %4.2f \t %4.2f \t %4.2f \t %4.2f \n",
                  F_[0], F_[1], F_[2], T_[0], T_[1], T_[2]);

        geometry_msgs::msg::Wrench ft_values;
        ft_values.force.x = F_[0];
        ft_values.force.y = F_[1];
        ft_values.force.z = F_[2];

        ft_values.torque.x = T_[0];
        ft_values.torque.y = T_[1];
        ft_values.torque.z = T_[2];

        publisher_->publish(ft_values);
      };

        //create sensor interface 
      sensor.initialize();
      std::this_thread::sleep_for(std::chrono::seconds(1));
      sensor.setBias();
      publisher_ = this->create_publisher<geometry_msgs::msg::Wrench>("ati_sensor", 10);

      //this->create_wall_timer(100ms, timerCallback);

      timer_ = this->create_wall_timer(10ms, timerCallback);
 }
 ~FTPublisher(){

 }

 private:
  ati_ft_sensor::AtiFTSensor sensor;
  rclcpp::Publisher<geometry_msgs::msg::Wrench>::SharedPtr publisher_;
  rclcpp::TimerBase::SharedPtr timer_;
  double F_[3];
  double T_[3];
  uint32_t rdt_seq, ft_seq, status;
};


int main(int argc, char** argv)
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<FTPublisher>());
  rclcpp::shutdown();

  return 0;
}