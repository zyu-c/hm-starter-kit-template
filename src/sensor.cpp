#include "sensor.hpp"

extern "C" {
#include <cstdlib>

#include "iodefine.h"
}

Sensor* Sensor::sensor_p = nullptr;

Sensor* Sensor::getInstance() {
    if (!sensor_p) {
        sensor_p = (Sensor*)malloc(sizeof(Sensor));
    }

    return sensor_p;
}

void Sensor::initInfraredLight() {
    // ポート出力設定
    PORTA.PDR.BIT.B3 = 1;
    PORT1.PDR.BIT.B5 = 1;
    PORT1.PDR.BIT.B4 = 1;
    PORT3.PDR.BIT.B1 = 1;

    ledOff();
}

void Sensor::initAdc() {
    // ADC有効化
    SYSTEM.PRCR.WORD = 0xA502;
    MSTP_S12AD = 0;
    SYSTEM.PRCR.WORD = 0xA500;

    // ポートを周辺機能で使用
    PORT4.PMR.BIT.B6 = 1;
    PORT4.PMR.BIT.B2 = 1;
    PORT4.PMR.BIT.B1 = 1;
    PORT4.PMR.BIT.B0 = 1;

    PORTE.PDR.BIT.B7 = 0;
    PORTE.PMR.BIT.B1 = 0;

    // プロテクト解除
    MPC.PWPR.BIT.B0WI = 0;
    MPC.PWPR.BIT.PFSWE = 1;
    // 前右
    MPC.P46PFS.BIT.ASEL = 1;
    // 右
    MPC.P42PFS.BIT.ASEL = 1;
    // 左
    MPC.P41PFS.BIT.ASEL = 1;
    // 前左
    MPC.P40PFS.BIT.ASEL = 1;
    // バッテリ
    MPC.PE1PFS.BIT.ASEL = 1;
    // 再プロテクト
    MPC.PWPR.BYTE = 0x80;

    // クロック選択
    S12AD.ADCSR.BIT.CKS = 3;
    // スキャンモード選択
    S12AD.ADCSR.BIT.ADCS = 0;

    S12AD.ADANS0.WORD = 0x0247;
}

void Sensor::init() {
    initInfraredLight();
    initAdc();
}

void Sensor::update() {
    S12AD.ADANS0.WORD = 0x0047;
    S12AD.ADCSR.BIT.ADST = 1;
    while (S12AD.ADCSR.BIT.ADST) {
    }

    sensor[0] = S12AD.ADDR0;
    sensor[1] = S12AD.ADDR1;
    sensor[2] = S12AD.ADDR2;
    sensor[3] = S12AD.ADDR6;
}

void Sensor::updateBatteryVoltage() {
    S12AD.ADANS0.WORD = 0x0200;
    S12AD.ADCSR.BIT.ADST = 1;
    while (S12AD.ADCSR.BIT.ADST)
        ;
    battery_voltage = S12AD.ADDR9 * 330 * 2 / 4096;
}

void Sensor::ledOn() {
    PORTA.PODR.BIT.B3 = 1;
    PORT1.PODR.BIT.B5 = 1;
    PORT1.PODR.BIT.B4 = 1;
    PORT3.PODR.BIT.B1 = 1;
}

void Sensor::ledOff() {
    PORTA.PODR.BIT.B3 = 0;
    PORT1.PODR.BIT.B5 = 0;
    PORT1.PODR.BIT.B4 = 0;
    PORT3.PODR.BIT.B1 = 0;
}

uint16_t Sensor::getSensorValue(sensor_dir_t dir) { return sensor[dir]; }

uint16_t Sensor::getBatteryVoltage() { return battery_voltage; }

bool Sensor::isLowBattery() {
    // LiPoの適正電圧について
    // 最大電圧 : 4.2V
    // 標準電圧 : 3.7V
    // 最低電圧 : 3.5V

    if (battery_voltage < 350) {
        return true;
    } else {
        return false;
    }
}
