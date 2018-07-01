// skill1: SPEED
action environ_fake_cloud()
{	
	set(me, PASSABLE);
	my->ENVIRONMENTALS_TYPE = ENVIRONMENTAL_FAKECLOUD;
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
		}
	}	
}