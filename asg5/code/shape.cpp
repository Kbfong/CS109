// $Id: shape.cpp,v 1.2 2019-02-28 15:24:20-08 - - $
/*

This program was completed using pair programming.
Partner:  Kody Fong (kbfong@ucsc.edu)
Partner:  Andy Huang (ahuang44@ucsc.edu)
*/ 
#include <typeinfo>
#include <unordered_map>
#include <cmath>
using namespace std;

#include "shape.h"
#include "util.h"

static unordered_map<void*,string> fontname {
   {GLUT_BITMAP_8_BY_13       , "Fixed-8x13"    },
   {GLUT_BITMAP_9_BY_15       , "Fixed-9x15"    },
   {GLUT_BITMAP_HELVETICA_10  , "Helvetica-10"  },
   {GLUT_BITMAP_HELVETICA_12  , "Helvetica-12"  },
   {GLUT_BITMAP_HELVETICA_18  , "Helvetica-18"  },
   {GLUT_BITMAP_TIMES_ROMAN_10, "Times-Roman-10"},
   {GLUT_BITMAP_TIMES_ROMAN_24, "Times-Roman-24"},
};


ostream& operator<< (ostream& out, const vertex& where) {
   out << "(" << where.xpos << "," << where.ypos << ")";
   return out;
}

shape::shape() {
   DEBUGF ('c', this);
}

text::text (void* glut_bitmap_font_, const string& textdata_):
      glut_bitmap_font(glut_bitmap_font_), textdata(textdata_) {
   DEBUGF ('c', this);
}

ellipse::ellipse (GLfloat width, GLfloat height):
dimension ({width, height}) {
   DEBUGF ('c', this);
}

circle::circle (GLfloat diameter): ellipse (diameter, diameter) {
   DEBUGF ('c', this);
}


polygon::polygon (const vertex_list& vertices_): vertices(vertices_) {
   DEBUGF ('c', this);
}

rectangle::rectangle (GLfloat width, GLfloat height):
            polygon({
      //(bot left corner)(bot right corner)    
      {width,height}, {width,0},
      //(top left corner)(top right corner) 
      {0,0}, {0,height}}){
   DEBUGF ('c', this << "(" << width << "," << height << ")");
}

square::square (GLfloat width): rectangle (width, width) {
   DEBUGF ('c', this);
}

diamond::diamond (GLfloat width, GLfloat height): polygon(
    {
        //top       and    bottom
        {0,0},{width/2,height/2},
        //left      and   right 
        {width,0},{width/2,0-height/2}
    }){
        DEBUGF ('c', this << "(" << width << "," << height << ")");
}
//modeled after polygo::polygon
triangle::triangle (const vertex_list& vertices_): polygon(vertices_){
    DEBUGF ('c', this);
}

//equilateral 
equilateral::equilateral(GLfloat width): triangle(
    {   //apex
        {0,width/2*sqrtf(3)},
        //left    and  right 
        {-width/2,0},{width/2,0}
    }){
        DEBUGF ('c', this << "width: "<< width);
}

void text::draw (const vertex& center, const rgbcolor& color) const {
   DEBUGF ('d', this << "(" << center << "," << color << ")");
   auto text_ = reinterpret_cast<const GLubyte*> (textdata.c_str());
   //sets color
   glColor3ubv (color.ubvec);
   //set raster to center 
   glRasterPos2f (center.xpos, center.ypos);
   glutBitmapString (glut_bitmap_font, text_);
}

void ellipse::draw (const vertex& center, const rgbcolor& color) const {
   DEBUGF ('d', this << "(" << center << "," << color << ")");
   glBegin (GL_POLYGON);
   glEnable (GL_LINE_SMOOTH);
   glColor3ubv (color.ubvec);
   //to loop and make 32 vertices
   const float change = 2 * M_PI / 32;
   for (float i = 0; i < 2 * M_PI; i += change) {
      // equation for points on a circumference of ellipse
      float xpos = dimension.xpos * cos (i) + center.xpos;
      float ypos = dimension.ypos * sin (i) + center.ypos;
      glVertex2f (xpos, ypos);
   }
   
   glEnd();
}

void polygon::draw (const vertex& center, const rgbcolor& color) const {
    DEBUGF ('d', this << "(" << center << "," << color << ")");
    glBegin(GL_POLYGON);
    glEnable(GL_LINE_SMOOTH);
    glColor3ubv(color.ubvec);
    float xAvg = 0;
    float yAvg = 0;
    //sum up all x and y parts of vertices
    for(const auto& vertex: vertices){
        xAvg += vertex.xpos;
        yAvg += vertex.ypos;
    }
    //divide by total to get the averages
    xAvg /= vertices.size(); 
    yAvg /= vertices.size();
    //subtract avg from each vertice to normalize 
    //add to intended center
    for(auto v = vertices.cbegin(); v != vertices.cend(); ++v){
      float xpos = center.xpos + v->xpos - xAvg;
      float ypos = center.ypos + v->ypos - yAvg;
      glVertex2f(xpos, ypos);
   }
   glEnd();
}

void shape::show (ostream& out) const {
   out << this << "->" << demangle (*this) << ": ";
}

void text::show (ostream& out) const {
   shape::show (out);
   out << glut_bitmap_font << "(" << fontname[glut_bitmap_font]
       << ") \"" << textdata << "\"";
}

void ellipse::show (ostream& out) const {
   shape::show (out);
   out << "{" << dimension << "}";
}

void polygon::show (ostream& out) const {
   shape::show (out);
   out << "{" << vertices << "}";
}

ostream& operator<< (ostream& out, const shape& obj) {
   obj.show (out);
   return out;
}

