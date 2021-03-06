// 这段 MFC 示例源代码演示如何使用 MFC Microsoft Office Fluent 用户界面 
// (“Fluent UI”)。该示例仅供参考，
// 用以补充《Microsoft 基础类参考》和 
// MFC C++ 库软件随附的相关电子文档。  
// 复制、使用或分发 Fluent UI 的许可条款是单独提供的。  
// 若要了解有关 Fluent UI 许可计划的详细信息，请访问  
// http://go.microsoft.com/fwlink/?LinkId=238214。
//
// 版权所有(C) Microsoft Corporation
// 保留所有权利。

// PolygonToTriangleDoc.cpp : CPolygonToTriangleDoc 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "PolygonToTriangle.h"
#endif

#include "PolygonToTriangleDoc.h"

#include <propkey.h>
#include <sstream>
#include <string>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CPolygonToTriangleDoc

IMPLEMENT_DYNCREATE(CPolygonToTriangleDoc, CDocument)

BEGIN_MESSAGE_MAP(CPolygonToTriangleDoc, CDocument)
END_MESSAGE_MAP()


// CPolygonToTriangleDoc 构造/析构

CPolygonToTriangleDoc::CPolygonToTriangleDoc()
{
	// TODO:  在此添加一次性构造代码
	mb_initData();
}

CPolygonToTriangleDoc::~CPolygonToTriangleDoc()
{
}

void CPolygonToTriangleDoc::mb_initData()
{
	m_a.mb_clear();
	m_b.mb_clear();
	m_tolerance = 1e-6; // 位置容差
	m_scale = 1.0; // 缩放比例
	m_translation.m_x = 0.0; // 坐标平移量
	m_translation.m_y = 0.0; // 坐标平移量
	m_result.mb_clear();
	m_flagBuildA = true; // true: A; false B。
	m_flagSelect = false;
	m_flagSelectType = 0;
	m_flagSelectPolygon = 0;
	m_flagSelectRegion = 0;
	m_flagSelectID = 0; // 定位拾取的内容
	m_flagShowSelect = false; // true:只显示选择集。
	m_edgeNumber = 3; // 正多边形的边数。
	m_flagMouseDown = false; // true: 按下鼠标左键
	m_flagAdd = 0;
	m_flagShowA = true;
	m_flagShowB = true;
	m_flagShowPointID = false;
	m_flagMoveSame = false;
	m_flagSelectIDSetInA.clear();
	m_flagSelectIDSetInB.clear();
	m_triagleMeshA.mb_clear();
	m_triagleMeshB.mb_clear();
	m_flagShowTriangleFace = false;  // true: 显示; false: 不显示。
	m_flagShowTriangleEdge = false;  // true: 显示; false: 不显示。
	m_flagShowTriangleEdgeFace = false; // true: 显示; false: 不显示。
	m_flagShowBezier = false;
	m_solid_color_fill = false;
	m_legal_check_time = 0;
	m_triangulation_time = 0;
} // 类CCP_PolygonPlatformDoc的成员函数mb_initData结束


BOOL CPolygonToTriangleDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO:  在此添加重新初始化代码
	// (SDI 文档将重用该文档)
	mb_initData();
	CString string;
	CMFCRibbonBar* robbon_bar = ((CFrameWndEx*)AfxGetMainWnd())->GetRibbonBar(); //获取Ribbon bar 句柄
	if (robbon_bar == NULL)
		return TRUE;
	CMFCRibbonEdit* slider = (CMFCRibbonEdit*)robbon_bar->FindByID(ID_TOLERANCE); // 获取编辑框句柄
	if (slider == NULL)
		return TRUE;
	string.Format(_T("%g"), m_tolerance);
	slider->SetEditText(string);
	CMFCRibbonComboBox* pbox = (CMFCRibbonComboBox*)robbon_bar->FindByID(ID_COMBO_AorB); // 获取编辑框句柄
	if (pbox == NULL)
		return TRUE;
	pbox->AddItem(_T("多边形A"));
	pbox->AddItem(_T("多边形B"));
	pbox->SelectItem(0);
	return TRUE;
}




// CPolygonToTriangleDoc 序列化

void CPolygonToTriangleDoc::Serialize(CArchive& ar)
{
	CString line;
	if (ar.IsStoring())
	{
		ar.WriteString(_T("# Polygon Data(雍俊海: 计算机辅助几何造型(清华大学, 2014秋))\r\n"));
		ar.WriteString(_T("# 文件创建时间: "));
		SYSTEMTIME st;
		CString strDate, strTime;
		GetLocalTime(&st);
		strDate.Format(_T("%4d-%2d-%2d "), st.wYear, st.wMonth, st.wDay);
		strTime.Format(_T("%2d:%2d:%2d\r\n\r\n"), st.wHour, st.wMinute, st.wSecond);
		ar.WriteString(strDate.GetString());
		ar.WriteString(strTime.GetString());
		line.Format(_T("Tolerance %g\r\n\r\n"), m_tolerance);
		ar.WriteString(line.GetString());
		line.Format(_T("Coordinate %g %g %g\r\n\r\n"), m_scale, m_translation.m_x, m_translation.m_y);
		ar.WriteString(line.GetString());
		line.Format(_T("A Polygon\r\n\r\n"));
		ar.WriteString(line.GetString());
		gb_SerializePolygon(ar, m_a);
		line.Format(_T("B Polygon\r\n\r\n"));
		ar.WriteString(line.GetString());
		gb_SerializePolygon(ar, m_b);
	}
	else
	{
		// TODO:  在此添加加载代码
		mb_initData();
		while (ar.ReadString(line))
		{
			if (!line.IsEmpty())
			{
				if (line[0] == 'T')
				{
					wstring temp;
					wstringstream ss(line.GetString());
					ss >> temp;
					ss >> m_tolerance;
					break;
				}
			}
		}
		while (ar.ReadString(line))
		{
			if (!line.IsEmpty())
			{
				if (line[0] == 'C')
				{
					wstring temp;
					wstringstream ss(line.GetString());
					ss >> temp;
					ss >> m_scale >> m_translation.m_x >> m_translation.m_y;
					break;
				}
			}
		} // while结束
		while (ar.ReadString(line))
		{
			if (!line.IsEmpty())
			{
				if (line[0] == 'A')
				{
					gb_SerializePolygon(ar, m_a);
					break;
				}
			}
		} // while结束
		while (ar.ReadString(line))
		{
			if (!line.IsEmpty())
			{
				if (line[0] == 'B')
				{
					gb_SerializePolygon(ar, m_b);
					break;
				}
			}
		} // while结束
		CString string;
		CMFCRibbonBar* robbon_bar = ((CFrameWndEx*)AfxGetMainWnd())->GetRibbonBar(); //获取Ribbon bar 句柄
		if (robbon_bar == NULL)
			return;
		CMFCRibbonEdit* slider = (CMFCRibbonEdit*)robbon_bar->FindByID(ID_TOLERANCE); // 获取编辑框句柄
		if (slider==NULL)
			return;
		string.Format(_T("%g"), m_tolerance);
		slider->SetEditText(string);
		CMFCRibbonComboBox* pbox = (CMFCRibbonComboBox*)robbon_bar->FindByID(ID_COMBO_AorB); // 获取编辑框句柄
		if (pbox==NULL)
			return;
		pbox->AddItem(_T("多边形A"));
		pbox->AddItem(_T("多边形B"));
		pbox->SelectItem(0);
	}
}

void gb_SerializePolygon(CArchive& ar, CP_Polygon& p)
{
	CString line;
	unsigned int i, j, k;
	if (ar.IsStoring())
	{	// storing code
		line.Format(_T("Pointsize %d\r\n"), p.m_pointArray.size( ));
		ar.WriteString(line.GetString());
		for (i=0; i<p.m_pointArray.size( ); i++)
		{
			line.Format(_T("%g %g\r\n"), p.m_pointArray[i].m_x, p.m_pointArray[i].m_y);
			ar.WriteString(line.GetString());
		} // for结束
		line.Format(_T("\r\nRegionsize %d\r\n"), p.m_regionArray.size( ));
		ar.WriteString(line.GetString()); 
		for (i = 0; i<p.m_regionArray.size(); i++)
		{
			line.Format(_T("Region %d\r\n"), i);
			ar.WriteString(line.GetString());
			line.Format(_T("Loopsize %d\r\n"), p.m_regionArray[i].m_loopArray.size( ));
			ar.WriteString(line.GetString());
			for (j=0; j<p.m_regionArray[i].m_loopArray.size( ); j++)
			{
				line.Format(_T("Loop %d\r\n"), j);
				ar.WriteString(line.GetString());
				line.Format(_T("PointIDsize %d\r\n"), p.m_regionArray[i].m_loopArray[j].m_pointIDArray.size( ));
				ar.WriteString(line.GetString());
				for (k=0; k<p.m_regionArray[i].m_loopArray[j].m_pointIDArray.size( ); k++)
				{
					line.Format(_T("%d "), p.m_regionArray[i].m_loopArray[j].m_pointIDArray[k]);
					ar.WriteString(line.GetString());
				} // for结束
				if (p.m_regionArray[i].m_loopArray[j].m_pointIDArray.size( )>0)
				{
					line.Format(_T("\r\n"));
					ar.WriteString(line.GetString());
				}
			} // for结束
		} // for结束
		line.Format(_T("\r\n"));
		ar.WriteString(line.GetString());
	}
	else
	{	// loading code
		p.m_pointArray.clear( );
		p.m_regionArray.clear( );
		while(ar.ReadString(line))
		{
			if (!line.IsEmpty())
			{
				if (line[0] == 'P')
				{
					wstring temp;
					wstringstream ss(line.GetString());
					ss >> temp;
					ss >> i;
					break;
				}
			}
		} // while结束
		if (i==0)
			return;
		p.m_pointArray.resize(i);
		for (i=0; i<p.m_pointArray.size( ); i++)
		{
			ar.ReadString(line);
			wstringstream ss(line.GetString());
			ss >> p.m_pointArray[i].m_x;
			ss >> p.m_pointArray[i].m_y;
		} // for结束
		while (ar.ReadString(line))
		{
			if (!line.IsEmpty())
			{
				if (line[0] == 'R')
				{
					wstring temp;
					wstringstream ss(line.GetString());
					ss >> temp;
					ss >> i;
					break;
				}
			}
		} // while结束
		if (i == 0)
			return;
		p.m_regionArray.resize(i);
		for (i = 0; i<p.m_regionArray.size(); i++)
		{
			p.m_regionArray[i].m_polygon = &p;
			p.m_regionArray[i].m_regionIDinPolygon = i;
			ar.ReadString(line);
			ar.ReadString(line);
			wstring temp;
			wstringstream ss(line.GetString());
			ss >> temp;
			ss >> j;
			p.m_regionArray[i].m_loopArray.resize(j);
			for (j = 0; j<p.m_regionArray[i].m_loopArray.size(); j++)
			{
				p.m_regionArray[i].m_loopArray[j].m_polygon = &p;
				p.m_regionArray[i].m_loopArray[j].m_regionIDinPolygon = i;
				p.m_regionArray[i].m_loopArray[j].m_loopIDinRegion = j;
				ar.ReadString(line);
				ar.ReadString(line);
				wstring temp;
				wstringstream ss(line.GetString());
				ss >> temp;
				ss >> k;
				p.m_regionArray[i].m_loopArray[j].m_pointIDArray.resize(k);
				if (k>0)
				{
					ar.ReadString(line);
					wstringstream ss(line.GetString());
					for (k = 0; k<p.m_regionArray[i].m_loopArray[j].m_pointIDArray.size(); k++)
					{
						ss >> p.m_regionArray[i].m_loopArray[j].m_pointIDArray[k];
					} // for结束
				} // if结束
			} // for结束
		} // for结束
	} // 外围if/else结束
} // 函数gb_SerializePolygon结束


#ifdef SHARED_HANDLERS

// 缩略图的支持
void CPolygonToTriangleDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// 修改此代码以绘制文档数据
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// 搜索处理程序的支持
void CPolygonToTriangleDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// 从文档数据设置搜索内容。
	// 内容部分应由“;”分隔

	// 例如:     strSearchContent = _T("point;rectangle;circle;ole object;")；
	SetSearchContent(strSearchContent);
}

void CPolygonToTriangleDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = NULL;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != NULL)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CPolygonToTriangleDoc 诊断

#ifdef _DEBUG
void CPolygonToTriangleDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CPolygonToTriangleDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG