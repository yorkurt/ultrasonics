/*
 * Authors: Meg, Matt, Adam
 */
# include <ros.h>
# include <std_msgs/Int16MultiArray.h>
ros::NodeHandle nh;
std_msgs::Int16MultiArray data_array;
ros::Publisher pub("ultrasonics", &data_array);

// Initialization of pins

// ensure that # sensors = # triggers, echoes
// ensure that triggers[i] --> sensor = echoes[i] --> sensor
int triggers [3];
//= [12, 11, 10];
int echoes [3];
//= [9, 8, 7];

const int numberSensors = 3;
int real_data_array[3];
//= [0, 0, 0];

// Initialization of values

// require numberSensors * 12 < 500
int reading_delay = 500 - numberSensors * 14;

// Timer variables
long unsigned int now; // current time in millis
long unsigned int then; // time of last ping in millis

// Utility functions
void publish() { // publishes array data
  pub.publish(&data_array);
}

// trigPin: digital pin triggering sensor
// echoPin: digital pin reading response
// returns: int representing distance in cm
// note: the arduino nano has a granularity of 4 microseconds
// this formula is not accurate with the hardware used
int ping(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(4);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  int duration = pulseIn(echoPin, HIGH);
  // Calculating the distance
  return duration * 0.034 / 2;;
}


void setup() {
  // pin initialization
  for (int i = 0; i < numberSensors; i++) {
    pinMode(triggers[i], OUTPUT);
    pinMode(echoes[i], INPUT);
  }
  nh.initNode(); // initialize the node
  nh.advertise(pub); // advertise the topic on core node
  data_array.layout.dim_length = 1; // set dimensions of output array
  data_array.data_length = 3; // set length of array
  data_array.data = real_data_array; // set pointer
  for (int i = 0; i < numberSensors; i++) { // initialize data to 0
    data_array.data[i] = 0;
  }
  // initialize timers
  now = millis();
  then = millis();
}

void loop() {
now = millis(); // get current time in millis
if (now - then > reading_delay){ // trigger sonar cascade every reading_delay ms
  for (int i = 0; i < numberSensors; i++){
    real_data_array[0] = ping(triggers[i], echoes[i]);
  }
  data_array.data = real_data_array;
  publish();
  then = now; // reset the timer
}
nh.spinOnce(); // new node cycle
}
