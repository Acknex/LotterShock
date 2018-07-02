#ifndef ENVIRONMENTALS_H
#define ENVIRONMENTALS_H

#define ENVIRONMENTALS_FAKECLOUD_ROTATESPEED skill1
#define ENVIRONMENTALS_TYPE skill10
#define ENVIRONMENTALS_TEMP skill11

#define ENVIRONMENTAL_FAKECLOUD     1
#define ENVIRONMENTAL_ICECLOUD      2
#define ENVIRONMENTAL_TOXICCLOUD    3

#define ENVIRONMENTAL_DAMAGE_TOXIC  5


void environ_fake_cloud();
void environ_toxic_cloud();
void environ_ice_cloud();

void environmentals_update();

#endif