//
// Created by Swung 0x48 on 2025/4/1.
//

#ifndef MOBILEGLUES_VERTEXDATA_H
#define MOBILEGLUES_VERTEXDATA_H

#include "types.h"

GLAPI void GLAPIENTRY glBegin( GLenum mode );

GLAPI void GLAPIENTRY glEnd( void );


GLAPI void GLAPIENTRY glVertex2d( GLdouble x, GLdouble y );
GLAPI void GLAPIENTRY glVertex2f( GLfloat x, GLfloat y );
GLAPI void GLAPIENTRY glVertex2i( GLint x, GLint y );
GLAPI void GLAPIENTRY glVertex2s( GLshort x, GLshort y );

GLAPI void GLAPIENTRY glVertex3d( GLdouble x, GLdouble y, GLdouble z );
GLAPI void GLAPIENTRY glVertex3f( GLfloat x, GLfloat y, GLfloat z );
GLAPI void GLAPIENTRY glVertex3i( GLint x, GLint y, GLint z );
GLAPI void GLAPIENTRY glVertex3s( GLshort x, GLshort y, GLshort z );

GLAPI void GLAPIENTRY glVertex4d( GLdouble x, GLdouble y, GLdouble z, GLdouble w );
GLAPI void GLAPIENTRY glVertex4f( GLfloat x, GLfloat y, GLfloat z, GLfloat w );
GLAPI void GLAPIENTRY glVertex4i( GLint x, GLint y, GLint z, GLint w );
GLAPI void GLAPIENTRY glVertex4s( GLshort x, GLshort y, GLshort z, GLshort w );

GLAPI void GLAPIENTRY glVertex2dv( const GLdouble *v );
GLAPI void GLAPIENTRY glVertex2fv( const GLfloat *v );
GLAPI void GLAPIENTRY glVertex2iv( const GLint *v );
GLAPI void GLAPIENTRY glVertex2sv( const GLshort *v );

GLAPI void GLAPIENTRY glVertex3dv( const GLdouble *v );
GLAPI void GLAPIENTRY glVertex3fv( const GLfloat *v );
GLAPI void GLAPIENTRY glVertex3iv( const GLint *v );
GLAPI void GLAPIENTRY glVertex3sv( const GLshort *v );

GLAPI void GLAPIENTRY glVertex4dv( const GLdouble *v );
GLAPI void GLAPIENTRY glVertex4fv( const GLfloat *v );
GLAPI void GLAPIENTRY glVertex4iv( const GLint *v );
GLAPI void GLAPIENTRY glVertex4sv( const GLshort *v );


GLAPI void GLAPIENTRY glNormal3b( GLbyte nx, GLbyte ny, GLbyte nz );
GLAPI void GLAPIENTRY glNormal3d( GLdouble nx, GLdouble ny, GLdouble nz );
GLAPI void GLAPIENTRY glNormal3f( GLfloat nx, GLfloat ny, GLfloat nz );
GLAPI void GLAPIENTRY glNormal3i( GLint nx, GLint ny, GLint nz );
GLAPI void GLAPIENTRY glNormal3s( GLshort nx, GLshort ny, GLshort nz );

GLAPI void GLAPIENTRY glNormal3bv( const GLbyte *v );
GLAPI void GLAPIENTRY glNormal3dv( const GLdouble *v );
GLAPI void GLAPIENTRY glNormal3fv( const GLfloat *v );
GLAPI void GLAPIENTRY glNormal3iv( const GLint *v );
GLAPI void GLAPIENTRY glNormal3sv( const GLshort *v );


//GLAPI void GLAPIENTRY glIndexd( GLdouble c );
//GLAPI void GLAPIENTRY glIndexf( GLfloat c );
//GLAPI void GLAPIENTRY glIndexi( GLint c );
//GLAPI void GLAPIENTRY glIndexs( GLshort c );
//GLAPI void GLAPIENTRY glIndexub( GLubyte c );  /* 1.1 */
//
//GLAPI void GLAPIENTRY glIndexdv( const GLdouble *c );
//GLAPI void GLAPIENTRY glIndexfv( const GLfloat *c );
//GLAPI void GLAPIENTRY glIndexiv( const GLint *c );
//GLAPI void GLAPIENTRY glIndexsv( const GLshort *c );
//GLAPI void GLAPIENTRY glIndexubv( const GLubyte *c );  /* 1.1 */

GLAPI void GLAPIENTRY glColor3b( GLbyte red, GLbyte green, GLbyte blue );
GLAPI void GLAPIENTRY glColor3d( GLdouble red, GLdouble green, GLdouble blue );
GLAPI void GLAPIENTRY glColor3f( GLfloat red, GLfloat green, GLfloat blue );
GLAPI void GLAPIENTRY glColor3i( GLint red, GLint green, GLint blue );
GLAPI void GLAPIENTRY glColor3s( GLshort red, GLshort green, GLshort blue );
GLAPI void GLAPIENTRY glColor3ub( GLubyte red, GLubyte green, GLubyte blue );
GLAPI void GLAPIENTRY glColor3ui( GLuint red, GLuint green, GLuint blue );
GLAPI void GLAPIENTRY glColor3us( GLushort red, GLushort green, GLushort blue );

GLAPI void GLAPIENTRY glColor4b( GLbyte red, GLbyte green,
                                 GLbyte blue, GLbyte alpha );
GLAPI void GLAPIENTRY glColor4d( GLdouble red, GLdouble green,
                                 GLdouble blue, GLdouble alpha );
GLAPI void GLAPIENTRY glColor4f( GLfloat red, GLfloat green,
                                 GLfloat blue, GLfloat alpha );
GLAPI void GLAPIENTRY glColor4i( GLint red, GLint green,
                                 GLint blue, GLint alpha );
GLAPI void GLAPIENTRY glColor4s( GLshort red, GLshort green,
                                 GLshort blue, GLshort alpha );
GLAPI void GLAPIENTRY glColor4ub( GLubyte red, GLubyte green,
                                  GLubyte blue, GLubyte alpha );
GLAPI void GLAPIENTRY glColor4ui( GLuint red, GLuint green,
                                  GLuint blue, GLuint alpha );
GLAPI void GLAPIENTRY glColor4us( GLushort red, GLushort green,
                                  GLushort blue, GLushort alpha );


GLAPI void GLAPIENTRY glColor3bv( const GLbyte *v );
GLAPI void GLAPIENTRY glColor3dv( const GLdouble *v );
GLAPI void GLAPIENTRY glColor3fv( const GLfloat *v );
GLAPI void GLAPIENTRY glColor3iv( const GLint *v );
GLAPI void GLAPIENTRY glColor3sv( const GLshort *v );
GLAPI void GLAPIENTRY glColor3ubv( const GLubyte *v );
GLAPI void GLAPIENTRY glColor3uiv( const GLuint *v );
GLAPI void GLAPIENTRY glColor3usv( const GLushort *v );

GLAPI void GLAPIENTRY glColor4bv( const GLbyte *v );
GLAPI void GLAPIENTRY glColor4dv( const GLdouble *v );
GLAPI void GLAPIENTRY glColor4fv( const GLfloat *v );
GLAPI void GLAPIENTRY glColor4iv( const GLint *v );
GLAPI void GLAPIENTRY glColor4sv( const GLshort *v );
GLAPI void GLAPIENTRY glColor4ubv( const GLubyte *v );
GLAPI void GLAPIENTRY glColor4uiv( const GLuint *v );
GLAPI void GLAPIENTRY glColor4usv( const GLushort *v );


GLAPI void GLAPIENTRY glTexCoord1d( GLdouble s );
GLAPI void GLAPIENTRY glTexCoord1f( GLfloat s );
GLAPI void GLAPIENTRY glTexCoord1i( GLint s );
GLAPI void GLAPIENTRY glTexCoord1s( GLshort s );

GLAPI void GLAPIENTRY glTexCoord2d( GLdouble s, GLdouble t );
GLAPI void GLAPIENTRY glTexCoord2f( GLfloat s, GLfloat t );
GLAPI void GLAPIENTRY glTexCoord2i( GLint s, GLint t );
GLAPI void GLAPIENTRY glTexCoord2s( GLshort s, GLshort t );

GLAPI void GLAPIENTRY glTexCoord3d( GLdouble s, GLdouble t, GLdouble r );
GLAPI void GLAPIENTRY glTexCoord3f( GLfloat s, GLfloat t, GLfloat r );
GLAPI void GLAPIENTRY glTexCoord3i( GLint s, GLint t, GLint r );
GLAPI void GLAPIENTRY glTexCoord3s( GLshort s, GLshort t, GLshort r );

GLAPI void GLAPIENTRY glTexCoord4d( GLdouble s, GLdouble t, GLdouble r, GLdouble q );
GLAPI void GLAPIENTRY glTexCoord4f( GLfloat s, GLfloat t, GLfloat r, GLfloat q );
GLAPI void GLAPIENTRY glTexCoord4i( GLint s, GLint t, GLint r, GLint q );
GLAPI void GLAPIENTRY glTexCoord4s( GLshort s, GLshort t, GLshort r, GLshort q );

GLAPI void GLAPIENTRY glTexCoord1dv( const GLdouble *v );
GLAPI void GLAPIENTRY glTexCoord1fv( const GLfloat *v );
GLAPI void GLAPIENTRY glTexCoord1iv( const GLint *v );
GLAPI void GLAPIENTRY glTexCoord1sv( const GLshort *v );

GLAPI void GLAPIENTRY glTexCoord2dv( const GLdouble *v );
GLAPI void GLAPIENTRY glTexCoord2fv( const GLfloat *v );
GLAPI void GLAPIENTRY glTexCoord2iv( const GLint *v );
GLAPI void GLAPIENTRY glTexCoord2sv( const GLshort *v );

GLAPI void GLAPIENTRY glTexCoord3dv( const GLdouble *v );
GLAPI void GLAPIENTRY glTexCoord3fv( const GLfloat *v );
GLAPI void GLAPIENTRY glTexCoord3iv( const GLint *v );
GLAPI void GLAPIENTRY glTexCoord3sv( const GLshort *v );

GLAPI void GLAPIENTRY glTexCoord4dv( const GLdouble *v );
GLAPI void GLAPIENTRY glTexCoord4fv( const GLfloat *v );
GLAPI void GLAPIENTRY glTexCoord4iv( const GLint *v );
GLAPI void GLAPIENTRY glTexCoord4sv( const GLshort *v );


GLAPI void GLAPIENTRY glMultiTexCoord1d( GLenum target, GLdouble s );
GLAPI void GLAPIENTRY glMultiTexCoord1f( GLenum target, GLfloat s );
GLAPI void GLAPIENTRY glMultiTexCoord1i( GLenum target, GLint s );
GLAPI void GLAPIENTRY glMultiTexCoord1s( GLenum target, GLshort s );

GLAPI void GLAPIENTRY glMultiTexCoord2d( GLenum target, GLdouble s, GLdouble t );
GLAPI void GLAPIENTRY glMultiTexCoord2f( GLenum target, GLfloat s, GLfloat t );
GLAPI void GLAPIENTRY glMultiTexCoord2i( GLenum target, GLint s, GLint t );
GLAPI void GLAPIENTRY glMultiTexCoord2s( GLenum target, GLshort s, GLshort t );

GLAPI void GLAPIENTRY glMultiTexCoord3d( GLenum target, GLdouble s, GLdouble t, GLdouble r );
GLAPI void GLAPIENTRY glMultiTexCoord3f( GLenum target, GLfloat s, GLfloat t, GLfloat r );
GLAPI void GLAPIENTRY glMultiTexCoord3i( GLenum target, GLint s, GLint t, GLint r );
GLAPI void GLAPIENTRY glMultiTexCoord3s( GLenum target, GLshort s, GLshort t, GLshort r );

GLAPI void GLAPIENTRY glMultiTexCoord4d( GLenum target, GLdouble s, GLdouble t, GLdouble r, GLdouble q );
GLAPI void GLAPIENTRY glMultiTexCoord4f( GLenum target, GLfloat s, GLfloat t, GLfloat r, GLfloat q );
GLAPI void GLAPIENTRY glMultiTexCoord4i( GLenum target, GLint s, GLint t, GLint r, GLint q );
GLAPI void GLAPIENTRY glMultiTexCoord4s( GLenum target, GLshort s, GLshort t, GLshort r, GLshort q );

GLAPI void GLAPIENTRY glMultiTexCoord1dv( GLenum target, const GLdouble *v );
GLAPI void GLAPIENTRY glMultiTexCoord1fv( GLenum target, const GLfloat *v );
GLAPI void GLAPIENTRY glMultiTexCoord1iv( GLenum target, const GLint *v );
GLAPI void GLAPIENTRY glMultiTexCoord1sv( GLenum target, const GLshort *v );

GLAPI void GLAPIENTRY glMultiTexCoord2dv( GLenum target, const GLdouble *v );
GLAPI void GLAPIENTRY glMultiTexCoord2fv( GLenum target, const GLfloat *v );
GLAPI void GLAPIENTRY glMultiTexCoord2iv( GLenum target, const GLint *v );
GLAPI void GLAPIENTRY glMultiTexCoord2sv( GLenum target, const GLshort *v );

GLAPI void GLAPIENTRY glMultiTexCoord3dv( GLenum target, const GLdouble *v );
GLAPI void GLAPIENTRY glMultiTexCoord3fv( GLenum target, const GLfloat *v );
GLAPI void GLAPIENTRY glMultiTexCoord3iv( GLenum target, const GLint *v );
GLAPI void GLAPIENTRY glMultiTexCoord3sv( GLenum target, const GLshort *v );

GLAPI void GLAPIENTRY glMultiTexCoord4dv( GLenum target, const GLdouble *v );
GLAPI void GLAPIENTRY glMultiTexCoord4fv( GLenum target, const GLfloat *v );
GLAPI void GLAPIENTRY glMultiTexCoord4iv( GLenum target, const GLint *v );
GLAPI void GLAPIENTRY glMultiTexCoord4sv( GLenum target, const GLshort *v );


GLAPI void GLAPIENTRY glRasterPos2d( GLdouble x, GLdouble y );
GLAPI void GLAPIENTRY glRasterPos2f( GLfloat x, GLfloat y );
GLAPI void GLAPIENTRY glRasterPos2i( GLint x, GLint y );
GLAPI void GLAPIENTRY glRasterPos2s( GLshort x, GLshort y );

GLAPI void GLAPIENTRY glRasterPos3d( GLdouble x, GLdouble y, GLdouble z );
GLAPI void GLAPIENTRY glRasterPos3f( GLfloat x, GLfloat y, GLfloat z );
GLAPI void GLAPIENTRY glRasterPos3i( GLint x, GLint y, GLint z );
GLAPI void GLAPIENTRY glRasterPos3s( GLshort x, GLshort y, GLshort z );

GLAPI void GLAPIENTRY glRasterPos4d( GLdouble x, GLdouble y, GLdouble z, GLdouble w );
GLAPI void GLAPIENTRY glRasterPos4f( GLfloat x, GLfloat y, GLfloat z, GLfloat w );
GLAPI void GLAPIENTRY glRasterPos4i( GLint x, GLint y, GLint z, GLint w );
GLAPI void GLAPIENTRY glRasterPos4s( GLshort x, GLshort y, GLshort z, GLshort w );

GLAPI void GLAPIENTRY glRasterPos2dv( const GLdouble *v );
GLAPI void GLAPIENTRY glRasterPos2fv( const GLfloat *v );
GLAPI void GLAPIENTRY glRasterPos2iv( const GLint *v );
GLAPI void GLAPIENTRY glRasterPos2sv( const GLshort *v );

GLAPI void GLAPIENTRY glRasterPos3dv( const GLdouble *v );
GLAPI void GLAPIENTRY glRasterPos3fv( const GLfloat *v );
GLAPI void GLAPIENTRY glRasterPos3iv( const GLint *v );
GLAPI void GLAPIENTRY glRasterPos3sv( const GLshort *v );

GLAPI void GLAPIENTRY glRasterPos4dv( const GLdouble *v );
GLAPI void GLAPIENTRY glRasterPos4fv( const GLfloat *v );
GLAPI void GLAPIENTRY glRasterPos4iv( const GLint *v );
GLAPI void GLAPIENTRY glRasterPos4sv( const GLshort *v );


GLAPI void GLAPIENTRY glRectd( GLdouble x1, GLdouble y1, GLdouble x2, GLdouble y2 );
GLAPI void GLAPIENTRY glRectf( GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2 );
GLAPI void GLAPIENTRY glRecti( GLint x1, GLint y1, GLint x2, GLint y2 );
GLAPI void GLAPIENTRY glRects( GLshort x1, GLshort y1, GLshort x2, GLshort y2 );


GLAPI void GLAPIENTRY glRectdv( const GLdouble *v1, const GLdouble *v2 );
GLAPI void GLAPIENTRY glRectfv( const GLfloat *v1, const GLfloat *v2 );
GLAPI void GLAPIENTRY glRectiv( const GLint *v1, const GLint *v2 );
GLAPI void GLAPIENTRY glRectsv( const GLshort *v1, const GLshort *v2 );


#endif //MOBILEGLUES_VERTEXDATA_H
