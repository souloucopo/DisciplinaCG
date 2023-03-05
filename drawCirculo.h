void pontosCirculo(int xc, int yc, int x, int y){
	glVertex2i(xc + x, yc + y);
    glVertex2i(xc + y, yc + x);
    glVertex2i(xc - y, yc + x);
    glVertex2i(xc - x, yc + y);
    glVertex2i(xc - x, yc - y);
    glVertex2i(xc - y, yc - x);
    glVertex2i(xc + y, yc - x);
    glVertex2i(xc + x, yc - y);
}

void circulo(int x1, int y1, int raio){
	int x, y;
	int d;
	
	d = 1 * raio;
	x = 0;
	y = raio;
	glBegin(GL_POINTS);
    while (x <= y) {
        pontosCirculo(x1, y1, x, y);

        if (d < 0) {
            d += 4 * x + 6;
        } else {
            d += 4 * (x - y) + 10;
            y--;
        }
        x++;
    }
    glEnd();
}

