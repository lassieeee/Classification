
// PPDlg.cpp : 实现文件
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


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
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


// CPPDlg 对话框
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


// CPPDlg 消息处理程序

BOOL CPPDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	m_pathOpen = ini.openDir;
	m_pathSave = ini.saveDir;
	m_kmK = ini.kmeans_k;
	m_ldaK = ini.lda_k;
	m_isSiftDraw = ini.isSiftDraw;
	UpdateData(FALSE);

	m_info.AddString(L"运行状态信息：");

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CPPDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
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

	//检查文件夹路径
	if(haveNext)
	{
		if(!PathIsDirectory(openDir))
		{
			MessageBox(L"图片源目录路径不正确！");
			return;
		}
		if(!PathIsDirectory(saveDir) && !CreateDirectory(saveDir, NULL))
		{
			MessageBox(L"结果保存文件夹创建失败！");
			return;
		}
		if(m_isSiftDraw && !PathIsDirectory(siftDrawDir) && !CreateDirectory(siftDrawDir, NULL))
		{
			MessageBox(L"图片特征绘制文件夹创建失败！");
			return; 
		}
		if(!PathIsDirectory(featureDir) && !CreateDirectory(featureDir, NULL))
		{
			MessageBox(L"特征文件夹创建失败！");
			return; 
		}
	}
	else
	{
		MessageBox(L"没有发现图片文件！");
	}

	//Do sift
	appendInfo("------特征提取开始------", 1);
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
		
		//相关文件的路径
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
			SIFT_CURV_THR/*10*/, SIFT_IMG_DBL/*1*/, SIFT_DESCR_WIDTH/*4*/, SIFT_DESCR_HIST_BINS/*8*/); //SIFTfeature提取
		featureSum += n;
		if( out_file_name != NULL )
			export_features(out_file_name, features, n); //将features导出为文件
		if(m_isSiftDraw && out_img_name != NULL)
		{
			draw_features(img, features, n); //在img图片上标记出features
			cvSaveImage(out_img_name, img, NULL); //将标记后的图片保存
		}
	}
	file.Close();

	CString temp;
	temp.Format(L"%d", featureSum);
	file.Open(ini.getInfoFilePath(), CStdioFile::modeCreate | CStdioFile::modeWrite);
	file.Close();
	WritePrivateProfileString(L"temp", L"featureSum", temp, ini.getInfoFilePath());

	appendInfo("特征文件保存路径：");
	appendInfo(featureDir, 1);
	if(m_isSiftDraw)
	{
		appendInfo("特征图保存路径：");
		appendInfo(siftDrawDir, 1);
	}
	appendInfo("------特征提取结束------", 1);
	appendInfo(" ", 1);
}


void CPPDlg::OnBnClickedBtnKmeans()
{
	UpdateData();
	CString strLine;
	CStdioFile file;
	if(!file.Open(ini.getImgListFilePath(), CStdioFile::modeRead))
	{
		MessageBox(L"原始图片文件名路径列表文件打开失败！");
		return;
	}

	CString featureDir = ini.getFeatureDirPath();
	CString info;
	CString fileName;
	int featureNum = GetPrivateProfileInt(L"temp", L"featureSum", NULL, ini.getInfoFilePath());

	appendInfo("------Kmeans就绪------", 1);
	struct feature* features;
	int dims = 128;
	int n = 0;
	int &k = ini.kmeans_k;
	k = m_kmK;
	int temp = 0;
	CvMat *samples=cvCreateMat(featureNum, dims, CV_32FC1); //包含所有图片的所有feature信息的矩阵，featureNum个feature，每个feature为dims（128）维向量，每一维的元素类型为32位浮点数
	CvMat *clusters=cvCreateMat(featureNum, 1, CV_32SC1); //每个feature所在“质心”的指针（实际上本例程中没有用到该信息）
	CvMat *centers=cvCreateMat(k, dims, CV_32FC1); //“质心”信息的数组，k个“质心”每个质心都是dims（128）维向量，每一维的元素类型为32位浮点数
	cvSetZero(clusters); //将矩阵初始化为0
	cvSetZero(centers); //将矩阵初始化为0
	while(file.ReadString(strLine)) 
	{
		CString featureFileName = CIni::removeSuffix(strLine) + SUFFIX_FEATURE;
		fileName = featureDir + L"\\" + featureFileName;

		info.Format(L"加载文件：%s", featureFileName);
		n = import_features(CIni::CStrToChar(fileName), FEATURE_LOWE, &features); //导入feature文件，n为导入的feature个数
		if(-1 == n)
		{
			info.Format(L"加载失败：%s。请检查文件路径！", featureFileName);
			MessageBox(info);
			appendInfo(info);
			appendInfo("异常结束！", 1);
			return;
		}
		//将feature文件内所有feature信息存入samples矩阵结构内
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

	appendInfo("Kmeans计算开始...");
	cvKMeans2(samples, k, clusters,cvTermCriteria(CV_TERMCRIT_EPS,10,1.0), 3, (CvRNG *)0, KMEANS_USE_INITIAL_LABELS, centers); //Kmeans聚类
	
	//保存单词表
	cvSave(CIni::CStrToChar(ini.getWordListFilePath()), centers);
	appendInfo("单词表已保存至：");
	appendInfo(ini.getWordListFilePath(), 1);
	appendInfo("------Kmeans聚类结束------", 1);
	appendInfo("", 1);
}

void CPPDlg::OnBnClickedBtnWordf()
{
	FileStorage readfs(CIni::CStrToChar(ini.getWordListFilePath()), FileStorage::READ); //以只读的形式打开yml。
 
	Mat wordList; //单词表矩阵
	if(!readfs.isOpened())
	{
		MessageBox(L"单词表打不开！");
		readfs.release();
		return;
	}
	readfs[CIni::CStrToChar(CIni::removeSuffix(CIni::getFileNameFromPath(ini.getWordListFilePath())))] >> wordList; //读取单词表

	CString info;
	CString resultDir = ini.saveDir;

	CString fileName;
	int wordNum = ini.kmeans_k;
	int* wordTimes = new int[wordNum];
	struct feature* features;
	int dims = 128;
	float* pa = new float[dims];
	float* pb = NULL;
	appendInfo("------算词频就绪------", 1);

	CString strLine;
	CStdioFile inFile;
	CStdioFile outFile;
	inFile.Open(ini.getImgListFilePath(), CStdioFile::modeRead);
	outFile.Open(ini.getWordfMergeFilePath(), CFile::modeCreate | CFile::modeWrite);
	while(inFile.ReadString(strLine)) 
	{
		CString featureFileName = CIni::removeSuffix(strLine) + SUFFIX_FEATURE;
		fileName = ini.getFeatureDirPath() + L"\\" + featureFileName;

		info.Format(L"处理文件：%s", featureFileName);
		appendInfo(info);

		int n = import_features(CIni::CStrToChar(fileName), FEATURE_LOWE, &features);

		//初始化词频数组
		for(int i = 0; i < wordNum; i++)
		{
			wordTimes[i] = 0;
		}

		for(int i = 0; i < n; i++)
		{
			float dist = 0, distMin = 0;
			int wordPos = 0;
			for(int j = 0; j < dims; j++) //把第i个feature的128个double浮点数复制到pa指向的128个float数
			{
				pa[j] = features[i].descr[j];
			}
			pb = (float*)wordList.data;
			distMin = normL2Sqr_(pa, pb, dims); //计算欧式距离
			for(int j = 1; j < wordNum; j++)
			{
				pb += dims;
				dist = normL2Sqr_(pa, pb, dims); //计算欧式距离
				if(dist < distMin)
				{
					distMin = dist;
					wordPos = j;
				}
			}
			wordTimes[wordPos]++;
		}
		
		//写词频文件
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

	appendInfo("词频文件保存至：");
	appendInfo(ini.getWordfMergeFilePath(), 1);
	appendInfo("------算词频结束------", 1);
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
	appendInfo("------LDA训练开始------", 1);
	cps = read_corpus(data); //读取训练集
	appendInfo("参数初始化");
	init_param(cps,&param,topic_num); //初始化参数
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
		info.Format(L"第%d次迭代开始...", iter_time+1);
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
					param.z[m][word_index] = sampling(m,word_index,cps->docs[m].words[l].id,topic_num,cps,&param,alpha,beta,p,s_talpha,vbeta); //sampling计算
					word_index++;
				}
			}
		}

		if ((iter_time >= burn_in_num) && (iter_time % SAMPLE_LAG == 0))
		{
			info.Format(L"\t第%d次计算参数phi,theta", sample_time+1);
			calcu_param(&param, cps,topic_num,alpha,beta); //计算参数
			sample_time++;
		}
		iter_time++;
		if (sample_time ==sample_num)
		{
			break;
		}
	}
	// calculate average of phi and theta
	appendInfo(L"计算theta，phi平均值");
	average_param(&param, cps,topic_num,alpha,beta,sample_num); //计算出最后的参数结果
	save_model(cps,&param,model_name,alpha,beta,topic_num,sample_num); //保存结果数据

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
					MessageBox(L"清空图片分类文件夹失败！");
					return;
				}
			}
		}
		else if(!CreateDirectory(path, NULL))
		{
			MessageBox(L"创建图片分类文件夹失败！");
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
			info.Format(L"拷贝文件%s失败！", fileName);
			appendInfo(info);
		}
		docIndex++;
	}
	file.Close();
	appendInfo("------LDA训练结束------", 1);
}

// 富文本框内的消息显示
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
