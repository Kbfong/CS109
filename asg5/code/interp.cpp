// $Id: interp.cpp,v 1.3 2019-03-19 16:18:22-07 - - $
/*

This program was completed using pair programming.
Partner:  Kody Fong (kbfong@ucsc.edu)
Partner:  Andy Huang (ahuang44@ucsc.edu)
*/ 
#include <memory>
#include <string>
#include <vector>
using namespace std;

#include <GL/freeglut.h>

#include "debug.h"
#include "interp.h"
#include "shape.h"
#include "util.h"

unordered_map<string,interpreter::interpreterfn>
interpreter::interp_map {
   {"define" , &interpreter::do_define },
   {"draw"   , &interpreter::do_draw   },
   {"moveby" , &interpreter::do_moveby },
   {"border" , &interpreter::do_border },
};

unordered_map<string,interpreter::factoryfn>
interpreter::factory_map {
   {"text"     , &interpreter::make_text     },
   {"ellipse"  , &interpreter::make_ellipse  },
   {"circle"   , &interpreter::make_circle   },
   {"polygon"  , &interpreter::make_polygon  },
   {"rectangle", &interpreter::make_rectangle},
   {"square"   , &interpreter::make_square   },
   {"diamond"  , &interpreter::make_diamond  },
   {"triangle" , &interpreter::make_triangle },
   {"equilateral" , &interpreter::make_equilateral },

};

interpreter::shape_map interpreter::objmap;

interpreter::~interpreter() {
   for (const auto& itor: objmap) {
      cout << "objmap[" << itor.first << "] = "
           << *itor.second << endl;
   }
}

void interpreter::interpret (const parameters& params) {
   DEBUGF ('i', params);
   param begin = params.cbegin();
   string command = *begin;
   auto itor = interp_map.find (command);
   if (itor == interp_map.end()) throw runtime_error ("syntax error");
   interpreterfn func = itor->second;
   func (++begin, params.cend());
}

void interpreter::do_define (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   string name = *begin;
   objmap.emplace (name, make_shape (++begin, end));
}




void interpreter::do_draw (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   if (end - begin != 4) throw runtime_error ("syntax error");
   string name = begin[1];
   shape_map::const_iterator itor = objmap.find (name);
   if (itor == objmap.end()) {
      throw runtime_error (name + ": no such shape");
   }
   rgbcolor color {begin[0]};
   vertex where {from_string<GLfloat> (begin[2]),
                 from_string<GLfloat> (begin[3])};
   window::push_back (object (itor->second, where, color));
}


void interpreter::do_border(param begin, param end) {
   DEBUGF ('f', range (begin, end));
   string color = *begin;
   *begin++;
   int width =  from_string<int> (*begin);
   window::border_color = color;
   window::border_width  = width;
}


void interpreter::do_moveby (param begin, param end) {
   DEBUGF ('f', range (begin, end));
    window::moveAmount = (stof(*begin));
}


shape_ptr interpreter::make_shape (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   string type = *begin++;
   auto itor = factory_map.find(type);
   if (itor == factory_map.end()) {
      throw runtime_error (type + ": no such shape");
   }
   factoryfn func = itor->second;
   return func (begin, end);
}

shape_ptr interpreter::make_text (param begin, param end) {
   DEBUGF ('f', range (begin, end));

   string type = *begin++;
   string str;
   auto itor = fontcode.find(type);
   if (itor == fontcode.end()) {
      throw runtime_error (type + ": no such font");
   }
   void * font = itor->second;
   while (begin != end)
   str += (*begin++) + " ";
   return make_shared<text> (font, str);
}

shape_ptr interpreter::make_ellipse (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   return make_shared<ellipse> (GLfloat(stof(begin[0])),
    GLfloat(stof(begin[1])));
}

shape_ptr interpreter::make_circle (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   return make_shared<circle> (GLfloat(stof(begin[0])));
}

shape_ptr interpreter::make_polygon (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   vertex_list verts;
   while(begin != end){


      verts.push_back({GLfloat(stof(begin[0])),
         GLfloat(stof(begin[1]))}); 
         //pushes the float values into the vertex list

      //increment begin twice
      begin++;
      begin++;
   }
   return make_shared<polygon> (verts);
}

shape_ptr interpreter::make_rectangle (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   return make_shared<rectangle> (GLfloat(stof(begin[0])), 
      GLfloat(stof(begin[1])));
}

shape_ptr interpreter::make_square (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   return make_shared<square> (GLfloat(stof(begin[0])));
}

shape_ptr interpreter::make_diamond(param begin, param end) {
   DEBUGF ('f', range (begin, end));
   return make_shared<diamond> (GLfloat(stof(begin[0])), 
    GLfloat(stof(begin[1])));
}

shape_ptr interpreter::make_triangle (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   vertex_list verts;
   while (begin != end){

      verts.push_back({GLfloat(stof(begin[0])), 
                          GLfloat(stof(begin[1]))});
      begin++;
      begin++;
   }
   return make_shared<triangle> (verts);
}



shape_ptr interpreter::make_equilateral (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   return make_shared<equilateral> (GLfloat(stof(begin[0])));
}

