#include "rclcpp/rclcpp.hpp"
#include "msg_srv_test/msg/test.hpp"
#include "msg_srv_test/srv/input.hpp"
#include <memory>
#include <iostream>
#include <string>
using namespace std;

float result(float a, float b, string sign){
  float _result = 0.0;
  if(sign=="+") _result = a + b;
  else if(sign=="-") _result = a - b;
  else if(sign=="/") _result = a / b;
  else if(sign=="*") _result = a * b;
  else _result = 0.0;
  return _result;
}

void add(const std::shared_ptr<msg_srv_test::srv::Input::Request> request,
          std::shared_ptr<msg_srv_test::srv::Input::Response>     response)
{
  RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "Incoming request\n a: %.2f, b: %.2f char: %s", request->a, request->b, request->data.c_str());
  response->result = result(request->a, request->b, request->data);
}

int main(int argc, char **argv){
  rclcpp::init(argc, argv);

  std::shared_ptr<rclcpp::Node> node = rclcpp::Node::make_shared("test_service");

  rclcpp::Service<msg_srv_test::srv::Input>::SharedPtr service =
    node->create_service<msg_srv_test::srv::Input>("calculate", &add);

  RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "Ready to calculate two ints.");

  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}