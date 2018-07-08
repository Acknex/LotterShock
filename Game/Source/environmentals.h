#ifndef ENVIRONMENTALS_H
#define ENVIRONMENTALS_H

#define ENVIRONMENTALS_FAKECLOUD_ROTATESPEED skill1
#define ENVIRONMENTALS_TYPE skill10
#define ENVIRONMENTALS_TEMP skill11
#define ENVIRONMENTALS_TIMER skill12

#define ENVIROMENTALS_ICE_DAMAGE skill21
#define ENVIROMENTALS_ICE_OFFSET skill22

#define ENVIRONMENTAL_FAKECLOUD         1
#define ENVIRONMENTAL_ICECLOUD          2
#define ENVIRONMENTAL_TOXICCLOUD        3
#define ENVIRONMENTAL_ENGINE_BEAM       4
#define ENVIRONMENTAL_ENGINE_TERMINAL   5
#define ENVIRONMENTAL_ICE               6
#define ENVIRONMENTAL_AIRLOCK           7
#define ENVIRONMENTAL_SERVER            8
#define ENVIRONMENTAL_RADIO             9

#define ENVIRONMENTAL_DAMAGE_TOXIC      5
#define ENVIRONMENTAL_DAMAGE_BEAM       50


void environ_fake_cloud();
void environ_toxic_cloud();
void environ_ice_cloud();
void environ_engine_beam();
void environ_engterm();
void environ_server();



void environmentals_close();
void environmentals_update();

#endif
