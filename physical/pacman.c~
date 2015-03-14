/*******************************
* 1. receive position of ghost
* 2. move escape route 
* 3. if got caught.. game over
* 4. else continue escaping
*******************************/
#include <stdio.h>
#include <libplayerc/playerc.h>
#include <math.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3

#define MAX_SPEED 0.40
#define MAX_TURN 0.10 
#define PI M_PI
#define TWO_PI 6.28318530718

#define PACPORT 1034

typedef struct camPosition_s
{
	double x;
	double y;
	double yaw;
	double other_x;
	double other_y;
	double other_yaw;
}camPosition_t;

int boundary_check();

playerc_client_t *client;
playerc_position2d_t *position2d;
camPosition_t pacPos;
int sd, nsd;

/****************************************
 * escape algorithm 
 * move away from the ghost 
 * using corner based approach:
 * Finding the longest-path for 
 * ghost to reach different positions and which takes
 * shortest move to reach that position...
 ***************************************/

 int return_secs() {
    time_t now;
    struct tm *tm;

    now = time(0);
    if ((tm = localtime (&now)) == NULL) {
        printf ("Error extracting time stuff\n");
        return 1;
    }

    printf ("Time in seconds : %02d\n", tm->tm_sec);

    return tm->tm_sec;
}




void
escape(int px, int py, int gx, int gy, int direction, double *next_x, double *next_y)
{		
	int mark_x, mark_y, mark;

	//printf("In escape......................\n");

	// determine which corner should pacman toward to
	if (gx < 2 || (gx == 2 && px > 2))
		mark_x = 4;
	else if (gx > 2 || (gx == 2 && px < 2))
		mark_x = 0;
	else
		mark_x = (direction == LEFT ? 0 : 4);

	if (gy < 2 || (gy == 2 && py > 2))
		mark_y = 4;
	else if (gy > 2 || (gy == 2 && py < 2))
		mark_y = 0;
	else
		mark_y = (direction == UP ? 0 : 4);

	
	if (mark_x < px && py != 1 && py != 3) 		// can move left
	{
		*next_x = px - 1;	// pre move left
  		mark = -1;
	}	
	else if (mark_x > px && py != 1 && py != 3) 	// can move right
	{
		*next_x = px + 1;	// pre move right
		mark = 1;
	}	
	else						// can not move left or right
		*next_x = px;

	if (mark_y < py && px != 1 && px != 3) 		// can move up
	{
		if (mark == 0 || direction == UP)
		{
			*next_x = px;		// correct x axis movement		
			*next_y = py - 1;	// move up			
		}		
		else
			*next_y = py;		// do not move up	
	}
	else if (mark_y > py && px != 1 && px != 3) 	// can move down
	{
		if (mark == 0 || direction == DOWN)
		{
			*next_x = px;		// correct x axis movement
			*next_y = py + 1;	// move down	
		}
		else
			*next_y = py;		// do not move down
	}
	else						// can not move up or down
		*next_y = py;
	
	
	// Boundary Check for pacman start
	
	if (px == 4 && py == 0 && direction == RIGHT)			// Upper-right corner
	{
		*next_x = px;		// move - down
		*next_y = py + 1;
	}	
	
	if (px == 4 && py == 0 && direction == UP)
	{
		*next_x = px - 1;		// move - left
		mark = -1;
	}
	

	if (px == 0 && py == 0 && direction == UP)		// Upper-left corner
	{

		*next_x = px + 1;		// move - right
		mark = 1;
	}
	if (px == 0 && py == 0 && direction == LEFT)
	{
		*next_x = px;		// move - down
		*next_y = py + 1;
	}
		

	if (px == 0 && py == 4 && direction == DOWN)		// Lower-left corner
	{
		*next_x = px + 1;		// move - left
		mark = -1;
	}
	if (px == 0 && py == 4 && direction == LEFT)
	{
		*next_x = px;		// move - up
		*next_y = py - 1;
	}


	if (px = 4 && py == 4 && direction == RIGHT)		// Lower-right corner
	{
		*next_x = px;		// move - up
		*next_y = py - 1;
	}
	if (px = 4 && py == 4 && direction == DOWN)
	{	
		*next_x = px - 1;		// move - right
		mark = 1;
	}
	
	// Boundary check end
}

/****************************************
 * get pacman's direction
 ***************************************/
int
getDirection(double angle)
{
	if (angle >= 45 && angle <= 135)
		return UP;
	else if (angle <= -45 && angle >= -135)
		return DOWN;
	else if (angle > -45 && angle < 45)
		return RIGHT;
	else
		return LEFT;	
}

void stop()
    {
    playerc_position2d_set_cmd_vel(position2d, 0.0, 0.0, 0.0, 1);
    }


void limit_ang(double *angle)
{
    while (*angle > PI)
        *angle -= TWO_PI;
    while (*angle < -PI)
        *angle += TWO_PI;
}


int
turnLeft (float ang_deg)
    {
    float cx, cy, ca;
    float ix, iy, ia;
    float leftToTurn;
    float turnedSoFar;

    if((recv(nsd,(char *)&pacPos,sizeof(pacPos),0))==-1)
	{
		perror("recv");
		return(1);
	}
    ia=ca=pacPos.yaw;
    leftToTurn=ang_deg*M_PI/180.0;
    fprintf(stderr,"turning LEFT \n");

    //while ( leftToTurn> 0)
    while ( fabs(leftToTurn)> DTOR(5))
        {

        //printf("turning.....\n");
		playerc_position2d_set_cmd_vel(position2d, 0, 0.0, MAX_TURN,1);
        if((recv(nsd,(char *)&pacPos,sizeof(pacPos),0))==-1)
		{
			perror("recv");
			return(1);
		}
        ca=pacPos.yaw;
        turnedSoFar=fabs(ia-ca);//need to check this for rollover****
        leftToTurn = ang_deg*M_PI/180.0 - turnedSoFar;
        }
    stop();
    }


int
turnRight (float ang_deg)
    {
    float cx, cy, ca;
    float ix, iy, ia;
    float leftToTurn;
    float turnedSoFar;
	

    playerc_client_read(client);
	usleep(3000);
    ia=ca=position2d->pa;
    leftToTurn=ang_deg*M_PI/180.0;

    fprintf(stderr,"Turning Right \n");

    //while ( leftToTurn> 0)
    while ( leftToTurn> DTOR(2))
        {
        playerc_position2d_set_cmd_vel(position2d, 0, 0.0, -1*MAX_TURN,1);
        playerc_client_read(client);
		usleep(3000);
        ca=position2d->pa;
        turnedSoFar=fabs(ia-ca);//need to check this for rollover****
        leftToTurn = ang_deg*M_PI/180.0 - turnedSoFar;
        }
    stop();
	}


/***************************
 * goFwd function
 ****************************/
int 
goFwd(int t)
	{

	float cx, cy, ca;
    float ix, iy, ia;
    float new_dist;
	double x,y,yaw;
	struct timeval ctv, ltv;

	double px,py,gx,gy;
	int pa, ga;

	px = ((pacPos.x - 90)/88);
	py = ((pacPos.y - 35)/85);
	gx = ((pacPos.other_x - 90)/88);
	gy = ((pacPos.other_y - 35)/85);

	pa = getDirection(RTOD(pacPos.yaw));
    ga = getDirection(RTOD(pacPos.other_yaw));


   if((recv(nsd,(char *)&pacPos,sizeof(pacPos),0))==-1)
	{
		perror("recv");
		return(1);
	}


    ix=cx=pacPos.x;
    iy=cy=pacPos.y;
    ia=ca=pacPos.yaw;
    new_dist=dist_mm;

    fprintf(stderr,"Going Forward............. \n");

	while ( new_dist> 0)
    {
        playerc_position2d_set_cmd_vel(position2d,
        new_dist > MAX_SPEED*1000?MAX_SPEED:MAX_SPEED/2, 0.0, 0.0, 1);
        if((recv(nsd,(char *)&pacPos,sizeof(pacPos),0))==-1)
		{
			perror("recv");
			return(1);
		}
        new_dist = dist_mm - sqrt((ix-pacPos.x)*(ix-pacPos.x)+(iy-pacPos.y)*(iy-pacPos.y));

        px = ((pacPos.x - 90)/88);
		py = ((pacPos.y - 35)/85);
		gx = ((pacPos.other_x - 90)/88);
		gy = ((pacPos.other_y - 35)/85);

		pa = getDirection(RTOD(pacPos.yaw));
	    ga = getDirection(RTOD(pacPos.other_yaw));


        printf(" px = %lf \t py= %lf \t p_direction = %d\n",px,py,pa );
		//printf(" gx = %lf \t gy= %lf \t g_direction = %d\n",gx,gy,ga );

		boundary_check();
    }

    stop();
    return 0;
	
}


int boundary_check()
{

	printf("In boundary_check\n");

	if((recv(nsd,(char *)&pacPos,sizeof(pacPos),0))==-1)
	{
		perror("recv");
		return(1);
	}

	double px,py,gx,gy;
	int pa, ga;

	px = ((pacPos.x - 90)/88);
	py = ((pacPos.y - 35)/85);
	gx = ((pacPos.other_x - 90)/88);
	gy = ((pacPos.other_y - 35)/85);

	pa = getDirection(RTOD(pacPos.yaw));
    ga = getDirection(RTOD(pacPos.other_yaw));

    printf(" px = %lf \t py= %lf \t p_direction = %d\n",px,py,pa );
	//printf(" gx = %lf \t gy= %lf \t g_direction = %d\n",gx,gy,ga );

	if (px <= 0.7 && py <= 0.6)
	{
		if (pa == LEFT)
		{
			turnLeft(90.0);
		}
		else
			if (pa == UP && ga == LEFT)
			{
				turnLeft(90.0);
			}
	}

	else
		if (px >= 4.2 && py <= 0.9)
		{
			if (pa == RIGHT)
			{
				turnLeft(90.0);
			}
			else
				if(pa == UP)
				{
					turnLeft(90.0);
				}

		}

		else
			if (px <= 0.7 && py >= 4.2)
			{
				if (pa == DOWN)
				{
					turnLeft(90.0);
				}
				else
					if (pa == LEFT)
					{
						turnRight(90.0);
					}
				else
					if (pa == RIGHT)
					{
						turnLeft(90.0);
					}
						
			}

	else
		if (px >= 4.3 && py >=4.3)
		{
			if (pa == RIGHT)
			{
				turnLeft(90.0);
			}
			else
				if (pa == DOWN)
				{
					turnRight(90.0);
				}
		}

	else
		if (py <= 0.5 && pa == UP)
		{
			if (ga == LEFT)
			{
				turnLeft(90.0);
			}
			else
				if (ga == RIGHT)
				{
					turnRight(90.0);
				}
				else
					turnRight(90.0);
		}
		else
			if (py > 4.3 && pa == DOWN)
			{
				if (ga == LEFT)
				{
					turnRight(90.0);
				}
				else
				if (ga == RIGHT)
					{
						turnLeft(90.0);
					}	
			}
		else
			if (px <= 0.5 && pa == LEFT)
			{
				if (ga == DOWN)
				{
					turnLeft(90.0);
				}
				else
					if (ga == UP)
					{
						turnRight(90.0);
					}

					else
						if (ga == LEFT)
						{
							turnLeft(90.0);
						}

						else
							turnRight(90.0);

			}

			else
				if (px == 2.500000 && py== 2.500000)
				{
					if (gx == 4.375000 && gy == 2.500000 && (pa == RIGHT || pa == UP)) 
					{
						turnLeft(180.0);
					}

					else
						if (gx < 0.8 && gy >4.0 && pa == RIGHT)
						{
							turnLeft(90.0);
						}

					
				}

			else
				if (px >= 4.3 && pa == RIGHT)
				{
					if (ga == DOWN)
					{
						turnRight(90.0);
					}
					else
						if (ga == UP)
						{
							turnLeft(90.0);
						}
				}

				else
					if (px < 2.7 && py < 0.4 && gx < 0.9 && gy < 0.9)
					{
						turnLeft(90.0);
					}
					else
						return 0;

	return 0;
}






/***********************************
 * driving function
 ***********************************/

int
drive(double nextX, double nextY)
	{
	float dx, dy, dq_degrees, dist;
	double dq;
    int i;
    int index_px, index_py;

    	//printf("In Drive.....\n");
		if((recv(nsd,(char *)&pacPos,sizeof(pacPos),0))==-1)
		{
		perror("recv");
		return(1);
		}

		dx = nextX - pacPos.x;
		dy = -1*(nextY - (pacPos.y));
		dist = sqrt(dx * dx + dy * dy) ;
		dq=atan2(dy, dx);
		dq -= (pacPos.yaw);
		limit_ang(&dq);
		dq_degrees = dq / PI * 180.0;

		//printf("simX %f \t nextX %f\n", simX, nextX );
        //printf("simY %f \t nextY %f\n", simY, nextY );
        //printf("dq_degrees %f \t Distance %f\n", dq_degrees, dist);

        
		if (dq > 0.0)
			//turnLeft (dq_degrees);
			turnLeft (dq_degrees);
		else if (dq < 0.0)
			//turnRight (-dq_degrees);
			turnRight (-dq_degrees);

		goFwd (dist);
    }




/****************************************
 * major function for pacman
 ***************************************/
int
run(double px, double py, double pa, double gx, double gy, double ga)
{
    double dist, angle;
	double index_gx, index_gy, index_px, index_py;
	int p_direction, g_direction;
	double next_x, next_y;
	int next_index_x, next_index_y;

	printf("In Run.......\n");
	// 1st step: get block info for pacman and ghost
	index_px = ((px - 90)/88);
	index_py = ((py - 35)/85);
	index_gx = ((gx - 90)/88);
	index_gy = ((gy - 35)/85);

	p_direction = getDirection(RTOD(pa));
	g_direction = getDirection(RTOD(ga));

	//printf("Direction \t%d \n",p_direction );

	printf(" px = %lf \t py= %lf \t p_direction = %d\n",index_px,index_py,p_direction );
	//printf(" gx = %lf \t gy= %lf \t g_direction = %d\n",index_gx,index_gy,g_direction );
	
	boundary_check();


	// 2nd step: get next movement for pacman
	escape(index_px, index_py, index_gx, index_gy, p_direction, &next_x, &next_y);

	next_index_x = next_x;
	next_index_y = next_y;


	next_x = 130 + (90 * next_x);
    next_y = 50 + (90 * next_y);


	fprintf(stderr,"next_index: %d  %d ,  index:  %lf  %lf \n",
	next_index_x, next_index_y, index_px, index_py);

	fprintf(stderr,"next: %3.2f   %3.2f\n", next_x, next_y);

	if (next_index_x != index_px)
		drive(next_x, next_y);
	if (next_index_y != index_py)
		drive(next_x, next_y);
	if ((next_index_x == index_px))
	{
		escape(index_px, index_py, index_gx, index_gy, p_direction, &next_x, &next_y);
		return 0;	
	}

	return 0;

}



int
main(int argc, const char **argv)
{
	struct sockaddr_in server, camClient;
	int yes = 1;
	socklen_t length;

	int i;
	
	double pacmanX, pacmanY, pacmanA;
	double ghostX, ghostY, ghostA;
	double turn, speed;

	//Create a client and connect it to the server.
	client = playerc_client_create(NULL, "localhost", 6665);
	if (0 != playerc_client_connect(client))
		return -1;

	// Create and subscribe to a position2d device.
	position2d = playerc_position2d_create(client, 0);
	if (playerc_position2d_subscribe(position2d, PLAYER_OPEN_MODE))
		return -1;


	if((sd=socket(PF_INET,SOCK_STREAM,0))==-1)
	{
		perror("socket");
		return(1);
	}

	server.sin_port=htons(PACPORT);
	server.sin_family=PF_INET;
	server.sin_addr.s_addr=inet_addr("127.0.0.1");

	if(setsockopt(sd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int))==-1)
	{
		perror("setsockopt");
		return(1);
	}

	if(bind(sd,(struct sockaddr  *)&server,sizeof(server))==-1)
	{
		perror("bind");
		return(1);
	}

	 if(listen(sd,8)==-1)
	{
		perror("listen");
		return(1);
	}

	printf("Waiting for connection.............\n");

	length = sizeof(camClient);
	if((nsd=accept(sd,(struct sockaddr *)&camClient,&length))==-1)
	{
		perror("accept");
		return(1);
	}

	printf("Got connection from client: %s \n",
	inet_ntoa(camClient.sin_addr));

	
	for (;;)
	{
		//usleep(9000);
		if((recv(nsd,(char*)&pacPos,sizeof(pacPos),0))==-1)
		{
			perror("recv");
			return(1);
		}

		//printf("pacman = ( %lf, %lf, %lf) \t ghost = ( %lf, %lf, %lf)\n",pacPos.x,pacPos.y,pacPos.yaw,pacPos.other_x,pacPos.other_y, pacPos.other_yaw );
		run(pacPos.x, pacPos.y, pacPos.yaw, pacPos.other_x, pacPos.other_y, pacPos.other_yaw);
	}

	// Shutdown
	
	playerc_position2d_unsubscribe(position2d);
	playerc_position2d_destroy(position2d);
	playerc_client_disconnect(client);
	playerc_client_destroy(client);

	return 0;
}
