// Open loop motor control example
 #include <SimpleFOC.h>


// BLDC motor & driver instance
// BLDCMotor motor = BLDCMotor(pole pair number);
// BLDCMotor motor = BLDCMotor(7);  //bldc的极对数是按永磁铁的极对数算的
// BLDCMotor motor = BLDCMotor(6);  //bldc的极对数是按永磁铁的极对数算的--钩子
BLDCMotor motor = BLDCMotor(24);  //bldc的极对数是按永磁铁的极对数算的--带hall -8
// BLDCDriver3PWM driver = BLDCDriver3PWM(pwmA, pwmB, pwmC, Enable(optional));
BLDCDriver3PWM driver = BLDCDriver3PWM(17, 18, 19, 5);

// Stepper motor & driver instance
//StepperMotor motor = StepperMotor(50);
//StepperDriver4PWM driver = StepperDriver4PWM(9, 5, 10, 6,  8);

//target variable
float target_position = 0;

// instantiate the commander
Commander command = Commander(Serial);
void doTarget(char* cmd) { command.scalar(&target_position, cmd); }

void setup() {

  // driver config
  // power supply voltage [V]
  driver.voltage_power_supply = 20;
  driver.init();
  // link the motor and the driver
  motor.linkDriver(&driver);

  // limiting motor movements
  motor.voltage_limit = 12;   // [V]
  motor.velocity_limit = 1000; // [rad/s] cca 50rpm
  // open loop control config
  // motor.controller = MotionControlType::angle_openloop;   //开环位置控制
  motor.controller = MotionControlType::velocity_openloop;   //开环速度控制

  // init motor hardware
  motor.init();

  // add target command T
  command.add('T', doTarget, "target angle");

  Serial.begin(115200);
  Serial.println("Motor ready!");
  Serial.println("Set target position [rad]");
  _delay(1000);
}

void loop() {
  // open  loop angle movements
  // using motor.voltage_limit and motor.velocity_limit
  motor.move(target_position);
  
  // user communication
  command.run();
}
