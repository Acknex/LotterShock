// skill1: SPEED
action environ_fake_cloud()
{	
    set(me, PASSABLE);
    my->ENVIRONMENTALS_TYPE = ENVIRONMENTAL_FAKECLOUD;
    framework_setup(my, SUBSYSTEM_ENVIRONMENT);
}

// skill1: SPEED
action environ_ice_cloud()
{	
    set(me, PASSABLE);
    set(me, TRANSLUCENT);

    my.alpha = 20 + random(80);    
    my->ENVIRONMENTALS_TEMP = integer(random(1));

    my->ENVIRONMENTALS_TYPE = ENVIRONMENTAL_ICECLOUD;
    framework_setup(my, SUBSYSTEM_ENVIRONMENT);
}

// skill1: SPEED
action environ_toxic_cloud()
{	
    set(me, PASSABLE);  
    set(me, LIGHT);
    set(me, TRANSLUCENT);

    vec_set(my->blue, vector(0,255,0)); 

    my.alpha = 30 + random(80);    
    my->ENVIRONMENTALS_TEMP = integer(random(1));

    my->ENVIRONMENTALS_TYPE = ENVIRONMENTAL_TOXICCLOUD;
    framework_setup(my, SUBSYSTEM_ENVIRONMENT);
}

void environmentals_update()
{
    ENTITY *ptr;

    SUBSYSTEM_LOOP(ptr, SUBSYSTEM_ENVIRONMENT) 
    {
        switch(ptr.ENVIRONMENTALS_TYPE) 
        {
            case ENVIRONMENTAL_FAKECLOUD:
                ptr.roll += ptr.ENVIRONMENTALS_FAKECLOUD_ROTATESPEED * time_step;
                break;	
            case ENVIRONMENTAL_ICECLOUD:
                ptr.roll += ptr.ENVIRONMENTALS_FAKECLOUD_ROTATESPEED * time_step;
                if(ptr.ENVIRONMENTALS_TEMP == 0)
                {
                    ptr.alpha -= ptr.ENVIRONMENTALS_FAKECLOUD_ROTATESPEED * 5 * time_step;
                    if(ptr.alpha <= 20)
                        ptr.ENVIRONMENTALS_TEMP = 1;
                }
                else
                {
                    ptr.alpha += ptr.ENVIRONMENTALS_FAKECLOUD_ROTATESPEED * 5 * time_step;
                    if(ptr.alpha >= 100)
                        ptr.ENVIRONMENTALS_TEMP = 0;
                }
                break;	
            case ENVIRONMENTAL_TOXICCLOUD:
                ptr.roll += ptr.ENVIRONMENTALS_FAKECLOUD_ROTATESPEED * time_step;
                if(ptr.ENVIRONMENTALS_TEMP == 0)
                {
                    ptr.alpha -= ptr.ENVIRONMENTALS_FAKECLOUD_ROTATESPEED * 3 * time_step;
                    if(ptr.alpha <= 30)
                        ptr.ENVIRONMENTALS_TEMP = 1;
                }
                else
                {
                    ptr.alpha += ptr.ENVIRONMENTALS_FAKECLOUD_ROTATESPEED * 3 * time_step;
                    if(ptr.alpha >= 100)
                        ptr.ENVIRONMENTALS_TEMP = 0;
                }

                // A bit overkill to do it here but I'm lazy
                if(playerHasHazmat == 0)
                {
                    var dmgTest = region_check("region_toxic", vector(player.x + player.min_x, player.y + player.min_y, player.z + player.min_z), vector(player.x + player.max_x, player.y + player.max_y, player.z + player.max_z));
                    if(dmgTest > 0)
                        playerAddHealth(-ENVIRONMENTAL_DAMAGE_TOXIC);
                }
                break;			
        }
    }	
}