////////////////////////////////////////////////////////////////////////
// Arduino Bluetooth Interface with Mindwave
//
// This is example code provided by NeuroSky, Inc. and is provided
// license free.
//
// This modification allows view data trough serial monitor
// Lozano Ramirez Angel Ivan
// Mexico  2.07.2021
////////////////////////////////////////////////////////////////////////

#include <SoftwareSerial.h>
SoftwareSerial BT(11, 10); // Rx/Tx

#define LED 13
#define BAUDRATE 57600
#define DEBUGOUTPUT 0

// checksum variables
byte generatedChecksum = 0;
byte checksum = 0;
int payloadLength = 0;
byte payloadData[64] = {0};
byte poorQuality = 0;
byte attention = 0;
byte meditation = 0;

int select = 0;

unsigned long;

// system variables
long lastReceivedPacket = 0;
boolean bigPacket = false;

//////////////////////////
// Microprocessor Setup //
//////////////////////////
void setup()
{
  pinMode(LED, OUTPUT);
  BT.begin(BAUDRATE);     // Software serial port  (ATMEGA328P)
  Serial.begin(BAUDRATE); // USB
}
void loop()
{
  int attentionLevel = check();
}

/**
 * 
*/
int functionSelection()
{

  return select;
}

/**
 * Hiển thị dữ liệu trên màn hình
 */
void viewShow()
{
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
int check()
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