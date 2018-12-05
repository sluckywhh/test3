#include "stdafx.h"
#include "update_file_name.h"

/************************************************************************/
/* author : wl
 * email  : lysgwl@163.com
 * date   : 2018.09.01 17:39
 */
/************************************************************************/

//update_file_data
update_file_data::update_file_data()
{
	InitializeCriticalSection(&m_csLockData);
}

update_file_data::~update_file_data()
{
	DeleteCriticalSection(&m_csLockData);
}

update_file_data& update_file_data::Instance()
{
	static update_file_data inst;
	return inst;
}

BOOL update_file_data::SetUpdateFileData(std::vector<UPDATE_FILEINFO*> &vecFileData, UPDATE_FILEDATA_CALLBACK_FUNC pfUpdateFileData)
{
	BOOL bRet = FALSE;
	
	UPDATE_FILEINFO *pFileInfo = NULL;
	UPDATE_FILEDATA *pFileData = NULL;

	UPDATE_FILEDATA stcUpdateFileData;
	std::vector<UPDATE_FILEINFO*>::iterator iterFileData;

	EnterCriticalSection(&m_csLockData);
	do 
	{
		memset(&stcUpdateFileData, 0x0, sizeof(UPDATE_FILEDATA));
		if (vecFileData.size() == 0 || pfUpdateFileData == NULL)
		{
			bRet = FALSE;
			break;
		}

		stcUpdateFileData.emUpdateStatus = STATE_UPDATEINPUTE_TYPE;
		if (!pfUpdateFileData(&stcUpdateFileData))
		{
			bRet = FALSE;
			break;
		}

		for (iterFileData = vecFileData.begin(); iterFileData!=vecFileData.end(); ++iterFileData)
		{
			pFileInfo = (UPDATE_FILEINFO *)(*iterFileData);
			if (pFileInfo == NULL)
			{
				continue;
			}

			pFileData = new UPDATE_FILEDATA;
			if (pFileData == NULL)
			{
				continue;
			}
			memset(pFileData, 0x0, sizeof(UPDATE_FILEDATA));

			memcpy(pFileData, &stcUpdateFileData, sizeof(UPDATE_FILEDATA));
			memcpy(&pFileData->stcFileInfo, pFileInfo, sizeof(UPDATE_FILEINFO));
			pFileData->pfUpdateFunc = pfUpdateFileData;

			m_vecFileData.push_back(pFileData);
		}

		bRet = TRUE;
	} while (FALSE);

	LeaveCriticalSection(&m_csLockData);
	return bRet;
}

BOOL update_file_data::GetUpdateFileData(std::vector<UPDATE_FILEDATA *> &vecFileData)
{
	BOOL bRet = FALSE;

	EnterCriticalSection(&m_csLockData);
	do 
	{
		if (m_vecFileData.size() == 0)
		{
			bRet = FALSE;
			break;
		}

		vecFileData.clear();
		vecFileData.assign(m_vecFileData.begin(), m_vecFileData.end());

		bRet = TRUE;
	} while (FALSE);

	LeaveCriticalSection(&m_csLockData);
	return bRet;
}

void update_file_data::ClearFileData()
{
	BOOL bRet = FALSE;
	UPDATE_FILEDATA *pFileData = NULL;
	std::vector<UPDATE_FILEDATA *>::iterator iterFileData;

	EnterCriticalSection(&m_csLockData);
	do 
	{
		for (iterFileData=m_vecFileData.begin(); iterFileData!=m_vecFileData.end();)
		{
			pFileData = (UPDATE_FILEDATA *)(*iterFileData);
			if (pFileData != NULL)
			{
				delete pFileData;
				pFileData = NULL;

				iterFileData = m_vecFileData.erase(iterFileData);
			}
			else
			{
				++iterFileData;
			}
		}

		m_vecFileData.clear();
		bRet = TRUE;
	} while (FALSE);

	LeaveCriticalSection(&m_csLockData);
}

//////////////////////////////////////////////////////////////////////////
//
update_file_func::update_file_func()
{
}

update_file_func::~update_file_func()
{
}

update_file_func& update_file_func::Instance()
{
	static update_file_func inst;
	return inst;
}

BOOL update_file_func::SetUpdateFileFunc(UPDATE_CONFIGTYPE emConfigType, UPDATE_FILEDATA *pFileData)
{
	BOOL bRet = FALSE;

	do 
	{
		switch (emConfigType)
		{
		case CONFIG_ADDFILENAME_TYPE:
			{
				if (!SetAddFileName(emConfigType, pFileData))
				{
					bRet = FALSE;
					break;
				}

				bRet = TRUE;
			}
			break;

		case CONFIG_DATEFILENAME_TYPE:
			{
				if (!SetDateFileName(emConfigType, pFileData))
				{
					bRet = FALSE;
					break;
				}

				bRet = TRUE;
			}
			break;

		case CONFIG_DELFILENAME_TYPE:
			{
				if (!SetDelFileName(emConfigType, pFileData))
				{
					bRet = FALSE;
					break;
				}

				bRet = TRUE;
			}
			break;

		case CONFIG_EXTFILENAME_TYPE:
			{
				if (!SetExtFileName(emConfigType, pFileData))
				{
					bRet = FALSE;
					break;
				}

				bRet = TRUE;
			}
			break;

		case CONFIG_INDEXFILENAME_TYPE:
			{
				if (!SetIndexFileName(emConfigType, pFileData))
				{
					bRet = FALSE;
					break;
				}

				bRet = TRUE;
			}
			break;

		case CONFIG_REPLACEFILENAME_TYPE:
			{
				if (!SetReplaceFileName(emConfigType, pFileData))
				{
					bRet = FALSE;
					break;
				}

				bRet = TRUE;
			}
			break;

		default:
			break;
		}

		if (!bRet)
		{
			break;
		}

		bRet = TRUE;
	} while (FALSE);

	return bRet;
}

BOOL update_file_func::SetAddFileName(UPDATE_CONFIGTYPE emConfigType, UPDATE_FILEDATA *pFileData)
{
	BOOL bRet = FALSE;

	int iAddIndex = -1;
	int iIndex = -1;

	unsigned int uiPos = 0;
	unsigned int uiLen = 0;
	unsigned int uiOffset = 0;

	char *p = NULL;
	char *ptr = NULL;
	char *pFileName = NULL;

	char szFileOldName[MAX_PATH] = {0};
	char szFileNewName[MAX_PATH] = {0};

	do 
	{
		if (emConfigType != pFileData->emConfigType || pFileData == NULL)
		{//判断文件名称修改类型
			bRet = FALSE;
			break;
		}

		if (strcmp(pFileData->stcAddFileName.szFileName, _T("")) == 0)
		{//判断规则名称是否为空
			bRet = FALSE;
			break;
		}

		ptr = strrchr(pFileData->stcFileInfo.szFileName, '.');	//strtok
		if (ptr == NULL)
		{
			uiPos = strlen(pFileData->stcFileInfo.szFileName);
			memcpy(szFileOldName, pFileData->stcFileInfo.szFileName, uiPos);
			pFileName = pFileData->stcFileInfo.szFileName;
		}
		else
		{
			uiPos = ptr - pFileData->stcFileInfo.szFileName;
			memcpy(szFileOldName, pFileData->stcFileInfo.szFileName, uiPos);
			pFileName = szFileOldName;
		}

		iAddIndex = pFileData->stcAddFileName.iIndex;	//待添加的位置
		uiLen = strlen(pFileName);	//名称长度
		
		if (iAddIndex >= 0)
		{//字符正向处理
			iIndex = 0;
			p = pFileName;

			while (*p != '\0')
			{
				if (iIndex == 0)
				{
					uiPos = 0;
				}
				else
				{
					if ((*p&0x80) && (*(p+1)&0x80))
					{
						uiPos += 2;
						p += 2;
					}
					else
					{
						uiPos += 1;
						p += 1;
					}
				}

				if (iIndex == iAddIndex)
				{
					memcpy(szFileNewName+uiOffset, pFileName, uiPos);
					uiOffset += uiPos;

					memcpy(szFileNewName+uiOffset, pFileData->stcAddFileName.szFileName, strlen(pFileData->stcAddFileName.szFileName));
					uiOffset += strlen(pFileData->stcAddFileName.szFileName);

					memcpy(szFileNewName+uiOffset, pFileName+uiPos, uiLen-uiPos);
					uiOffset += (uiLen-uiPos);

					memcpy(szFileNewName+uiOffset, pFileData->stcFileInfo.szFileExt, strlen(pFileData->stcFileInfo.szFileExt));
					uiOffset += strlen(pFileData->stcFileInfo.szFileExt);

					bRet = TRUE;
					break;
				}

				iIndex++;
			}
		}
		else
		{//字符反向处理
			iIndex = uiLen;
			p = pFileName + uiLen;

			do 
			{
				if (iIndex == uiLen)	
				{
					uiPos = uiLen;
					p -= 1;
				}
				else
				{
					if ((*p&0x80) && (*(p-1)&0x80))
					{
						uiPos -= 2;
						p -= 2;
					}
					else
					{
						uiPos -= 1;
						p -= 1;
					}
				}

				if ((iIndex-uiLen) == iAddIndex)	//正数转负数:~num+1;	负数转正数:~(num-1)
				{
					memcpy(szFileNewName+uiOffset, pFileName, uiPos);
					uiOffset += uiPos;

					memcpy(szFileNewName+uiOffset, pFileData->stcAddFileName.szFileName, strlen(pFileData->stcAddFileName.szFileName));
					uiOffset += strlen(pFileData->stcAddFileName.szFileName);

					memcpy(szFileNewName+uiOffset, pFileName+uiPos, uiLen-uiPos);
					uiOffset += (uiLen-uiPos);

					memcpy(szFileNewName+uiOffset, pFileData->stcFileInfo.szFileExt, strlen(pFileData->stcFileInfo.szFileExt));
					uiOffset += strlen(pFileData->stcFileInfo.szFileExt);

					bRet = TRUE;
					break;
				}

				iIndex--;
			} while (*p != '\0');
		}

	} while (FALSE);

	return bRet;
}

BOOL update_file_func::SetDateFileName(UPDATE_CONFIGTYPE emConfigType, UPDATE_FILEDATA *pFileData)
{
	BOOL bRet = FALSE;

	unsigned int uiPos = 0;
	unsigned int uiLen = 0;

	unsigned int uiIndex = 0;
	unsigned int uiOffset = 0;

	char *p = NULL;
	char *ptr = NULL;
	char *pFileName = NULL;

	char szFileOldName[MAX_PATH] = {0};
	char szFileNewName[MAX_PATH] = {0};

	do 
	{
		if (emConfigType != pFileData->emConfigType || pFileData == NULL)
		{//判断文件名称修改类型
			bRet = FALSE;
			break;
		}

		ptr = strrchr(pFileData->stcFileInfo.szFileName, '.');
		if (ptr == NULL)
		{//文件名称
			uiPos = strlen(pFileData->stcFileInfo.szFileName);
			memcpy(szFileOldName, pFileData->stcFileInfo.szFileName, uiPos);
			pFileName = pFileData->stcFileInfo.szFileName;
		}
		else
		{
			uiPos = ptr - pFileData->stcFileInfo.szFileName;
			memcpy(szFileOldName, pFileData->stcFileInfo.szFileName, uiPos);
			pFileName = szFileOldName;
		}

		bRet = TRUE;
	} while (FALSE);

	return bRet;
}

BOOL update_file_func::SetDelFileName(UPDATE_CONFIGTYPE emConfigType, UPDATE_FILEDATA *pFileData)
{
	BOOL bRet = FALSE;

	int iDelIndex = -1;
	int iDelCount = -1;

	int iSpecIndex = -1;
	int iIndex = -1;

	unsigned int uiPos = 0;
	unsigned int uiLen = 0;
	unsigned int uiOffset = 0;

	char *p = NULL;
	char *ptr = NULL;
	char *pFileName = NULL;

	char szFileOldName[MAX_PATH] = {0};
	char szFileNewName[MAX_PATH] = {0};

	do 
	{
		if (emConfigType != pFileData->emConfigType || pFileData == NULL)
		{//判断文件名称修改类型
			bRet = FALSE;
			break;
		}

		ptr = strrchr(pFileData->stcFileInfo.szFileName, '.');	//strtok
		if (ptr == NULL)
		{
			uiPos = strlen(pFileData->stcFileInfo.szFileName);
			memcpy(szFileOldName, pFileData->stcFileInfo.szFileName, uiPos);
			pFileName = pFileData->stcFileInfo.szFileName;
		}
		else
		{
			uiPos = ptr - pFileData->stcFileInfo.szFileName;
			memcpy(szFileOldName, pFileData->stcFileInfo.szFileName, uiPos);
			pFileName = szFileOldName;
		}

		if (strcmp(pFileData->stcDelFileName.szFileName, _T("")) != 0)
		{
			ptr = strtok(pFileName, pFileData->stcDelFileName.szFileName);
			while(ptr)
			{
				memcpy(szFileNewName+uiOffset, ptr, strlen(ptr));
				uiOffset += strlen(ptr);

				ptr = strtok(NULL, pFileData->stcDelFileName.szFileName);
			}

			if (strcmp(szFileNewName, _T("")) == 0)
			{
				bRet = FALSE;
				break;
			}

			memcpy(szFileNewName+uiOffset, pFileData->stcFileInfo.szFileExt, strlen(pFileData->stcFileInfo.szFileExt));
			uiOffset += strlen(pFileData->stcFileInfo.szFileExt);
		}
		else
		{
			uiLen = strlen(pFileName);
			iDelIndex = pFileData->stcDelFileName.iIndex;	//待删除的位置
			if (pFileData->stcDelFileName.iCount > uiLen)
			{
				iDelCount = uiLen;
			}
			else
			{
				iDelCount = pFileData->stcDelFileName.iCount;
			}

			if (iDelIndex >= 0)
			{//正向处理
				iIndex = 0;
				p = pFileName;

				while (*p != '\0')
				{
					if (iIndex == 0)
					{
						uiPos = 0;
					}
					else
					{
						if ((*p&0x80) && (*(p+1)&0x80))
						{
							uiPos += 2;
							p += 2;
						}
						else
						{
							uiPos += 1;
							p += 1;
						}
					}

					if (iIndex == iDelIndex)
					{
						iSpecIndex = iIndex;
					}
					else
					{
						if (iSpecIndex > 0)
						{
							if (iIndex == (iSpecIndex+iDelCount))
							{
								memcpy(szFileNewName+uiOffset, pFileName+uiPos, (uiLen - uiPos));
								uiOffset += (uiLen-uiPos);

								if (strcmp(szFileNewName, _T("")) == 0)
								{
									bRet = FALSE;
									break;
								}

								memcpy(szFileNewName+uiOffset, pFileData->stcFileInfo.szFileExt, strlen(pFileData->stcFileInfo.szFileExt));
								uiOffset += strlen(pFileData->stcFileInfo.szFileExt);

								bRet = TRUE;
								break;
							}
						}
					}

					iIndex++;
				}
			}
			else
			{//反向处理
				iIndex = uiLen;
				p = pFileName + uiLen;

				do 
				{
					if (iIndex == uiLen)
					{
						uiPos = uiLen;
						p -= 1;
					}
					else
					{
						if ((*p&0x80) && (*(p-1)&0x80))
						{
							uiPos -= 2;
							p -= 2;
						}
						else
						{
							uiPos -= 1;
							p -= 1;
						}
					}

					if ((iIndex-uiLen) == iDelIndex)		//正数转负数:~num+1;	负数转正数:~(num-1)
					{
						iSpecIndex = iIndex;
					}
					else
					{
						if (iSpecIndex > 0)
						{
							if (iIndex == (iSpecIndex-iDelCount))
							{
								memcpy(szFileNewName+uiOffset, pFileName, uiPos);
								uiOffset += uiPos;

								if (strcmp(szFileNewName, _T("")) == 0)
								{
									bRet = FALSE;
									break;
								}

								memcpy(szFileNewName+uiOffset, pFileData->stcFileInfo.szFileExt, strlen(pFileData->stcFileInfo.szFileExt));
								uiOffset += strlen(pFileData->stcFileInfo.szFileExt);

								bRet = TRUE;
								break;
							}
						}
					}
					
					iIndex--;
				} while (*p != '\0');
			}
		}

		bRet = TRUE;
	} while (FALSE);

	return bRet;
}

BOOL update_file_func::SetExtFileName(UPDATE_CONFIGTYPE emConfigType, UPDATE_FILEDATA *pFileData)
{
	BOOL bRet = FALSE;

	unsigned int uiPos = 0;
	unsigned int uiLen = 0;
	unsigned int uiOffset = 0;

	char *p = NULL;
	char *ptr = NULL;
	char *pFileExt = NULL;
	char *pFileName = NULL;

	char szExtName[MAX_PATH] = {0};
	char szFileOldName[MAX_PATH] = {0};
	char szFileNewName[MAX_PATH] = {0};

	do 
	{
		if (emConfigType != pFileData->emConfigType || pFileData == NULL)
		{//判断文件名称修改类型
			bRet = FALSE;
			break;
		}

		ptr = strrchr(pFileData->stcFileInfo.szFileName, '.');	//strtok
		if (ptr == NULL)
		{
			uiPos = strlen(pFileData->stcFileInfo.szFileName);
			memcpy(szFileOldName, pFileData->stcFileInfo.szFileName, uiPos);

			pFileName = pFileData->stcFileInfo.szFileName;
			pFileExt = NULL;
		}
		else
		{
			uiPos = ptr - pFileData->stcFileInfo.szFileName;
			memcpy(szFileOldName, pFileData->stcFileInfo.szFileName, uiPos);

			pFileName = szFileOldName;
			pFileExt = ptr;
		}

		uiLen = strlen(pFileName);	//名称长度
		if (strcmp(pFileData->stcExtFileName.szExtName, _T("")) != 0)
		{
			ptr = strrchr(pFileData->stcExtFileName.szExtName, '.');
			if (ptr == NULL)
			{
				sprintf(szExtName, _T(".%s"), pFileData->stcExtFileName.szExtName);
			}
			else
			{
				strcpy(szExtName, pFileData->stcExtFileName.szExtName);
			}

			if (pFileData->stcExtFileName.bIsUppercase)
			{//转换大写
				pFileExt = strupr(szExtName);
			}
			else
			{//转换小写
				pFileExt = strlwr(szExtName);
			}
		}
		else
		{
			if (pFileData->stcExtFileName.bIsUppercase)
			{
				pFileExt = strupr(pFileExt);
			}
			else
			{
				pFileExt = strlwr(pFileExt);
			}
		}

		memcpy(szFileNewName+uiOffset, pFileName, uiLen);
		uiOffset += uiLen;

		memcpy(szFileNewName+uiOffset, pFileExt, strlen(pFileExt));
		uiOffset += strlen(pFileExt);

		bRet = TRUE;
	} while (FALSE);

	return bRet;
}

BOOL update_file_func::SetIndexFileName(UPDATE_CONFIGTYPE emConfigType, UPDATE_FILEDATA *pFileData)
{
	BOOL bRet = FALSE;

	UPDATE_INDEXFILENAME stcIndexFileName = {0};

	do 
	{
		if (emConfigType == CONFIG_EMPTYTYPE || pFileData == NULL)
		{
			bRet = FALSE;
			break;
		}

		if (emConfigType != pFileData->emConfigType)
		{
			bRet = FALSE;
			break;
		}

		bRet = TRUE;
	} while (FALSE);

	return bRet;
}

BOOL update_file_func::SetReplaceFileName(UPDATE_CONFIGTYPE emConfigType, UPDATE_FILEDATA *pFileData)
{
	BOOL bRet = FALSE;

	UPDATE_REPLACEFILENAME stcReplaceFileName = {0};

	do 
	{
		if (emConfigType == CONFIG_EMPTYTYPE || pFileData == NULL)
		{
			bRet = FALSE;
			break;
		}

		if (emConfigType != pFileData->emConfigType)
		{
			bRet = FALSE;
			break;
		}

		bRet = TRUE;
	} while (FALSE);

	return bRet;
}

//////////////////////////////////////////////////////////////////////////
//update_file_name
update_file_name::update_file_name()
{
	m_bExit = FALSE;
	
	m_dwProcTimeOver = 500;
	m_dwCloseTimeOver = INFINITE;
	
	m_hThread = NULL;
	m_dwThreadID = 0;

	m_hStartEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_hEndEvent   = CreateEvent(NULL, TRUE, FALSE, NULL);
}

update_file_name::~update_file_name()
{
	if (m_hStartEvent != NULL)
	{
		CloseHandle(m_hStartEvent);
		m_hStartEvent = NULL;
	}

	if (m_hEndEvent != NULL)
	{
		CloseHandle(m_hEndEvent);
		m_hEndEvent = NULL;
	}
}

DWORD update_file_name::UpdateFileThreadProc(LPVOID lpParam)
{
	update_file_name* pUpdateFileProc = (update_file_name*)lpParam;
	if(pUpdateFileProc != NULL)
	{
		pUpdateFileProc->UpdateFileInfo();
	}

	return 0;
}

void update_file_name::UpdateFileInfo()
{
	BOOL bRet = FALSE;

	do 
	{
		if (!UpdateFileName())
		{
			bRet = FALSE;
			break;
		}

		bRet = TRUE;
	} while (FALSE);
}

BOOL update_file_name::UpdateFileName()
{
	BOOL bRet = FALSE;

	UPDATE_FILEDATA *pUpdateFileData;
	std::vector<UPDATE_FILEDATA*> vecFileData;
	std::vector<UPDATE_FILEDATA*>::iterator iterFileData;

	do 
	{
		if (!update_file_data::Instance().GetUpdateFileData(vecFileData))
		{
			bRet = FALSE;
			break;
		}

		for (iterFileData=vecFileData.begin(); iterFileData!=vecFileData.end(); ++iterFileData)
		{
			pUpdateFileData = (UPDATE_FILEDATA *)(*iterFileData);
			if (pUpdateFileData == NULL)
			{
				continue;
			}

			if (!update_file_func::Instance().SetUpdateFileFunc(pUpdateFileData->emConfigType, pUpdateFileData))
			{
				continue;
			}
		}
		
		bRet = TRUE;
	} while (FALSE);

	return bRet;
}
//////////////////////////////////////////////////////////////////////////
//
BOOL update_file_name::CreateUpdateProc()
{
	BOOL bRet = FALSE;

	do
	{
		CloseUpdateProc();
		if (WaitForSingleObject(m_hStartEvent, 0) != WAIT_OBJECT_0)
		{
			m_hThread = CreateThread(NULL, 0, UpdateFileThreadProc, (LPVOID)this, 0, &m_dwThreadID);
			if (m_hThread == NULL || m_hThread == INVALID_HANDLE_VALUE)
			{
				bRet = FALSE;
				break;
			}

			SetEvent(m_hStartEvent);
			ResetEvent(m_hEndEvent);
			
			bRet = TRUE;
		}	
	} while(FALSE);

	return bRet;
}

BOOL update_file_name::CloseUpdateProc()
{
	BOOL bRet = FALSE;

	do 
	{
		if (m_hThread == NULL || m_hThread == INVALID_HANDLE_VALUE)
		{
			bRet = FALSE;
			break;
		}

		m_bExit = TRUE;
		WaitForSingleObject(m_hEndEvent, m_dwCloseTimeOver);

/*
		if (m_mapEnumInfo.size() > 0)
		{
			for (std::map<std::string, UPDATE_FILEINFO*>::iterator mapIter mapIter = m_mapEnumInfo.begin(); mapIter != m_mapEnumInfo.end();)
			{
				if (mapIter->second != NULL)
				{
					delete mapIter->second;
					mapIter->second = NULL;
				}

				mapIter = m_mapEnumInfo.erase(mapIter);
			}

			m_mapEnumInfo.clear();
		}
*/
		if (m_hThread != NULL)
		{
			CloseHandle(m_hThread);
			m_hThread = NULL;
		}
		
		ResetEvent(m_hStartEvent);
		ResetEvent(m_hEndEvent);

		bRet = TRUE;
	} while (FALSE);

	return bRet;
}