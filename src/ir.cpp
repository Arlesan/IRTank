#include "Arduino.h"
#include "TimerOne.h"
#include "ir.h"

unsigned int inputPinNo;
unsigned char state = 0;
unsigned char countLow = 0;
unsigned char countHigh = 0;
unsigned long lastReceptionTime = millis();
unsigned int makerCode;
unsigned int dataCode;
bool isReadingMakerCode;
unsigned char countBit;

/* 赤外線受信処理の初期化 */
void initIR(unsigned int n) {
  inputPinNo = n;
  pinMode(inputPinNo, INPUT);
  Timer1.initialize(100);
  Timer1.attachInterrupt(checkIR);
}

/* 赤外線信号のデコード */
void checkIR() {
  int pinLevel = digitalRead(inputPinNo);
  switch (state) {
    case 0:
      // リーダコード待機
      // ・80カウント以上のLOWからHIGHに変化したらリピート判定へ
      if (pinLevel == LOW) {
        countLow++;
      } else {
        if (countLow >= 80) {
          countLow = 0;
          countHigh = 1;
          state = 1;
        } else {
          countLow = 0;
        }
      }
      break;
    case 1:
      // リピート信号の判定
      // ・40カウント以上のHIGHからLOWに変化したらデータ受信へ
      // ・20カウント以上のHIGHからLOWに変化したらリピート入力の処理
      if (pinLevel == LOW) {
        if (countHigh >= 40) {
          countHigh = 0;
          countLow = 1;
          makerCode = 0;
          dataCode = 0;
          isReadingMakerCode = true;
          countBit = 0;
          state = 2;
        } else {
          if (countHigh >= 20 && millis() - lastReceptionTime < 150) {
            lastReceptionTime = millis();
            onReceiveIR(makerCode, dataCode);
          }
          countHigh = 0;
          state = 0;
        }
      } else {
        countHigh++;
      }
      break;
    case 2:
      // ビット信号のLOW部分を待機
      // ・5カウント以上のLOWからHIGHに変化したら論理部分の判定へ
      if (pinLevel == LOW) {
        countLow++;
      } else {
        if (countLow >= 5) {
          countLow = 0;
          countHigh = 1;
          state = 3;
        } else {
          countLow = 0;
          state = 0;
        }
      }
      break;
    case 3:
      // ビット論理の判定と受信コードの構築
      // ・一定カウント以上のHIGHからLOWに変化したら論理判定し、コード変数にシフト
      if (pinLevel == LOW) {
        if (countHigh >= 5) {
          unsigned int logic = countHigh >= 15 ? 1 : 0;
          if (isReadingMakerCode) {
            makerCode = (makerCode << 1) | logic;
            if (++countBit >= 16) {
              isReadingMakerCode = false;
              countBit = 0;
            }
          } else {
            dataCode = (dataCode << 1) | logic;
            if (++countBit >= 16) {
              lastReceptionTime = millis();
              onReceiveIR(makerCode, dataCode);
              countHigh = 0;
              countLow = 0;
              state = 0;
              break;
            }
          }
          countHigh = 0;
          countLow = 1;
          state = 2;
        }
      } else {
        countHigh++;
      }
      break;
  }
}
