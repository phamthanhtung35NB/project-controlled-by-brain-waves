/**
 *
 * https://github.com/phamthanhtung35NB/project-controlled-by-brain-waves
 *
 */
#include <SoftwareSerial.h>

SoftwareSerial BT(11, 10); // Rx/Tx

#define LED 13
#define BAUDRATE 57600
#define DEBUGOUTPUT 0

// Chân cắm nút test
#define button 8

///////////////////////////////////////////////////////
/**
 * Ngưỡng của sóng não để chuyển chế độ.
 */
#define threshold 50
///////////////////////////////////////////////////////

///////////////////////////////////////////////////////
/**
 * Số chức năng tối đa.
 */
#define regimeMAX 3
///////////////////////////////////////////////////////

///////////////////////////////////////////////////////
/**
 * Thời gian chờ để chọn chế độ.
 */
#define waiting 5000 // = 5s
///////////////////////////////////////////////////////

///////////////////////////////////////////////////////
/**
 * Chế độ 0 là không dùng chế độ nào
 * (KHÔNG thay đổi biến select này)
 */
int select = 0;
///////////////////////////////////////////////////////

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
  pinMode(button, INPUT);
  BT.begin(BAUDRATE);     // Software serial port  (ATMEGA328P)
  Serial.begin(BAUDRATE); // USB
}
void loop()
{
  //*******************************************************************
  checkThreshold();
}

/**
 * Lựa chọn chế độ.
 */
void functionSelection()
{
  unsigned long waitingTime = millis();
  while (millis() - waitingTime < waiting)
  {
    //*******************************************************************
    if (attention >= threshold)
    {
      Serial.println("*******************************************************************");
      select++;
      if (select > regimeMAX)
      {
        select = 0;
      }
      delay(1500); // test
      waitingTime = millis();
    }
    else
    {
      Serial.print("Time: ");
      Serial.print(waiting - (millis() - waitingTime));
      Serial.println("S");
      delay(1000); // test
    }
    Serial.print("Select: ");
    Serial.println(select);
  }
}

/**
 * Hiển thị dữ liệu trên màn hình + thực thi chế độ
 */
void viewShow()
{
  Serial.print("Đang chạy chế độ: ");
  Serial.println(select);
  if (select == 1)
  {
    // TODO: Add your code here
  }
  else if (select == 2)
  {
    // TODO: Add your code here
  }
  else if (select == 3)
  {
    // TODO: Add your code here
  }
  else if (select == 0)
  {
    // TODO: Add your code here
  }
}

/**
 * Đọc một byte đơn từ kết nối nối tiếp Bluetooth và đợi cho đến khi có dữ liệu.
 * Nếu đầu ra gỡ lỗi được bật, nó sẽ lặp lại byte tới kết nối nối tiếp USB nhằm
 * mục đích gỡ lỗi.
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
void checkThreshold()
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
        if (bigPacket)
        {
          if (poorQuality == 0)
            digitalWrite(LED, HIGH);
          else
            digitalWrite(LED, LOW);
          Serial.print("Attention: ");
          Serial.println(attention);
          if (select == 0 && attention >= threshold)
          {
            Serial.println("Vào rồi ");
            // chọn chế độ
            functionSelection();
            Serial.println("Húp.................");
          }
          else if (select != 0)
          {
            viewShow();
          }
          Serial.println("_____________");
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
}