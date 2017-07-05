#ifndef STRUCTS_H
#define STRUCTS_H

#include <QString>

struct logTxtInfo {
    QString cpu_load;
    QString freemem;
    QString uptime;
    QString firmwareVersion;
    QString hardware_id;
    QString serial_no;
    QString fps_value;
    QString resolutions;
    QString mac_addr;
    QString error_code;
    QString info_test;
    QString audio_card_record;
    QString low_state;
    QString high_state;
    QString zoom_in;
    QString zoom_out;
    QString zoom_in_out;
};


struct logCamTest {
    QString cam_hid;
    QString cam_serial;
    QString cam_audio;
    QString cam_io;
    QString cam_zoom;
    QString cam_fps;
    QString cam_mac;
    QString cam_version;
    QString cam_cpu;
    QString test_state;
    QString test_date;
    QString test_person;
};

struct logEncoderTest {
    QString encoder_serial;
    QString opcode;
    QString err_code;
    QString date;
    QString operator_;
    QString terminal;
};

#endif // STRUCTS_H
