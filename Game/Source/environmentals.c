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

action environ_engine_beam()
{	
    set(me, PASSABLE);  
    set(me, LIGHT);
    set(me, TRANSLUCENT);

    vec_set(my->blue, vector(255,0,0)); 

    my.alpha = 0;    
    my->ENVIRONMENTALS_TEMP = 0;
    my->ENVIRONMENTALS_TIMER = 60 + random(120);

    my->ENVIRONMENTALS_TYPE = ENVIRONMENTAL_ENGINE_BEAM;
    framework_setup(my, SUBSYSTEM_ENVIRONMENT);
}

void environmentals_update()
{
    ENTITY *ptr;
    var dmgTest;

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
                    dmgTest = region_check("region_toxic", vector(player.x + player.min_x, player.y + player.min_y, player.z + player.min_z), vector(player.x + player.max_x, player.y + player.max_y, player.z + player.max_z));
                    if(dmgTest > 0)
                        playerAddHealth(-ENVIRONMENTAL_DAMAGE_TOXIC);
                }
                break;
            case ENVIRONMENTAL_ENGINE_BEAM:
                if(story_enginesEnabled == 1)
                {
                    if(ptr.ENVIRONMENTALS_TEMP > ptr.ENVIRONMENTALS_TIMER)
                    {
                        if(ptr.alpha < 100) 
                        {
                            ptr.alpha += 20 * time_step;
                            ptr.lightrange += 400 * time_step;
                        }
                        else if(random(100) > 95)
                        {
                            ptr.alpha = 75;
                            ptr.lightrange = ptr.lightrange * 0.75;
                        }

                        dmgTest = c_intersect(vector(ptr.x + (ptr.min_x * 0.77), ptr.y + (ptr.min_y * 0.77), ptr.z + (ptr.min_z * 0.77)), vector(ptr.x + (ptr.max_x * 0.77), ptr.y + (ptr.max_y * 0.77), ptr.z + (ptr.max_z * 0.77)), NULL, vector(player.x + player.min_x, player.y + player.min_y, player.z + player.min_z), vector(player.x + player.max_x, player.y + player.max_y, player.z + player.max_z), NULL);
                        if(dmgTest == -1)
                            playerAddHealth(-ENVIRONMENTAL_DAMAGE_BEAM);
                    }
                    else
                    {
                        ptr.ENVIRONMENTALS_TEMP += 2 * time_step;
                    }
                }
                break;
        }
    }	
}