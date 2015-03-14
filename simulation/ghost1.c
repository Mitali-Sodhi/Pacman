/**********************************
 1.  receive position of pacman
 2.  move[Attack] accordingly
 3.  if caught.. game over
 4.  else continue attack
***********************************/

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
 * attack algorithm
 * move towards the pacman position
 * using shortest path based approach
 ***************************************/
void
attack(int px, int py, int gx, int gy, int direction, double *next_x, double *next_y)
{
	//printf("In Attack\n");

    int mark = 0;
	if (px < gx && gy != 1 && gy != 3) 		// can move left
	{
		*next_x = gx - 1;	// pre move left
  		mark = -1;
	}
	else if (px > gx && gy != 1 && gy != 3) 	// can move right
	{
		*next_x = gx + 1;	// pre move right
		mark = 1;
	}
	else						// can not move left or right
		*next_x = gx;

	if (py < gy && gx != 1 && gx != 3) 		// can move up
	{
		if (mark == 0 || (direction == UP && py != 1 && py !=3) || (direction != UP && px != 1 && px != 3))
		{
			*next_x = gx;		// correct x axis movement
			*next_y = gy - 1;	// move up
		}
		else
			*next_y = gy;		// do not move up
	}
	else if (py > gy && gx != 1 && gx != 3) 	// can move down
	{
		if (mark == 0 || (direction == DOWN && py != 1 && py !=3) || (direction != DOWN && px != 1 && px != 3))
		{
			*next_x = gx;		// correct x axis movement
			*next_y = gy + 1;	// move down
		}
		else
			*next_y = gy;		// do not move down
	}
	else						// can not move up or down
		*next_y = gy;

    // Boundary Check for ghost1 start

    printf(" gx = %d \t gy = %d\n",gx, gy );

    if (gx == 4 && gy == 0 && direction == RIGHT)           // Upper-right corner
    {
        *next_x = gx;       // move - down
        *next_y = gy + 1;
    }

    if (gx == 4 && gy == 0 && direction == UP)
    {
        *next_x = gx - 1;       // move - left
        mark = -1;
    }


    if (gx == 0 && gy == 0 && direction == UP)      // Upper-left corner
    {

        *next_x = gx + 1;       // move - right
        mark = 1;
    }
    if (gx == 0 && gy == 0 && direction == LEFT)
    {
        *next_x = gx;       // move - down
        *next_y = gy + 1;
    }


    if (gx == 0 && gy == 4 && direction == DOWN)        // Lower-left corner
    {
        *next_x = gx + 1;       // move - left
        mark = -1;
    }
    if (gx == 0 && gy == 4 && direction == LEFT)
    {
        *next_x = gx;       // move - up
        *next_y = gy - 1;
    }


    if (gx = 4 && gy == 4 && direction == RIGHT)        // Lower-right corner
    {
        *next_x = gx;       // move - up
        *next_y = gy - 1;
    }
    if (gx = 4 && gy == 4 && direction == DOWN)
    {
        *next_x = gx - 1;       // move - right
        mark = 1;
    }
    // Boundary check end
}

/****************************************
 * get ghost's direction
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


int
goFwd(double dist_mm)
    {

    float cx, cy, ca;
    float ix, iy, ia;
    float new_dist;
    double simX, simY, simA;

    float px, py, pa;
    double simpX,simpY,simpA;
    playerc_simulation_get_pose2d(sim, (char *)"pacman",&simpX,&simpY,&simpA);
    playerc_simulation_get_pose2d(sim, (char *)"ghost1",&simX,&simY,&simA);
	usleep(300);
    ix=cx=simX;
    iy=cy=simY;
    ia=ca=simA;

    px = simpX;
    py = simpY;
    pa = simpA;

    double speed=0.0, turn=0.0;

    fprintf(stderr,"Going Forward \n");
	new_dist = dist_mm;

    double index_px, index_py, index_gx, index_gy;
    int p_direction, g_direction;

    index_px = ((8 + ix)/3.2);
    index_py = ((8 - iy)/3.2);
    index_gx = ((8 + ix)/3.2);
    index_gy = ((8 - iy)/3.2);
    p_direction = getDirection(RTOD(ia));
    g_direction = getDirection(RTOD(ia));


    while ( new_dist> 5)
        {
        //playerc_client_read(client);
        playerc_position2d_set_cmd_vel(position2d,
new_dist > MAX_SPEED*1000?MAX_SPEED:new_dist/500, 0.0, 0.0, 1);
        //playerc_client_read(client);
        playerc_simulation_get_pose2d(sim, (char *)"ghost1",&simX,&simY,&simA);
		usleep(300);
        cx = simX;
        cy = simY;
        ca = simA;
        new_dist = dist_mm - sqrt((ix-cx)*(ix-cx)+(iy-cy)*(iy-cy))*1000;
        index_px = ((8 + ix)/3.2);
        index_py = ((8 - iy)/3.2);
        index_gx = ((8 + cx)/3.2);
        index_gy = ((8 - cy)/3.2);
        p_direction = getDirection(RTOD(ia));
        g_direction = getDirection(RTOD(ca));
        boundary_check();
        }
    stop();
    return 0;
    }


void
turnLeft (float ang_deg)
    {
    float cx, cy, ca;
    float ix, iy, ia;
    float leftToTurn;
    float turnedSoFar;
    double simX, simY, simA;
	playerc_simulation_get_pose2d(sim, (char *)"ghost1",&simX, &simY, &simA  );
	usleep(3000);
    ia=ca=simA;
    leftToTurn=ang_deg*M_PI/180.0;

    fprintf(stderr,"turning LEFT \n");
    //while ( leftToTurn> 0)
    while ( leftToTurn> DTOR(2))
        {
        playerc_position2d_set_cmd_vel(position2d, 0, 0.0, MAX_TURN,1);
        //playerc_client_read(client);
        playerc_simulation_get_pose2d(sim, (char *)"ghost1",&simX, &simY, &simA  );
		usleep(3000);
        ca=simA;

        //printf("Simulation angle = %f\n", ca);

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

    playerc_simulation_get_pose2d(sim, (char *)"ghost1",&simX, &simY, &simA  );
	usleep(3000);
    ia=ca=simA;
    leftToTurn=ang_deg*M_PI/180.0;

   // printf(" ang_degree = %f \t left to turn = %f \t M_PI = %f\n",ang_deg, leftToTurn, M_PI );

    fprintf(stderr,"turning Right \n");

   // playerc_position2d_set_cmd_vel(position2d, 0.0, 0, DTOR(ang_deg), 1);

    //printf("DTOR %f\n", DTOR(3));

   //while ( leftToTurn> 0)
    while ( leftToTurn> DTOR(2))
        {
        playerc_position2d_set_cmd_vel(position2d, 0, 0.0, -1*MAX_TURN,1);
        playerc_simulation_get_pose2d(sim, (char *)"ghost1",&simX, &simY, &simA  );
		usleep(3000);
        ca=simA;

        //printf("Simulation angle = %f\n", ca);

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

int boundary_check()
{

    //printf("In boundary_check\n");



    //usleep(4000);

    double simpX, simpY, simpA;
    double simgX, simgY, simgA;

    double px, py;
    double gx, gy;

    playerc_simulation_get_pose2d(sim,(char *)"pacman",&simpX,&simpY,&simpA);
    playerc_simulation_get_pose2d(sim, (char *)"ghost1",&simgX,&simgY,&simgA);

    px = ((8 + simpX)/3.2);
    py = ((8 - simpY)/3.2);
    gx = ((8 + simgX)/3.2);
    gy = ((8 - simgY)/3.2);

    int pa, ga;

    pa = getDirection(RTOD(simpA));
    ga = getDirection(RTOD(simgA));

    printf("gx = %lf \t gy= %lf \t ga =%d\n",gx,gy,ga );


    if (gx <= 0.1 && gy <= 0.9)
    {
        printf("Here1\n");
        if (ga == LEFT)
        {
            turnLeft(90.0);
        }
        else
            if (ga == UP && pa == LEFT)
            {
                turnRight(90.0);
            }
    }

    else
        if (gx >= 4.5 && gy <= 0.8)
        {
            printf("Here2\n");
            if (ga == RIGHT)
            {
                turnRight(90.0);
            }
            else
                if(ga == UP)
                {
                    turnLeft(90.0);
                }

        }

        else
            if (gx <= 0.1 && gy >= 4.0)
            {
                printf("Here3\n");
                if (ga == DOWN)
                {
                    turnLeft(90.0);
                }
                else
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
        if (gx >= 4.5 && gy >=2.5)
        {
            printf("Here4\n");
            if (ga == RIGHT)
            {
                turnLeft(90.0);
            }
            else
                if (ga == DOWN)
                {
                    turnRight(90.0);
                }
        }

    else
        if (gx >= 2.9 && gy >=4.0)
        {
            printf("Here5\n");
            //printf("gx = %lf \t gy= %lf \t ga =%d\n",gx,gy,ga );
            if (ga == RIGHT)
            {
                turnLeft(90.0);
            }
            else
                if (ga == DOWN)
                {
                    turnRight(90.0);
                }
        }


    else
        if (gx <= 3.4 && gy > 2.0 && gy < 2.8)
        {
            printf("Here6\n");
            if (ga == LEFT)
            {
                turnLeft(90.0);
            }
            else
                if (ga == UP && px >= 4.2)
                {
                    turnRight(90.0);
                }
                //else
                    //turnRight(90.0);
        }

        else
            if (gy < 0.7 && ga == UP)
            {
                printf("Here7\n");
                if (pa == LEFT)
                {
                    turnRight(90.0);
                }
                else
                if (pa == RIGHT)
                    {
                        turnLeft(90.0);
                    }
                else
                    turnRight(90.0);
            }

/*      else
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
                    //  turnLeft(90.0);
                else
                    return 0;

    //goFwd(100.0);

    return 0;
}








int
drive(double nextX, double nextY)
    {
    float dx, dy, dq_degrees, dist,dq;
    double simX, simY, simA;
    int i;

        playerc_simulation_get_pose2d(sim, (char *)"ghost1",&simX, &simY, &simA  );
		usleep(300);

		//printf("In Run.. running\n");

        dx = (nextX - simX)*1000;
        dy = (nextY - simY)*1000;
        dist = sqrt(dx * dx + dy * dy) ;
        dq=atan2(dy, dx);
        dq -= simA;
        limit_ang(&dq);
        dq_degrees = dq / PI * 180.0;

        printf("simX %f \t nextX %f\n", simX, nextX );
        printf("simY %f \t nextY %f\n", simY, nextY );
        printf("dq_degrees %f \t Distance %f\n", dq_degrees, dist);

        if (dq > 0.0)
            turnLeft (dq_degrees);
            //turnLeft (90);
        else if (dq < 0.0)
            turnRight (-dq_degrees);
            //turnRight (90);

        goFwd (dist);
    }

/****************************************
 * major function for ghost
 ***************************************/
int
run(double gx, double gy, double ga, double px, double py, double pa, double *speed, double *turn)
{

	double dist, angle ;
	double index_gx, index_gy, index_px, index_py;
	int p_direction, g_direction;
	double next_x, next_y;
	int index_next_x, index_next_y;

	// 1st step: get block info for pacman and ghost
	index_px = ((8 + px)/3.2);
	index_py = ((8 - py)/3.2);
	index_gx = ((8 + gx)/3.2);
	index_gy = ((8 - gy)/3.2);

	g_direction = getDirection(RTOD(ga));
    p_direction = getDirection(RTOD(pa));

    boundary_check();

	// 2nd step: get next movement for ghost
	attack(index_px, index_py, index_gx, index_gy, g_direction, &next_x, &next_y);

	index_next_x = next_x;
	index_next_y = next_y;

	fprintf(stderr,"next_index: %d   %d,  index:  %lf   %lf \n",
	index_next_x, index_next_y, index_gx, index_gy);

	next_x = -6.4 + 3.2 * next_x;
	next_y = 6.4 - 3.2 * next_y;

	fprintf(stderr,"next: %3.2f   %3.2f\n", next_x, next_y);


	if (index_next_x != index_gx)
		{
		drive(next_x,py);
		return 0;
		}

	if (index_next_y != index_gy)
		drive(next_x,next_y);
}


int
main(int argc, const char **argv)
{
	 int i;
    playerc_client_t *simclient, *client;
    playerc_mclient_t *mclient;
    //playerc_position2d_t *position2d;
    //playerc_simulation_t *sim;
    double pacmanX, pacmanY, pacmanA;
    double ghost1X, ghost1Y, ghost1A;
    double turn, speed;

    // Create a client and connect it to the server.
    mclient = playerc_mclient_create();
    simclient = playerc_client_create(mclient, "localhost", 6665);
    client = playerc_client_create(mclient, "localhost", 6666);
    if (0 != playerc_client_connect(client) + playerc_client_connect(simclient))
        return -1;

    // Create and subscribe to a position2d device.
    position2d = playerc_position2d_create(client, 0);
    if (playerc_position2d_subscribe(position2d, PLAYER_OPEN_MODE))
        return -1;

    // Create and subscribe to a simulation interface device.
    sim = playerc_simulation_create(simclient, 0);
    if (playerc_simulation_subscribe(sim, PLAYER_OPEN_MODE))
        return -1;

   for (;;)
    {
        // Wait for new data from server
        playerc_mclient_read(mclient,100);
        // Read pacman's & ghost's global pose via simulation interface
        playerc_simulation_get_pose2d(sim, (char *)"pacman", &pacmanX, &pacmanY, &pacmanA);
        //printf("pm = (%.0f,%.0f,%.0f)\t",pacmanX,pacmanY,pacmanA);
        playerc_simulation_get_pose2d(sim, (char *)"ghost1", &ghost1X, &ghost1Y, &ghost1A);
        //printf("g1 = (%.0f,%.0f,%.0f)\t",ghost1X,ghost1Y,ghost1A);

        //run(pacmanX, pacmanY, pacmanA, ghost1X, ghost1Y, &speed, &turn);
        run(ghost1X, ghost1Y, ghost1A, pacmanX, pacmanY, pacmanA, &speed, &turn);
        //turnRight(90);
        //turnLeft(90);
        speed = 1;

        playerc_position2d_set_cmd_vel(position2d, speed, 0, 0,1);
            //return -1;

    }

    // Shutdown
    playerc_simulation_unsubscribe(sim);
    playerc_simulation_destroy(sim);
    playerc_position2d_unsubscribe(position2d);
    playerc_position2d_destroy(position2d);
    playerc_client_disconnect(client);
    playerc_client_disconnect(simclient);
    playerc_mclient_destroy(mclient);

    return 0;
}
