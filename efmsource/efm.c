/* efm.c - 1/2016, Version 154a.  

Steven L. Garman
USGS Geosciences and Environmental Change Science Center
Denver Federal Center, Denver, CO.


This is the main routine of the Energy Footprint Model developed for SW Wyoming. In short, the model relies on
user-provided requirements for developing future oil/gas fields (project areas), referred to as build-out designs.  Designs
specify the oil/gas project areas of interest, nos. of wells, nos. of wells/pad, bottom-hole area
of each well by type (vertical, directional, horizontal), and begin year and duration of development.  Maps of the initial 
landscape delineates existing oil/gas pads and roads, areas where development is prohibited (lakes, reservoirs, 
urban centers,adiminstrative withdraws), and were develpoment is restricted (e.g., Greater sage-grouse core areas).
The model cyles through the specified project areas and establishes the specified annual allotment of wells for the specified
duration.  For each project area, the model first randomly selects PLSS sections for development, Oil/gas pads are then randomly selected
using digitized pads from SW WY, then pad location within the section is randomly selected but prohibited to overlap existing pads
and non-developable areas.  After pad establishment, a road is generated along the shortest, flattest path from the pad to 
an existing road.  Where multiple paths are possible, the path used is randomly selected.  Because this is a stochastic model,
Monte Carlo methods are used to forecast multiple possible trajectories of future oil/gas footprint patterns.  The
intented purpose of this model is to explore future alternatives to oil/gas development based on different combinations
of new drilling technologies (vertical, directional, horizontal wells), and to evaluate the implications on biophysical 
properties.  The latter is performed in post-processing procedures which are developed specific to the types of questions
being addressed and availability of response models (e.g., statistical models of the effects of infrastructure on 
pygmy rabbit habitat occupancy).

This is a hybrid raster, vector model.  Ingested maps are 30-m grids.  However, features such as oil/gas pads are ingested
as both 30-m grids and as pad-boundary coordinates in relativized UTM coordinates.  Pads are fitted on the landscape using
the 30-m representations, but are output using the boundary coordinates, adjusted to their actual location on the landscape. 
A virtual 10-m road grid is generated to increase the resolution of new roads; however, once a gridded road-path is determine it 
is translated into a vector where the UTM coordinates of end points and every 10-m center-line point is derived and output.  The primary
model output is pad and road location information which is translated to vector shapefiles using post-processing python scripts.
These shapefiles are combined with the baseline or initial landscape pad and road shapefiles, and used in the post-processing
procedures to evaluate biophysical impacts.


1) This main module initializes the many data structures with initial conditions, ingested as ASCII and Binary files.
Binary files are geospatial maps of landscape features.  ASCII files are typically tables containing
information about oil/gas pads and roads.  Binary formats are customized and created using a pre-processing routine called
crinput.exe.  Binary files contain 1 header (HEptr in binary.h), and n number of non-null entries (SCptr in binary.h), where the 
row and the column are specified along with the cell value.  These files are 'unwraped' internally and referenced by
a pointer (stored as a long piece of memory but accessed like a matrix using row and column).  ALL maps are georegistered to
the road ID layer in pre-processing procedures, so all have the same dimensions.

2) After initialization, this module manages the Monte Carlo replications over the specified duration (yrs) of simulations.  
Re-initializes internal structures, and in a few cases, re-reads input files within each Monte Carlo loop.
Simulated pads and roads are output in other routines.

Type emf ? to view the order of required information.
To run the model, type emf followed by the required input files (on a single line - see documentation for current inuput files
for SW Wyoming simulations).
*/



#include <stdio.h>
#include <math.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include "efm.h"
#include "geo.h"
#include "roads.h"
#include "padalloc.h"
#include "binary.h"
#include "fnames.h"
#include "padpat.h"
#include "plss.h"
#include "reduce.h"
#include "rdend.h"
#include "rdgrid.h"
#include "TBHA.h"
#include "curve.h"

FILE	*themap,*outmap;
int		row,col;
float	grain;
float	utme,utmn;
float	idum,jdum,kdum;



main(int argc, char *argv[])
{
	void	ReadRoadsB(int);
	void	ReadDEMB(char *);
	void	ReadPadsB();
	void	ReadSageB();
	void	ReadSGB();
	void	ReadFedMB(),ReadSurfB();
	void	ReadPadCords(),ReadPatterns();
	void	ReInit(),SimDevelop();
	void	ReadGInfo(char *),ReadSecPad(char *),ReadAUSecList(char *,char *);
	void	ReadLek(),OpenOutputFiles(),ReadLekGridB(char *);
	void	FillMod(char *),DataRecord(),ReadRdIDB(char *);
	void	DeriveBHA(),RecordBHA();
	void	ReadAreaPatchesB(char *);
	void	ReadPadInfo(char *);
	void	ReadFillR(char *,char *),ReadAnchorPts(int);
	void	UpdateAnchorPts();
	void	SDCoreArea();
	void	PrintCoreArea();
	void	ReadMSpace(char *),ReadMSect(char *),Perform(),ReadNonTraverseB(char *);
	void	DumpPads();
	void	DeActivateWells();


	void	Init();
	int		i,j,k,l,jkl;
	char	Valid(int,int);
	void	pause();
	void	PadRdOverlap(),RdGrid(),RdNodeUP();
	void	OutputSpec();
	void	PadShapefile(int,int,int,char *);
	void	RdShapefile(int,int,int);
	float	Ran2(float *);

	int		id,ret;

	if(strcmp(argv[1],"?")==0) {
		printf("emf <1 - Road-type grid> <2 - Road anchor pts>\n");
		printf("<3 - Road EndPts> <4 - Road ID grid>\n"); 
		printf("<5 - Pad patterns> <6 - Section pad/well info>\n");
		printf("<7 - Initial pad grid> <8-Pad Info list>\n");
		printf("<9 - Sagebrush map> <10-Nontraverse grid>\n");
		printf("<11 - Sage-grouse core area grid> <12-Fed mineral grid>\n"); 
		printf("<13 - Surface ownership> <14-Lek Pts> <15- DEM>\n");
		printf("<16 - the AU x Project section list> <17 - Build-out design>\n");
		printf("<18 - Synthetic combos file or none> <19 - Frequency of design specs or none>\n"); 
		printf("<20 - Section attributes> <21- Pad IDs by section>\n"); 
		printf("<22 - Pad boundary coords for the pad pattern file>\n");
		printf("<23 - GRTS points> <24 - Sections with higher development priority>\n");
		printf("<25 - Lek perimeter map> <26 - Area patch ID file>\n");
		printf("<27 - UTM vertices of initial roads> <28- Random no. seed>\n");
		printf("<29 - Starting rep number (>=1)> <30 - Ending rep number>\n");
		printf("<31 - yr interval to record pads/rds> <32 - Sim duration (yrs)>\n");
		printf("<33 - Prob. of reusing expired pads (fraction)>\n");
		printf("<34 - Section-weighting option; 1 2 3 or 4\n");
		exit(-1);
	}

/* NOTE - MAXRDEND (no. of incoming road IDs) is a global constant used to output RD IDs, in CheckRdActive, and used to initialize maxrdend */

	TIGHT=0; /*  mediates pad spacing within a section; =1 if spacing is moderated by pads of same AU x Proj; 
			 else spacing (the older standard method) is moderated by distance to any and all pads */
	CURVEACTIVE=0; /* reset in dopads along with TIGHT to flush final wells */

	Init(); 
	monte=atoi(argv[30]);
	duration=atoi(argv[32]);
	datarecord=atoi(argv[31]);
	reusepadsprob=atof(argv[33]);
	SECTselect=1.0;  
	STARTREP=atoi(argv[29]);
	strcpy(RDENDnam,argv[3]);  /* store name of rdendpts.in for use in output map */
	WEOPT=atoi(argv[34]);if(WEOPT==6)WEOPT=-1;  /* randomly select */
	strcpy(RDVERTSin,argv[27]);

	idum=atof(argv[28]);jdum=idum;kdum=idum;

/*	***************************************************************************************** */
	/* 3/2015 - Check  to see if we need to adjust MAXWEI=20000 */
	themap=fopen(argv[16],"r");
	fscanf(themap,"%d ",&i);fscanf(themap,"%d ",&i);fscanf(themap,"%d ",&i);
	for(j=1;j<=i;j++) {  /* just read the summaries for each au x proj combo - specified as i here */
		fscanf(themap,"%d %d %d\n",&jkl,&k,&l);
		if(l>MAXWEI) {printf("Need to increase MAXWEI %d %d\n",MAXWEI,l);MAXWEI=l;}
	}
	fclose(themap);

/*	*********************************************************************************************  */



	/* read & store pad patterns */
	themap=fopen(argv[5],"r");
	ReadPatterns();	/* has to occur before readpadcords() */
	/* read and store the pad-pattern vertices [patbnd.cor] */
	themap=fopen(argv[22],"r");
	ReadPadCords();



	strcpy(theroads,argv[1]);
	themap=fopen(argv[1],"rb");
	ReadRoadsB( (int)1);			/* roads contains 1-8 for the different road codes */
	ha=(grain*grain)/10000.0;

	/* place here cause maxcnt isn't set until ReadRoadsB!! */
	/* read nontraverse info */
	ReadNonTraverseB(argv[10]);


/* read masterspace.in. */
	ReadMSpace(argv[23]); /* this is where maxsec is set */

	/* read secstr3.in */
	ReadMSect(argv[6]);

	/* read general section info (neighbors, federal, prob, ha1,ha3,nlha,propnl - has to come after ReadMSect() */
	ReadGInfo(argv[20]);

	/* read section pad list */
	ReadSecPad(argv[21]);

	/* read auseclist.in info */
	ReadAUSecList(argv[16],argv[18]);  /* argv[18] is the name of the combos file, OR none */

	themap=fopen(argv[2],"r");
	ReadAnchorPts((int)1);				/* initialize road anchor pt storage  */
	themap=fopen(argv[2],"r");
	ReadAnchorPts((int)2);				 /* store the road anchor pt info */



	ReadDEMB(argv[15]);							/* reads dem.b */


	/* read pads and set padnum - master no. of pads */
	strcpy(thepads,argv[7]);
	themap=fopen(argv[7],"rb");
	ReadPadsB();  

	/* themap=fopen(argv[8],"r"); */	/* read pad info */
	ReadPadInfo(argv[8]);


	/* where we deal with the 10-m grid and setting RDNptr and PADNptr structs */
	RdGrid();
	PadRdOverlap();


	/* read/set sage map */
	themap=fopen(argv[9],"rb");
	ReadSageB();



	/* read/set sagegrouse core area map */
	SGcore=(short *) calloc(maxcnt,sizeof(short));
	if(SGcore==NULL) {printf("ERROR, SGcore==NULL\n");exit(-1);}
	themap=fopen(argv[11],"rb");
	ReadSGB();

	/* read/set federal mineral rights */
	fedmin=(char *) calloc(maxcnt,sizeof(char));
	if(fedmin==NULL) {printf("ERROR, fedmin==NULL\n");exit(-1);}
	themap=fopen(argv[12],"rb");
	ReadFedMB();

	/* read/set surface ownerships - see document for codeing - may differ among projects */
	surf=(char *) calloc(maxcnt,sizeof(char));
	if(surf==NULL) {printf("ERROR, surf==NULL\n");exit(-1);}
	themap=fopen(argv[13],"rb");
	ReadSurfB();

	/* read/set lek pts */
	lek=(int *) calloc(maxcnt,sizeof(int));
	if(lek==NULL) {printf("ERROR, lek==NULL\n");exit(-1);}
	themap=fopen(argv[14],"r");
	ReadLek();
	/* read lek grid */
	ReadLekGridB(argv[25]);

	/* Determine & record existing surface disturbance in SG Core Areas */
	SDCoreArea();


/* read & set infill rates */
	ReadFillR(argv[17],argv[18]);
	FillMod(argv[19]);


	/* read the road IDs */
	strcpy(theroadsid,argv[4]);
	ReadRdIDB(theroadsid);

	reducetrue=0;rfactor=1.0; /* obsolete, but retain init of global vars that deactivates processing of automatic reduction of
							  well numbers. */
	reducetrueBHA=0;


	/* temp place to try out priority sections */
	PRIOR= (char *) calloc(maxsec+1,sizeof(char));
	themap=fopen(argv[24],"r");
	ret=255;
	while(ret!=EOF) {
		ret=fscanf(themap,"%d\n",&id);
		if(ret!=EOF) {
			PRIOR[id]=1;
		}
	}
	fclose(themap);

	 ReadAreaPatchesB(argv[26]);   /* read pid.b */

	/* allocate the new BHA tracking struct */
	TrackBptr=(TRACK *) calloc(AUSptr->auproj+1,sizeof(TRACK));

	printf("Initialized\n");

	/* ************************************************************************************************ /

	/* repnumber=monte; */
	for(repnumber=STARTREP;repnumber<=monte;repnumber++) {
		/* rep should start here */
		if(repnumber>STARTREP)ReInit();   /* For the first rep, init is done above.  After reps, need to Reinit() */
		OpenOutputFiles();
		DeriveBHA();


		time=2012;				/* used to set begin yr of pads */
		Perform();				/* records initial wells, pads=0, and BH */


		printf("Padnum at time 0 = %d\n",padnum);


		for(i=1;i<=duration;i++) { 
			loop=i;

			/* initialize temp road storage */
			anchor=fopen("tanc","w");
			newanchors=0;

			time++;
			printf("yr = %d %d   rep#= %d \n",i,time,repnumber);

			/* TEMPRptr is temp storage of road anchor pts established within a time step */
			TEMPRptr = (TEMPRD *) calloc(maxcnt,sizeof(TEMPRD));
			if(TEMPRptr==NULL) {printf,"ERROR, TEMPRD == NULL\n",exit(-1);}


			/* establish pads & roads */
			/* if WEOPT is a genative number then we are reandomly selecting the
			weighting method annually. */
			if(WEOPT<0) {
				WEOPT=(int)(Ran2(&idum)*5);
				if(WEOPT==5) WEOPT=4;
				WEOPT=WEOPT*(-1);
				if(WEOPT==0)WEOPT=-1;
			}
			 SimDevelop(); 

			free(TEMPRptr);


			UpdateAnchorPts();		/* all roads have been generated, update the anchor pt record.
									This also updates core-area disturbance after ensuring 
									only 1 vertices per cell.  */
	



			/* printf("rfdstats\n"); 
			RfdStats();			tallies by RFD */
			Perform();			/* tallies of no. wells, pads, bhole remaining per auxproj each time step */


			PrintCoreArea();			/* print esp. proportion of disturbed area */

			DeActivateWells();   /* set mrecord for info about pads and well totals and do deactivation */
			RecordBHA();  /* record BHA consumption */

			/* dump info if time */
			if(datarecord>0) {
				if( loop/datarecord * datarecord == loop){
								/* The following needs to happen before deactivating rds. */
					free(RDNptr);  /* this is set as part of init so it will not be NULL */
					RdNodeUP();    /* in RdGridUp() */
					free(PADRptr); /* this is set as part of init so it will not be NULL */
					PadRdOverlap();      /* use the exact same module */

					DataRecord(); /* in this we flag roads as inactive & check to see
															      if deactivated roads should be activated */
				    DumpPads();  /* output info about pads and wells on a pad */
				}
			}
		} /* for i; END OF DURATION */

			OutputSpec();  /* record to specs# */
		
			/* output tfill frequency */
			fprintf(filetfill,"%d %d %d %d\n",TFILL[1],TFILL[2],TFILL[3],TFILL[4]);


			/* close files */
			fclose(linesf);			/* DrawSLine & DrawSLineP*/ /* coordinates of new road lines */
			fclose(padpatID);		/* Fill/PadSize - records pattern ID and centroid utms of newly created pads */
			fclose(sumrates);		/* records summary of pads auxproj - output in Perform() () */
			fclose(sumratesp);
			fclose(dumppads);		/* records pad and active well info */
			fclose(mrecord);        /* records active pads/wells before, after deactivation */
			fclose(ratef);			/* stores rate of establishment info - used in Perform */
			fclose(croads);			/* time-interval rdlines */
			fclose(endpts);			/* appended file containing rd end pts */
			fclose(outspecs);		/* specs */
			fclose(fileBH);			/* BHA tracking */

			/* Create pad and road shapefiles */
			PadShapefile(repnumber,duration,datarecord,argv[22]);
			RdShapefile(repnumber,duration,datarecord);

		} /* end of repnumber */





}


char	FindInactive(int id)
{
	int		i;

	for(i=1;i<=padinactive;i++) {
/*		if(inactivelist[i]==id)return( (char)1); */
	}
	return( (char)0);
}