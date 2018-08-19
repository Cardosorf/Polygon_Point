#include <OpenGL/gl.h>     // The GL Header File
#include <cmath>
#include <stdlib.h>
#include <GL/glut.h>   // The GL Utility Toolkit (Glut) Header
#include <iostream>
#include <vector>

#define _USE_MATH_DEFINES
//Width and height of the window. If necessary, change these values.
#define WIDTH 800
#define HEIGHT 600

using namespace std;

//Creates a class to handle OrderedPairs
class OrderedPair {

public:
	float getAxis_x(void);
	float getAxis_y(void);
	void setAxis_x(float x_value);
	void setAxis_y(float y_value);
	OrderedPair(float x_value, float y_value);

private:
	float x;
	float y;
};

//Sets the ordered pair
OrderedPair::OrderedPair(float x_value, float y_value) {
	x = x_value;
	y = y_value;
}

float OrderedPair::getAxis_x(void) {
	return x;
}

float OrderedPair::getAxis_y(void) {
	return y;
}

void OrderedPair::setAxis_x(float x_value) {
	x = x_value;
}

void OrderedPair::setAxis_y(float y_value){
	y = y_value;
}


int firstClick = 0;
bool polygon = false;
vector<OrderedPair> pointsArray;

void mouse(int button, int state, int x, int y) {
	/*
	If the left mouse button was pressed then the coordinates of the points will be stored inside the vector (pointsArray).
	If the size of the vector is greater than 1, then it'll start drawing the lines connecting each point after each left mouse button click.
	Once the user draws the polygon (by clicking the left mouse button near the first drawn point), the next clicks will draw points and the console
	will display if this point is outside or inside the polygon.
	*/

	float dx, dy;
	int intercept = 0;

	glColor3f(0.0, 0.0, 1.0);
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		//Draws the edges from the polygon until the user clicks the left mouse button near the first drawn point and ends up the polygon.
		if (!polygon) {
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			if (firstClick == 0) {
				pointsArray.push_back(OrderedPair((2.*x) / WIDTH - 1, -(2.*y) / HEIGHT + 1));
				firstClick++;
			}
			else {
				pointsArray.push_back(OrderedPair((2.*x) / WIDTH - 1, -(2.*y) / HEIGHT + 1));
				dx = fabs(pointsArray.back().getAxis_x() - pointsArray.front().getAxis_x());
				dy = fabs(pointsArray.back().getAxis_y() - pointsArray.front().getAxis_y());
				//Determines if last drawn point is near the first one. If so, then the polygon is ended up.
				if (dx <= 0.05 && dy <= 0.05) {
					pointsArray.back().setAxis_x(pointsArray.front().getAxis_x());
					pointsArray.back().setAxis_y(pointsArray.front().getAxis_y());
					polygon = true;
				}

				glBegin(GL_LINE_STRIP);
					for (int i = 0; i < (int)pointsArray.size(); i++) {
						glVertex2f(pointsArray[i].getAxis_x(), pointsArray[i].getAxis_y());
					}
				glEnd();
				glFlush();
				glutSwapBuffers();
				firstClick++;
			}
		}

		//Draws the point and check if it's inside or outside the polygon.
		else {
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glColor3f(0.0, 0.0, 1.0);
			glBegin(GL_LINE_STRIP);
				for (int i = 0; i < (int)pointsArray.size(); i++) {
					glVertex2f(pointsArray[i].getAxis_x(), pointsArray[i].getAxis_y());
				}
			glEnd();
			
			glPointSize(6);
			glColor3f(1.0, 0.0, 0.0);
			glBegin(GL_POINTS);
				OrderedPair point((2.*x) / WIDTH - 1, -(2.*y) / HEIGHT + 1);
				glVertex2f(point.getAxis_x(), point.getAxis_y());
			glEnd();
			glFlush();
			glutSwapBuffers();

			OrderedPair linePoint(1.0, point.getAxis_y()); //Generates a point at the right end of the window which has the same height of the last drawn point.
			vector<OrderedPair> polygonLines;

			//Generates vectors which correspond to polygon edges.
			for (int i = 0; i < (int)pointsArray.size()-1; i++) {
				dx = pointsArray[i + 1].getAxis_x() - pointsArray[i].getAxis_x();
				dy = pointsArray[i + 1].getAxis_y() - pointsArray[i].getAxis_y();
				polygonLines.push_back(OrderedPair(dx, dy));
			}

			/*For each polygon edge vectors, it will be checked if the line vector that connects the last drawn point to the point at the
			right end of the window intercepts it. If it intercepts an odd number of times, then the point is outside the polygon. Otherwise, it is inside.
			Polygon edge vetors are represented by the variables CDx and CDy (wich is the vector CD).
			The line vector that connects the last drawn point to the point at the right end of the window is represented by the variables ABx and ABy (wich is the vector AB).
			They intercept if the points C and D are on the opposite side to the vector AB and if the points A and B are on the opposite side to the vector CD.
			In order to verify if they intercept, it's necessery to check if the vectors AC and AD have opposite orientation with the vector AB.
			It's also needed to verify if the vectors CA and CB have opposite orientation with the vector CD.
			This can be done by the cross product of the vectors AB and AC and by the cross product of the vectors AB and AD.
			If these cross products have opossite signs, then they have opposite orientation.
			The same thing need to be done with the vectors CD, CA, CD and CB. Therefore, the cross product of the vectors CD and CA and the cross product of the vectors CD and CB.
			If these cross products have opossite signs, then they have opposite orientation.
			If both have opposite orientation, then the vector AB and CD intercept each other.

			The formula is  (AB x AC)*(AB x AD) < O AND (CD x CA)*(CD x CB) < O.
			*/
			for (int i = 0; i < (int)pointsArray.size() - 1; i++){
				
				float ABx = linePoint.getAxis_x() - point.getAxis_x();
				float ABy = linePoint.getAxis_y() - point.getAxis_y();
				float ADx = pointsArray[i + 1].getAxis_x() - point.getAxis_x();
				float ADy = pointsArray[i + 1].getAxis_y() - point.getAxis_y();
				float ACx = pointsArray[i].getAxis_x() - point.getAxis_x();
				float ACy = pointsArray[i].getAxis_y() - point.getAxis_y();
				float CDx = pointsArray[i + 1].getAxis_x() - pointsArray[i].getAxis_x();
				float CDy = pointsArray[i + 1].getAxis_y() - pointsArray[i].getAxis_y();
				float CAx = point.getAxis_x() - pointsArray[i].getAxis_x();
				float CAy = point.getAxis_y() - pointsArray[i].getAxis_y();
				float CBx = linePoint.getAxis_x() - pointsArray[i].getAxis_x();
				float CBy = linePoint.getAxis_y() - pointsArray[i].getAxis_y();
				
				
				if ((ABx*ADy - ABy * ADx)*(ABx*ACy - ABy * ACx) < 0 && (CDx*CAy - CDy * CAx)*(CDx*CBy - CDy * CBx) < 0) {
					intercept++;
				}
				
			}

			cout << (intercept % 2 == 0 ? "It's outside." : "It's inside.") << endl;

			polygonLines.clear();
		}
		
	}

}

//Print the header with my name, dre and instructions
void header() {
	//Sets the position of the text in the window using the x and y coordinates
	glRasterPos2f(-1, 0.9);
	string name = "Rafael Pais Cardoso\n";
	string dre = "DRE: 116140788";
	string instruction1 = "Left mouse button -> creates connected points by lines";
	string instruction2 = "Once the polygon is drawn, the left mouse button will draw points";
	string instruction3 = "The console will display if this point is outside or inside the polygon.";
	int len_name = (int)name.length();
	int len_dre = (int)dre.length();
	int len_instruction1 = (int)instruction1.length();
	int len_instruction2 = (int)instruction2.length();
	int len_instruction3 = (int)instruction3.length();

	//Loops to display character by character
	for (int i = 0; i < len_name; i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, name[i]);
	}

	glRasterPos2f(-1, 0.8);
	for (int i = 0; i < len_dre; i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, dre[i]);
	}

	glRasterPos2f(-1, 0.7);
	for (int i = 0; i < len_instruction1; i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, instruction1[i]);
	}

	glRasterPos2f(-1, 0.6);
	for (int i = 0; i < len_instruction2; i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, instruction2[i]);
	}

	glRasterPos2f(-1, 0.5);
	for (int i = 0; i < len_instruction3; i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, instruction3[i]);
	}
}

void display()
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(1.0, 0.0, 0.0);
	glLineWidth(4.0);
	header();
	glFlush();
	glutSwapBuffers();
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitWindowSize(WIDTH, HEIGHT);
	glutCreateWindow("Rafael Pais Cardoso");
	glutDisplayFunc(display);
	glutMouseFunc(mouse);
	glutMainLoop();
	return EXIT_SUCCESS;
}