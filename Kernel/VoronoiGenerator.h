/*
MapManager library for the conversion, manipulation and analysis 
of maps used in Mobile Robotics research.
Copyright (C) 2005 Shane O'Sullivan

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

email: shaneosullivan1@gmail.com
*/

/* Lightly modified by George Moromisato for incorporation into Hexarc
*/

#ifndef VORONOI_DIAGRAM_GENERATOR
#define VORONOI_DIAGRAM_GENERATOR

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

//#define DEF_PLOT

#define DEF_LOG					/##/
#define GET_FILE_LOG			/##/
#define ENTRYEXIT_LOG_ON		/##/
#define ENTRYEXIT_LOG_OFF		/##/
#define LOGGING_OFF				/##/
#define GET_SCREEN_LOG			/##/
#define GET_FILE_LOG_GLOBAL		/##/
#define GET_SCREEN_LOG_GLOBAL   /##/
#define LOGGING_OFF_GLOBAL		/##/
#define LOG						/##/
#define LOGTIME					/##/
#define LOGENTRY(a)				/##/
#define LOGEXIT(a)				/##/
#define LOGCODE					/##/
#define LOGCODEEND				*##/
#define LOGCODESTART			/##*

#ifndef NULL
#define NULL 0
#endif

typedef double VFLOAT;

class VoronoiDiagramGenerator
{
public:
	enum Constants
	{
		DELETED =			-2,
		le =				0,
		re =				1,
	};

	struct	Freenode	
	{
		struct	Freenode *nextfree;
	};

	struct FreeNodeArrayList
	{
		struct	Freenode* memory;
		struct	FreeNodeArrayList* next;

	};

	struct	Freelist	
	{
		struct	Freenode	*head;
		int		nodesize;
	};

	struct PointVDG	
	{
		VFLOAT x,y;
	};

	struct Point3
	{
		VFLOAT x,y,z;
		int count;
	};

	struct VertexLink
	{
		PointVDG coord;
		PointVDG v[3];
		int count;
	};

	// structure used both for sites and for vertices 
	struct Site	
	{
		struct	PointVDG	coord;
		int		sitenbr;
		int		refcnt;
		int		overallRefcnt;
	};

	struct Edge	
	{
		VFLOAT   a,b,c;
		struct	Site 	*ep[2];
		struct	Site	*reg[2];
		int		edgenbr;

	};

	struct GraphEdge
	{
		VFLOAT x1,y1,x2,y2;
		long s1, s2;
		struct GraphEdge* next;
	};

	struct Halfedge 
	{
		struct	Halfedge	*ELleft, *ELright;
		struct	Edge	*ELedge;
		int		ELrefcnt;
		char	ELpm;
		struct	Site	*vertex;
		VFLOAT	ystar;
		struct	Halfedge *PQnext;
	};

	VoronoiDiagramGenerator();
	~VoronoiDiagramGenerator();

	bool generateVoronoi(VFLOAT *xValues, VFLOAT *yValues, int numPoints, 
		VFLOAT minX, VFLOAT maxX, VFLOAT minY, VFLOAT maxY, VFLOAT minDist,bool genVectorInfo=true);

	//By default, the delaunay triangulation is NOT generated
	void setGenerateDelaunay(bool genDel);

	//By default, the voronoi diagram IS generated
	void setGenerateVoronoi(bool genVor);

	void resetIterator()
	{
		iteratorEdges = allEdges;
	}

	bool getNext(VFLOAT& x1, VFLOAT& y1, VFLOAT& x2, VFLOAT& y2, int *retiSite1 = NULL, int *retiSite2 = NULL)
	{
		if(iteratorEdges == 0)
			return false;

		x1 = iteratorEdges->x1;
		x2 = iteratorEdges->x2;
		y1 = iteratorEdges->y1;
		y2 = iteratorEdges->y2;

		if (retiSite1)
			*retiSite1 = iteratorEdges->s1;
		if (retiSite2)
			*retiSite2 = iteratorEdges->s2;

//		LOG<<"getNext returned the edge ("<<x1<<","<<y1<<") -> ("<<x2<<","<<y2<<")";

		iteratorEdges = iteratorEdges->next;

		return true;
	}

	int getEdgeCount (void)
	{
		int count = 0;

		GraphEdge *next = allEdges;
		while (next)
		{
			count++;
			next = next->next;
		}

		return count;
	}
	
	void resetDelaunayEdgesIterator()
	{
		iteratorDelaunayEdges = delaunayEdges;
		LOG<<"resetDelaunayEdgesIterator set iteratorDelaunayEdges = "<<iteratorDelaunayEdges;
	}

	bool getNextDelaunay(VFLOAT& x1, VFLOAT& y1, VFLOAT& x2, VFLOAT& y2)
	{
		if(iteratorDelaunayEdges == 0)
		{
			LOG<<"iteratorDelaunayEdges = 0, returning false";
			return false;
		}
		x1 = iteratorDelaunayEdges->x1;
		x2 = iteratorDelaunayEdges->x2;
		y1 = iteratorDelaunayEdges->y1;
		y2 = iteratorDelaunayEdges->y2;

		iteratorDelaunayEdges = iteratorDelaunayEdges->next;

		LOG<<"getNextDelaunay returned the edge ("<<x1<<","<<y1<<") -> ("<<x2<<","<<y2<<")";

		return true;
	}

	void resetVertexPairIterator()
	{
		currentVertexLink = 0;
	}

	bool getNextVertexPair(VFLOAT& x1, VFLOAT& y1, VFLOAT& x2, VFLOAT& y2);

	void resetVerticesIterator()
	{
		currentVertex = 0;
	}

	bool getNextVertex(VFLOAT& x, VFLOAT& y)
	{
		if(finalVertices == 0)
			return false;

		if(currentVertex >= sizeOfFinalVertices) return false;
		x = finalVertices[currentVertex].x;
		y = finalVertices[currentVertex].y;
		currentVertex++;
		return true;
	}

	int getVerticesCount (void)
	{
		return sizeOfFinalVertices;
	}

	void reset();


private:
	void cleanup();
	void cleanupEdges();
	char *getfree(struct Freelist *fl);	
	struct	Halfedge *PQfind();
	int PQempty();
	
	struct	Halfedge **ELhash;
	struct	Halfedge *HEcreate(), *ELleft(), *ELright(), *ELleftbnd();
	struct	Halfedge *HEcreate(struct Edge *e,int pm);


	struct PointVDG PQ_min();
	struct Halfedge *PQextractmin();	
	void freeinit(struct Freelist *fl,int size);
	void makefree(struct Freenode *curr,struct Freelist *fl);
	void geominit();
#ifdef DEF_PLOT
	void plotinit();
#endif
	bool voronoi(bool genVectorInfo);
	void ref(struct Site *v);
	void deref(struct Site *v);
	void endpoint(struct Edge *e,int lr,struct Site * s);

	void ELdelete(struct Halfedge *he);
	struct Halfedge *ELleftbnd(struct PointVDG *p);
	struct Halfedge *ELright(struct Halfedge *he);
	void makevertex(struct Site *v);
	void out_triple(struct Site *s1, struct Site *s2,struct Site * s3);
	
	void		PQinsert(struct Halfedge *he,struct Site * v, VFLOAT offset);
	void		PQdelete(struct Halfedge *he);
	bool		ELinitialize();
	void		ELinsert(struct	Halfedge *lb, struct Halfedge *newHe);
	struct Halfedge * VoronoiDiagramGenerator::ELgethash(int b);
	struct Halfedge *ELleft(struct Halfedge *he);
	struct Site *leftreg(struct Halfedge *he);
	void		out_site(struct Site *s);
	bool		PQinitialize();
	int			PQbucket(struct Halfedge *he);
	void		clip_line(struct Edge *e);
	char		*myalloc(unsigned n);
	int			right_of(struct Halfedge *el,struct PointVDG *p);

	struct Site *rightreg(struct Halfedge *he);
	struct Edge *bisect(struct	Site *s1,struct	Site *s2);
	VFLOAT dist(struct Site *s,struct Site *t);
	struct Site *intersect(struct Halfedge *el1, struct Halfedge *el2, struct PointVDG *p=0);

	void		out_bisector(struct Edge *e);
	void		out_ep(struct Edge *e);
	void		out_vertex(struct Site *v);
	struct Site *nextone();

	void		pushGraphEdge(VFLOAT x1, VFLOAT y1, VFLOAT x2, VFLOAT y2, long s1, long s2);
	void		pushDelaunayGraphEdge(VFLOAT x1, VFLOAT y1, VFLOAT x2, VFLOAT y2);

#ifdef DEF_PLOT
	void		openpl();
	void		line(VFLOAT x1, VFLOAT y1, VFLOAT x2, VFLOAT y2);
	void		circle(VFLOAT x, VFLOAT y, VFLOAT radius);
	void		range(VFLOAT minX, VFLOAT minY, VFLOAT maxX, VFLOAT maxY);
#endif

	void  		insertVertexAddress(long vertexNum, struct Site* address);
	void		insertVertexLink(long vertexNum, long vertexLinkedTo);
	void		generateVertexLinks();

	static int scomp(const void *p1,const void *p2);

	bool		genDelaunay;
	bool		genVoronoi;

	struct		Freelist	hfl;
	struct		Halfedge *ELleftend, *ELrightend;
	int 		ELhashsize;

	int			triangulate, sorted, plot, debug;
	VFLOAT		xmin, xmax, ymin, ymax, deltax, deltay;

	struct		Site	*sites;
	struct		Site	*saved_sites;
	int			nsites;
	int			siteidx;
	int			sqrt_nsites;
	int			nvertices;
	struct 		Freelist sfl;
	struct		Site	*bottomsite;

	int			nedges;
	struct		Freelist efl;
	int			PQhashsize;
	struct		Halfedge *PQhash;
	int			PQcount;
	int			PQmin;

	int			ntry, totalsearch;
	VFLOAT		pxmin, pxmax, pymin, pymax, cradius;
	int			total_alloc;

	VFLOAT		borderMinX, borderMaxX, borderMinY, borderMaxY;

	FreeNodeArrayList* allMemoryList;
	FreeNodeArrayList* currentMemoryBlock;

	GraphEdge*	allEdges;
	GraphEdge*	iteratorEdges;

	GraphEdge*	delaunayEdges;
	GraphEdge*	iteratorDelaunayEdges;

	Point3*		vertexLinks; //lists all the vectors that each vector is directly connected to	
	long		sizeOfVertexLinks;
	
	Site**		vertices;
	long		sizeOfVertices ;

	VertexLink* finalVertexLinks;
	long 		sizeOfFinalVertexLinks;
	long		currentVertexLink;

	PointVDG	*finalVertices;	
	long		sizeOfFinalVertices ;	
	long 		currentVertex;

	VFLOAT		minDistanceBetweenSites;

	DEF_LOG
};

#endif


