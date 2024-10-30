#include <micro_ros_arduino.h>

#include <stdio.h>
#include <rcl/rcl.h>
#include <rcl/error_handling.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>

#include <std_msgs/msg/int32.h>
#include <sensor_msgs/msg/imu.h>

//setup BNO055
#include <Wire.h>
#include <Adafruit_BNO055.h>
#include <Ticker.h>
Ticker bno055ticker; //タイマー割り込み用のインスタンス
#define BNO055interval 10 //何ms間隔でデータを取得するか

//Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x28, &Wire); //ICSの名前, デフォルトアドレス, 謎
Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x28);

//setup micro-ros
rcl_publisher_t publisher;
std_msgs__msg__Int32 msg;
sensor_msgs__msg__Imu msg_imu;
rclc_executor_t executor;
rclc_support_t support;
rcl_allocator_t allocator;
rcl_node_t node;
rcl_timer_t timer;

#define LED_PIN 13

#define RCCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){error_loop();}}
#define RCSOFTCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){}}


void error_loop(){
  while(1){
    digitalWrite(LED_PIN, !digitalRead(LED_PIN));
    delay(100);
  }
}
/*
void timer_callback(rcl_timer_t * timer, int64_t last_call_time)
{  
  RCLC_UNUSED(last_call_time);
  if (timer != NULL) {
    RCSOFTCHECK(rcl_publish(&publisher, &msg, NULL));
    msg.data++;
  }
}
*/

void setup() {
  set_microros_transports();
  
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);

  //BNO055
  pinMode(21, INPUT_PULLUP); //SDA 21番ピンのプルアップ
  pinMode(22, INPUT_PULLUP); //SDA 22番ピンのプルアップ
  if (!bno.begin()) // センサの初期化
  {
    while (1);
  }
  
  delay(2000);

  allocator = rcl_get_default_allocator();

  //create init_options
  RCCHECK(rclc_support_init(&support, 0, NULL, &allocator));

  // create node
  RCCHECK(rclc_node_init_default(&node, "micro_ros_arduino_node", "", &support));

  // create publisher
  RCCHECK(rclc_publisher_init_default(
    &publisher,
    &node,
    ROSIDL_GET_MSG_TYPE_SUPPORT(sensor_msgs, msg, Imu),
    "micro_ros_imu"));

  /*
  // create timer,
  const unsigned int timer_timeout = 1000;
  RCCHECK(rclc_timer_init_default(
    &timer,
    &support,
    RCL_MS_TO_NS(timer_timeout),
    timer_callback));
  */
  // create executor
  //RCCHECK(rclc_executor_init(&executor, &support.context, 1, &allocator));
  //RCCHECK(rclc_executor_add_timer(&executor, &timer));

  //msg.data = 0;
}

void loop() {
  delay(10);
  // ジャイロセンサ値の取得と表示
  imu::Vector<3> gyroscope = bno.getVector(Adafruit_BNO055::VECTOR_GYROSCOPE);
  // 加速度センサ値の取得と表示
  imu::Vector<3> accelermetor = bno.getVector(Adafruit_BNO055::VECTOR_ACCELEROMETER);
  // 磁力センサ値の取得と表示
  imu::Vector<3> magnetmetor = bno.getVector(Adafruit_BNO055::VECTOR_MAGNETOMETER);
  // センサフュージョンによる方向推定値の取得と表示
  imu::Vector<3> euler = bno.getVector(Adafruit_BNO055::VECTOR_EULER);
  // センサフュージョンの方向推定値のクオータニオン
  imu::Quaternion quat = bno.getQuat();

  msg_imu.angular_velocity.x = gyroscope.x();
  msg_imu.angular_velocity.y = gyroscope.y();
  msg_imu.angular_velocity.z = gyroscope.z();

  msg_imu.linear_acceleration.x = accelermetor.x();
  msg_imu.linear_acceleration.y = accelermetor.y();
  msg_imu.linear_acceleration.z = accelermetor.z();

  msg_imu.orientation.x = quat.x();
  msg_imu.orientation.y = quat.y();
  msg_imu.orientation.z = quat.z();
  msg_imu.orientation.w = quat.w();


  RCSOFTCHECK(rcl_publish(&publisher, &msg_imu, NULL));

  //RCSOFTCHECK(rclc_executor_spin_some(&executor, RCL_MS_TO_NS(100)));
}
