#include "rclcpp/rclcpp.hpp"
#include "msg_srv_test/msg/test.hpp"
#include "msg_srv_test/srv/input.hpp"
#include <memory>
#include <iostream>
using namespace std;

using namespace std::chrono_literals;
using std::placeholders::_1;
class CallService : public rclcpp::Node
{
public:
  CallService()
  : Node("callNum")
  {
    caller_ = this->create_client<msg_srv_test::srv::Input>("calculate");
    sub_ = this->create_subscription<msg_srv_test::msg::Test>(
      "input_num", 1, std::bind(&CallService::topic_callback, this, _1));
    // request = std::make_shared<msg_srv_test::srv::Input::Request>();
    RCLCPP_INFO(this->get_logger(), "Send to num node /input_num : a = ?_num, b = ?_num, data = ?_sign");
  }
private:
  void topic_callback(const msg_srv_test::msg::Test::ConstSharedPtr msg)
  {
    RCLCPP_INFO(this->get_logger(), "I receive: '%.2f' '%s' '%.2f'", msg->a, msg->data.c_str(), msg->b);

    while (!caller_->wait_for_service(1s)) {
      if (!rclcpp::ok()) {
        RCLCPP_ERROR(this->get_logger(), "Interrupted while waiting for the service.");
        return;
      }
      RCLCPP_INFO(this->get_logger(), "Service not available, waiting again...");
    }

    auto request = std::make_shared<msg_srv_test::srv::Input::Request>();
    request->a = msg->a;
    request->b = msg->b;
    request->data = msg->data;

    using ServiceResponseFuture = rclcpp::Client<msg_srv_test::srv::Input>::SharedFuture;
    auto response_received_callback = [this](ServiceResponseFuture future){
      auto response = future.get();
      RCLCPP_INFO(this->get_logger(), "Result %.2f", response->result);
      return;
    };
    auto future_result = 
      caller_->async_send_request(request, response_received_callback);
  }
  rclcpp::Client<msg_srv_test::srv::Input>::SharedPtr caller_;
  rclcpp::Subscription<msg_srv_test::msg::Test>::SharedPtr sub_;
};

int main(int argc, char **argv){
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<CallService>());
  rclcpp::shutdown();
  return 0;
}