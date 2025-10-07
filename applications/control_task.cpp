#include "cmsis_os.h"
#include "io/bmi088/bmi088.hpp"
#include "io/vision/vision.hpp"
#include "tools/crc/crc.hpp"
#include "tools/mahony/mahony.hpp"

const float r_ab[3][3] = {{1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}};
// C板
sp::BMI088 bmi088(&hspi1, GPIOA, GPIO_PIN_4, GPIOB, GPIO_PIN_0, r_ab);

// 达妙
// sp::BMI088 bmi088(&hspi2, GPIOC, GPIO_PIN_0, GPIOC, GPIO_PIN_3, r_ab);

sp::Mahony imu(1e-3f);
sp::Vision vis;

uint32_t a = 0;
uint16_t count = 0;
extern "C" void control_task()
{
  bmi088.init();

  while (true) {
    // 推荐实际使用时放在imu_task
    bmi088.update();
    imu.update(bmi088.acc, bmi088.gyro);
    count++;
    if (count == 1000) {
      a++;
      vis.send(0, imu.q, imu.yaw, imu.vyaw, imu.pitch, imu.vpitch, a, 0);
      count = 0;
    }
    if (a == 2 ^ 32 - 1) a = 0;
    // 使用调试(f5)查看vis内部变量的变化

    osDelay(1);  // 1s
  }
}

extern "C" void vision_callback(uint8_t * buf, uint32_t len) { vis.update(buf, len); }