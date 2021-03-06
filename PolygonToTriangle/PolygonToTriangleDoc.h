#pragma once
#include "CP_Polygon.h"
#include "CP_Triangle.h"

class CPolygonToTriangleDoc : public CDocument
{
protected:
	CPolygonToTriangleDoc();
	DECLARE_DYNCREATE(CPolygonToTriangleDoc)


public:
	// //////////////////////////////////////////////////////////////
	
	CP_Polygon m_a, m_b;
	double m_tolerance;
	double m_scale;
	CP_Point m_translation;
	// //////////////////////////////////////////////////////////////
	
	CP_Polygon m_result;
	CP_TriagleMesh m_triagleMeshA;
	CP_TriagleMesh m_triagleMeshB;
	bool m_flagBuildA;
	bool m_flagSelect;
	int  m_flagSelectType;

	int m_flagSelectPolygon, m_flagSelectRegion, m_flagSelectID;
	

	VT_IntArray m_flagSelectIDSetInA, m_flagSelectIDSetInB;
	bool m_flagShowSelect;
	int  m_edgeNumber;
	int  m_flagAdd;
	int  m_flagAddIDPolygon, m_flagAddIDRegion, m_flagAddIDLoop, m_flagAddIDPointInLoop;
	VT_PointArray m_flagAddPointArray;
	bool m_flagShowA, m_flagShowB, m_flagShowPointID;
	bool m_flagShowBezier;

	bool m_flagMoveSame;
	bool m_flagShowTriangleFace;
	bool m_flagShowTriangleEdge;
	bool m_flagShowTriangleEdgeFace;
	
	CP_Point m_basePoint;
	bool m_flagMouseDown;
	bool m_solid_color_fill;
	double m_legal_check_time;
	double m_triangulation_time;

public:
	void mb_initData();

public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif


public:
	virtual ~CPolygonToTriangleDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
};

extern void gb_SerializePolygon(CArchive& ar, CP_Polygon& p);
