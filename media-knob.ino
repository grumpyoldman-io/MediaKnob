#include <ClickEncoder.h>
#include <TimerOne.h>
#include <HID-Project.h>

#define ENCODER_CLK A3
#define ENCODER_DT A2
#define ENCODER_SW A1

#define ENCODER_STEPS_PER_NOTCH 4

ClickEncoder *encoder;
int16_t lastRotationValue, rotationValue;
bool isVolumeMode;

void timerInterrupt()
{
  encoder->service();
}

void setup()
{
  Serial.begin(9600);
  Consumer.begin();

  encoder = new ClickEncoder(ENCODER_DT, ENCODER_CLK, ENCODER_SW, ENCODER_STEPS_PER_NOTCH);
  encoder->setDoubleClickTime(300);
  encoder->setHoldTime(400);

  Timer1.initialize(200);
  Timer1.attachInterrupt(timerInterrupt);

  lastRotationValue = -1;
  rotationValue = -1;
  isVolumeMode = true;
}

void loop()
{
  ClickEncoder::Button buttonState = encoder->getButton();
  rotationValue += encoder->getValue();

  if (buttonState == ClickEncoder::Held && isVolumeMode)
  {
    Serial.println("Media Mode");
    isVolumeMode = false;
  }
  else if (buttonState == ClickEncoder::Released)
  {
    Serial.println("Volume Mode");
    isVolumeMode = true;
  }

  if (isVolumeMode)
  {
    // Volume
    if (lastRotationValue < rotationValue)
    {
      Serial.println("volume up");
      Consumer.write(MEDIA_VOLUME_UP);
    }
    else if (lastRotationValue > rotationValue)
    {
      Serial.println("volume down");
      Consumer.write(MEDIA_VOLUME_DOWN);
    }
    lastRotationValue = rotationValue;

    // Click Events
    if (buttonState == ClickEncoder::Clicked)
    {
      Serial.println("play/pause");
      Consumer.write(MEDIA_PLAY_PAUSE);
    }

    if (buttonState == ClickEncoder::DoubleClicked)
    {
      Serial.println("mute/un-mute");
      Consumer.write(MEDIA_VOL_MUTE);
    }
  }
  else
  {
    // Media
    if (abs(rotationValue - lastRotationValue) > 2)
    {
      if (rotationValue > lastRotationValue)
      {
        Serial.println("skip next");
        Consumer.write(MEDIA_NEXT);
      }
      else if (rotationValue < lastRotationValue)
      {
        Serial.println("skip prev");
        Consumer.write(MEDIA_PREV);
      }
      lastRotationValue = rotationValue;
    }
  }

  delay(10);
}
