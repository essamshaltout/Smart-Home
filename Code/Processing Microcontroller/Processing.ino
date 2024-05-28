// Processing Microcontroller

#define trig 2
#define echo 3
#define ringAnswer 4
#define motionSensor 5
#define first_ir_pin 6
#define second_ir_pin 7

int motionSensorValue = 0;
bool lamp;
int numberOfPeople;
int distance;


bool first_ir;
bool second_ir;
bool first_trig = 0;
bool second_trig = 0;
bool first_first = 0;
bool second_first = 0;


bool approching(int distance);
bool buzzerState();
int getDistance();
bool motionAndPeople();
void firstTriggered();
void secondTriggered();
void personEntry();
void personLeaving();


unsigned long int t_distance;
unsigned long buzzerState_t;
unsigned long int t;



void setup() 
{
  Serial.begin(9600);
  Serial.setTimeout(5);
  pinMode(trig , OUTPUT);
  pinMode(echo , INPUT);
  pinMode(ringAnswer , INPUT_PULLUP) ;
  pinMode(motionSensor,INPUT);
  pinMode(first_ir_pin , INPUT);
  pinMode(second_ir_pin , INPUT);
}

void loop() 
{
  first_ir = digitalRead(first_ir_pin);
  second_ir = digitalRead(second_ir_pin);
  
  if(first_ir == 0 && first_trig == 0)        firstTriggered();
  if(second_ir == 0 && second_trig == 0)      secondTriggered();
  
  if(first_first == 1 && first_trig == 1 && second_trig == 1 && first_ir == 1 && second_ir == 1)        personEntry();
  if(second_first == 1 && first_trig == 1 && second_trig == 1 && first_ir == 1 && second_ir == 1)       personLeaving();
  
  motionSensorValue = digitalRead(motionSensor);
  lamp = motionAndPeople();
  delay(20);
  while(!(Serial.availableForWrite()));
  Serial.write(lamp);

  distance = getDistance();
  delay(20);
  while(!(Serial.availableForWrite()));
  Serial.write(buzzerState());
}

  

bool motionAndPeople()
{
  if(numberOfPeople == 0)
  {
    return false;
  }
  
  if(motionSensorValue != 0)
  {
    t = millis();
    return true;
  }
  else
  {
    if((millis() - t) > 100)        return false;
    else                            return true;
  }
}

bool approching(int distance)
{
  static int old_motionSensorValue = 200;
  if(distance < old_motionSensorValue && distance < 200)
  {
    old_motionSensorValue = distance;
    return true;
  }
  old_motionSensorValue = 200;
  return false;
}

bool buzzerState()
{
  
  static bool old_state = 0;
  static int no_of_approches = 0;
  if(approching(distance))
  {
    no_of_approches++;
  }
  else
  {
    no_of_approches = 0;
  }

  if(digitalRead(ringAnswer) == LOW)
  {
    old_state = false;
    no_of_approches = 0;
    return false;
  }

  if(millis() - buzzerState_t >= 1000 && old_state == HIGH)
  {
      old_state = false;
      no_of_approches = 0;
      return false;
  }

  if(old_state == true)
  {
    old_state = true;
    return true;
  }

  if(no_of_approches > 4 && distance < 20)
  {
    old_state = true;
    buzzerState_t = millis();
    return true;
  }
  else
  {
    old_state = false;
    return false;
  }
}


int getDistance()
{
  float d;
  int int_d;
  digitalWrite(trig,LOW);
  delayMicroseconds(2);
  digitalWrite(trig , HIGH);
  delayMicroseconds(10);
  digitalWrite(trig , LOW);
  t_distance = pulseIn(echo , HIGH);
  d = 0.01715 * t_distance ;
  int_d = (int) d;
  return int_d;
}

void firstTriggered()
{
   first_trig = 1;
   if(second_first == 0)    first_first = 1;
}

void secondTriggered()
{
 second_trig = 1;
 if(first_first == 0)       second_first = 1;
}

void personEntry()
{
  numberOfPeople++;
  
  first_first = 0;
  second_trig = 0;
  first_trig = 0;
}

void personLeaving()
{
  numberOfPeople--;
  second_first = 0;
  first_trig = 0;
  second_trig = 0;
}
