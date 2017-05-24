#pragma once

#define DEF_SECTION L"PPINI"
#define MAX_BUF_LEN 255
#define SUFFIX_FEATURE L".feature" // feature�ļ��ĺ�׺��
#define SUFFIX_WORD L".word" //��Ƶ�ļ���׺��
#define DIR_FEATURE L"feature" //���feature�ļ���Ŀ¼
#define DIR_SIFT L"siftDraw" //���sift����ͼ��Ŀ¼
#define DIR_RESULT L"result" //Ĭ�ϵĹ���Ŀ¼
#define DIR_LDA_CLASS L"ldaC" //LDA��������ͼƬ���Ŀ¼
#define FILE_NAME_WORD_LIST L"wordList.yml" //���ʱ��ļ�������kmeans�����
#define FILE_NAME_WORDF_MERGE L"wordF.data" //�����ĵ��Ĵ�Ƶͳ���ļ���������Ƶͳ�������LDA���룩
#define FILE_NAME_IMG_LIST L"imgPath.list" //������˳�򱣴�ԭʼͼƬ�ļ����б���ļ�
#define FILE_NAME_INFO L"info.ini" //һЩ��ʱ��Ϣ

class CIni
{
public:
	CIni(void);
	~CIni(void);
	CString openDir; //������ͼƬ����Ŀ¼
	CString saveDir; //��������Ŀ¼
	int kmeans_k;
	int lda_k;
	int isSiftDraw;
private:
	CString fileName;
	BOOL writeString(LPCTSTR key, LPCTSTR val, LPCTSTR section = DEF_SECTION);
	CString getString(LPCTSTR key, LPCTSTR section = DEF_SECTION);
public:
	CString getIniFileName(); //��ȡ�����ļ�ȫ·��
	static char* CStrToChar(CString strSrc);
	CString getSiftDirPath(void);
	CString getFeatureDirPath(void);
	CString getLdaClassDirPath(void);
	CString getWordListFilePath(void);
	CString getWordfMergeFilePath(void);
	CString getImgListFilePath(void);
	CString getInfoFilePath(void);
	CString convertToFeatureSuffix(CString fileName);
	static CString removeSuffix(CString fileName);
	static CString getFileNameFromPath(CString fullFileName);
};

