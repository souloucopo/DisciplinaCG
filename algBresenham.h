void reduceToFirstOctant(int x1, int y1, int x2, int y2) {
    int delta_X = x2 - x1;
    int delta_Y = y2 - y1;

    if (delta_X < 0) {
        x1 = -(x1);
        x2 = -(x2);
        delta_X = -delta_X;
    }

    if (delta_Y < 0) {
        y1 = -(y1);
        y2 = -(y2);
        delta_Y = -delta_Y;
    }

    if (delta_Y > delta_X) {
        int temp = x1;
        x1 = y1;
        y1 = temp;
        temp = x2;
        x2 = y2;
        y2 = temp;
    }

    if (x1 > x2) {
        int temp = x1;
        x1 = x2;
        x2 = temp;
        temp = y1;
        y1 = y2;
        y2 = temp;
    }

    y2 -= y1;
    y1 = 0;
}

void drawLineBresenham(int x1, int y1, int x2, int y2) {
    int delta_X = abs(x2 - x1);
    int delta_Y = abs(y2 - y1);
    int x = x1 < x2 ? 1 : -1;
    int y = y1 < y2 ? 1 : -1;

    int d = delta_X - delta_Y; // Calcula diferenca dos Deltas
    int e2;

	reduceToFirstOctant(x1,y1,x2,y2);
    glBegin(GL_POINTS);

    while (1) {
        glVertex2i(x1, y1);

        if (x1 == x2 && y1 == y2) {
            break;
        }

        e2 = 2 * d;

        if (e2 > -delta_Y) {
            d -= delta_Y;		//Aqui serve para calcular o erro dependendo de
            x1 += x;			//for maior
        }

        if (e2 < delta_X) {
            d += delta_X;
            y1 += y;
        }
    }

    glEnd();
}


	

