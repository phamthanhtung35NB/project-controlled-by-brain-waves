#include <SoftwareSerial.h>

SoftwareSerial BT(11, 10); // Rx/Tx
/**
 * COMMENT dòng 104 mở COMMENT  dòng 103
 * COMMENT  dòng 51 mở COMMENT  dòng 50
 */
#define LED 13
#define BAUDRATE 57600
#define DEBUGOUTPUT 0
#define cN1 31
#define cN2 33
#define cN3 35
#define button 8
#define threshold 50 // ngưỡng
#define regimeMAX 3  // chức năng tối đa
#define waiting 5000 // thời gian chờ
int select = 0;      // lựa chọn

unsigned long;

// checksum variables
byte generatedChecksum = 0;
byte checksum = 0;
int payloadLength = 0;
byte payloadData[64] = {0};
byte poorQuality = 0;
byte attention = 0;
byte meditation = 0;

// system variables
long lastReceivedPacket = 0;
boolean bigPacket = false;

void setup()
{
  pinMode(LED, OUTPUT);
  pinMode(cN1, OUTPUT);
  pinMode(cN2, OUTPUT);
  pinMode(cN3, OUTPUT);
  digitalWrite(cN1, 1);
  digitalWrite(cN2, 1);
  digitalWrite(cN3, 1);
  pinMode(button, INPUT);
  BT.begin(BAUDRATE);     // Software serial port  (ATMEGA328P)
  Serial.begin(BAUDRATE); // USB
}
void loop()
{
  // if (checkThreshold() >= threshold)
  if (digitalRead(button) == 1)
  {
    Serial.print("vao roi ");
    functionSelection(); // chọn chế độ

    // enforceTheRegime();  // thực thi chế độ
  }
  else
  {
    Serial.print("Chay che do : ");
    viewShow();
  }
  Serial.println("_");
  delay(500);
}
/**
 * chạy chế độ đã chọn
 */
void enforceTheRegime()
{
  switch (select)
  {
  case 1:
    Serial.print("Regime: " + select);
    break;
  case 2:
    Serial.print("Regime: " + select);
    break;
  case 3:
    Serial.print("Regime: " + select);
    break;
  case 4:
    Serial.print("Regime: " + select);
    break;
  case 5:
    Serial.print("Regime: " + select);
    break;
  default:
    Serial.print("Wait");
    break;
  }
}

/**
 * return _ Lựa chọn chế độ
 */
void functionSelection()
{
  unsigned long waitingTime = millis();
  while (millis() - waitingTime < waiting)
  {

    // if (checkThreshold() >= threshold)
    if (digitalRead(button) == 1)
    {
      Serial.println(digitalRead(button));
      Serial.println(select);
      select++;
      if (select > regimeMAX)
      {
        select = 0;
      }
      waitingTime = millis();
      delay(1000);
    }
    else
    {
      Serial.print(millis() - waitingTime);
      Serial.println("S");
    }
    Serial.print("select ");
    Serial.println(select);
    delay(500);
  }
}

/**
 * Hiển thị dữ liệu trên màn hình
 */
void viewShow()
{
  Serial.println(select);
  if (select == 1)
  {
    digitalWrite(cN1, 0);
    digitalWrite(cN2, 1);
    digitalWrite(cN3, 1);
  }
  else if (select == 2)
  {
    digitalWrite(cN1, 1);
    digitalWrite(cN2, 0);
    digitalWrite(cN3, 1);
  }
  else if (select == 3)
  {
    digitalWrite(cN1, 1);
    digitalWrite(cN2, 1);
    digitalWrite(cN3, 0);
  }
  else if (select == 0)
    digitalWrite(cN1, 1);
  digitalWrite(cN2, 1);
  digitalWrite(cN3, 1);
  {
  }
}

/**
 * Đọc một byte đơn từ kết nối nối tiếp Bluetooth và đợi cho đến khi có dữ liệu.
 * Nếu đầu ra gỡ lỗi được bật, nó sẽ lặp lại byte tới kết nối nối tiếp USB nhằm mục đích gỡ lỗi.
 */
byte ReadOneByte()
{
  int ByteRead;
  while (!BT.available())
    ;
  ByteRead = BT.read();

#if DEBUGOUTPUT
  Serial.print((char)ByteRead); // echo the same byte out the USB serial (for debug purposes)
#endif

  return ByteRead;
}

/**
 * return _ mức độ tập trung
 */
int checkThreshold()
{
  // Tìm kiếm các byte đồng bộ
  if (ReadOneByte() == 170)
  {
    if (ReadOneByte() == 170)
    {
      payloadLength = ReadOneByte();
      if (payloadLength > 169) // Độ dài tải trọng không thể lớn hơn 169
        return;

      generatedChecksum = 0;
      for (int i = 0; i < payloadLength; i++)
      {
        payloadData[i] = ReadOneByte(); // Đọc tải trọng vào bộ nhớ
        generatedChecksum += payloadData[i];
      }

      checksum = ReadOneByte();                    // Đọc byte tổng kiểm tra từ luồng
      generatedChecksum = 255 - generatedChecksum; // Nhận lời khen của một người được tạo ra

      if (checksum == generatedChecksum)
      {

        poorQuality = 200;
        attention = 0;
        meditation = 0;

        for (int i = 0; i < payloadLength; i++)
        { // Phân tích tải trọng
          switch (payloadData[i])
          {
          case 2:
            i++;
            poorQuality = payloadData[i];
            bigPacket = true;
            break;
          case 4:
            i++;
            attention = payloadData[i];
            break;
          case 5:
            i++;
            meditation = payloadData[i];
            break;
          case 0x80:
            i = i + 3;
            break;
          case 0x83:
            i = i + 25;
            break;
          default:
            break;
          } // switch
        }   // for loop

#if !DEBUGOUTPUT
          // *** Add your code here ***
        if (bigPacket)
        {
          if (poorQuality == 0)
            digitalWrite(LED, HIGH);
          else
            digitalWrite(LED, LOW);
          Serial.print("Attention: ");
          Serial.print(attention);
          Serial.print("\n");
        }
#endif
        bigPacket = false;
      }
      else
      {
        // Kiểm tra lỗi
      } // kết thúc nếu khác để kiểm tra
    }   // kết thúc nếu đọc byte 0xaa
  }     // kết thúc nếu đọc byte 0xaa
  return attention;
}
