#include "StdAfx.h"
#include "Ini.h"


CIni::CIni(void) :
fileName(L"Classificationini.txt")
	, openDir(_T(""))
	, saveDir(_T(""))
	, kmeans_k(2)
	, lda_k(2)
	, isSiftDraw(1)
{
	CFileFind finder;
	if(!finder.FindFile(fileName))
	{
		CFile file;
		file.Open(fileName, CFile::modeCreate);
		fileName = file.GetFilePath();
		file.Close();
	}
	else
	{
		finder.FindNextFileW();
		fileName = finder.GetFilePath();
		openDir = getString(L"openDir");
		saveDir = getString(L"saveDir");
		kmeans_k = atoi(CIni::CStrToChar(getString(L"kmens_k")));
		lda_k = atoi(CIni::CStrToChar(getString(L"lda_k")));
		isSiftDraw = atoi(CIni::CStrToChar(getString(L"isSiftDraw")));
	}
}


CIni::~CIni(void)
{
	writeString(L"openDir", openDir);
	writeString(L"saveDir", saveDir);
	CString temp;
	temp.Format(L"%d", kmeans_k);
	writeString(L"kmens_k", temp);
	temp.Format(L"%d", lda_k);
	writeString(L"lda_k", temp);
	temp.Format(L"%d", isSiftDraw);
	writeString(L"isSiftDraw", temp);

}

CString CIni::getIniFileName()
{
	return fileName;
}

BOOL CIni::writeString(LPCTSTR key, LPCTSTR val, LPCTSTR section)
{
	return WritePrivateProfileString(section, key, val, fileName);
}

CString CIni::getString(LPCTSTR key, LPCTSTR section)
{
	WCHAR temp[MAX_BUF_LEN];
	DWORD k = GetPrivateProfileString(section, key, NULL, temp, MAX_BUF_LEN, fileName);
	return CString(temp);
}


char* CIni::CStrToChar(CString strSrc)
{
#ifdef UNICODE
	DWORD dwNum = WideCharToMultiByte(CP_OEMCP, NULL, strSrc.GetBuffer(0), -1, NULL, 0, NULL, FALSE);
	char *psText;
	psText = new char[dwNum];
	if (!psText)
		delete []psText;
	WideCharToMultiByte(CP_OEMCP, NULL, strSrc.GetBuffer(0), -1, psText, dwNum, NULL, FALSE);
	return (char*)psText;
#else
	return (LPCTSRT)strSrc;
#endif
}


CString CIni::getSiftDirPath(void)
{
	return saveDir + L"\\" + DIR_SIFT;
}


CString CIni::getFeatureDirPath(void)
{
	return saveDir + L"\\" + DIR_FEATURE;
}

CString CIni::getLdaClassDirPath(void)
{
	return saveDir + L"\\" + DIR_LDA_CLASS;
}

CString CIni::getWordListFilePath(void)
{
	return saveDir + L"\\" + FILE_NAME_WORD_LIST;
}

CString CIni::getWordfMergeFilePath(void)
{
	return saveDir + L"\\" + FILE_NAME_WORDF_MERGE;
}

CString CIni::getImgListFilePath(void)
{
	return saveDir + L"\\" + FILE_NAME_IMG_LIST;
}

CString CIni::getInfoFilePath(void)
{
	return saveDir + L"\\" + FILE_NAME_INFO;
}

CString CIni::convertToFeatureSuffix(CString fileName)
{
	CString featureFileName = fileName;
	int dotpos = featureFileName.ReverseFind(L'.');
	featureFileName.Delete(dotpos, featureFileName.GetLength() - dotpos);
	featureFileName.Append(SUFFIX_FEATURE);
	return featureFileName;
}


CString CIni::removeSuffix(CString fileName)
{
	int pos = fileName.ReverseFind('.');
	CString newFileName(fileName);
	newFileName.Delete(pos, fileName.GetLength() - pos);
	return newFileName;
}


CString CIni::getFileNameFromPath(CString fullFileName)
{
	int pos = fullFileName.ReverseFind('\\');
	CString newFileName(fullFileName);
	newFileName.Delete(0, pos + 1);
	return newFileName;
}
