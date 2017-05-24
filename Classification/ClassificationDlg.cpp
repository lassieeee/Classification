
// PPDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Classification.h"
#include "ClassificationDlg.h"
#include "afxdialogex.h"

#include "..\sift\sift.h"
#include "..\sift\imgfeatures.h"
#include "..\sift\utils.h"
#include "..\lda\lda.h"
#include "..\lda\document.h"
#include "..\lda\file_access.h"
#include <highgui.h>
#include <cv.h>

#include "Ini.h"

using namespace std;
using namespace cv;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CPPDlg �Ի���
CPPDlg::CPPDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CPPDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_kmK = 2;
	m_pathOpen = _T("");
	m_ldaK = 2;
	m_pathSave = _T("");
}

void CPPDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_KM_K, m_kmK);
	DDV_MinMaxInt(pDX, m_kmK, 2, 9999);
	DDX_Text(pDX, IDC_PATH_OPEN, m_pathOpen);
	DDX_Check(pDX, IDC_CHECK_SAVE_SIFT_DRAW, m_isSiftDraw);
	DDX_Text(pDX, IDC_LDA_K, m_ldaK);
	DDV_MinMaxInt(pDX, m_ldaK, 2, 9999);
	DDX_Text(pDX, IDC_PATH_SAVE, m_pathSave);
	DDX_Control(pDX, IDC_INFO, m_info);
}

BEGIN_MESSAGE_MAP(CPPDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_SIFT, &CPPDlg::OnBnClickedBtnSift)
	ON_BN_CLICKED(IDC_BTN_KMEANS, &CPPDlg::OnBnClickedBtnKmeans)
	ON_BN_CLICKED(IDC_BTN_WORDF, &CPPDlg::OnBnClickedBtnWordf)
	ON_BN_CLICKED(IDC_BTN_LDA, &CPPDlg::OnBnClickedBtnLda)
	ON_EN_CHANGE(IDC_PATH_OPEN, &CPPDlg::OnEnChangePathOpen)
	ON_EN_CHANGE(IDC_PATH_SAVE, &CPPDlg::OnEnChangePathSave)
END_MESSAGE_MAP()


// CPPDlg ��Ϣ�������

BOOL CPPDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	m_pathOpen = ini.openDir;
	m_pathSave = ini.saveDir;
	m_kmK = ini.kmeans_k;
	m_ldaK = ini.lda_k;
	m_isSiftDraw = ini.isSiftDraw;
	UpdateData(FALSE);

	m_info.AddString(L"����״̬��Ϣ��");

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CPPDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CPPDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CPPDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CPPDlg::OnBnClickedBtnSift()
{
	CFileFind finder;
	CString &openDir = ini.openDir;
	CString &saveDir = ini.saveDir;
	CString siftDrawDir;
	CString featureDir;
	CString fileName;

	UpdateData();
	openDir = m_pathOpen;
	saveDir = m_pathSave;
	ini.isSiftDraw = m_isSiftDraw;
	siftDrawDir = ini.getSiftDirPath();
	featureDir = ini.getFeatureDirPath();
	BOOL haveNext = finder.FindFile(openDir + "\\*.*");

	//����ļ���·��
	if(haveNext)
	{
		if(!PathIsDirectory(openDir))
		{
			MessageBox(L"ͼƬԴĿ¼·������ȷ��");
			return;
		}
		if(!PathIsDirectory(saveDir) && !CreateDirectory(saveDir, NULL))
		{
			MessageBox(L"��������ļ��д���ʧ�ܣ�");
			return;
		}
		if(m_isSiftDraw && !PathIsDirectory(siftDrawDir) && !CreateDirectory(siftDrawDir, NULL))
		{
			MessageBox(L"ͼƬ���������ļ��д���ʧ�ܣ�");
			return; 
		}
		if(!PathIsDirectory(featureDir) && !CreateDirectory(featureDir, NULL))
		{
			MessageBox(L"�����ļ��д���ʧ�ܣ�");
			return; 
		}
	}
	else
	{
		MessageBox(L"û�з���ͼƬ�ļ���");
	}

	//Do sift
	appendInfo("------������ȡ��ʼ------", 1);
	CStdioFile file;
	file.Open(ini.getImgListFilePath(), CStdioFile::modeCreate | CStdioFile::modeWrite);
	int featureSum = 0;
	while(haveNext) 
	{ 
		haveNext = finder.FindNextFile(); 
		if(finder.IsDirectory()) 
			continue;

		fileName = finder.GetFileName();
		file.WriteString(fileName + L"\n");
		
		//����ļ���·��
		char* img_file_name = CIni::CStrToChar(openDir + "\\" + fileName);
		char* out_img_name = CIni::CStrToChar(siftDrawDir + "\\" + fileName);
		char* out_file_name  = CIni::CStrToChar(featureDir + "\\" + ini.convertToFeatureSuffix(fileName));

		IplImage* img;
		struct feature* features;
		int n = 0;

		CString info("Sift ");
		info.Append(fileName);
		appendInfo(info);
		img = cvLoadImage(img_file_name, 1);
		if(!img)
		{
			return;
		}
		n = _sift_features(img, &features, SIFT_INTVLS/*3*/, SIFT_SIGMA/*1.6*/, SIFT_CONTR_THR/*0.04*/,
			SIFT_CURV_THR/*10*/, SIFT_IMG_DBL/*1*/, SIFT_DESCR_WIDTH/*4*/, SIFT_DESCR_HIST_BINS/*8*/); //SIFTfeature��ȡ
		featureSum += n;
		if( out_file_name != NULL )
			export_features(out_file_name, features, n); //��features����Ϊ�ļ�
		if(m_isSiftDraw && out_img_name != NULL)
		{
			draw_features(img, features, n); //��imgͼƬ�ϱ�ǳ�features
			cvSaveImage(out_img_name, img, NULL); //����Ǻ��ͼƬ����
		}
	}
	file.Close();

	CString temp;
	temp.Format(L"%d", featureSum);
	file.Open(ini.getInfoFilePath(), CStdioFile::modeCreate | CStdioFile::modeWrite);
	file.Close();
	WritePrivateProfileString(L"temp", L"featureSum", temp, ini.getInfoFilePath());

	appendInfo("�����ļ�����·����");
	appendInfo(featureDir, 1);
	if(m_isSiftDraw)
	{
		appendInfo("����ͼ����·����");
		appendInfo(siftDrawDir, 1);
	}
	appendInfo("------������ȡ����------", 1);
	appendInfo(" ", 1);
}


void CPPDlg::OnBnClickedBtnKmeans()
{
	UpdateData();
	CString strLine;
	CStdioFile file;
	if(!file.Open(ini.getImgListFilePath(), CStdioFile::modeRead))
	{
		MessageBox(L"ԭʼͼƬ�ļ���·���б��ļ���ʧ�ܣ�");
		return;
	}

	CString featureDir = ini.getFeatureDirPath();
	CString info;
	CString fileName;
	int featureNum = GetPrivateProfileInt(L"temp", L"featureSum", NULL, ini.getInfoFilePath());

	appendInfo("------Kmeans����------", 1);
	struct feature* features;
	int dims = 128;
	int n = 0;
	int &k = ini.kmeans_k;
	k = m_kmK;
	int temp = 0;
	CvMat *samples=cvCreateMat(featureNum, dims, CV_32FC1); //��������ͼƬ������feature��Ϣ�ľ���featureNum��feature��ÿ��featureΪdims��128��ά������ÿһά��Ԫ������Ϊ32λ������
	CvMat *clusters=cvCreateMat(featureNum, 1, CV_32SC1); //ÿ��feature���ڡ����ġ���ָ�루ʵ���ϱ�������û���õ�����Ϣ��
	CvMat *centers=cvCreateMat(k, dims, CV_32FC1); //�����ġ���Ϣ�����飬k�������ġ�ÿ�����Ķ���dims��128��ά������ÿһά��Ԫ������Ϊ32λ������
	cvSetZero(clusters); //�������ʼ��Ϊ0
	cvSetZero(centers); //�������ʼ��Ϊ0
	while(file.ReadString(strLine)) 
	{
		CString featureFileName = CIni::removeSuffix(strLine) + SUFFIX_FEATURE;
		fileName = featureDir + L"\\" + featureFileName;

		info.Format(L"�����ļ���%s", featureFileName);
		n = import_features(CIni::CStrToChar(fileName), FEATURE_LOWE, &features); //����feature�ļ���nΪ�����feature����
		if(-1 == n)
		{
			info.Format(L"����ʧ�ܣ�%s�������ļ�·����", featureFileName);
			MessageBox(info);
			appendInfo(info);
			appendInfo("�쳣������", 1);
			return;
		}
		//��feature�ļ�������feature��Ϣ����samples����ṹ��
		for(int i = 0; i < n; i++)
		{
			for(int j = 0; j < dims; j++)
			{
				samples->data.fl[temp++] = features[i].descr[j];
			}
		}
		appendInfo(info);
	}
	file.Close();

	appendInfo("Kmeans���㿪ʼ...");
	cvKMeans2(samples, k, clusters,cvTermCriteria(CV_TERMCRIT_EPS,10,1.0), 3, (CvRNG *)0, KMEANS_USE_INITIAL_LABELS, centers); //Kmeans����
	
	//���浥�ʱ�
	cvSave(CIni::CStrToChar(ini.getWordListFilePath()), centers);
	appendInfo("���ʱ��ѱ�������");
	appendInfo(ini.getWordListFilePath(), 1);
	appendInfo("------Kmeans�������------", 1);
	appendInfo("", 1);
}

void CPPDlg::OnBnClickedBtnWordf()
{
	FileStorage readfs(CIni::CStrToChar(ini.getWordListFilePath()), FileStorage::READ); //��ֻ������ʽ��yml��
 
	Mat wordList; //���ʱ����
	if(!readfs.isOpened())
	{
		MessageBox(L"���ʱ�򲻿���");
		readfs.release();
		return;
	}
	readfs[CIni::CStrToChar(CIni::removeSuffix(CIni::getFileNameFromPath(ini.getWordListFilePath())))] >> wordList; //��ȡ���ʱ�

	CString info;
	CString resultDir = ini.saveDir;

	CString fileName;
	int wordNum = ini.kmeans_k;
	int* wordTimes = new int[wordNum];
	struct feature* features;
	int dims = 128;
	float* pa = new float[dims];
	float* pb = NULL;
	appendInfo("------���Ƶ����------", 1);

	CString strLine;
	CStdioFile inFile;
	CStdioFile outFile;
	inFile.Open(ini.getImgListFilePath(), CStdioFile::modeRead);
	outFile.Open(ini.getWordfMergeFilePath(), CFile::modeCreate | CFile::modeWrite);
	while(inFile.ReadString(strLine)) 
	{
		CString featureFileName = CIni::removeSuffix(strLine) + SUFFIX_FEATURE;
		fileName = ini.getFeatureDirPath() + L"\\" + featureFileName;

		info.Format(L"�����ļ���%s", featureFileName);
		appendInfo(info);

		int n = import_features(CIni::CStrToChar(fileName), FEATURE_LOWE, &features);

		//��ʼ����Ƶ����
		for(int i = 0; i < wordNum; i++)
		{
			wordTimes[i] = 0;
		}

		for(int i = 0; i < n; i++)
		{
			float dist = 0, distMin = 0;
			int wordPos = 0;
			for(int j = 0; j < dims; j++) //�ѵ�i��feature��128��double���������Ƶ�paָ���128��float��
			{
				pa[j] = features[i].descr[j];
			}
			pb = (float*)wordList.data;
			distMin = normL2Sqr_(pa, pb, dims); //����ŷʽ����
			for(int j = 1; j < wordNum; j++)
			{
				pb += dims;
				dist = normL2Sqr_(pa, pb, dims); //����ŷʽ����
				if(dist < distMin)
				{
					distMin = dist;
					wordPos = j;
				}
			}
			wordTimes[wordPos]++;
		}
		
		//д��Ƶ�ļ�
		CString strLine("");
		int count = 0;
		for(int i = 0; i < wordNum; i++)
		{
			if(0 < wordTimes[i])
			{
				strLine.AppendFormat(L" %d:%d", i, wordTimes[i]);
				count++;
			}
		}
		CString preContent;
		preContent.Format(L"%d", count);
		strLine.Append(L"\n");
		outFile.WriteString(preContent + strLine);
	}
	delete[] pa;
	delete[] wordTimes;
	inFile.Close();
	outFile.Close();
	readfs.release();

	appendInfo("��Ƶ�ļ���������");
	appendInfo(ini.getWordfMergeFilePath(), 1);
	appendInfo("------���Ƶ����------", 1);
	appendInfo("", 1);
}

void CPPDlg::OnBnClickedBtnLda()
{
	UpdateData();
	ini.lda_k = m_ldaK;
	CString info;
	int topic_num = m_ldaK;
	int sample_num = 50; // how many samples we want, we use these sample's to calculate phi and theta respectively and then use their averge to represent theta and phi
	char * data = CIni::CStrToChar(ini.getWordfMergeFilePath());
	string model_name = ini.saveDir + "\\lda";
	int burn_in_num = 1000; //how many iterations are there until we are out of burn-in period
	// set beta = 0.1 and alpha = 50 / K
	double beta = 0.1;
	double alpha = (double)50 / (double)topic_num;
	int SAMPLE_LAG = 10;
	struct corpus *cps;
	struct est_param param;
	appendInfo("------LDAѵ����ʼ------", 1);
	cps = read_corpus(data); //��ȡѵ����
	appendInfo("������ʼ��");
	init_param(cps,&param,topic_num); //��ʼ������
	//do gibbs_num times sampling and compute averge theta,phi
	// burn in period
	//for (int iter_time=0; iter_time<burn_in_num + sample_num * SAMPLE_LAG; iter_time++)
	int iter_time = 0;
	int sample_time = 0;
	int p_size = topic_num<2?2:topic_num; 
	double *p = (double*)malloc(sizeof(double)*p_size); //multinomial sampling tempoary storage space;
	double vbeta = cps->num_terms * beta; // v*beta is used in funciton sampling and this value never change so I put it here
	while (1)
	{
		info.Format(L"��%d�ε�����ʼ...", iter_time+1);
		appendInfo(info);
		//foreach documents, apply gibbs sampling
		for (int m=0; m<cps->num_docs; m++)
		{
			int word_index = 0; // word_index indicates that a word is the (word_index)th word in the document.
			double s_talpha = param.nd_sum[m] - 1 + topic_num * alpha; //nd_num[m]+topic_num * alpha will not change per document, so I put it here
			for (int l=0; l<cps->docs[m].length; l++)
			{
				for (int c=0; c<cps->docs[m].words[l].count; c++)
				{
					param.z[m][word_index] = sampling(m,word_index,cps->docs[m].words[l].id,topic_num,cps,&param,alpha,beta,p,s_talpha,vbeta); //sampling����
					word_index++;
				}
			}
		}

		if ((iter_time >= burn_in_num) && (iter_time % SAMPLE_LAG == 0))
		{
			info.Format(L"\t��%d�μ������phi,theta", sample_time+1);
			calcu_param(&param, cps,topic_num,alpha,beta); //�������
			sample_time++;
		}
		iter_time++;
		if (sample_time ==sample_num)
		{
			break;
		}
	}
	// calculate average of phi and theta
	appendInfo(L"����theta��phiƽ��ֵ");
	average_param(&param, cps,topic_num,alpha,beta,sample_num); //��������Ĳ������
	save_model(cps,&param,model_name,alpha,beta,topic_num,sample_num); //����������

	for(int i = 0; i < topic_num; i++)
	{
		CString path;
		path.Format(L"\\%s%d", DIR_LDA_CLASS, i);
		path = ini.saveDir + path;
		if(PathIsDirectory(path))
		{
			CFileFind finder;
			BOOL hasNext = finder.FindFile(path + "\\*.*");
			while(hasNext)
			{
				hasNext = finder.FindNextFileW();
				if(finder.IsDirectory()) 
					continue;
				if(!DeleteFile(finder.GetFilePath()))
				{
					MessageBox(L"���ͼƬ�����ļ���ʧ�ܣ�");
					return;
				}
			}
		}
		else if(!CreateDirectory(path, NULL))
		{
			MessageBox(L"����ͼƬ�����ļ���ʧ�ܣ�");
			return;
		}
	}

	CString fileName;
	CStdioFile file;
	file.Open(ini.getImgListFilePath(), CStdioFile::modeRead);
	int docIndex = 0;
	while(file.ReadString(fileName))
	{
		int maxPTopic = 0;
		CString sourceFile = ini.openDir + L"\\" + fileName;
		for(int i = 1; i < topic_num; i++)
		{
			if(param.theta[docIndex][i] > param.theta[docIndex][maxPTopic])
			{
				maxPTopic = i;
			}
		}
		CString destFile = ini.saveDir;
		destFile.Format(L"\\%s%d\\", DIR_LDA_CLASS, maxPTopic);
		destFile = ini.saveDir + destFile + fileName;
		if(!CopyFile(sourceFile, destFile, TRUE))
		{
			info.Format(L"�����ļ�%sʧ�ܣ�", fileName);
			appendInfo(info);
		}
		docIndex++;
	}
	file.Close();
	appendInfo("------LDAѵ������------", 1);
}

// ���ı����ڵ���Ϣ��ʾ
void CPPDlg::appendInfo(CString info, int flag)
{
	switch(flag)
	{
	case 1:
		m_info.AddString(info);
		break;
	case 2:
		m_info.AddString(CString(" ") + info);
		break;
	default:
		m_info.AddString(CString(">") + info);
	}
	m_info.SetCurSel(m_info.GetCount()-1);
	m_info.SetCurSel(-1);
}

void CPPDlg::appendInfo(char* info, int flag)
{
	CString strInfo(info);
	appendInfo(strInfo, flag);
}

void CPPDlg::OnEnChangePathOpen()
{
	UpdateData();
	ini.openDir = m_pathOpen;
	if(ini.saveDir.IsEmpty())
	{
		m_pathSave = m_pathOpen;
		m_pathSave.Append(L"\\");
		m_pathSave.Append(DIR_RESULT);
		ini.saveDir = m_pathSave;
	}
	UpdateData(FALSE);
}


void CPPDlg::OnEnChangePathSave()
{
	ini.saveDir = m_pathSave;
}
