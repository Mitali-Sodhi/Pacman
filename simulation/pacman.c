/*******************************
* 1. receive position of ghost
* 2. move escape route
* 3. if got caught.. game over
* 4. else continue escaping
*******************************/
#include <stdio.h>
#include <libplayerc/playerc.h>
#include <math.h>

#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3

#define MAX_SPEED 1.0
#define MAX_TURN 0.08
#define PI M_PI
#define TWO_PI 6.28318530718

int boundary_check();



playerc_simulation_t *sim;
playerc_position2d_t *position2d;

/****************************************
 * escape algorithm
 * move away from the ghost
 * using corner based approach:
 * Finding the longest-path for
 * ghost to reach different positions and which takes
 * shortest move to reach that position...
 ***************************************/


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


void
turnLeft (float ang_deg)
    {
    float cx, cy, ca;
    float ix, iy, ia;
    float leftToTurn;
    float turnedSoFar;
	double simX, simY, simA;

    playerc_simulation_get_pose2d(sim,(char *)"pacman",&simX, &simY, &simA  );
	usleep(3000);
    ia=ca=simA;
    leftToTurn=ang_deg*M_PI/180.0;
    fprintf(stderr,"turning LEFT \n");

    //while ( leftToTurn> 0)
    while ( leftToTurn> DTOR(2))
        {

        //printf("turning.....\n");
		playerc_position2d_set_cmd_vel(position2d, 0, 0.0, MAX_TURN,1);
        //playerc_client_read(client);
        playerc_simulation_get_pose2d(sim,(char *)"pacman",&simX, &simY, &simA  );
		usleep(3000);
        ca=simA;
        turnedSoFar=fabs(ia-ca);//need to check this for rollover****
        leftToTurn = ang_deg*M_PI/180.0 - turnedSoFar;
        }
    stop();
    }


void
turnRight (float ang_deg)
    {
    float cx, cy, ca;
    float ix, iy, ia;
    float leftToTurn;
    float turnedSoFar;
	double simX, simY, simA;

	//printf("In Turning Right\n");

    playerc_simulation_get_pose2d(sim,"pacman",&simX, &simY, &simA  );
	usleep(3000);
    ia=ca=simA;
    leftToTurn=ang_deg*M_PI/180.0;

    fprintf(stderr,"turning Right \n");

    //playerc_position2d_set_cmd_vel(position2d, 0, 0.0, DTOR(ang_deg), 0);

    //while ( leftToTurn> 0)
    while ( leftToTurn> DTOR(2))
        {
        playerc_position2d_set_cmd_vel(position2d, 0, 0.0, -1*MAX_TURN,1);
        playerc_simulation_get_pose2d(sim,(char *)"pacman",&simX, &simY, &simA  );
		usleep(3000);
        ca=simA;
        turnedSoFar=fabs(ia-ca);//need to check this for rollover****
        leftToTurn = ang_deg*M_PI/180.0 - turnedSoFar;
        }
    stop();
	}

void limit_ang(float *angle)
    {
    while (*angle > PI)
        *angle -= TWO_PI;
    while (*angle < -PI)
        *angle += TWO_PI;
    }


 int game_over()
 {
 	double simX, simY, simA;
	double simgX, simgY, simgA;
	playerc_simulation_get_pose2d(sim, (char *)"pacman",&simX,&simY,&simA);
    playerc_simulation_get_pose2d(sim, (char *)"ghost1",&simgX,&simgY,&simgA);
    printf("pacman = %lf \t %lf\n",simX, simY);
    printf("ghost = %lf \t %lf\n", simgX, simgY );
 }

/***************************
 * goFwd function
 ****************************/
int
goFwd(double dist_mm)
	{

	float cx, cy, ca;
    float ix, iy, ia;
    float gx, gy, ga;
    float new_dist;
	double simX, simY, simA;
	double simgX, simgY, simgA;

	double index_px, index_py, index_gx, index_gy;
	int p_direction, g_direction;

    playerc_simulation_get_pose2d(sim, (char *)"pacman",&simX,&simY,&simA);
    playerc_simulation_get_pose2d(sim, (char *)"ghost1",&simgX,&simgY,&simgA);
	usleep(300);
    ix=cx=simX;
    iy=cy=simY;
    ia=ca=simA;

    gx = simgX;
    gy = simgY;
    ga = simgA;


    index_px = ((8 + ix)/3.2);
	index_py = ((8 - iy)/3.2);
	index_gx = ((8 + gx)/3.2);
	index_gy = ((8 - gy)/3.2);
	p_direction = getDirection(RTOD(ia));
	g_direction = getDirection(RTOD(ga));

	//printf("Direction \t%d \n",p_direction );

    double speed=0.0, turn=0.0;

    fprintf(stderr,"Going Forward............. \n");

    int dir;

    new_dist = dist_mm;


	while ( new_dist> 5)
        {
        //printf("Distance = %lf\n", dist_mm);
        //playerc_client_read(client);
        playerc_position2d_set_cmd_vel(position2d,
        new_dist > MAX_SPEED*1000?MAX_SPEED:new_dist/500, 0.0, 0.0, 1);
        //playerc_client_read(client);
        playerc_simulation_get_pose2d(sim,(char *)"pacman",&simX,&simY,&simA);
        playerc_simulation_get_pose2d(sim, (char *)"ghost1",&simgX,&simgY,&simgA);
		usleep(300);
        cx = simX;
        cy = simY;
        ca = simA;
        gx = simgX;
        gy = simgY;
        ga = simgA;
        new_dist = dist_mm - sqrt((ix-cx)*(ix-cx)+(iy-cy)*(iy-cy))*1000;
        index_px = ((8 + cx)/3.2);
		index_py = ((8 - cy)/3.2);
		index_gx = ((8 + gx)/3.2);
		index_gy = ((8 - gy)/3.2);
		p_direction = getDirection(RTOD(ca));
		g_direction = getDirection(RTOD(ga));

        boundary_check();
        //game_over();
        }
    stop();
    return 0;

}


int boundary_check()
{

	printf("In boundary_check\n");

	//printf("px = %lf \t py= %lf \t pa =%d \t gx = %lf \t gy= %lf \t ga =%d\n",px,py,pa,gx,gy,ga);


	//usleep(4000);

	double simpX, simpY, simpA;
	double simgX, simgY, simgA;

	double px, py;
	double gx, gy;

	int pa, ga;

	playerc_simulation_get_pose2d(sim,(char *)"pacman",&simpX,&simpY,&simpA);
	playerc_simulation_get_pose2d(sim, (char *)"ghost1",&simgX,&simgY,&simgA);

	px = ((8 + simpX)/3.2);
	py = ((8 - simpY)/3.2);
	gx = ((8 + simgX)/3.2);
	gy = ((8 - simgY)/3.2);

	pa = getDirection(RTOD(simpA));
    ga = getDirection(RTOD(simgA));

    printf("px = %lf \t py= %lf \t pa =%d\n",px,py,pa );

    if (simpX == -7.5 && simpY == 0.0 && pa == RIGHT)
			{
				turnLeft(90.0);
			}

	else
		if (px <= 0.2 && py <= 1.0)
		{
			printf("Here1\n");
			if (pa == LEFT)
			{
				turnLeft(90.0);
			}
			else
				if (pa == UP && ga == LEFT)
				{
				turnLeft(90.0);
				}
			else
				turnRight(90.0);
		}

	else
		if (px >= 4.7 && py <= 0.8)
		{
			printf("Here2\n");
			if (pa == RIGHT)
			{
				turnRight(90.0);
			}
			else
				if(pa == UP)
				{
					turnLeft(90.0);
				}

		}

		else
			if (px <= 0.1 && py >= 4.0)
			{
				printf("Here3\n");
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
		if (px >= 4.5 && py >=2.5)
		{
			printf("Here4\n");
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
		if (px >= 2.9 && py >=4.0)
		{
			printf("Here5\n");
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
		if (px <= 3.2 && py > 2.3 && py < 2.7)
		{
			printf("Here6\n");
			if (pa == LEFT)
			{
				turnLeft(90.0);
			}
			// else
			// 	if (pa == UP &&)
			// 	{
			// 		turnRight(90.0);
			// 	}
				//else
					//turnRight(90.0);
		}

		else
			if (py < 1.0 && pa == UP)
			{
				printf("here7\n");
				if (ga == LEFT)
				{
					turnLeft(90.0);
				}
				else
				if (ga == RIGHT)
					{
						turnRight(90.0);
					}
			}

/*		else
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
						//if (ga == UP)
						{
							turnLeft(90.0);
						}
				}

				else
					if (px < 2.7 && py < 0.4 && gx < 0.9 && gy < 0.9)
					{
						turnLeft(90.0);
					}
*/
				//else
					//if (pa == RIGHT)
					//	turnLeft(90.0);
				else
					return 0;

	//goFwd(100.0);

	return 0;
}






/***********************************
 * driving function
 ***********************************/

int
drive(double nextX, double nextY)
	{
	float dx, dy, dq_degrees, dist,dq;
	double simX, simY, simA;
    int i;

    	//printf("In Drive.....\n");
		playerc_simulation_get_pose2d(sim,(char *)"pacman",&simX, &simY, &simA  );
		usleep(3000);
		dx = (nextX - simX)*1000;
		dy = (nextY - simY)*1000;
		dist = sqrt(dx * dx + dy * dy) ;
		dq=atan2(dy, dx);
		dq -= simA;
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
run(double px, double py, double pa, double gx, double gy, double ga, double *speed, double *turn)
{
    	double dist, angle;
	double index_gx, index_gy, index_px, index_py;
	int p_direction, g_direction;
	double next_x, next_y;
	int next_index_x, next_index_y;

	//printf("In Run.......\n");
	// 1st step: get block info for pacman and ghost
	index_px = ((8 + px)/3.2);
	index_py = ((8 - py)/3.2);
	index_gx = ((8 + gx)/3.2);
	index_gy = ((8 - gy)/3.2);
	p_direction = getDirection(RTOD(pa));
	g_direction = getDirection(RTOD(ga));

	printf("Direction \t%d \n",p_direction );

	printf("\n\n px = %lf \t py= %lf \t p_direction = %d\n",index_px,index_py,p_direction );

	boundary_check();


	// 2nd step: get next movement for pacman
	escape(index_px, index_py, index_gx, index_gy, p_direction, &next_x, &next_y);

	next_index_x = next_x;
	next_index_y = next_y;


	next_x = -6.4 + 3.2 * next_x;
	next_y = 6.4 - 3.2 * next_y;

	//fprintf(stderr,"next_index: %d  %d ,  index:  %lf  %lf \n",
	//next_index_x, next_index_y, index_px, index_py);

	fprintf(stderr,"next: %3.2f   %3.2f\n", next_x, next_y);

	// changed by madhavRao
	if (next_index_x != index_px)
		drive(next_x, next_y);
	if (next_index_y != index_py)
		drive(next_x, next_y);
	if ((next_index_x == index_px))
	{
		//turnRight(90.0);
		escape(index_px, index_py, index_gx, index_gy, p_direction, &next_x, &next_y);
		return 0;
	}

	return 0;

}



int
main(int argc, const char **argv)
{
	int i;
	playerc_client_t *client;
	double pacmanX, pacmanY, pacmanA;
	double ghostX, ghostY, ghostA;
	double turn, speed;

	// Create a client and connect it to the server.
	client = playerc_client_create(NULL, "localhost", 6665);
	if (0 != playerc_client_connect(client))
		return -1;

	// Create and subscribe to a position2d device.
	position2d = playerc_position2d_create(client, 0);
	if (playerc_position2d_subscribe(position2d, PLAYER_OPEN_MODE))
		return -1;

	// Create and subscribe to a simulation interface device.
	sim = playerc_simulation_create(client, 0);
	if (playerc_simulation_subscribe(sim, PLAYER_OPEN_MODE))
		return -1;

	for (;;)
	{
		// Wait for new data from server
		playerc_client_read(client);
		// Read pacman's & ghost's global pose via simulation interface
		playerc_simulation_get_pose2d(sim, (char *)"pacman", &pacmanX, &pacmanY, &pacmanA);
		playerc_simulation_get_pose2d(sim, (char *)"ghost1", &ghostX, &ghostY, &ghostA);

		run(pacmanX, pacmanY, pacmanA, ghostX, ghostY, ghostA, &speed, &turn);
		// turnRight(90);
		// turnLeft(90);

		//game_over()
		playerc_position2d_set_cmd_vel(position2d, speed, 0, 0,1);
			//return -1;

	}

	// Shutdown
	playerc_simulation_unsubscribe(sim);
	playerc_simulation_destroy(sim);
	playerc_position2d_unsubscribe(position2d);
	playerc_position2d_destroy(position2d);
	playerc_client_disconnect(client);
	playerc_client_destroy(client);

	return 0;
}
