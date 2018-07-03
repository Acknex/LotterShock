//////////////////////////////
// meshFun.h
//////////////////////////////

var meshFunGetDistanceToRegion(VECTOR* vpos, STRING* str_region, int regionNum, VECTOR* vresult)
{
	var dist = 0,fac,vdot;
	VECTOR vmin,vmax,vdir,vpdir,vproj;

	if(!region_get(str_region,regionNum,vmin,vmax)) return -1;
	if(region_check(str_region,vpos,vpos))
	{
		if(vresult) vec_set(vresult,vpos);
		return 0;
	}
	vec_diff(vpdir,vpos,vmin);
	fac = clamp(vpdir.x/(double)(vmax.x-vmin.x),0,1);
	vproj.x = vmin.x*(1-fac) + vmax.x*fac;
	fac = clamp(vpdir.y/(double)(vmax.y-vmin.y),0,1);
	vproj.y = vmin.y*(1-fac) + vmax.y*fac;
	fac = clamp(vpdir.z/(double)(vmax.z-vmin.z),0,1);
	vproj.z = vmin.z*(1-fac) + vmax.z*fac;
	if(vresult) vec_set(vresult,vproj);
	dist = vec_dist(vpos,vproj);

	return dist;	
}

TEXT *meshFunRegionNames_txt =
{
	string("blue","red","region_toxic","green","yellow");
}
COLOR meshFunRegionColors[5];


void meshFunDoForEnt(ENTITY* ent)
{
	ent_clone(ent);
	D3DVERTEX* vbuffer;
	short* ibuffer;
	if(!ent_buffers(ent,0,0,&vbuffer,&ibuffer,NULL)) { error("error"); return; }
	int numVertices = ent_status(ent,1);
	int i,j,regionNum,numIterations = 0;
	for(i = 0; i < numVertices; i++)
	{
		D3DVERTEX* vert = &vbuffer[i];
		VECTOR vpos;
		COLOR color;
		vec_fill(color,128);
		vec_set(vpos,vector(vert.x,vert.z,vert.y));
		vec_for_ent(vpos,ent);
		for(j = 0; j < meshFunRegionNames_txt.strings; j++)
		{
			STRING* strRegion = (meshFunRegionNames_txt.pstring)[j];
			regionNum = 1;
			var minDist = 99999;
			while(1)
			{
				var dist = meshFunGetDistanceToRegion(vpos, strRegion, regionNum, NULL);
				if(dist < 0) break;
				if(!dist)
				{
					minDist = 0;
					break;
				}
				minDist = minv(minDist,dist);
				regionNum++;
			}
			if(minDist < 2048)
			{
				float lerpFac = 1.0-(float)minDist/2048.0;
				vec_lerp(color,color,meshFunRegionColors[j],lerpFac);
			}
		}
		vert.x3 = (color.red-128)/128.0;
		vert.y3 = (color.green-128)/128.0;
		vert.z3 = (color.blue-128)/128.0;
		
		numIterations++;
	}
}

void meshFunDo()
{
	vec_set(meshFunRegionColors[0],vector(255,100,40));
	vec_set(meshFunRegionColors[1],vector(5,32,255));
	vec_set(meshFunRegionColors[2],vector(15,255,96));
	vec_set(meshFunRegionColors[3],vector(80,255,80));
	vec_set(meshFunRegionColors[4],vector(5,190,255));

	ENTITY* ent;
	for(ent = ent_next(NULL); ent; ent = ent_next(ent))
	{
		if(ent.albedo == 49) meshFunDoForEnt(ent); //!(ent.emask & DYNAMIC)
	}
}


//////////////////////////////
// meshFun.h
//////////////////////////////