/**********************************************************
*      This software is part of the graphviz package      *
*                http://www.graphviz.org/                 *
*                                                         *
*            Copyright (c) 1994-2007 AT&T Corp.           *
*                and is licensed under the                *
*            Common Public License, Version 1.0           *
*                      by AT&T Corp.                      *
*                                                         *
*        Information and Software Systems Research        *
*              AT&T Research, Florham Park NJ             *
**********************************************************/
#include "glcomptext.h"
#include "glpangofont.h"
#include "memory.h"
void init_gl_vars(glCompText* f)
{

	glGetIntegerv(GL_POLYGON_MODE, f->glcache.poly);

	if (f->glcache.poly[0] != GL_FILL)
		glPolygonMode (GL_FRONT, GL_FILL);
	if (f->glcache.poly[1] != GL_FILL)
		glPolygonMode (GL_BACK, GL_FILL);

	f->glcache.istextureon = glIsEnabled (GL_TEXTURE_2D);
	if (!f->glcache.istextureon)
		glEnable (GL_TEXTURE_2D);
	f->glcache.isdepthon = glIsEnabled (GL_DEPTH_TEST);
	if (f->glcache.isdepthon)
		glDisable (GL_DEPTH_TEST);
	f->glcache.islightingon= glIsEnabled (GL_LIGHTING);        
	if (f->glcache.islightingon) 
		glDisable(GL_LIGHTING);
	glGetIntegerv(GL_MATRIX_MODE, &f->glcache.matrix); 
	
	f->glcache.isblendon= glIsEnabled (GL_BLEND);        
    glGetIntegerv (GL_BLEND_SRC, &f->glcache.blendfrom);
    glGetIntegerv (GL_BLEND_DST, &f->glcache.blendto);
	if (&f->glcache.isblendon)
		glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}
void restore_gl_vars(glCompText* f)
{
	if (f->glcache.poly[0] != GL_FILL)
		glPolygonMode (GL_FRONT, f->glcache.poly[0]);
	if (f->glcache.poly[1] != GL_FILL)
		glPolygonMode (GL_BACK, f->glcache.poly[1]);
	
	if (f->glcache.islightingon)
		glEnable(GL_LIGHTING);
	
	if (!f->glcache.isblendon)
	{
		glDisable (GL_BLEND);
		glBlendFunc (f->glcache.blendfrom, f->glcache.blendto);
	} else glBlendFunc (f->glcache.blendfrom, f->glcache.blendto);

	if (f->glcache.isdepthon)
		glEnable (GL_DEPTH_TEST);
	if (!f->glcache.istextureon)
		glDisable (GL_TEXTURE_2D);
	glMatrixMode (f->glcache.matrix);
}



void 
glprintf (glCompText* font, GLfloat xpos, GLfloat ypos, 
    GLfloat width, char *bf)
{
    int vPort[4];
	GLfloat size = font->fontheight;
	GLfloat x = xpos;
	GLfloat y = ypos;
    int xMax;
	int charCount;
	int maxcharCount;
	char* tempC;
	GLfloat charGap;
	
	glGetIntegerv (GL_VIEWPORT, vPort);

	init_gl_vars(font);
	glBindTexture (GL_TEXTURE_2D, font->texId);


	xMax = vPort[0] + vPort[2];



	tempC=bf;
	charCount=0;
	maxcharCount=0;

	while (*tempC != '\0')
	{
		if ( *tempC == '\t')
			charCount=charCount+FONT_TAB_SPACE;
		else
			charCount++;
		tempC++;
	}
	if (charCount > maxcharCount)
		maxcharCount=charCount;
	charGap=(width / (float)maxcharCount);


	for ( ; *bf; *bf ++, x += charGap) //size*0.7 is the distance between2 characters
	{

	    glBegin (GL_QUADS);
		glTexCoord2f (font->bmp[(int)(*bf)][0], font->bmp[(int)(*bf)][1]);
		glVertex3f (x, y,0);

    	glTexCoord2f (font->bmp[(int)(*bf)][0] + font->tIncX, font->bmp[(int)(*bf)][1]);
		glVertex3f (x + size, y,0);

		glColor4f (font->color.R,font->color.G,font->color.B,font->color.A);

        glTexCoord2f (font->bmp[(int)(*bf)][0] + font->tIncX, font->bmp[(int)(*bf)][1] + font->tIncY);
		glVertex3f (x + size, y + size,0);

        glTexCoord2f (font->bmp[(int)(*bf)][0], font->bmp[(int)(*bf)][1] + font->tIncY);
		glVertex3f (x , y + size,0);
		glEnd ();
	}


	restore_gl_vars(font);
}





static int fontId(fontset_t* fontset,char* fontdesc)
{
	int ind=0;
	for (ind=0;ind < fontset->count;ind ++)
	{
		if (strcmp(fontset->fonts[ind]->fontdesc,fontdesc) == 0)
			return ind;
	}
	return -1;
}


glCompText* font_init()
{
    glCompText* font = NEW(glCompText);
	int idx = 0;
	float x, y;

	font->color.R=1.00;
	font->color.G=1.00;
	font->color.B=1.00;
	font->color.A=1.00;


	font->fontheight=12;
	font->tIncX=0.0;
	font->tIncY=0.0;
	font->texId=-1;
	font->fontdesc=(char*)0;



	font->tIncX = (float)pow (C_DPI, -1);
	font->tIncY = (float)pow (R_DPI, -1);

	
	for (y = 1 - font->tIncY; y >= 0; y -= font->tIncY)
	{
		for (x = 0; x <= 1 - font->tIncX; x += font->tIncX, idx ++)
		{
			font->bmp[idx][0]=x;
			font->bmp[idx][1]=y;
		}
	}
    return font;
}

void copy_font(glCompText* targetfont,const glCompText* sourcefont)
{
	int idx = 0;
	float x, y;

	targetfont->color.R=sourcefont->color.R;
	targetfont->color.G=sourcefont->color.G;
	targetfont->color.B=sourcefont->color.B;
	targetfont->color.A=sourcefont->color.A;



	targetfont->fontheight=sourcefont->fontheight;
	targetfont->tIncX=sourcefont->tIncX;
	targetfont->tIncY=sourcefont->tIncY;
	targetfont->texId=sourcefont->texId;
	if (targetfont->fontdesc)
		free(targetfont->fontdesc);
	if(sourcefont->fontdesc!=(char*)0)
		targetfont->fontdesc=strdup(sourcefont->fontdesc);
	else
		targetfont->fontdesc=(char*)0;

	targetfont->tIncX = (float)pow (C_DPI, -1);
	targetfont->tIncY = (float)pow (R_DPI, -1);

	
	for (y = 1 - targetfont->tIncY; y >= 0; y -= targetfont->tIncY)
	{
		for (x = 0; x <= 1 - targetfont->tIncX; x += targetfont->tIncX, idx ++)
		{
			targetfont->bmp[idx][0]=x;
			targetfont->bmp[idx][1]=y;
		}
	}

}

#ifndef _WIN32
#define TMPTEMP "/tmp/_sfXXXX"
#endif

fontset_t* fontset_init()
{
    fontset_t* fs = NEW(fontset_t);
	fs->activefont=-1;
	fs->count=0;
#ifdef _WIN32
	fs->font_directory = "c:/fontfiles"; //FIX ME
#else
    fs->font_directory = strdup (TMPTEMP);
    mkdtemp (fs->font_directory);
#endif
	fs->fonts=0;
    return fs;
}

static char* fontpath = NULL;
static size_t fontpathsz = 0;

int add_font(fontset_t* fontset,char* fontdesc)
{
    int id;	
    size_t sz;
    glCompText* tf;

    id=fontId(fontset,fontdesc);

    if (id==-1) {
	sz = strlen(fontset->font_directory)+strlen(fontdesc)+6;
	if (sz > fontpathsz) {
	    fontpathsz = 2*sz;
	    fontpath = ALLOC (fontpathsz, fontpath, char); 
        }
	sprintf(fontpath,"%s/%s.png",fontset->font_directory,fontdesc);
	if(create_font_file(fontdesc,fontpath,(float)32,(float)32)==0) {
	    fontset->fonts = ALLOC(fontset->count+1,fontset->fonts,glCompText*);
	    fontset->fonts[fontset->count] = tf = font_init ();
	    tf->fontdesc = strdup(fontdesc);
	    glGenTextures (1, &(tf->texId));	//get  opengl texture name
	    if ((tf->texId >= 0) && glCompLoadFontPNG (fontpath, tf->texId)) {
		fontset->activefont=fontset->count;
		fontset->count++;
		return fontset->count;
	    }
	    else
		return -1;
	}
	else
	    return -1;
    }
    else
	return id;
}

void free_font_set(fontset_t* fontset)
{
    int ind;
    for (ind=0;ind < fontset->count;ind ++) {
#ifndef _WIN32
	sprintf(fontpath,"%s/%s.png",fontset->font_directory,fontset->fonts[ind]->fontdesc);
	unlink (fontpath);
#endif
	free(fontset->fonts[ind]->fontdesc);
	free(fontset->fonts[ind]);
    }
    free(fontset->fonts);
#ifndef _WIN32
    if (fontset->font_directory)
	rmdir (fontset->font_directory);
#endif
	if (fontset->font_directory)
		free(fontset->font_directory);
    free(fontset);
}

void fontColor (glCompText* font,float r, float g, float b,float a)
{

	font->color.R=r;
	font->color.G=g;
	font->color.B=b;
	font->color.A=a;
}
